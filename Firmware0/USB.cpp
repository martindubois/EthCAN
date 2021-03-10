
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/UDP.cpp

#include <Arduino.h>

#include "Component.h"

#include "CAN.h"
#include "Config.h"
#include "Info.h"

#include "USB.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

// --> STATE_TRACE <--+
//             |      |
//             +--> DATA
typedef enum
{
    STATE_DATA,
    STATE_TRACE,
}
State;

// Constants
/////////////////////////////////////////////////////////////////////////////

static const uint8_t SYNC = EthCAN_SYNC;

// Static variables
/////////////////////////////////////////////////////////////////////////////

static uint8_t      sSerial_Buffer[256];
static unsigned int sSerial_Level = 0;
static State        sSerial_State = STATE_TRACE;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void OnPacket(const EthCAN_Header * aIn);

static void OnConfigGet  (const EthCAN_Header * aIn);
static void OnConfigReset(const EthCAN_Header * aIn);
static void OnConfigSet  (const EthCAN_Header * aIn);
static void OnConfigStore(const EthCAN_Header * aIn);
static void OnInfoGet    (const EthCAN_Header * aIn);
static void OnReset      (const EthCAN_Header * aIn);
static void OnSend       (const EthCAN_Header * aIn);

static void Header_Init(EthCAN_Header * aOut, const EthCAN_Header * aIn);

// Functions
/////////////////////////////////////////////////////////////////////////////

void USB_Loop()
{
    while(0 < Serial.available())
    {
        uint8_t lByte = Serial.read();

        switch (sSerial_State)
        {
        case STATE_DATA:
            sSerial_Buffer[sSerial_Level] = lByte;
            sSerial_Level ++;
            if (sizeof(EthCAN_Header) <= sSerial_Level)
            {
                const EthCAN_Header * lHeader = reinterpret_cast<EthCAN_Header *>(sSerial_Buffer);

                if (lHeader->mTotalSize_byte <= sSerial_Level)
                {
                    OnPacket(lHeader);
                    sSerial_Level = 0;
                    sSerial_State = STATE_TRACE;
                }
                else if (sizeof(sSerial_Buffer) <= sSerial_Level)
                {
                    sSerial_Level = 0;
                    sSerial_State = STATE_TRACE;
                }
            }
            break;

        case STATE_TRACE:
            if (EthCAN_SYNC == lByte)
            {
                sSerial_State = STATE_DATA;
            }
            break;
        }
    }
}

void USB_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame)
{
    MSG_DEBUG("USB_OnFrame( ,  )");

    Serial.write(&SYNC, sizeof(SYNC));

    Serial.write(reinterpret_cast<const uint8_t *>(&aHeader), sizeof(aHeader));
    Serial.write(reinterpret_cast<const uint8_t *>(&aFrame ), sizeof(aFrame ));
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void OnPacket(const EthCAN_Header * aIn)
{
    if (sizeof(EthCAN_Header) + aIn->mDataSize_byte <= aIn->mTotalSize_byte)
    {
        Info_Count_Request(aIn->mCode, aIn->mId);

        switch (aIn->mCode)
        {
        case EthCAN_REQUEST_CONFIG_GET  : OnConfigGet  (aIn); break;
        case EthCAN_REQUEST_CONFIG_RESET: OnConfigReset(aIn); break;
        case EthCAN_REQUEST_CONFIG_SET  : OnConfigSet  (aIn); break;
        case EthCAN_REQUEST_CONFIG_STORE: OnConfigStore(aIn); break;
        case EthCAN_REQUEST_INFO_GET    : OnInfoGet    (aIn); break;
        case EthCAN_REQUEST_RESET       : OnReset      (aIn); break;
        case EthCAN_REQUEST_SEND        : OnSend       (aIn); break;

        default: MSG_ERROR("OnPacket - Invalid request code : ", aIn->mCode);
        }
    }
    else
    {
        MSG_ERROR("OnPacket - Invalid request size : ", aIn->mTotalSize_byte);
    }
}

#define BEGIN_USB                                     \
    if (0 == (aIn->mFlags & EthCAN_FLAG_NO_RESPONSE)) \
    {                                                 \
        EthCAN_Header lHeader;                        \
        Header_Init(&lHeader, aIn);                   \
        Serial.write(&SYNC, sizeof(SYNC));

#define END_USB \
    }

void OnConfigGet(const EthCAN_Header * aIn)
{
    BEGIN_USB
    {
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        Serial.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_USB
}

void OnConfigReset(const EthCAN_Header * aIn)
{
    uint8_t lFlags = Config_Reset();

    BEGIN_USB
    {
        lHeader.mFlags = lFlags;

        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB
}

void OnConfigSet(const EthCAN_Header * aIn)
{
    uint8_t lFlags;

    EthCAN_Result lResult = Config_Set(aIn, &lFlags);

    BEGIN_USB
    {
        lHeader.mFlags = lFlags;
        lHeader.mResult = static_cast<uint16_t>(lResult);
  
        lHeader.mDataSize_byte  = sizeof(gConfig);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(gConfig);
  
        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        Serial.write(reinterpret_cast<const uint8_t *>(&gConfig), sizeof(gConfig));
    }
    END_USB
}

void OnConfigStore(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = Config_Store(aIn);

    BEGIN_USB
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);  
    
        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB
}

void OnInfoGet(const EthCAN_Header * aIn)
{
    const uint8_t * lInfo = Info_Get();

    BEGIN_USB
    {
        lHeader.mDataSize_byte  = sizeof(EthCAN_Info);
        lHeader.mTotalSize_byte = sizeof(lHeader) + sizeof(EthCAN_Info);
    
        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
        Serial.write(lInfo, sizeof(EthCAN_Info));
    }
    END_USB
}

void OnReset(const EthCAN_Header * aIn)
{
    BEGIN_USB
    {
        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB

    ESP.restart();
}

void OnSend(const EthCAN_Header * aIn)
{
    EthCAN_Result lResult = CAN_Send(aIn);

    BEGIN_USB
    {
        lHeader.mResult = static_cast<uint16_t>(lResult);

        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB
}

void Header_Init(EthCAN_Header * aOut, const EthCAN_Header * aIn)
{
    aOut->mCode           = aIn->mCode;
    aOut->mDataSize_byte  = 0;
    aOut->mFlags          = 0;
    aOut->mId             = aIn->mId;
    aOut->mResult         = EthCAN_OK;
    aOut->mTotalSize_byte = sizeof(EthCAN_Header);
}
