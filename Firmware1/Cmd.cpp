
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/Cmd.cpp

#include <Arduino.h>

#include "Component.h"

// ===== Firmware1 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
    #include "Includes/EthCAN_Result.h"
}

#include "Common/Version.h"

#include "Buffer.h"
#include "CAN.h"

#include "Cmd.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

// --> INIT <==+=======+
//      |      |       |
//      +--> SYNC --> REQUEST 
typedef enum
{
    STATE_INIT,
    STATE_REQUEST,
    STATE_SYNC,
}
State;

// Constants
////////////////////////////////////////////////////////////////////////////

static const uint8_t SYNC = EthCAN_SYNC;

// Static variable
/////////////////////////////////////////////////////////////////////////////

static uint8_t sBuffer[sizeof(FW_Config)];
static FW_Info sInfo;
static uint8_t sLevel_byte;
static uint8_t sExpected_byte;
static State   sState = STATE_INIT;

// Static function declarations
////////////////////////////////////////////////////////////////////////////

static void Loop_Rx();
static void Loop_Tx();

static void Result(EthCAN_Result aResult);

// ===== Commands ===========================================================

static void Config_Reset();
static void Config_Set();
static void Info_Get();
static void Reset();
static void Send();

// Macro
////////////////////////////////////////////////////////////////////////////

#define Sync() Serial.write(&SYNC, sizeof(SYNC))

// Function
////////////////////////////////////////////////////////////////////////////

// Critical path
void Cmd_Loop()
{
    Loop_Rx();
    Loop_Tx();
}

void Cmd_Setup()
{
    sInfo.mFirmware[0] = VERSION_MAJOR;
    sInfo.mFirmware[1] = VERSION_MINOR;
    sInfo.mFirmware[2] = VERSION_BUILD;
    sInfo.mFirmware[3] = VERSION_COMPATIBILITY;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Loop_Rx()
{
    static uint8_t sRequest;

    if(Serial.available())
    {
        uint8_t lByte = Serial.read();

        switch (sState)
        {
        case STATE_INIT:
            if (EthCAN_SYNC == lByte) { sState = STATE_SYNC; }
            break;

        case STATE_SYNC:
            sLevel_byte = 0;
            sRequest = lByte;
            sState = STATE_REQUEST;
            switch (sRequest)
            {
            case EthCAN_REQUEST_CONFIG_RESET: Config_Reset(); break;
            case EthCAN_REQUEST_INFO_GET    : Info_Get    (); break;
            case EthCAN_REQUEST_RESET       : Reset       (); break;

            case EthCAN_REQUEST_CONFIG_SET: sExpected_byte = sizeof(FW_Config); break;
            case EthCAN_REQUEST_SEND      : sExpected_byte = CAN_HEADER_SIZE_byte; break;

            default: sState = STATE_INIT;
            }
            break;

        case STATE_REQUEST:
            sBuffer[sLevel_byte] = lByte;
            sLevel_byte++;
            if (sExpected_byte <= sLevel_byte)
            {
                switch (sRequest)
                {
                case EthCAN_REQUEST_CONFIG_SET: Config_Set(); break;
                case EthCAN_REQUEST_SEND      : Send      (); break;
                }
            }
        }
    }
}

// Critical path
void Loop_Tx()
{
    static EthCAN_Frame * sFrame = NULL;
    static unsigned int   sSent_byte = 0;

    if (NULL == sFrame)
    {
        sFrame = Buffer_Pop(BUFFER_TYPE_TX_SERIAL);
    }

    if (NULL != sFrame)
    {
        unsigned int lReady_byte = Serial.availableForWrite();

        if ((0 == sSent_byte) && (0 < lReady_byte))
        {
            Sync();
            sSent_byte ++;
            lReady_byte --;
        }

        if ((1 == sSent_byte) && (0 < lReady_byte))
        {
            Serial.write(EthCAN_RESULT_REQUEST);
            sSent_byte ++;
            lReady_byte --;
        }

        if ((2 <= sSent_byte) && (0 < lReady_byte))
        {
            unsigned int lTotal_byte = 2 + EthCAN_FRAME_TOTAL_SIZE(*sFrame);
            unsigned int lSize_byte = lTotal_byte - sSent_byte;
            if (lReady_byte < lSize_byte)
            {
                lSize_byte = lReady_byte;
            }

            Serial.write(reinterpret_cast<const uint8_t *>(sFrame) + sSent_byte - 2, lSize_byte);
            sSent_byte += lSize_byte;

            if (lTotal_byte == sSent_byte)
            {
                Buffer_Push(sFrame, BUFFER_TYPE_FREE);
                sSent_byte = 0;
                sFrame = NULL;
            }
        }
    }
}

void Result(EthCAN_Result aResult)
{
    Sync();

    Serial.write(aResult);

    sState = STATE_INIT;
}

// ===== Commands ===========================================================

void Config_Reset()
{
    Result(CAN_Config_Reset());
}

void Config_Set()
{
    CAN_Config_Set(* reinterpret_cast<FW_Config *>(sBuffer));

    sState = STATE_INIT;
}

void Info_Get()
{
    CAN_GetInfo(&sInfo);

    Result(EthCAN_OK);

    Serial.write(reinterpret_cast<uint8_t *>(&sInfo), sizeof(sInfo));
}

void Reset()
{
    Result(EthCAN_OK);

    asm volatile ("  jmp 0");
}

void Send()
{
    if (CAN_HEADER_SIZE_byte == sExpected_byte)
    {
        sExpected_byte += sBuffer[CAN_OFFSET_DATA_SIZE] & ~ CAN_FLAG_RTR;
    }

    if (sExpected_byte <= sLevel_byte)
    {
        EthCAN_Frame * lFrame = Buffer_Pop(BUFFER_TYPE_FREE);
        if (NULL != lFrame)
        {
            memcpy(lFrame, sBuffer, sExpected_byte);

            Buffer_Push(lFrame, BUFFER_TYPE_TX_CAN);

            sState = STATE_INIT;
        }
    }
}
