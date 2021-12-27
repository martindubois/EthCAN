
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/TCP.cpp

#include <Arduino.h>

#include <WiFi.h>

#include "Component.h"

// ===== Firmware0 ==========================================================
#include "CAN.h"
#include "Config.h"
#include "Header.h"
#include "Info.h"

#include "TCP.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void OnPacket(const void * aPacket, unsigned int aSize_byte, WiFiClient aClient);

static void OnConfigErase(const EthCAN_Header * aIn, WiFiClient aClient);
static void OnConfigGet  (const EthCAN_Header * aIn, WiFiClient aClient);
static void OnConfigReset(const EthCAN_Header * aIn, WiFiClient aClient);
static void OnConfigSet  (const EthCAN_Header * aIn, WiFiClient aClient);
static void OnConfigStore(const EthCAN_Header * aIn, WiFiClient aClient);
static void OnDoNothing  (const EthCAN_Header * aIn, WiFiClient aClient);
static void OnInfoGet    (const EthCAN_Header * aIn, WiFiClient aClient);
static void OnReset      (const EthCAN_Header * aIn, WiFiClient aClient);
static void OnSend       (const EthCAN_Header * aIn, WiFiClient aClient);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static WiFiServer sTCP(EthCAN_TCP_PORT);

// Functions
/////////////////////////////////////////////////////////////////////////////

void TCP_Loop()
{
    WiFiClient lClient = sTCP.available();
    if (lClient && lClient.connected())
    {
        unsigned int lSize_byte = lClient.available();
        if (sizeof(EthCAN_Header) <= lSize_byte)
        {
            uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];

            int lRet_byte = lClient.read(lBuffer, lSize_byte);
            if (lSize_byte == lRet_byte)
            {
                OnPacket(lBuffer, lSize_byte, lClient);
            }
            else
            {
                MSG_ERROR("TCP_Loop - WiFiClient.read( ,  ) failed", lRet_byte);
            }
        }
    }
}

bool TCP_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame)
{
    WiFiClient lClient = sTCP.available();
    if (lClient && lClient.connected())
    {
        lClient.write(reinterpret_cast<const uint8_t *>(&aHeader), sizeof(aHeader));
        lClient.write(reinterpret_cast<const uint8_t *>(&aFrame ), sizeof(aFrame ));
        lClient.flush();
        return true;
    }

    return false;
}

void TCP_Setup()
{
    sTCP.begin();
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void OnPacket(const void * aPacket, unsigned int aSize_byte, WiFiClient aClient)
{
    const EthCAN_Header * lHeader = reinterpret_cast<const EthCAN_Header *>(aPacket);
    if (Header_Validate(*lHeader, aSize_byte))
    {
        Info_Count_Request(lHeader->mCode, lHeader->mId);

        switch (lHeader->mCode)
        {
        case EthCAN_REQUEST_CONFIG_ERASE: OnConfigErase(lHeader, aClient); break;
        case EthCAN_REQUEST_CONFIG_GET  : OnConfigGet  (lHeader, aClient); break;
        case EthCAN_REQUEST_CONFIG_RESET: OnConfigReset(lHeader, aClient); break;
        case EthCAN_REQUEST_CONFIG_SET  : OnConfigSet  (lHeader, aClient); break;
        case EthCAN_REQUEST_CONFIG_STORE: OnConfigStore(lHeader, aClient); break;
        case EthCAN_REQUEST_DO_NOTHING  : OnDoNothing  (lHeader, aClient); break;
        case EthCAN_REQUEST_INFO_GET    : OnInfoGet    (lHeader, aClient); break;
        case EthCAN_REQUEST_RESET       : OnReset      (lHeader, aClient); break;
        case EthCAN_REQUEST_SEND        : OnSend       (lHeader, aClient); break;

        default: MSG_ERROR("OnPacket - Invalid request code : ", lHeader->mCode);
        }
    }
}

#define BEGIN_TCP                                     \
    if (0 == (aIn->mFlags & EthCAN_FLAG_NO_RESPONSE)) \
    {                                                 \
        EthCAN_Header lHeader;                        \
        Header_Init(&lHeader, aIn);

#define END_TCP          \
        aClient.flush(); \
    }

void OnConfigErase(const EthCAN_Header * aIn, WiFiClient aClient)
{
    Config_Erase();

    BEGIN_TCP
    {
        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnConfigGet(const EthCAN_Header * aIn, WiFiClient aClient)
{
    BEGIN_TCP
    {
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        aClient.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_TCP
}

void OnConfigReset(const EthCAN_Header * aIn, WiFiClient aClient)
{
    uint8_t lFlags = Config_Reset();

    BEGIN_TCP
    {
        lHeader.mFlags = lFlags;

        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnConfigSet(const EthCAN_Header * aIn, WiFiClient aClient)
{
    uint8_t lFlags;

    EthCAN_Result lResult = Config_Set(aIn, &lFlags);

    BEGIN_TCP
    {
        lHeader.mFlags = lFlags;
        lHeader.mResult = static_cast<uint16_t>(lResult);
  
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        aClient.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_TCP
}

void OnConfigStore(const EthCAN_Header * aIn, WiFiClient aClient)
{
    EthCAN_Result lResult = Config_Store(aIn);

    BEGIN_TCP
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);  
    
        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnDoNothing(const EthCAN_Header * aIn, WiFiClient aClient)
{
    BEGIN_TCP
    {
        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnInfoGet(const EthCAN_Header * aIn, WiFiClient aClient)
{
    BEGIN_TCP
    {
        lHeader.mDataSize_byte  = sizeof(EthCAN_Info);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(EthCAN_Info);

        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        aClient.write(Info_Get(), sizeof(EthCAN_Info));
    }
    END_TCP
}

void OnReset(const EthCAN_Header * aIn, WiFiClient aClient)
{
    BEGIN_TCP
    {
        lHeader.mFlags |= EthCAN_FLAG_BUSY;

        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP

    CAN_Reset();
}

void OnSend(const EthCAN_Header * aIn, WiFiClient aClient)
{
    EthCAN_Result lResult = CAN_Send(aIn);

    BEGIN_TCP
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);

        aClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}
