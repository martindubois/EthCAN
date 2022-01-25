
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021-2022 KMS
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
#include "UDP.h"

#include "TCP.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static bool IsConnected();

static void OnPacket(const void * aPacket, unsigned int aSize_byte);

static void OnCANReset   (const EthCAN_Header * aIn);
static void OnConfigErase(const EthCAN_Header * aIn);
static void OnConfigGet  (const EthCAN_Header * aIn);
static void OnConfigReset(const EthCAN_Header * aIn);
static void OnConfigSet  (const EthCAN_Header * aIn);
static void OnConfigStore(const EthCAN_Header * aIn);
static void OnDeviceReset(const EthCAN_Header * aIn);
static void OnDoNothing  (const EthCAN_Header * aIn);
static void OnInfoGet    (const EthCAN_Header * aIn);
static void OnSend       (const EthCAN_Header * aIn);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static WiFiClient sClient;
static WiFiServer sTCP(EthCAN_TCP_PORT);

// Functions
/////////////////////////////////////////////////////////////////////////////

void TCP_Loop()
{
    if (IsConnected())
    {
        unsigned int lAvailable_byte = sClient.available();
        if (sizeof(EthCAN_Header) <= lAvailable_byte)
        {
            uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];

            int lRead_byte = sClient.read(lBuffer, sizeof(EthCAN_Header));
            if (sizeof(EthCAN_Header) == lRead_byte)
            {
                EthCAN_Header * lHeader = reinterpret_cast<EthCAN_Header *>(lBuffer);

                unsigned int lPayload_byte = lHeader->mTotalSize_byte - lRead_byte;
                if (0 < lPayload_byte)
                {
                    lRead_byte += sClient.read(lBuffer + lRead_byte, lPayload_byte);
                }

                OnPacket(lBuffer, lRead_byte);
            }
            else
            {
                UDP_Trace("ERROR  TCP_Loop - WiFiClient.read( ,  ) failed");
            }
        }
    }
}

bool TCP_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame)
{
    if (IsConnected())
    {
        sClient.write(reinterpret_cast<const uint8_t *>(&aHeader), sizeof(aHeader));
        sClient.write(reinterpret_cast<const uint8_t *>(&aFrame ), sizeof(aFrame ));
        sClient.flush();
        return true;
    }

    UDP_Trace("WARNING  TCP_OnFrame - Not connected");

    return false;
}

void TCP_Setup()
{
    sTCP.begin();
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

bool IsConnected()
{
    if ((!sClient) || (!sClient.connected()))
    {
        sClient = sTCP.available();
    }

    return sClient && sClient.connected();
}

void OnPacket(const void * aPacket, unsigned int aSize_byte)
{
    const EthCAN_Header * lHeader = reinterpret_cast<const EthCAN_Header *>(aPacket);
    if (Header_Validate(*lHeader, aSize_byte))
    {
        Info_Count_Request(lHeader->mCode, lHeader->mId);

        switch (lHeader->mCode)
        {
        case EthCAN_REQUEST_CAN_RESET   : OnCANReset   (lHeader); break;
        case EthCAN_REQUEST_CONFIG_ERASE: OnConfigErase(lHeader); break;
        case EthCAN_REQUEST_CONFIG_GET  : OnConfigGet  (lHeader); break;
        case EthCAN_REQUEST_CONFIG_RESET: OnConfigReset(lHeader); break;
        case EthCAN_REQUEST_CONFIG_SET  : OnConfigSet  (lHeader); break;
        case EthCAN_REQUEST_CONFIG_STORE: OnConfigStore(lHeader); break;
        case EthCAN_REQUEST_DEVICE_RESET: OnDeviceReset(lHeader); break;
        case EthCAN_REQUEST_DO_NOTHING  : OnDoNothing  (lHeader); break;
        case EthCAN_REQUEST_INFO_GET    : OnInfoGet    (lHeader); break;
        case EthCAN_REQUEST_SEND        : OnSend       (lHeader); break;

        default: UDP_Trace("ERROR  OnPacket - Invalid request code");
        }
    }
    else
    {
        UDP_Trace("ERROR  OnPacket - Header_Validate( ,  ) failed");
    }
}

#define BEGIN_TCP                                     \
    if (0 == (aIn->mFlags & EthCAN_FLAG_NO_RESPONSE)) \
    {                                                 \
        EthCAN_Header lHeader;                        \
        Header_Init(&lHeader, aIn);

#define END_TCP          \
        sClient.flush(); \
    }

void OnCANReset(const EthCAN_Header * aIn)
{
    BEGIN_TCP
    {
        lHeader.mFlags |= EthCAN_FLAG_BUSY;

        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP

    CAN_Reset();
}

void OnConfigErase(const EthCAN_Header * aIn)
{
    Config_Erase();

    BEGIN_TCP
    {
        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnConfigGet(const EthCAN_Header * aIn)
{
    BEGIN_TCP
    {
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sClient.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_TCP
}

void OnConfigReset(const EthCAN_Header * aIn)
{
    uint8_t lFlags = Config_Reset();

    BEGIN_TCP
    {
        lHeader.mFlags = lFlags;

        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnConfigSet(const EthCAN_Header * aIn)
{
    uint8_t lFlags;

    EthCAN_Result lResult = Config_Set(aIn, &lFlags);

    BEGIN_TCP
    {
        lHeader.mFlags = lFlags;
        lHeader.mResult = static_cast<uint16_t>(lResult);
  
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sClient.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_TCP
}

void OnConfigStore(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = Config_Store(aIn);

    BEGIN_TCP
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);  
    
        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnDeviceReset(const EthCAN_Header * aIn)
{
    BEGIN_TCP
    {
        lHeader.mFlags |= EthCAN_FLAG_BUSY;

        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP

    ESP.restart();
}

void OnDoNothing(const EthCAN_Header * aIn)
{
    BEGIN_TCP
    {
        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}

void OnInfoGet(const EthCAN_Header * aIn)
{
    BEGIN_TCP
    {
        lHeader.mDataSize_byte  = sizeof(EthCAN_Info);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(EthCAN_Info);

        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        sClient.write(Info_Get(), sizeof(EthCAN_Info));
    }
    END_TCP
}

void OnSend(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = CAN_Send(aIn);

    BEGIN_TCP
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);

        sClient.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_TCP
}
