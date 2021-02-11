
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

    const char* System::GetResultName(EthCAN_Result aIn)
    {
        switch (aIn)
        {
        case EthCAN_OK: return "EthCAN_OK";

        case EthCAN_ERROR                       : return "EthCAN_ERROR";
        case EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER: return "EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER";
        case EthCAN_ERROR_EXCEPTION             : return "EthCAN_ERROR_EXCEPTION";
        case EthCAN_ERROR_INVALID_BUFFER        : return "EthCAN_ERROR_INVALID_BUFFER";
        case EthCAN_ERROR_INVALID_CAN_RATE      : return "EthCAN_ERROR_INVALID_CAN_RATE";
        case EthCAN_ERROR_INVALID_DATA_SIZE     : return "EthCAN_ERROR_INVALID_DATA_SIZE";
        case EthCAN_ERROR_INVALID_FUNCTION      : return "EthCAN_ERROR_INVALID_FUNCTION";
        case EthCAN_ERROR_INVALID_INPUT_BUFFER  : return "EthCAN_ERROR_INVALID_INPUT_BUFFER";
        case EthCAN_ERROR_INVALID_IPv4_ADDRESS  : return "EthCAN_ERROR_INVALID_IPv4_ADDRESS";
        case EthCAN_ERROR_INVALID_IPv4_MASK     : return "EthCAN_ERROR_INVALID_IPv4_MASK";
        case EthCAN_ERROR_INVALID_OUTPUT_BUFFER : return "EthCAN_ERROR_INVALID_OUTPUT_BUFFER";
        case EthCAN_ERROR_INVALID_RESPONSE_SIZE : return "EthCAN_ERROR_INVALID_RESPONSE_SIZE";
        case EthCAN_ERROR_INVALID_SOCKET        : return "EthCAN_ERROR_INVALID_SOCKET";
        case EthCAN_ERROR_NETWORK               : return "EthCAN_ERROR_NETWORL";
        case EthCAN_ERROR_NOT_CONNECTED         : return "EthCAN_ERROR_NOT_CONNECTED";
        case EthCAN_ERROR_NOT_CONNECTED_ETH     : return "EthCAN_ERROR_NOT_CONNECTED_ETH";
        case EthCAN_ERROR_NOT_RUNNING           : return "EthCAN_ERROR_NOT_RUNNING";
        case EthCAN_ERROR_RUNNING               : return "EthCAN_ERROR_RUNNING";
        case EthCAN_ERROR_SEMAPHORE             : return "EthCAN_ERROR_SEMAPHORE";
        case EthCAN_ERROR_SERIAL                : return "EthCAN_ERROR_SERIAL";
        case EthCAN_ERROR_SERIAL_CONFIG         : return "EthCAN_ERROR_SERIAL_CONFIG";
        case EthCAN_ERROR_SERIAL_OPEN           : return "EthCAN_ERROR_SERIAL_OPEN";
        case EthCAN_ERROR_SERIAL_RECEIVE        : return "EthCAN_ERROR_SERIAL_RECEIVE";
        case EthCAN_ERROR_SERIAL_SEND           : return "EthCAN_ERROR_SERIAL_SEND";
        case EthCAN_ERROR_SOCKET                : return "EthCAN_ERROR_SOCKET";
        case EthCAN_ERROR_SOCKET_BIND           : return "EthCAN_ERROR_SOCKET_BIND";
        case EthCAN_ERROR_SOCKET_NAME           : return "EthCAN_ERROR_SOCKET_NAME";
        case EthCAN_ERROR_SOCKET_OPTION         : return "EthCAN_ERROR_SOCKET_OPTION";
        case EthCAN_ERROR_SOCKET_RECEIVE        : return "EthCAN_ERROR_SOCKET_RECEIVE";
        case EthCAN_ERROR_SOCKET_SEND           : return "EthCAN_ERROR_SOCKET_SEND";
        case EthCAN_ERROR_THREAD                : return "EthCAN_ERROR_THREAD";
        case EthCAN_ERROR_TIMEOUT               : return "EthCAN_ERROR_TIMEOUT";
        case EthCAN_ERROR_UNEXPECTED_DATA       : return "EthCAN_ERROR_UNEXPECTED_DATA";

        case EthCAN_RESULT_INVALID: return "EthCAN_RESULT_INVALID";
        case EthCAN_RESULT_REQUEST: return "EthCAN_RESULT_REQUEST";
        }

        return NULL;
    }

    EthCAN_Result System::GetVersion(uint8_t aOut[4])
    {
        if (NULL == aOut)
        {
            return EthCAN_ERROR_INVALID_OUTPUT_BUFFER;
        }

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
