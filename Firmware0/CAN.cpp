
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/CAN.cpp

#include <Arduino.h>

#include "Component.h"

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
    MSG_DEBUG("CAN_Setup()");

    Serial2.begin(115200, SERIAL_8N1, 36, 4, false);
}

EthCAN_Result CAN_Send(const EthCAN_Header * aIn)
{
    MSG_DEBUG("CAN_Send(  )");

    if (sizeof(EthCAN_Frame) > aIn->mDataSize_byte)
    {
        MSG_ERROR("CAN_Send - EthCAN_INVALID_DATA_SIZE");
        return EthCAN_ERROR_INVALID_DATA_SIZE;
    }

    // TODO Firmware.CAN.Send
  
    return EthCAN_OK;
}
