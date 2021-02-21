
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/CAN.cpp

// TODO Firmware1
//      Use our own firmware on the Serial CAN bus module and use faster
//      UART speed.

#include <Arduino.h>

#include "Component.h"

#include "Config.h"
#include "Info.h"

#include "CAN.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define BAUD_RATE_DEFAULT_bps (  9600)
#define BAUD_RATE_WORK_bps    (115200)

static const char * ERROR_0 = "ERROR CMD\r\n";

static const char * HEX_DIGITS = "0123456789ABCDEF";

// Variables
/////////////////////////////////////////////////////////////////////////////

static uint8_t      sBuffer[12];
static unsigned int sCount = 0;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static EthCAN_Result ConfigId(char aCmd, unsigned int aIndex, uint32_t aId);

static void Connect(unsigned int aBaudRate_bps);

static EthCAN_Result EnterSettingMode();
static EthCAN_Result LeaveSettingMode();

static EthCAN_Result WaitResponse(const char * aExpected, unsigned int aLength);

static uint32_t IdFromBytes(const uint8_t * aIn);
static void     IdToBytes(uint8_t * aOut, uint32_t aIn);

// Functions
/////////////////////////////////////////////////////////////////////////////

void CAN_Config()
{
    // MSG_DEBUG("CAN_Config()");

    bool lSet = false;

    EthCAN_Result lRet = EnterSettingMode();
    if (EthCAN_OK == lRet)
    {
        Serial2.print("AT+C=");
        Serial2.println(gConfig.mCAN_Rate);

        lRet = WaitResponse("OK\r\n", 4);
        if (EthCAN_OK == lRet)
        {
            unsigned int i;

            for (i = 0; i < 6; i ++)
            {
                lRet = ConfigId('F', i, gConfig.mCAN_Filters[i]);
                if (EthCAN_OK != lRet)
                {
                    break;
                }
            }

            if (EthCAN_OK == lRet)
            {
                for (i = 0; i < 2; i ++)
                {
                    lRet = ConfigId('M', i, gConfig.mCAN_Masks[i]);
                    if (EthCAN_OK != lRet)
                    {
                        break;
                    }
                }
            }
        }

        if (EthCAN_OK != lRet)
        {
            Info_Set_Result_CAN(lRet);
            lSet = true;
        }

        lRet = LeaveSettingMode();
    }

    if (!lSet)
    {
        Info_Set_Result_CAN(lRet);
    }
}

void CAN_Loop()
{
    while (Serial2.available())
    {
        sBuffer[sCount] = Serial2.read();
        sCount++;
        MSG_DEBUG(sBuffer[sCount]);

        if (12 == sCount)
        {
            MSG_DEBUG("CAN_Loop - Frame received");

            EthCAN_Frame lFrame;

            lFrame.mId = IdFromBytes(sBuffer);

            // TODO Firmware1
            //      Indicate the true data size
            lFrame.mDataSize_byte = 8;

            memcpy(lFrame.mData, sBuffer + 4, 8);

            Info_Count_Rx_Frame(lFrame.mDataSize_byte, lFrame.mId);

            Config_OnFrame(lFrame);

            sCount = 0;
        }
    }
}

void CAN_Setup()
{
    // MSG_DEBUG("CAN_Setup()");

    Connect(BAUD_RATE_WORK_bps);

    CAN_Config();
}

