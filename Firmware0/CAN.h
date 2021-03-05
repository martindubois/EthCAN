
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware0/CAN.h

#pragma once

extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
    #include "Includes/EthCAN_Result.h"
    #include "Includes/EthCAN_Types.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void CAN_Config();
extern void CAN_Loop();
extern void CAN_Setup();

extern EthCAN_Result CAN_GetInfo(EthCAN_Info * aInfo);
extern EthCAN_Result CAN_Send(const EthCAN_Header * aIn);
