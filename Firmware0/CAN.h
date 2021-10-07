
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware0/CAN.h

#pragma once

// ===== Firmware0 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
    #include "Includes/EthCAN_Result.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void CAN_Config();
extern void CAN_Loop();
extern void CAN_Reset();
extern void CAN_Setup();

extern void CAN_GetInfo(EthCAN_Info * aInfo);

extern EthCAN_Result CAN_Send(const EthCAN_Header * aIn);
