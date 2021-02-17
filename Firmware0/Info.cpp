
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Info.h

#include <Arduino.h>

#include <ETH.h>

#include "Component.h"

#include "Common/Version.h"

#include "Config.h"

#include "Info.h"

// Global variables
/////////////////////////////////////////////////////////////////////////////

EthCAN_Info gInfo;

// Functions
/////////////////////////////////////////////////////////////////////////////

void Info_Init()
{
    MSG_DEBUG("Info_Init()");

    esp_eth_get_mac(gInfo.mEth_Addr);

    gInfo.mFirmware0_Major  = VERSION_MAJOR;
    gInfo.mFirmware0_Minor  = VERSION_MINOR;
    gInfo.mFirmware0_Build  = VERSION_BUILD;
    gInfo.mFirmware0_Compat = VERSION_COMPATIBILITY;

    strcpy(gInfo.mName, gConfig.mName);
}
