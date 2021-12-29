
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol_UDP.cpp

// CODE REVIEW 2021-12-29 KMS - Martin Dubois, P. Eng.

// TEST COVERAGE 2021-12-28 KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Protocol_UDP.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Protocol_UDP::Protocol_UDP() : Protocol("UDP")
{
}

// ===== Protocol ===========================================================

unsigned int Protocol_UDP::Receive(void* aOut, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom)
{
    return mClient.Receive(aOut, aSize_byte, aTimeout_ms, aFrom);
}

void Protocol_UDP::Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4)
{
    mClient.Send(aIn, aSize_byte, aIPv4);
}

void Protocol_UDP::Signal() { assert(false); }
void Protocol_UDP::Wait  () { assert(false); }
