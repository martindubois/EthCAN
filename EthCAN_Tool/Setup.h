
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Tool/Setup.h

#pragma once

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Result.h>
}

#include <EthCAN/Device.h>
#include <EthCAN/System.h>

// Functions
/////////////////////////////////////////////////////////////////////////////

extern EthCAN_Result Setup_AccessPoint(EthCAN::Device* aDevice, uint32_t aAddress, uint32_t aNetMask, const char* aSSID, const char* aPassword);
extern EthCAN_Result Setup_Bridge     (EthCAN::System* aSystem, unsigned int aIndex[2]);
extern EthCAN_Result Setup_IPv4       (EthCAN::Device* aDevice, uint32_t aAddress, uint32_t aGateway, uint32_t aNetMask);
extern EthCAN_Result Setup_Link       (EthCAN::System* aSystem, unsigned int aIndex[2], uint32_t aAddress, uint32_t aNetMask, const char* aSSID, const char* aPassword);
extern EthCAN_Result Setup_Sniffer    (EthCAN::Device* aDevice);
extern EthCAN_Result Setup_WiFi       (EthCAN::Device* aDevice, const char* aSSID, const char* aPassword);
extern EthCAN_Result Setup_Wireshark  (EthCAN::Device* aDevice);
