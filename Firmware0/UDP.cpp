
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/UDP.cpp

#include <Arduino.h>

#include <WiFi.h>

#include "Component.h"

#include "CAN.h"
#include "Config.h"
#include "Info.h"

#include "UDP.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void OnPacket(const void * aPacket, unsigned int aSize_byte);

static void OnConfigGet  (const EthCAN_Header * aIn);
static void OnConfigReset(const EthCAN_Header * aIn);
static void OnConfigSet  (const EthCAN_Header * aIn);
static void OnConfigStore(const EthCAN_Header * aIn);
static void OnInfoGet    (const EthCAN_Header * aIn);
static void OnReset      (const EthCAN_Header * aIn);
static void OnSend       (const EthCAN_Header * aIn);

static void Header_Init(EthCAN_Header * aOut, const EthCAN_Header * aIn);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static WiFiUDP sUDP;

// Functions
/////////////////////////////////////////////////////////////////////////////

void UDP_Loop()
{
    sUDP.parsePacket();

    uint8_t lBuffer[256];

    unsigned int lSize_byte = sUDP.read(lBuffer, sizeof(lBuffer));
    if (0 < lSize_byte)
    {
        if (sizeof(lBuffer) >= lSize_byte)
        {
            OnPacket(lBuffer, lSize_byte);
        }
        else
        {
            MSG_ERROR("UDP_Loop - UDP packet larger than the buffer");
        }
    }
}

void UDP_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame, uint32_t aIPv4, uint16_t aPort)
{
    MSG_DEBUG("UDP_OnFrame( , , ,  )");

    sUDP.beginPacket(aIPv4, aPort);
    {
        sUDP.write(reinterpret_cast<const uint8_t *>(&aHeader), sizeof(aHeader));
        sUDP.write(reinterpret_cast<const uint8_t *>(&aFrame ), sizeof(aFrame ));
    }
    sUDP.endPacket();
}

void UDP_Setup()
{
    // MSG_DEBUG("UDP_Setup()");

    sUDP.begin(EthCAN_UDP_PORT);
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void OnPacket(const void * aPacket, unsigned int aSize_byte)
{
    MSG_DEBUG("OnPacket( ,  )");

    if (sizeof(EthCAN_Header) <= aSize_byte)
    {
        const EthCAN_Header * lHeader = reinterpret_cast<const EthCAN_Header *>(aPacket);

        if (sizeof(EthCAN_Header) + lHeader->mDataSize_byte <= lHeader->mTotalSize_byte)
        {
            gInfo.mRequestId_UDP = lHeader->mId;

            switch (lHeader->mCode)
            {
            case EthCAN_REQUEST_CONFIG_GET  : OnConfigGet  (lHeader); break;
            case EthCAN_REQUEST_CONFIG_RESET: OnConfigReset(lHeader); break;
            case EthCAN_REQUEST_CONFIG_SET  : OnConfigSet  (lHeader); break;
            case EthCAN_REQUEST_CONFIG_STORE: OnConfigStore(lHeader); break;
            case EthCAN_REQUEST_INFO_GET    : OnInfoGet    (lHeader); break;
            case EthCAN_REQUEST_RESET       : OnReset      (lHeader); break;
            case EthCAN_REQUEST_SEND        : OnSend       (lHeader); break;

            default: MSG_ERROR("OnPacket - Invalid request code");
            }
        }
        else
        {
            MSG_ERROR("OnPacket - Invalid request size");
        }
    }
    else
    {
        MSG_ERROR("OnPacket - UDP packet smaller than the request header");
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
    Config_Reset();

    BEGIN_UDP
    {
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_UDP
}

void OnConfigSet(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = Config_Set(aIn);

    BEGIN_UDP
    {
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

void OnInfoGet(const EthCAN_Header * aIn)
{
    BEGIN_UDP
    {
        lHeader.mDataSize_byte  = sizeof(gInfo);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gInfo);
    
        sUDP.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sUDP.write(reinterpret_cast<const uint8_t *>(&gInfo  ), sizeof(gInfo));
    }
    END_UDP
}

void OnReset(const EthCAN_Header * aIn)
{
    BEGIN_UDP
    {
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

void Header_Init(EthCAN_Header * aOut, const EthCAN_Header * aIn)
{
    MSG_DEBUG("Header_Init( ,  )");

    aOut->mCode           = aIn->mCode;
    aOut->mDataSize_byte  = 0;
    aOut->mFlags          = 0;
    aOut->mId             = aIn->mId;
    aOut->mResult         = EthCAN_OK;
    aOut->mSalt           = 0;
    aOut->mSign           = 0;
    aOut->mTotalSize_byte = sizeof(EthCAN_Header);
}