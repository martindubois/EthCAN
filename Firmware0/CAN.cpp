
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/CAN.cpp

#include <Arduino.h>

extern "C"
{
    #include "Includes/EthCAN_Types.h"
}

#include "CAN.h"

// Functions
/////////////////////////////////////////////////////////////////////////////

void CAN_Loop()
{
    // TODO
}

void CAN_Setup()
{
    Serial2.begin(115200, SERIAL_8N1, 36, 4);
}

EthCAN_Result CAN_Send(const EthCAN_Header * aIn)
{
    if (sizeof(EthCAN_Frame) > aIn->mDataSize_byte)
    {
        return EthCAN_ERROR_INVALID_DATA_SIZE;
    }

    // TODO Firmware.CAN.Send
  
    return EthCAN_OK;
}
