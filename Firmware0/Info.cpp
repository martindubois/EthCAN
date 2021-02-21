
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Info.h

#include <Arduino.h>

#include <ETH.h>

#include <esp_wifi.h>

extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
}

#include "Component.h"

#include "Common/Version.h"

#include "Info.h"

// Static variables
/////////////////////////////////////////////////////////////////////////////

static EthCAN_Info sInfo;

// Functions
/////////////////////////////////////////////////////////////////////////////

EthCAN_Result Info_Count_Error(unsigned int aLine, EthCAN_Result aResult)
{
    MSG_ERROR("Line  : ", aLine);
    MSG_ERROR("Error : ", aResult);

    sInfo.mCounter_Errors++;
    sInfo.mLast_Error_Code = aResult;
    sInfo.mLast_Error_Line = aLine;
    return aResult;
}

void Info_Count_Events()
{
    sInfo.mCounter_Events++;
}

void Info_Count_Fx_Frame(unsigned int aSize_byte)
{
    sInfo.mCounter_Fx_byte += aSize_byte;
    sInfo.mCounter_Fx_frame ++;
}

void Info_Count_Rx_Frame(unsigned int aSize_byte, uint32_t aId)
{
    sInfo.mCounter_Rx_byte += aSize_byte;
    sInfo.mCounter_Rx_frame ++;
    sInfo.mLast_Rx_Id = aId;
}

void Info_Count_Tx_Frame(unsigned int aSize_byte)
{
    sInfo.mCounter_Tx_byte += aSize_byte;
    sInfo.mCounter_Tx_frame ++;
}

void Info_Count_Request(uint8_t aCode, uint16_t aId)
{
    sInfo.mCounter_Requests ++;
    sInfo.mLast_Request_Code = aCode;
    sInfo.mLast_Request_Id = aId;
}

const uint8_t * Info_Get()
{
    return reinterpret_cast<uint8_t *>(&sInfo);
}

uint16_t Info_Get_MessageId()
{
    sInfo.mMessageId++;

    return sInfo.mMessageId;
}

void Info_Init(const char * aName)
{
    // MSG_DEBUG("Info_Init(  )");

    esp_eth_get_mac(sInfo.mEth_Address);

    // TODO Firmware0.Info
    //      Add the WiFi mac address

    sInfo.mFirmware0_Version[0] = VERSION_MAJOR;
    sInfo.mFirmware0_Version[1] = VERSION_MINOR;
    sInfo.mFirmware0_Version[2] = VERSION_BUILD;
    sInfo.mFirmware0_Version[3] = VERSION_COMPATIBILITY;

    strcpy(sInfo.mName, aName);

    sInfo.mResult_CAN = EthCAN_ERROR_PENDING;

    esp_wifi_get_mac(WIFI_IF_STA, sInfo.mWiFi_Address);

    sInfo.mLast_Error_Code   = EthCAN_RESULT_NO_ERROR;
    sInfo.mLast_Request_Code = EthCAN_REQUEST_INVALID;
}

void Info_Set_IPv4(uint32_t aAddress, uint32_t aGateway, uint32_t aNetMask)
{
    MSG_INFO("IPv4 Address : ", static_cast<IPAddress>(aAddress));
    MSG_INFO("IPv4 Gateway : ", static_cast<IPAddress>(aGateway));
    MSG_INFO("IPv4 NetMask : ", static_cast<IPAddress>(aNetMask));

    sInfo.mIPv4_Address = aAddress;
    sInfo.mIPv4_Gateway = aGateway;
    sInfo.mIPv4_NetMask = aNetMask;
}

void Info_Set_Name(const char * aName)
{
    strcpy(sInfo.mName, aName);
}

void Info_Set_Result_CAN(EthCAN_Result aResult)
{
    sInfo.mResult_CAN = aResult;
    if (EthCAN_OK != aResult)
    {
        MSG_ERROR("CAN Error : ", aResult);
    }
}
