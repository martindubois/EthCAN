
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Info.h

#pragma once

extern "C"
{
    #include "Includes/EthCAN_Types.h"
}

// Global variable
/////////////////////////////////////////////////////////////////////////////

extern EthCAN_Info gInfo;

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void Info_Init();
