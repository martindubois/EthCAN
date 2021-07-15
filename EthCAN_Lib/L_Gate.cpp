
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/L_Gate.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Gate.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Gate::Gate()
{
    int lRet = pthread_mutex_init(&mInternal, NULL);
    assert(0 == lRet);
}

Gate::~Gate()
{
    int lRet = pthread_mutex_destroy(&mInternal);
    assert(0 == lRet);
}

void Gate::Enter()
{
    int lRet = pthread_mutex_lock(&mInternal);
    assert(0 == lRet);
}

void Gate::Leave()
{
    int lRet = pthread_mutex_unlock(&mInternal);
    assert(0 == lRet);
}

pthread_mutex_t * Gate::GetInternal()
{
    return &mInternal;
}
