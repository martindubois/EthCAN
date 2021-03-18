
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
static uint8_t sLevel;
static uint8_t sExpected;
static uint8_t sRequest;
static State   sState = STATE_INIT;

// Static function declarations
////////////////////////////////////////////////////////////////////////////

static void Result(EthCAN_Result aResult);

static void Sync();

// ===== Commands ===========================================================

static void Config_Reset();
static void Config_Set();
static void Info_Get();
static void Reset();
static void Send();

// Function
////////////////////////////////////////////////////////////////////////////

void Cmd_Loop()
{
    if(Serial.available())
    {
        uint8_t lByte = Serial.read();

        switch (sState)
        {
        case STATE_INIT:
            if (EthCAN_SYNC == lByte) { sState = STATE_SYNC; }
            break;

        case STATE_SYNC:
            sLevel = 0;
            sRequest = lByte;
            sState = STATE_REQUEST;
            switch (sRequest)
            {
            case EthCAN_REQUEST_CONFIG_RESET: Config_Reset(); break;
            case EthCAN_REQUEST_INFO_GET    : Info_Get    (); break;
            case EthCAN_REQUEST_RESET       : Reset       (); break;

            case EthCAN_REQUEST_CONFIG_SET: sExpected = sizeof(FW_Config); break;
            case EthCAN_REQUEST_SEND      : sExpected = 5; break;

            default: sState = STATE_INIT;
            }
            break;

        case STATE_REQUEST:
            sBuffer[sLevel] = lByte;
            sLevel++;
            if (sExpected <= sLevel)
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

void Cmd_Send(const EthCAN_Frame & aFrame)
{
    MSG_DEBUG("Cmd_Send(  )");
    
    Result(EthCAN_RESULT_MESSAGE);

    Serial.write(reinterpret_cast<const uint8_t *>(&aFrame), 5 + EthCAN_FRAME_DATA_SIZE(aFrame));
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

void Result(EthCAN_Result aResult)
{
    Sync();

    Serial.write(aResult);

    sState = STATE_INIT;
}

void Sync()
{
    Serial.write(&SYNC, sizeof(SYNC));
}

// ===== Commands ===========================================================

void Config_Reset()
{
    Result(CAN_Config_Reset());
}

void Config_Set()
{
    Result(CAN_Config_Set(* reinterpret_cast<FW_Config *>(sBuffer)));
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
    if (5 == sExpected)
    {
        sExpected += sBuffer[4] & ~ EthCAN_FLAG_CAN_RTR;
    }

    if (sExpected <= sLevel)
    {
        Result(CAN_Send(*reinterpret_cast<EthCAN_Frame *>(sBuffer)));
    }
}
