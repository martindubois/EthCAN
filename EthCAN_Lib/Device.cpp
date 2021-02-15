
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Device.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>

extern "C"
{
    #include <EthCAN_Types.h>
}

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Display(FILE* aOut, const uint8_t aEth[6]);
static void Display(FILE* aOut, uint32_t aIPv4);

namespace EthCAN
{

    // Public
    /////////////////////////////////////////////////////////////////////////

    void Device::Display(FILE* aOut, const EthCAN_Config& aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == &aIn)
        {
            fprintf(lOut, "Invalid reference\n");
            return;
        }

        unsigned int i;

        fprintf(lOut, "    CAN Filters  :");
        for (i = 0; i < 6; i++)
        {
            fprintf(lOut, " 0x%x", aIn.mCAN_Filters[i]);
        }
        fprintf(lOut, "\n");

        fprintf(lOut, "    CAN Masks    :");
        for (i = 0; i < 2; i++)
        {
            fprintf(lOut, " 0x%x", aIn.mCAN_Masks[i]);
        }
        fprintf(lOut, "\n");

        fprintf(lOut, "    CAN Rate     : "); Display(lOut, static_cast<EthCAN_Rate>(aIn.mCAN_Rate));

        // TODO Device.Display
        //      Display flags by name
        fprintf(lOut, "    Flags        : 0x%02x\n", aIn.mFlags);
        fprintf(lOut, "    IPv4 Address : "); ::Display(lOut, aIn.mIPv4_Addr);
        fprintf(lOut, "    IPv4 Mask    : "); ::Display(lOut, aIn.mIPv4_Mask);
        fprintf(lOut, "    Name         : %s\n", aIn.mName);
        fprintf(lOut, "    Server IPv4  : "); ::Display(lOut, aIn.mServer_IPv4);
        fprintf(lOut, "    Server Port  : %u\n", aIn.mServer_Port);
        fprintf(lOut, "    WiFI Name    : %s\n", aIn.mWiFi_Name);
        fprintf(lOut, "    WiFi Password: %s\n", aIn.mWifi_Password);
    }

    void Device::Display(FILE* aOut, const EthCAN_Frame& aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == &aIn)
        {
            fprintf(lOut, "Invalid reference\n");
            return;
        }

        fprintf(lOut, "%s %08x", 0 != (aIn.mFlags & EthCAN_FLAG_EXTENDED) ? "Ext." : "Std ", aIn.mDestId);

        for (unsigned int i = 0; i < aIn.mDataSize_byte; i++)
        {
            fprintf(lOut, " %02x", aIn.mData[i]);
        }

        fprintf(lOut, "\n");
    }

    void Device::Display(FILE* aOut, const EthCAN_Info& aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        if (NULL == &aIn)
        {
            fprintf(lOut, "Invalid reference\n");
            return;
        }

        fprintf(lOut, "        Ethernet address : "); ::Display(lOut, aIn.mEth_Addr);

        fprintf(lOut,
            "        Firmware 0 : %u.%u.%u.%u\n"
            "        Firmware 1 : %u.%u.%u.%u\n"
            "        Hardware   : %u.%u.%u.%u\n",
            aIn.mFirmware0_Major,
            aIn.mFirmware0_Minor,
            aIn.mFirmware0_Build,
            aIn.mFirmware0_Compat,
            aIn.mFirmware1_Major,
            aIn.mFirmware1_Minor,
            aIn.mFirmware1_Build,
            aIn.mFirmware1_Compat,
            aIn.mHardware_Major,
            aIn.mHardware_Minor,
            aIn.mHardware_Rev,
            aIn.mHardware_Compat);

        fprintf(lOut, "        IPv4 address : "); ::Display(lOut, aIn.mIPv4_Addr);
        fprintf(lOut, "        IPv4 mask    : "); ::Display(lOut, aIn.mIPv4_Mask);

        fprintf(lOut,
            "        Last message id     : %u\n"
            "        Name                : %s\n"
            "        Last request id UDP : %u\n"
            "        Last request id USB : %u\n",
            aIn.mMessageId,
            aIn.mName,
            aIn.mRequestId_UDP,
            aIn.mRequestId_USB);
    }

    void Device::Display(FILE* aOut, EthCAN_Rate aIn)
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        const char* lName = GetRateName(aIn);
        if (NULL == lName)
        {
            fprintf(lOut, "Invalid rate (%u)\n", aIn);
        }
        else
        {
            fprintf(lOut, "%s\n", lName);
        }
    }

    const char* Device::GetRateName(EthCAN_Rate aIn)
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

    // Internal
    /////////////////////////////////////////////////////////////////////////

    Device::Device() : Object(2)
    {
    }

    // Private
    /////////////////////////////////////////////////////////////////////////

    Device::~Device()
    {
    }

}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Display(FILE* aOut, const uint8_t aEth[6])
{
    assert(NULL != aOut);
    assert(NULL != aEth);

    for (unsigned int i = 0; i < 6; i++)
    {
        fprintf(aOut, "%02x", aEth[i]);

        if (i < 5)
        {
            fprintf(aOut, ":");
        }
    }

    fprintf(aOut, "\n");
}

void Display(FILE* aOut, uint32_t aIPv4)
{
    assert(NULL != aOut);

    const uint8_t* lIPv4 = reinterpret_cast<uint8_t *>(&aIPv4);

    for (unsigned int i = 0; i < 4; i++)
    {
        fprintf(aOut, "%u", lIPv4[i]);

        if (i < 3)
        {
            fprintf(aOut, ".");
        }
    }

    fprintf(aOut, "\n");
}
