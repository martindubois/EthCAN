
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_Gate.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Gate.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Gate::Gate()
{
    InitializeCriticalSection(&mInternal);
}

Gate::~Gate()
{
    DeleteCriticalSection(&mInternal);
}

void Gate::Enter()
{
    EnterCriticalSection(&mInternal);
}

void Gate::Leave()
{
    LeaveCriticalSection(&mInternal);
}
