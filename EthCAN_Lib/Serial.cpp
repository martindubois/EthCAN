
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Serial.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
}

// ===== EthCAN_Lib =========================================================
#include "Thread.h"

#include "Serial.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define MSG_LOOP_ITERATION (1)

static const uint8_t SYNC = EthCAN_SYNC;

// Public
/////////////////////////////////////////////////////////////////////////////

Serial::Serial(const char* aLink, FILE* aTrace) : mBufferLevel(0), mContext(NULL), mReceiver(NULL), mState(STATE_TRACE), mThread(NULL), mTrace(aTrace)
{
    memset(&mBuffer, 0, sizeof(mBuffer));
    memset(&mLink  , 0, sizeof(mLink));

    strcpy_s(mLink, aLink);

    Connect();

    mThread = new Thread(this, MSG_LOOP_ITERATION);
}

Serial::~Serial()
{
    assert(NULL != mThread);

    delete mThread;

    Disconnect();
}

Thread* Serial::GetThread()
{
    return mThread;
}

void Serial::Receiver_Start(IMessageReceiver* aReceiver, unsigned int aMessage)
{
    assert(NULL != aReceiver);

    assert(NULL != mThread);

    mThread->Zone0_Enter();
    {
        mMessage = aMessage;
        mReceiver = aReceiver;
    }
    mThread->Zone0_Leave();
}

void Serial::Send(const void* aIn, unsigned int aSize_byte)
{
    assert(NULL != aIn);
    assert(0 < aSize_byte);

    Raw_Send(&SYNC, sizeof(SYNC));
    Raw_Send(aIn, aSize_byte);
}

// ===== IMessageReceiver ===================================================

bool Serial::OnMessage(void* aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte)
{
    bool lResult = false;

    switch (aMessage)
    {
    case MSG_LOOP_ITERATION: lResult = OnLoopIteration(); break;

    default: assert(false);
    }

    return lResult;
}

// Private
/////////////////////////////////////////////////////////////////////////////

void Serial::Buffer_Flush(unsigned int aSize_byte)
{
    assert(0 < aSize_byte);

    assert(aSize_byte <= mBufferLevel);

    mBufferLevel -= aSize_byte;
    if (0 < mBufferLevel)
    {
        memmove(mBuffer, mBuffer + aSize_byte, mBufferLevel);
    }
}

void Serial::Buffer_Trace(unsigned int aSize_byte)
{
    if (NULL != mTrace)
    {
        for (unsigned int i = 0; i < aSize_byte; i++)
        {
            uint8_t lC = mBuffer[i];
            switch (lC)
            {
            case '\n':
            case '\r':
                fprintf(mTrace, "%c", lC);
                break;

            default:
                if ((32 <= lC) && (126 >= lC))
                {
                    fprintf(mTrace, "%c", lC);
                }
            }
        }
    }

    Buffer_Flush(aSize_byte);
}

bool Serial::OnLoopIteration()
{
    assert(NULL != mThread);

    bool lResult = true;

    unsigned int lSize_byte = Raw_Receive(mBuffer + mBufferLevel, sizeof(mBuffer) - mBufferLevel);
    if (0 < lSize_byte)
    {
        mBufferLevel += lSize_byte;
        switch (mState)
        {
        case STATE_DATA  : lResult = Receive_Data  (); break;
        case STATE_HEADER: lResult = Receive_Header(); break;
        case STATE_TRACE : lResult = Receive_Trace (); break;

        default: assert(false);
        }
    }

    return lResult;
}

bool Serial::Receive_Data()
{
    assert(sizeof(EthCAN_Header) <= mBufferLevel);
    assert(STATE_DATA == mState);
    assert(NULL != mThread);

    bool lResult = true;

    EthCAN_Header* lHeader = reinterpret_cast<EthCAN_Header *>(mBuffer);
    assert(sizeof(EthCAN_Header) <= lHeader->mTotalSize_byte);
    if (lHeader->mTotalSize_byte <= mBufferLevel)
    {
        mThread->Zone0_Enter();
        {
            if (NULL != mReceiver)
            {
                lResult = mReceiver->OnMessage(this, mMessage, mBuffer, lHeader->mTotalSize_byte);
            }
        }
        mThread->Zone0_Leave();

        Buffer_Flush(lHeader->mTotalSize_byte);

        mState = STATE_TRACE;

        if (0 < mBufferLevel)
        {
            Receive_Trace();
        }
    }

    return lResult;
}

bool Serial::Receive_Header()
{
    assert(0 < mBufferLevel);
    assert(STATE_HEADER == mState);

    bool lResult = true;

    if (sizeof(EthCAN_Header) <= mBufferLevel)
    {
        EthCAN_Header* lHeader = reinterpret_cast<EthCAN_Header*>(mBuffer);
        if ((sizeof(EthCAN_Header) + lHeader->mDataSize_byte) <= lHeader->mTotalSize_byte)
        {
            mState = STATE_DATA;

            lResult = Receive_Data();
        }
        else
        {
            TRACE_WARNING(mTrace, "Serial::Receive_Header - Invalid header");

            mState = STATE_TRACE;

            Receive_Trace();
        }
    }

    return lResult;
}

bool Serial::Receive_Trace()
{
    assert(0 < mBufferLevel);
    assert(STATE_TRACE == mState);

    for (unsigned int i = 0; i < mBufferLevel; i++)
    {
        if (EthCAN_SYNC == mBuffer[i])
        {
            Buffer_Trace(i + 1);

            mState = STATE_HEADER;

            bool lResult = true;

            if (0 < mBufferLevel)
            {
                lResult = Receive_Header();
            }

            return lResult;
        }
    }

    Buffer_Trace(mBufferLevel);

    return true;
}
