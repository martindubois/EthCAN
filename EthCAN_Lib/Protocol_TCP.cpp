
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol_TCP.cpp

// CODE REVIEW 2021-12-29 KMS - Martin Dubois, P. Eng.

// TEST COVERAGE 2021-12-28 KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Thread.h"

#include "Protocol_TCP.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Protocol_TCP::Protocol_TCP(uint32_t aIPv4) : Protocol("TCP"), mSocket(aIPv4)
{
}

void Protocol_TCP::Init(Thread* aThread)
{
    assert(NULL != aThread);

    mThread = aThread;
}

// ===== Protocol ===========================================================

unsigned int Protocol_TCP::Receive(void*aOut, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t*)
{
    return mSocket.Receive(aOut, aSize_byte, aTimeout_ms);
}

void Protocol_TCP::Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4)
{
    mSocket.Send(aIn, aSize_byte);
}

void Protocol_TCP::Signal()
{
    assert(NULL != mThread);

    mThread->Sem_Signal();
}

void Protocol_TCP::Wait()
{
    assert(NULL != mThread);

    mThread->Sem_Wait(2000);
}
