
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/L_OS.cpp

#include "Component.h"

// ===== C ==================================================================
#include <time.h>

// ===== EthCAN_Lib =========================================================
#include "OS.h"

// Functions
/////////////////////////////////////////////////////////////////////////////

uint64_t OS_GetTickCount()
{
    struct timespec lTS;

    memset(&lTS, 0, sizeof(lTS));
    
    int lRet = clock_gettime(CLOCK_MONOTONIC, &lTS);
    assert(0 == lRet);
    (void)lRet;

    uint64_t lResult_ms = lTS.tv_sec;
    
    lResult_ms *= 1000;
    lResult_ms += lTS.tv_nsec / 1000000;

    return lResult_ms;
}
