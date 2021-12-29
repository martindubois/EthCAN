
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol.cpp

// CODE REVIEW 2021-12-29 KMS - Martin Dubois, P. Eng.

// TEST COVERAGE 2021-12-28 KMS - Martin Dubois, P. Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Protocol.h"

// Public
// //////////////////////////////////////////////////////////////////////////

Protocol::~Protocol() {}

const char* Protocol::GetName() const { return mName; }

// Protected
// //////////////////////////////////////////////////////////////////////////

Protocol::Protocol(const char* aName) : mName(aName)
{
    assert(NULL != aName);
}
