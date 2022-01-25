
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021-2022 KMS
// Product   EthCan
// File      Firmware0/UDP.cpp

// CODE REVIEW 2021-03-24 KMS - Martin Dubois, P. Eng.

#include <Arduino.h>

#include "Component.h"

// ===== Firmware0 ==========================================================
#include "CAN.h"
#include "Config.h"
#include "Header.h"
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

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void OnPacket(const EthCAN_Header * aIn);

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

// Functions
/////////////////////////////////////////////////////////////////////////////

void USB_Loop()
{
    static uint8_t      sBuffer[EthCAN_PACKET_SIZE_MAX_byte];
    static unsigned int sLevel_byte = 0;
    static State        sState = STATE_TRACE;

    while(0 < Serial.available())
    {
        uint8_t lByte = Serial.read();

        switch (sState)
        {
        case STATE_DATA:
            sBuffer[sLevel_byte] = lByte;
            sLevel_byte ++;
            if (sizeof(EthCAN_Header) <= sLevel_byte)
            {
                const EthCAN_Header * lHeader = reinterpret_cast<EthCAN_Header *>(sBuffer);

                if (lHeader->mTotalSize_byte <= sLevel_byte)
                {
                    OnPacket(lHeader);
                    sLevel_byte = 0;
                    sState = STATE_TRACE;
                }
                else if (sizeof(sBuffer) <= sLevel_byte)
                {
                    MSG_WARNING("USB_Loop - Buffer overflow");
                    sLevel_byte = 0;
                    sState = STATE_TRACE;
                }
            }
            break;

        case STATE_TRACE:
            if (EthCAN_SYNC == lByte)
            {
                sState = STATE_DATA;
            }
            break;
        }
    }
}

void USB_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame)
{
    Serial.write(&SYNC, sizeof(SYNC));

    Serial.write(reinterpret_cast<const uint8_t *>(&aHeader), sizeof(aHeader));
    Serial.write(reinterpret_cast<const uint8_t *>(&aFrame ), sizeof(aFrame ));
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void OnPacket(const EthCAN_Header * aIn)
{
    if (Header_Validate(*aIn, aIn->mTotalSize_byte))
    {
        Info_Count_Request(aIn->mCode, aIn->mId);

        switch (aIn->mCode)
        {
        case EthCAN_REQUEST_CAN_RESET   : OnCANReset   (aIn); break;
        case EthCAN_REQUEST_CONFIG_ERASE: OnConfigErase(aIn); break;
        case EthCAN_REQUEST_CONFIG_GET  : OnConfigGet  (aIn); break;
        case EthCAN_REQUEST_CONFIG_RESET: OnConfigReset(aIn); break;
        case EthCAN_REQUEST_CONFIG_SET  : OnConfigSet  (aIn); break;
        case EthCAN_REQUEST_CONFIG_STORE: OnConfigStore(aIn); break;
        case EthCAN_REQUEST_DEVICE_RESET: OnDeviceReset(aIn); break;
        case EthCAN_REQUEST_DO_NOTHING  : OnDoNothing  (aIn); break;
        case EthCAN_REQUEST_INFO_GET    : OnInfoGet    (aIn); break;
        case EthCAN_REQUEST_SEND        : OnSend       (aIn); break;

        default: MSG_ERROR("OnPacket - Invalid request code : ", aIn->mCode);
        }
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

void OnCANReset(const EthCAN_Header * aIn)
{
    BEGIN_USB
    {
        lHeader.mFlags |= EthCAN_FLAG_BUSY;

        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB

    CAN_Reset();
}

void OnConfigErase(const EthCAN_Header * aIn)
{
    Config_Erase();

    BEGIN_USB
    {
        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB
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

void OnDeviceReset(const EthCAN_Header * aIn)
{
    BEGIN_USB
    {
        lHeader.mFlags |= EthCAN_FLAG_BUSY;

        Serial.write(reinterpret_cast<const uint8_t *>(&lHeader), sizeof(lHeader));
    }
    END_USB

    ESP.restart();
}

void OnDoNothing(const EthCAN_Header * aIn)
{
    BEGIN_USB
    {
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
