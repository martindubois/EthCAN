
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/CAN.h

#pragma once

extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
    #include "Includes/EthCAN_Result.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void CAN_Loop();
extern void CAN_Setup();

extern EthCAN_Result CAN_Send(const EthCAN_Header * aIn);
