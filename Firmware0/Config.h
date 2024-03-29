
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Config.h

#pragma once

// ===== Firmware0 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
    #include "Includes/EthCAN_Result.h"
}

// Global variable
/////////////////////////////////////////////////////////////////////////////

extern EthCAN_Config gConfig;

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void Config_Erase();
extern void Config_Load();
extern void Config_Loop();
extern void Config_OnFrame(const EthCAN_Frame & aFrame);

extern uint8_t Config_Reset();

extern EthCAN_Result Config_Set  (const EthCAN_Header * aIn, uint8_t * aFlags);
extern EthCAN_Result Config_Store(const EthCAN_Header * aIn);
