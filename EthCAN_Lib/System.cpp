
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/System.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>

#include <EthCan/System.h>

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
            lResult = NULL;
        }

        return lResult;
    }

    EthCAN_Result System::GetVersion(uint8_t aOut[4])
    {
        if (NULL == aOut) { return EthCAN_ERROR_OUTPUT_BUFFER; }

        aOut[0] = VERSION_MAJOR;
        aOut[1] = VERSION_MINOR;
        aOut[2] = VERSION_BUILD;
        aOut[3] = VERSION_COMPATIBILITY;

        return EthCAN_OK;
    }

    bool System::IsResultOK(EthCAN_Result aIn)
    {
        return (EthCAN_OK_QTY > aIn);
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
