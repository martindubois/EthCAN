
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Component.h

#pragma once

#include <KmsBase.h>

// ===== C ==================================================================
#include <memory.h>
#include <stdint.h>
#include <stdio.h>

#ifdef _KMS_LINUX_

    // ===== System =========================================================
    #include <netinet/ip.h>

#endif

#ifdef _KMS_WINDOWS_

    // ===== Windows ========================================================
    #include <WinSock2.h>

#endif

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Result.h>
}