EthCAN_Result CAN_Send(const EthCAN_Header * aIn)
{
    MSG_DEBUG("CAN_Send(  )");

    if (sizeof(EthCAN_Frame) > aIn->mDataSize_byte)
    {
        return Info_Count_Error(__LINE__, EthCAN_ERROR_INVALID_DATA_SIZE);
    }

    const EthCAN_Frame * lFrame = reinterpret_cast<const EthCAN_Frame *>(aIn + 1);

    // TODO Firmware1
    //      Do no send useless data.

    uint8_t lData[14];

    IdToBytes(lData, lFrame->mId & ~ EthCAN_ID_EXTENDED);
    lData[4] = (0 == (lFrame->mId & EthCAN_ID_EXTENDED)) ? 0 : 1;
    lData[5] = 0;

    memcpy(lData + 6, lFrame->mData, 8);

    Serial2.write(lData, sizeof(lData));

    Info_Count_Tx_Frame(lFrame->mDataSize_byte);
  
    return EthCAN_OK;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

EthCAN_Result ConfigId(char aCmd, unsigned int aIndex, uint32_t aId)
{
    // MSG_DEBUG("ConfigId( , ,  )");

    uint32_t lId = aId & ~ EthCAN_ID_EXTENDED;
    char lStr[22];

    lStr[ 0] = 'A';
    lStr[ 1] = 'T';
    lStr[ 2] = '+';
    lStr[ 3] = aCmd;
    lStr[ 4] = '=';
    lStr[ 5] = '[';
    lStr[ 6] = '0' + aIndex;
    lStr[ 7] = ']';
    lStr[ 8] = '[';
    lStr[ 9] = (0 == (EthCAN_ID_EXTENDED & aId)) ? '0' : '1';
    lStr[10] = ']';
    lStr[11] = '[';
    lStr[12] = HEX_DIGITS[(lId >> 28) & 0xf];
    lStr[13] = HEX_DIGITS[(lId >> 24) & 0xf];
    lStr[14] = HEX_DIGITS[(lId >> 20) & 0xf];
    lStr[15] = HEX_DIGITS[(lId >> 16) & 0xf];
    lStr[16] = HEX_DIGITS[(lId >> 12) & 0xf];
    lStr[17] = HEX_DIGITS[(lId >>  8) & 0xf];
    lStr[18] = HEX_DIGITS[(lId >>  4) & 0xf];
    lStr[19] = HEX_DIGITS[(lId      ) & 0xf];
    lStr[20] = ']';
    lStr[21] = '\0';

    Serial2.println(lStr);

    return WaitResponse("OK\r\n", 4);
}

void Connect(unsigned int aBaudRate_bps)
{
    // MSG_DEBUG("Connect(  )");

    Serial2.begin(aBaudRate_bps, SERIAL_8N1, 36, 4, false);
}

EthCAN_Result EnterSettingMode()
{
    // MSG_DEBUG("EnterSettingMode()");

    Serial2.print("+++");
    EthCAN_Result lResult = WaitResponse("ENTER INTO SETTING MODE\r\n", 25);
    if (EthCAN_OK != lResult)
    {
        Connect(BAUD_RATE_DEFAULT_bps);
        Serial2.print("+++");
        lResult = WaitResponse("ENTER INTO SETTING MODE\r\n", 25);
        if (EthCAN_OK == lResult)
        {
            Serial2.println("AT+S=4");
            lResult = WaitResponse("OK\r\n", 4);
            if (EthCAN_OK == lResult)
            {
                Connect(BAUD_RATE_WORK_bps);
            }
        }
    }

    return lResult;
}

EthCAN_Result LeaveSettingMode()
{
    // MSG_DEBUG("LeaveSettingMode()");

    Serial2.println("AT+Q");

    return WaitResponse("OK\r\nENTER DATA MODE\r\n", 21);
}

EthCAN_Result WaitResponse(const char * aExpected, unsigned int aLength)
{
    // MSG_DEBUG("WaitResponse( ,  )");

    const char * lExp = aExpected;
    const char * lEr0 = ERROR_0;

    for (unsigned int lRetry = 0; lRetry < 10; lRetry ++)
    {
        while (Serial2.available())
        {
            char lC = Serial2.read();

            if (*lExp == lC)
            {
                lExp ++;
                if ('\0' == *lExp)
                {
                    // MSG_DEBUG("WaitResponse - EthCAN_OK");
                    return EthCAN_OK;
                }
            }
            else
            {
                lExp = aExpected;
            }

            if (*lEr0 == lC)
            {
                lEr0 ++;
                if ('\0' == *lEr0)
                {
                    return Info_Count_Error(__LINE__, EthCAN_ERROR_CAN);
                }
            }
            else
            {
                lEr0 = ERROR_0;
            }
        }

        delay(100);
    }

    MSG_ERROR("WaitResponse - EthCAN_ERROR_TIMEOUT", "");
    return EthCAN_ERROR_TIMEOUT;
}

uint32_t IdFromBytes(const uint8_t * aIn)
{
    MSG_DEBUG("IdFromBytes(  )");

    uint32_t lResult = 0;

    for (unsigned int i = 0; i < 4; i ++)
    {
        lResult <<= 8;
        lResult |= sBuffer[i];
    }

    return lResult;
}

void IdToBytes(uint8_t * aOut, uint32_t aIn)
{
    MSG_DEBUG("IdToBytes( ,  )");

    aOut[0] = (aIn >> 24) & 0xff;
    aOut[1] = (aIn >> 16) & 0xff;
    aOut[2] = (aIn >>  8) & 0xff;
    aOut[3] = (aIn      ) & 0xff;
}
