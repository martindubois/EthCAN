
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/System.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>

#include <EthCAN/System.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// ===== EthCAN_Lib =========================================================
#include "System_Impl.h"

namespace EthCAN
{

    // Public
    /////////////////////////////////////////////////////////////////////////

    System* System::Create()
    {
        System* lResult;

        try
        {
            lResult = new System_Impl();
        }
        catch (...)
        {
            TRACE_ERROR(stderr, "System::Create - Exception");
            lResult = NULL;
        }

        return lResult;
    }

    EthCAN_Result System::GetVersion(uint8_t aOut[EthCAN_VERSION_SIZE_byte])
    {
        if (NULL == aOut) { return EthCAN_ERROR_OUTPUT_BUFFER; }

        aOut[0] = VERSION_MAJOR;
        aOut[1] = VERSION_MINOR;
        aOut[2] = VERSION_BUILD;
        aOut[3] = VERSION_COMPATIBILITY;

        return EthCAN_OK;
    }

    // Protected
    /////////////////////////////////////////////////////////////////////////

    System::System() : Object(1)
    {
    }

    // ===== Object =========================================================

    System::~System()
    {
    }

}
