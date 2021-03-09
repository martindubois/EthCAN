
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Serial.cpp

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

Serial::Serial(const char* aLink) : mBufferLevel(0), mContext(NULL), mReceiver(NULL), mState(STATE_TRACE), mThread(NULL)
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

void Serial::Receiver_Stop()
{
    assert(NULL != mThread);

    mThread->Zone0_Enter();
    {
        mContext = NULL;
        mReceiver = NULL;
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

bool Serial::OnLoopIteration()
{
    assert(NULL != mThread);

    bool lResult = true;

    unsigned int lSize_byte = Raw_Receive(mBuffer + mBufferLevel, sizeof(mBuffer) - mBufferLevel);
    while (0 < lSize_byte)
    {
        switch (mState)
        {
        case STATE_DATA:
            mBufferLevel += lSize_byte;
            break;

        case STATE_TRACE:
            for (unsigned int i = 0; i < lSize_byte; i++)
            {
                if (EthCAN_SYNC == mBuffer[i])
                {
                    mBufferLevel = lSize_byte - i - 1;
                    mState = STATE_DATA;

                    if (0 < mBufferLevel)
                    {
                        memmove(mBuffer, mBuffer + i + 1, mBufferLevel);
                    }

                    break;
                }

                printf("%c", mBuffer[i]);
            }
            break;

        default: assert(false);
        }

        lSize_byte = 0;

        if ((mState == STATE_DATA) && (sizeof(EthCAN_Header) <= mBufferLevel))
        {
            const EthCAN_Header* lHeader = reinterpret_cast<EthCAN_Header *>(mBuffer);

            if (mBufferLevel >= lHeader->mTotalSize_byte)
            {
                mThread->Zone0_Enter();
                {
                    if (NULL != mReceiver)
                    {
                        lResult = mReceiver->OnMessage(this, mMessage, mBuffer, lHeader->mTotalSize_byte);
                    }
                }
                mThread->Zone0_Leave();

                lSize_byte = mBufferLevel - lHeader->mTotalSize_byte;

                mBufferLevel = 0;
                mState = STATE_TRACE;

                if (0 < lSize_byte)
                {
                    memmove(mBuffer, mBuffer + lHeader->mTotalSize_byte, lSize_byte);
                }
            }
        }
    }

    return lResult;
}
