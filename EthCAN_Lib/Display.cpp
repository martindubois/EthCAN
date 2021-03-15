
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Display.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Display.h>

namespace EthCAN
{

    // Functions
    /////////////////////////////////////////////////////////////////////////

    void Display(FILE* aOut, const EthCAN_Config& aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == &aIn)
        {
            fprintf(lOut, "Invalid reference\n");
            return;
        }

        unsigned int i;

        fprintf(lOut, "    CAN Filters    :");
        for (i = 0; i < 6; i++)
        {
            fprintf(lOut, " 0x%x", aIn.mCAN_Filters[i]);
        }
        fprintf(lOut, "\n");

        // TODO Device.Display
        //      Display CAN flags by name
        fprintf(lOut, "    CAN Flags      : 0x%02x\n", aIn.mCAN_Flags);

        fprintf(lOut, "    CAN Masks      :");
        for (i = 0; i < 2; i++)
        {
            fprintf(lOut, " 0x%x", aIn.mCAN_Masks[i]);
        }
        fprintf(lOut, "\n");

        fprintf(lOut, "    CAN Rate       : "); Display(lOut, static_cast<EthCAN_Rate>(aIn.mCAN_Rate));

        // TODO Device.Display
        //      Display server and WiFi flags by name
        if (0 != aIn.mIPv4_Address    ) { fprintf(lOut, "    IPv4 Address   : "); Display_IPv4Address(lOut, aIn.mIPv4_Address); }
        if (0 != aIn.mIPv4_Gateway    ) { fprintf(lOut, "    IPv4 Gateway   : "); Display_IPv4Address(lOut, aIn.mIPv4_Gateway); }
        if (0 != aIn.mIPv4_NetMask    ) { fprintf(lOut, "    IPv4 Net. Mask : "); Display_IPv4Address(lOut, aIn.mIPv4_NetMask); }
        if (0 != aIn.mName[0]         ) { fprintf(lOut, "    Name           : %s\n", aIn.mName); }
        if (0 != aIn.mServer_Flags    ) { fprintf(lOut, "    Server Flags   : 0x%02x\n", aIn.mServer_Flags); }
        if (0 != aIn.mServer_IPv4     ) { fprintf(lOut, "    Server IPv4    : "); Display_IPv4Address(lOut, aIn.mServer_IPv4); }
        if (0 != aIn.mServer_Port     ) { fprintf(lOut, "    Server Port    : %u\n", aIn.mServer_Port); }
        if (0 != aIn.mWiFi_Flags      ) { fprintf(lOut, "    WiFi Flags     : 0x%02x\n", aIn.mWiFi_Flags); }
        if (0 != aIn.mWiFi_Name[0]    ) { fprintf(lOut, "    WiFI Name      : %s\n", aIn.mWiFi_Name); }
        if (0 != aIn.mWiFi_Password[0]) { fprintf(lOut, "    WiFi Password  : %s\n", aIn.mWiFi_Password); }
    }

    void Display(FILE* aOut, const EthCAN_Frame& aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == &aIn)
        {
            fprintf(lOut, "Invalid reference\n");
            return;
        }

        unsigned int lDataSize_byte = EthCAN_FRAME_DATA_SIZE(aIn);
        fprintf(lOut, "%u bytes sent to ", lDataSize_byte);

        Display_Id(lOut, aIn.mId);

        for (unsigned int i = 0; i < lDataSize_byte && i < sizeof(aIn.mData); i++)
        {
            fprintf(lOut, " %02x", aIn.mData[i]);
        }

        if (EthCAN_FRAME_RTR(aIn))
        {
            fprintf(lOut, " (RTR)");
        }

        fprintf(lOut, "\n");
    }

    void Display(FILE* aOut, const EthCAN_Info& aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == &aIn)
        {
            fprintf(lOut, "Invalid reference\n");
            return;
        }

        fprintf(lOut, "        Ethernet address : "); Display_EthAddress(lOut, aIn.mEth_Address);
        fprintf(lOut, "        Firmware 0       : "); Display_Version(lOut, aIn.mFirmware0_Version);
        fprintf(lOut, "        Firmware 1       : "); Display_Version(lOut, aIn.mFirmware1_Version);
        fprintf(lOut, "        Hardware         : "); Display_Version(lOut, aIn.mHardware_Version);
        if (0 != aIn.mIPv4_Address) { fprintf(lOut, "        IPv4 Address     : "); Display_IPv4Address(lOut, aIn.mIPv4_Address); }
        if (0 != aIn.mIPv4_Gateway) { fprintf(lOut, "        IPv4 Gateway     : "); Display_IPv4Address(lOut, aIn.mIPv4_Gateway); }
        if (0 != aIn.mIPv4_NetMask) { fprintf(lOut, "        IPv4 Net. Mask   : "); Display_IPv4Address(lOut, aIn.mIPv4_NetMask); }
        fprintf(lOut, "        Last message id  : %u\n", aIn.mMessageId);
        if (0 != aIn.mName[0]) { fprintf(lOut, "        Name             : %s\n", aIn.mName); }
        fprintf(lOut, "        Result CAN       : "); Display(lOut, static_cast<EthCAN_Result>(aIn.mResult_CAN));

        if (0 != aIn.mCAN_Errors)
        {
            fprintf(lOut, "        CAN Errors       :");
            if (0 != (aIn.mCAN_Errors & 0x02)) { fprintf(lOut, " RxWarning" ); }
            if (0 != (aIn.mCAN_Errors & 0x04)) { fprintf(lOut, " TxWarning" ); }
            if (0 != (aIn.mCAN_Errors & 0x08)) { fprintf(lOut, " RxError"   ); }
            if (0 != (aIn.mCAN_Errors & 0x10)) { fprintf(lOut, " TxError"   ); }
            if (0 != (aIn.mCAN_Errors & 0x20)) { fprintf(lOut, " TxBusOff"  ); }
            if (0 != (aIn.mCAN_Errors & 0xc0)) { fprintf(lOut, " RxOverflow"); }
            fprintf(lOut, "\n");
        }

        fprintf(lOut, "        CAN Result       : "); Display(lOut, static_cast<EthCAN_Result>(aIn.mCAN_Result));

        fprintf(lOut, "        Counters\n");
        if (0 != aIn.mCounter_Errors) { fprintf(lOut, "            Errors   : %u\n", aIn.mCounter_Errors); }
        if (0 != aIn.mCounter_Events) { fprintf(lOut, "            Events   : %u\n", aIn.mCounter_Events); }
        fprintf(lOut, "            Fx       : %u bytes, %u frames\n", aIn.mCounter_Fx_byte, aIn.mCounter_Fx_frame);
        if (0 != aIn.mCounter_Requests) { fprintf(lOut, "            Requests : %u\n", aIn.mCounter_Requests); }
        fprintf(lOut, "            Rx       : %u bytes, %u frames, %u errors\n", aIn.mCounter_Rx_byte, aIn.mCounter_Rx_frame, aIn.mCounter_RxErrors);
        fprintf(lOut, "            Tx       : %u bytes, %u frames, %u errors\n", aIn.mCounter_Tx_byte, aIn.mCounter_Tx_frame, aIn.mCounter_TxErrors);

        fprintf(lOut, "        Last\n");
        fprintf(lOut, "            Error Code   : "); Display(lOut, static_cast<EthCAN_Result>(aIn.mLast_Error_Code));
        fprintf(lOut, "            Error Line   : %u\n", aIn.mLast_Error_Line);
        fprintf(lOut, "            Request Code : "); Display(lOut, static_cast<EthCAN_RequestCode>(aIn.mLast_Request_Code));
        fprintf(lOut, "            Request Id   : %u\n", aIn.mLast_Request_Id);
        fprintf(lOut, "            Rx Id        : 0x%x\n", aIn.mLast_Rx_Id);
    }

    void Display(FILE* aOut, EthCAN_Rate aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        const char* lName = GetName(aIn);
        if (NULL == lName)
        {
            fprintf(lOut, "Invalid rate (%u)\n", aIn);
        }
        else
        {
            fprintf(lOut, "%s\n", lName);
        }
    }

    void Display(FILE* aOut, EthCAN_RequestCode aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        const char* lIn;

        switch (aIn)
        {
        case EthCAN_REQUEST_CONFIG_ERASE: lIn = "EthCAN_REQUEST_CONFIG_ERASE"; break;
        case EthCAN_REQUEST_CONFIG_GET  : lIn = "EthCAN_REQUEST_CONFIG_GET"  ; break;
        case EthCAN_REQUEST_CONFIG_RESET: lIn = "EthCAN_REQUEST_CONFIG_RESET"; break;
        case EthCAN_REQUEST_CONFIG_SET  : lIn = "EthCAN_REQUEST_CONFIG_SET"  ; break;
        case EthCAN_REQUEST_CONFIG_STORE: lIn = "EthCAN_REQUEST_CONFIG_STORE"; break;
        case EthCAN_REQUEST_DO_NOTHING  : lIn = "EthCAN_REQUEST_DO_NOTHING"  ; break;
        case EthCAN_REQUEST_INFO_GET    : lIn = "EthCAN_REQUEST_INFO_GET"    ; break;
        case EthCAN_REQUEST_RESET       : lIn = "EthCAN_REQUEST_RESET"       ; break;
        case EthCAN_REQUEST_SEND        : lIn = "EthCAN_REQUEST_SEND"        ; break;

        default:
            fprintf(lOut, "Invalid request (%u)\n", aIn);
            return;
        }

        fprintf(lOut, "%s\n", lIn);
    }

    void Display(FILE* aOut, EthCAN_Result aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        const char* lIn = EthCAN::GetName(aIn);
        if (NULL == lIn)
        {
            fprintf(lOut, "Invalid result (%u)\n", aIn);
        }
        else
        {
            fprintf(lOut, "%s\n", lIn);
        }
    }

    void Display_EthAddress(FILE* aOut, const uint8_t aIn[6])
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == aIn)
        {
            fprintf(lOut, "Invalid pointer\n");
            return;
        }

        char lStr[20];

        GetName_EthAddress(lStr, sizeof(lStr), aIn);

        fprintf(lOut, "%s\n", lStr);
    }

    void Display_Id(FILE* aOut, uint32_t aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        fprintf(lOut, "0x%x %s\n", aIn & ~EthCAN_ID_EXTENDED, 0 == (aIn & EthCAN_ID_EXTENDED) ? "Std" : "Ext.");
    }

    void Display_IPv4Address(FILE* aOut, uint32_t aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        char lStr[16];

        GetName_IPv4Address(lStr, sizeof(lStr), reinterpret_cast<uint8_t*>(&aIn));

        fprintf(lOut, "%s\n", lStr);
    }

    void Display_Version(FILE* aOut, const uint8_t aIn[4])
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == aIn)
        {
            fprintf(lOut, "Invalid pointer\n");
            return;
        }

        fprintf(lOut, "%u.%u.%u.%u\n", aIn[0], aIn[1], aIn[2], aIn[3]);
    }

    const char* GetName(EthCAN_Rate aIn)
    {
        const char* lResult = NULL;

        switch (aIn)
        {
        case EthCAN_RATE_100_Kb : lResult = "EthCAN_RATE_100_Kb" ; break;
        case EthCAN_RATE_10_Kb  : lResult = "EthCAN_RATE_10_Kb"  ; break;
        case EthCAN_RATE_125_Kb : lResult = "EthCAN_RATE_125_Kb" ; break;
        case EthCAN_RATE_1_Mb   : lResult = "EthCAN_RATE_1_Mb"   ; break;
        case EthCAN_RATE_200_Kb : lResult = "EthCAN_RATE_200_Kb" ; break;
        case EthCAN_RATE_20_Kb  : lResult = "EthCAN_RATE_20_Kb"  ; break;
        case EthCAN_RATE_250_Kb : lResult = "EthCAN_RATE_250_Kb" ; break;
        case EthCAN_RATE_25_Kb  : lResult = "EthCAN_RATE_25_Kb"  ; break;
        case EthCAN_RATE_31_2_Kb: lResult = "EthCAN_RATE_31_2_Kb"; break;
        case EthCAN_RATE_33_Kb  : lResult = "EthCAN_RATE_33_Kb"  ; break;
        case EthCAN_RATE_40_Kb  : lResult = "EthCAN_RATE_40_Kb"  ; break;
        case EthCAN_RATE_500_Kb : lResult = "EthCAN_RATE_500_Kb" ; break;
        case EthCAN_RATE_50_Kb  : lResult = "EthCAN_RATE_50_Kb"  ; break;
        case EthCAN_RATE_5_Kb   : lResult = "EthCAN_RATE_5_Kb"   ; break;
        case EthCAN_RATE_666_Kb : lResult = "EthCAN_RATE_666_Kb" ; break;
        case EthCAN_RATE_80_Kb  : lResult = "EthCAN_RATE_80_Kb"  ; break;
        case EthCAN_RATE_83_3_Kb: lResult = "EthCAN_RATE_83_3_Kb"; break;
        case EthCAN_RATE_95_Kb  : lResult = "EthCAN_RATE_95_Kb"  ; break;
        }

        return lResult;
    }

    const char* GetName(EthCAN_Result aIn)
    {
        switch (aIn)
        {
        case EthCAN_OK: return "EthCAN_OK";

        case EthCAN_ERROR                        : return "EthCAN_ERROR";
        case EthCAN_ERROR_BUFFER                 : return "EthCAN_ERROR_BUFFER";
        case EthCAN_ERROR_CAN                    : return "EthCAN_ERROR_CAN";
        case EthCAN_ERROR_CAN_RATE               : return "EthCAN_ERROR_CAN_RATE";
        case EthCAN_ERROR_DATA_SIZE              : return "EthCAN_ERROR_DATA_SIZE";
        case EthCAN_ERROR_DATA_UNEXPECTED        : return "EthCAN_ERROR_UNEXPECTED_DATA";
        case EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER : return "EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER";
        case EthCAN_ERROR_EXCEPTION              : return "EthCAN_ERROR_EXCEPTION";
        case EthCAN_ERROR_FILE_NAME              : return "EthCAN_ERROR_FILE_NAME";
        case EthCAN_ERROR_FUNCTION               : return "EthCAN_ERROR_FUNCTION";
        case EthCAN_ERROR_INPUT_BUFFER           : return "EthCAN_ERROR_INPUT_BUFFER";
        case EthCAN_ERROR_INPUT_FILE_CLOSE       : return "EthCAN_ERROR_INPUT_FILE_CLOSE";
        case EthCAN_ERROR_INPUT_FILE_OPEN        : return "EthCAN_ERROR_INPUT_FILE_OPEN";
        case EthCAN_ERROR_INPUT_STREAM           : return "EthCAN_ERROR_INPUT_STREAM";
        case EthCAN_ERROR_INPUT_STREAM_READ      : return "EthCAN_ERROR_INPUT_STREAM_READ";
        case EthCAN_ERROR_IPv4_ADDRESS           : return "EthCAN_ERROR_IPv4_ADDRESS";
        case EthCAN_ERROR_IPv4_MASK              : return "EthCAN_ERROR_IPv4_MASK";
        case EthCAN_ERROR_NETWORK                : return "EthCAN_ERROR_NETWORL";
        case EthCAN_ERROR_NOT_CONNECTED          : return "EthCAN_ERROR_NOT_CONNECTED";
        case EthCAN_ERROR_NOT_CONNECTED_ETH      : return "EthCAN_ERROR_NOT_CONNECTED_ETH";
        case EthCAN_ERROR_NOT_RUNNING            : return "EthCAN_ERROR_NOT_RUNNING";
        case EthCAN_ERROR_OUTPUT_BUFFER          : return "EthCAN_ERROR_OUTPUT_BUFFER";
        case EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL: return "EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL";
        case EthCAN_ERROR_OUTPUT_FILE_CLOSE      : return "EthCAN_ERROR_OUTPUT_FILE_CLOSE";
        case EthCAN_ERROR_OUTPUT_FILE_OPEN       : return "EthCAN_ERROR_OUTPUT_FILE_OPEN";
        case EthCAN_ERROR_OUTPUT_STREAM          : return "EthCAN_ERROR_OUTPUT_STREAM";
        case EthCAN_ERROR_OUTPUT_STREAM_WRITE    : return "EthCAN_ERROR_OUTPUT_STREAM_WRITE";
        case EthCAN_ERROR_PENDING                : return "EthCAN_ERROR_PENDING";
        case EthCAN_ERROR_REFERENCE              : return "EthCAN_ERROR_REFERENCE";
        case EthCAN_ERROR_RESPONSE_SIZE          : return "EthCAN_ERROR_RESPONSE_SIZE";
        case EthCAN_ERROR_RUNNING                : return "EthCAN_ERROR_RUNNING";
        case EthCAN_ERROR_SEMAPHORE              : return "EthCAN_ERROR_SEMAPHORE";
        case EthCAN_ERROR_SERIAL                 : return "EthCAN_ERROR_SERIAL";
        case EthCAN_ERROR_SERIAL_CONFIG          : return "EthCAN_ERROR_SERIAL_CONFIG";
        case EthCAN_ERROR_SERIAL_OPEN            : return "EthCAN_ERROR_SERIAL_OPEN";
        case EthCAN_ERROR_SERIAL_RECEIVE         : return "EthCAN_ERROR_SERIAL_RECEIVE";
        case EthCAN_ERROR_SERIAL_SEND            : return "EthCAN_ERROR_SERIAL_SEND";
        case EthCAN_ERROR_SOCKET                 : return "EthCAN_ERROR_SOCKET";
        case EthCAN_ERROR_SOCKET_BIND            : return "EthCAN_ERROR_SOCKET_BIND";
        case EthCAN_ERROR_SOCKET_NAME            : return "EthCAN_ERROR_SOCKET_NAME";
        case EthCAN_ERROR_SOCKET_OPTION          : return "EthCAN_ERROR_SOCKET_OPTION";
        case EthCAN_ERROR_SOCKET_RECEIVE         : return "EthCAN_ERROR_SOCKET_RECEIVE";
        case EthCAN_ERROR_SOCKET_SEND            : return "EthCAN_ERROR_SOCKET_SEND";
        case EthCAN_ERROR_THREAD                 : return "EthCAN_ERROR_THREAD";
        case EthCAN_ERROR_TIMEOUT                : return "EthCAN_ERROR_TIMEOUT";

        case EthCAN_RESULT_INVALID : return "EthCAN_RESULT_INVALID";
        case EthCAN_RESULT_NO_ERROR: return "EthCAN_RESULT_NO_ERROR";
        case EthCAN_RESULT_REQUEST : return "EthCAN_RESULT_REQUEST";
        }

        return NULL;
    }

    EthCAN_Result GetName_EthAddress(char* aOut, unsigned int aOutSize_byte, const uint8_t aIn[6])
    {
        if (NULL == aOut      ) { return EthCAN_ERROR_OUTPUT_BUFFER  ; }
        if (18 > aOutSize_byte) { return EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL; }
        if (NULL == aIn       ) { return EthCAN_ERROR_INPUT_BUFFER   ; }

        sprintf_s(aOut SIZE_INFO(aOutSize_byte), "%02x:%02x:%02x:%02x:%02x:%02x", aIn[0], aIn[1], aIn[2], aIn[3], aIn[4], aIn[5]);

        return EthCAN_OK;
    }

    EthCAN_Result GetName_IPv4Address(char* aOut, unsigned int aOutSize_byte, const uint8_t aIn[4])
    {
        if (NULL == aOut      ) { return EthCAN_ERROR_OUTPUT_BUFFER  ; }
        if (16 > aOutSize_byte) { return EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL; }
        if (NULL == aIn       ) { return EthCAN_ERROR_INPUT_BUFFER   ; }

        sprintf_s(aOut SIZE_INFO(aOutSize_byte), "%3u.%3u.%3u.%3u", aIn[0], aIn[1], aIn[2], aIn[3]);

        return EthCAN_OK;
    }

}
