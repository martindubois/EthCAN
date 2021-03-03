
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/CAN.h

#pragma once

// ====== Firmware1 =========================================================
extern "C"
{
    #include "Includes/EthCAN_Result.h"
    #include "Includes/EthCAN_Types.h"
}

#include "Common/InternalProtocol.h"

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void CAN_Begin();
extern void CAN_Loop();

extern EthCAN_Result CAN_Config_Reset();
extern EthCAN_Result CAN_Config_Set(const IntPro_Config_Set & aConfig);
extern EthCAN_Result CAN_Send(const EthCAN_Frame & aFrame);
