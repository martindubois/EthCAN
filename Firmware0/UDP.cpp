
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/UDP.cpp

// CODE REVIEW 2021-03-24 KMS - Martin Dubois, P.Eng.

#include <Arduino.h>

#include <WiFi.h>

#include "Component.h"

// ===== Firmware0 ==========================================================
#include "CAN.h"
#include "Config.h"
#include "Header.h"
#include "Info.h"

#include "UDP.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void OnPacket(const void * aPacket, unsigned int aSize_byte);

static void OnConfigErase(const EthCAN_Header * aIn);
static void OnConfigGet  (const EthCAN_Header * aIn);
static void OnConfigReset(const EthCAN_Header * aIn);
static void OnConfigSet  (const EthCAN_Header * aIn);
static void OnConfigStore(const EthCAN_Header * aIn);
static void OnDoNothing  (const EthCAN_Header * aIn);
static void OnInfoGet    (const EthCAN_Header * aIn);
static void OnReset      (const EthCAN_Header * aIn);
static void OnSend       (const EthCAN_Header * aIn);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static WiFiUDP sUDP;

// Functions
/////////////////////////////////////////////////////////////////////////////

void UDP_Loop()
{
    sUDP.parsePacket();

    uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];

    unsigned int lSize_byte = sUDP.read(lBuffer, sizeof(lBuffer));
    if (0 < lSize_byte)
    {
        if (sizeof(lBuffer) >= lSize_byte)
        {
            OnPacket(lBuffer, lSize_byte);
        }
        else
        {
            MSG_ERROR("UDP_Loop - UDP packet larger than the buffer", "");
        }
    }
}

void UDP_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame, uint32_t aIPv4, uint16_t aPort)
{
    sUDP.beginPacket(aIPv4, aPort);
    {
        sUDP.write(reinterpret_cast<const uint8_t *>(&aHeader), sizeof(aHeader));
        sUDP.write(reinterpret_cast<const uint8_t *>(&aFrame ), sizeof(aFrame ));
    }
    sUDP.endPacket();
}

void UDP_Setup()
{
    sUDP.begin(EthCAN_UDP_PORT);
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void OnPacket(const void * aPacket, unsigned int aSize_byte)
{
    const EthCAN_Header * lHeader = reinterpret_cast<const EthCAN_Header *>(aPacket);
    if (Header_Validate(*lHeader, aSize_byte))
    {
        Info_Count_Request(lHeader->mCode, lHeader->mId);

        switch (lHeader->mCode)
        {
        case EthCAN_REQUEST_CONFIG_ERASE: OnConfigErase(lHeader); break;
        case EthCAN_REQUEST_CONFIG_GET  : OnConfigGet  (lHeader); break;
        case EthCAN_REQUEST_CONFIG_RESET: OnConfigReset(lHeader); break;
        case EthCAN_REQUEST_CONFIG_SET  : OnConfigSet  (lHeader); break;
        case EthCAN_REQUEST_CONFIG_STORE: OnConfigStore(lHeader); break;
        case EthCAN_REQUEST_DO_NOTHING  : OnDoNothing  (lHeader); break;
        case EthCAN_REQUEST_INFO_GET    : OnInfoGet    (lHeader); break;
        case EthCAN_REQUEST_RESET       : OnReset      (lHeader); break;
        case EthCAN_REQUEST_SEND        : OnSend       (lHeader); break;

        default: MSG_ERROR("OnPacket - Invalid request code : ", lHeader->mCode);
        }
    }
}

#define BEGIN_UDP                                             \
    if (0 == (aIn->mFlags & EthCAN_FLAG_NO_RESPONSE))         \
    {                                                         \
        EthCAN_Header lHeader;                                \
        Header_Init(&lHeader, aIn);                           \
        sUDP.beginPacket(sUDP.remoteIP(), sUDP.remotePort()); \

#define END_UDP           \
        sUDP.endPacket(); \
    }

void OnConfigErase(const EthCAN_Header * aIn)
{
    Config_Erase();

    BEGIN_UDP
    {
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP
}

void OnConfigGet(const EthCAN_Header * aIn)
{
    BEGIN_UDP
    {
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sUDP.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_UDP
}

void OnConfigReset(const EthCAN_Header * aIn)
{
    uint8_t lFlags = Config_Reset();

    BEGIN_UDP
    {
        lHeader.mFlags = lFlags;

        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP
}

void OnConfigSet(const EthCAN_Header * aIn)
{
    uint8_t lFlags;

    EthCAN_Result lResult = Config_Set(aIn, &lFlags);

    BEGIN_UDP
    {
        lHeader.mFlags = lFlags;
        lHeader.mResult = static_cast<uint16_t>(lResult);
  
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sUDP.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_UDP
}

void OnConfigStore(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = Config_Store(aIn);

    BEGIN_UDP
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);  
    
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP
}

void OnDoNothing(const EthCAN_Header * aIn)
{
    BEGIN_UDP
    {
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP
}

void OnInfoGet(const EthCAN_Header * aIn)
{
    BEGIN_UDP
    {
        lHeader.mDataSize_byte  = sizeof(EthCAN_Info);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(EthCAN_Info);

        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sUDP.write(Info_Get(), sizeof(EthCAN_Info));
    }
    END_UDP
}

void OnReset(const EthCAN_Header * aIn)
{
    BEGIN_UDP
    {
        lHeader.mFlags |= EthCAN_FLAG_BUSY;

        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP

    ESP.restart();
}

void OnSend(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = CAN_Send(aIn);

    BEGIN_UDP
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);

        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP
}
