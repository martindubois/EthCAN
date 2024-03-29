
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Info.h

#pragma once

// ===== Firmware0 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Result.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern EthCAN_Result Info_Count_Error(unsigned int aLine, EthCAN_Result aResult);

extern void Info_Count_Events();

extern void Info_Count_Fx_Frame(unsigned int aSize_byte);
extern void Info_Count_Rx_Frame(unsigned int aSize_byte, uint32_t aId);
extern void Info_Count_Tx_Frame(unsigned int aSize_byte);

extern void Info_Count_Request(uint8_t aCode, uint16_t aId);

extern const uint8_t * Info_Get();

extern uint16_t Info_Get_MessageId();

extern void Info_Init(const char * aName);

extern void Info_Loop();

extern void Info_Set_EthAddress(const uint8_t * aIn);
extern void Info_Set_IPv4(uint32_t aAddress, uint32_t aGateway, uint32_t aNetMask);
extern void Info_Set_Name(const char * aName);
