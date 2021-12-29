
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol_USB.cpp

// CODE REVIEW 2021-12-29 KMS - Martin Dubois, P. Eng.

// TEST COVERAGE 2021-12-29 KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Serial.h"
#include "Thread.h"

#include "Protocol_USB.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Protocol_USB::Protocol_USB(Serial* aSerial) : Protocol("USB"), mSerial(aSerial)
{
    assert(NULL != aSerial);
}

// ===== Protocol ===========================================================

Protocol_USB::~Protocol_USB()
{
    assert(NULL != mSerial);

    mSerial->Receiver_Stop();
}

unsigned int Protocol_USB::Receive(void* aOut, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom)
{
    assert(false);
    return 0;
}

void Protocol_USB::Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4)
{
    assert(NULL != mSerial);

    mSerial->Send(aIn, aSize_byte);
}

void Protocol_USB::Signal()
{
    assert(NULL != mSerial);

    Thread * lThread = mSerial->GetThread();
    assert(NULL != lThread);

    lThread->Sem_Signal();
}

void Protocol_USB::Wait()
{
    assert(NULL != mSerial);

    Thread* lThread = mSerial->GetThread();
    assert(NULL != lThread);

    lThread->Sem_Wait(2000);
}
