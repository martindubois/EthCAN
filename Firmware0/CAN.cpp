
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/CAN.cpp

#include <Arduino.h>

#include "Component.h"

// ===== Common =============================================================
#include "Common/Firmware.h"

// ===== Firmware0 ==========================================================
#include "Config.h"
#include "Info.h"

#include "CAN.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

// --> INIT <==+=======+====+
//      |      |       |    |
//      +--> SYNC --> FRAME |
//             |            |
//             +--> DATA ---+
typedef enum
{
    STATE_DATA,
    STATE_FRAME,
    STATE_INIT,
    STATE_SYNC,
}
State;

// Constants
/////////////////////////////////////////////////////////////////////////////

#define BAUD_RATE_bps (1000000)

// Variables
/////////////////////////////////////////////////////////////////////////////

static EthCAN_Result sResult = EthCAN_RESULT_NO_ERROR;
static State         sState  = STATE_INIT;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static EthCAN_Result Receive(uint8_t * aOut, unsigned int aOutSize_byte);

static void          Receive_Frame(uint8_t aByte);
static void          Receive_Init (uint8_t aByte);
static EthCAN_Result Receive_Sync (uint8_t aByte, bool aDataExpected);

static EthCAN_Result Request(EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte, void * aOut, unsigned int aOutSize_byte);

// Functions
/////////////////////////////////////////////////////////////////////////////

void CAN_Config()
{
    FW_Config lConfig;
    unsigned int i;

    for (i = 0; i < 6; i ++)
    {
        lConfig.mFilters[i] = gConfig.mCAN_Filters[i];
    }

    lConfig.mFlags = gConfig.mCAN_Flags;

    for (i = 0; i < 2; i ++)
    {
        lConfig.mMasks[i] = gConfig.mCAN_Masks[i];
    }

    lConfig.mRate = gConfig.mCAN_Rate;

    sResult = Request(EthCAN_REQUEST_CONFIG_SET, &lConfig, sizeof(lConfig), NULL, 0);
}

void CAN_Loop()
{
    Receive(NULL, 0);
}

void CAN_Setup()
{
    Serial2.begin(BAUD_RATE_bps, SERIAL_8N1, 36, 4, false);

    CAN_Config();
}

void CAN_GetInfo(EthCAN_Info * aInfo)
{
    FW_Info lInfo;

    aInfo->mResult_CAN = Request(EthCAN_REQUEST_INFO_GET, NULL, 0, &lInfo, sizeof(lInfo));
    if (EthCAN_OK == aInfo->mResult_CAN)
    {
        for (unsigned int i = 0; i < 4; i ++)
        {
            aInfo->mFirmware1_Version[i] = lInfo.mFirmware[i];
        }

        aInfo->mCAN_Errors       = lInfo.mErrors;
        aInfo->mCAN_Result       = lInfo.mResult;
        aInfo->mCounter_RxErrors = lInfo.mRxErrors;
        aInfo->mCounter_TxErrors = lInfo.mTxErrors;
    }
}

EthCAN_Result CAN_Send(const EthCAN_Header * aIn)
{
    if (sizeof(EthCAN_Frame) > aIn->mDataSize_byte)
    {
        return Info_Count_Error(__LINE__, EthCAN_ERROR_DATA_SIZE);
    }

    const EthCAN_Frame * lFrame = reinterpret_cast<const EthCAN_Frame *>(aIn + 1);

    unsigned int lDataSize_byte = EthCAN_FRAME_DATA_SIZE(*lFrame);

    EthCAN_Result lResult = Request(EthCAN_REQUEST_SEND, lFrame, 5 + lDataSize_byte, NULL, 0);
    if (EthCAN_OK == lResult)
    {
        Info_Count_Tx_Frame(lDataSize_byte);
    }

    return lResult;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

EthCAN_Result Receive(uint8_t * aOut, unsigned int aOutSize_byte)
{
    unsigned int lOutSize_byte = 0;
    EthCAN_Result lResult;
    bool lRetry = true;

    for (;;)
    {
        if (!Serial2.available())
        {
            if (lRetry && (lOutSize_byte < aOutSize_byte))
            {
                lRetry = false;
                delay(100);
                continue;
            }

            break;
        }

        lRetry = true;

        uint8_t lByte = Serial2.read();

        switch (sState)
        {
        case STATE_FRAME   : Receive_Frame(lByte); break;
        case STATE_INIT    : Receive_Init (lByte); break;

        case STATE_DATA:
            aOut[lOutSize_byte] = lByte;
            lOutSize_byte ++;
            if (aOutSize_byte <= lOutSize_byte)
            {
                sState = STATE_INIT;
                return EthCAN_OK;
            }
            break;

        case STATE_SYNC:
            lResult = Receive_Sync(lByte, 0 < aOutSize_byte);
            if (EthCAN_OK_PENDING != lResult)
            {
                return lResult;
            }
        }
    }

    return EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER;
}

void Receive_Frame(uint8_t aByte)
{
    static uint8_t       sBuffer[sizeof(EthCAN_Frame)];
    static unsigned int  sCount = 0;

    sBuffer[sCount] = aByte;
    sCount ++;

    const EthCAN_Frame * lFrame = reinterpret_cast<EthCAN_Frame *>(sBuffer);

    if (   (sizeof(EthCAN_Frame) <= sCount)
        || ((5 < sCount) && ((5 + EthCAN_FRAME_DATA_SIZE(*lFrame)) <= sCount)))
    {
        Info_Count_Rx_Frame(sCount - 5, lFrame->mId);

        Config_OnFrame(*lFrame);
        sCount = 0;
        sState = STATE_INIT;
    }
}

void Receive_Init(uint8_t aByte)
{
    if (EthCAN_SYNC == aByte)
    {
        sState = STATE_SYNC;
    }
    else
    {
        Serial.write(aByte);
    }
}

EthCAN_Result Receive_Sync(uint8_t aByte, bool aDataExpected)
{
    EthCAN_Result lResult = EthCAN_OK_PENDING;

    switch (aByte)
    {
    case EthCAN_RESULT_MESSAGE:
        sState = STATE_FRAME;
        break;

    case EthCAN_OK:
        if (aDataExpected)
        {
            sState = STATE_DATA;
            break;
        }
        // no break;

    case EthCAN_ERROR_TIMEOUT:
        sState = STATE_INIT;
        lResult = static_cast<EthCAN_Result>(aByte);
        break;

    default:
        sState = STATE_INIT;
        Serial.write(aByte);
    }

    return lResult;
}

EthCAN_Result Request(EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte, void * aOut, unsigned int aOutSize_byte)
{
    Serial2.write(EthCAN_SYNC);
    Serial2.write(aCode);

    if (0 < aInSize_byte)
    {
        Serial2.write(reinterpret_cast<const uint8_t *>(aIn), aInSize_byte);
    }

    return Receive(reinterpret_cast<uint8_t *>(aOut), aOutSize_byte);
}
