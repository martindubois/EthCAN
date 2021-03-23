
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/CAN.cpp

// CODE REVIEW 2021-03-23 KMS - Martin Dubois, P.Eng.

#include <Arduino.h>

#include "Component.h"

// ===== Firmware0 ==========================================================

#include "Includes/EthCAN_Types.h"

#include "Common/Firmware.h"

#include "Config.h"
#include "Info.h"

#include "CAN.h"

// Data types
/////////////////////////////////////////////////////////////////////////////

// --> INIT <===============+==============================+====+
//      |                   |                              |    |
//      +--EthCAN_SYNC--> SYNC --EthCAN_RESULT_REQUEST--> FRAME |
//                          |                                   |
//                          +--EthCAN_OK--> DATA ---------------+
typedef enum
{
    STATE_DATA,
    STATE_FRAME,
    STATE_INIT,
    STATE_SYNC,
}
State;

// Variables
/////////////////////////////////////////////////////////////////////////////

static State sState = STATE_INIT;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static EthCAN_Result Receive(uint8_t * aOut, unsigned int aOutSize_byte, bool aWait);

static void          Receive_Frame(uint8_t aByte);
static void          Receive_Init (uint8_t aByte);
static EthCAN_Result Receive_Sync (uint8_t aByte, bool aDataExpected);

static void Send(EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte);

static EthCAN_Result SendAndReceive(EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte, void * aOut, unsigned int aOutSize_byte);

// Functions
/////////////////////////////////////////////////////////////////////////////

void CAN_Config()
{
    FW_Config lConfig;
    unsigned int i;

    for (i = 0; i < EthCAN_FILTER_QTY; i ++)
    {
        lConfig.mFilters[i] = gConfig.mCAN_Filters[i];
    }

    lConfig.mFlags = gConfig.mCAN_Flags;

    for (i = 0; i < EthCAN_MASK_QTY; i ++)
    {
        lConfig.mMasks[i] = gConfig.mCAN_Masks[i];
    }

    lConfig.mRate = gConfig.mCAN_Rate;

    Send(EthCAN_REQUEST_CONFIG_SET, &lConfig, sizeof(lConfig));
}

void CAN_Loop()
{
    Receive(NULL, 0, false);
}

void CAN_Setup()
{
    Serial2.begin(FIRMWARE_BAUD_RATE_bps, SERIAL_8N1, 36, 4, false);

    CAN_Config();
}

void CAN_GetInfo(EthCAN_Info * aInfo)
{
    FW_Info lInfo;

    aInfo->mFirmware1_Result = SendAndReceive(EthCAN_REQUEST_INFO_GET, NULL, 0, &lInfo, sizeof(lInfo));
    if (EthCAN_OK == aInfo->mFirmware1_Result)
    {
        for (unsigned int i = 0; i < EthCAN_VERSION_SIZE_byte; i ++)
        {
            aInfo->mFirmware1_Version[i] = lInfo.mFirmware[i];
        }

        aInfo->mCAN_Errors       = lInfo.mErrors;
        aInfo->mCAN_Result       = lInfo.mResult;
        aInfo->mCounter_RxErrors = lInfo.mRxErrors;
        aInfo->mCounter_TxErrors = lInfo.mTxErrors;

        // Copy debug counter, if not 0! This way, Firmaware0 can use the
        // mCounter_Debug[0] if the Firmware1 does not use it.

        uint32_t lDebug = lInfo.mDebug[1];
        lDebug <<= 16;
        lDebug |= lInfo.mDebug[0];

        if (0 != lDebug)
        {
            aInfo->mCounter_Debug[0] = lDebug;
        }
    }
}

EthCAN_Result CAN_Send(const EthCAN_Header * aIn)
{
    if (CAN_HEADER_SIZE_byte > aIn->mDataSize_byte)
    {
        return Info_Count_Error(__LINE__, EthCAN_ERROR_DATA_SIZE);
    }

    const EthCAN_Frame * lFrame = reinterpret_cast<const EthCAN_Frame *>(aIn + 1);

    unsigned int lDataSize_byte = EthCAN_FRAME_DATA_SIZE(*lFrame);
    unsigned int lTotalSize_byte = CAN_HEADER_SIZE_byte + lDataSize_byte;

    if (aIn->mDataSize_byte < lTotalSize_byte)
    {
        return Info_Count_Error(__LINE__, EthCAN_ERROR_DATA_SIZE);
    }

    Send(EthCAN_REQUEST_SEND, lFrame, lTotalSize_byte);

    Info_Count_Tx_Frame(lDataSize_byte);

    return EthCAN_OK_PENDING;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

EthCAN_Result Receive(uint8_t * aOut, unsigned int aOutSize_byte, bool aWait)
{
    unsigned int lOutSize_byte = 0;
    unsigned int lRetry = 0;

    for (;;)
    {
        if (!Serial2.available())
        {
            if (aWait && (10 < lRetry))
            {
                lRetry ++;
                delay(5);
                continue;
            }

            break;
        }

        lRetry = 0;

        uint8_t lByte = Serial2.read();
        EthCAN_Result lResult;

        switch (sState)
        {
        case STATE_FRAME: Receive_Frame(lByte); break;
        case STATE_INIT : Receive_Init (lByte); break;

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
            if (EthCAN_RESULT_INVALID != lResult)
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
        || ((CAN_HEADER_SIZE_byte < sCount) && (EthCAN_FRAME_TOTAL_SIZE(*lFrame) <= sCount)))
    {
        Info_Count_Rx_Frame(sCount - CAN_HEADER_SIZE_byte, lFrame->mId);

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
        #ifdef _TRACE_
            Serial.write(aByte);
        #endif
    }
}

EthCAN_Result Receive_Sync(uint8_t aByte, bool aDataExpected)
{
    EthCAN_Result lResult = EthCAN_RESULT_INVALID;

    switch (aByte)
    {
    case EthCAN_RESULT_REQUEST:
        sState = STATE_FRAME;
        break;

    case EthCAN_OK:
    case EthCAN_OK_PENDING:
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

void Send(EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte)
{
    Serial2.write(EthCAN_SYNC);
    Serial2.write(aCode);

    if (0 < aInSize_byte)
    {
        Serial2.write(reinterpret_cast<const uint8_t *>(aIn), aInSize_byte);
    }
}

EthCAN_Result SendAndReceive(EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte, void * aOut, unsigned int aOutSize_byte)
{
    Send(aCode, aIn, aInSize_byte);

    return Receive(reinterpret_cast<uint8_t *>(aOut), aOutSize_byte, true);
}
