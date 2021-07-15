
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/CAN.cpp

// CODE REVIEW 2021-03-24 KMS - Martin Dubois, P.Eng.

#include <Arduino.h>

#include "Component.h"

// ===== Arduino ============================================================
#include <SPI.h>

// ===== Firmware1 ==========================================================
#include "Buffer.h"
#include "Cmd.h"
#include "MCP2515.h"

#include "CAN.h"

// Constants
////////////////////////////////////////////////////////////////////////////

static const uint8_t CONFIG_BYTE[EthCAN_RATE_QTY][MCP_CNF_QTY]
{
    { 0x3f, 0xff, 0x87 }, //   5 Kb
    { 0x1f, 0xff, 0x87 }, //  10 Kb
    { 0x0f, 0xff, 0x87 }, //  20 Kb
    { 0x0f, 0xba, 0x07 }, //  25 Kb
    { 0x0f, 0xf1, 0x85 }, //  31.25 Kb
    { 0x09, 0xbe, 0x07 }, //  33 Kb
    { 0x07, 0xff, 0x87 }, //  40 Kb
    { 0x07, 0xfa, 0x87 }, //  50 Kb
    { 0x03, 0xff, 0x87 }, //  80 Kb
    { 0x03, 0xbe, 0x07 }, //  83.3 Kb
    { 0x03, 0xad, 0x07 }, //  95 Kb
    { 0x03, 0xfa, 0x87 }, // 100 Kb
    { 0x03, 0xf0, 0x86 }, // 125 Kb
    { 0x01, 0xfa, 0x87 }, // 200 Kb
    { 0x41, 0xf1, 0x85 }, // 250 Kb
    { 0x00, 0xf0, 0x86 }, // 500 Kb
    { 0x00, 0xa0, 0x04 }, // 666 Kb
    { 0x00, 0xd0, 0x82 }, //   1 Mb
};

// ===== Pins ==============================================================
#define SPI_CS (9)

// Static variables
////////////////////////////////////////////////////////////////////////////

static EthCAN_Frame * sFrame = NULL;

static EthCAN_Result sResult = EthCAN_RESULT_NO_ERROR;

// Static function declarations
////////////////////////////////////////////////////////////////////////////

static void ClearBuffers();

static uint32_t Id_FromMCP(const MCP_Id & aIn);
static void     Id_ToMCP  (MCP_Id * aOut, uint32_t aIn);
static void     Id_Write  (uint8_t aReg, uint32_t aIn);
static void     Id_Write  (uint8_t aReg, uint32_t aIn, bool aAdvanced);

static void Loop_Rx();
static void Loop_Tx();

static void ReadFrame(uint8_t aIndex);

static void   Register_Modify(uint8_t aReg, uint8_t aMask, uint8_t aVal);
static int8_t Register_Read  (uint8_t aReg);
static void   Register_Set   (uint8_t aReg, uint8_t aVal);

static void Registers_Set(uint8_t aReg, const uint8_t * aIn, uint8_t aSize_byte);

static void Reset();

static EthCAN_Result SetMode(uint8_t aMode);

static void SetRate(EthCAN_Rate aRate);

static void WriteFrame(uint8_t aIndex);

// Macros
////////////////////////////////////////////////////////////////////////////

#define Select()   digitalWrite(SPI_CS, LOW )
#define Unselect() digitalWrite(SPI_CS, HIGH)

// Function
////////////////////////////////////////////////////////////////////////////

void CAN_Begin()
{
    pinMode(SPI_CS, OUTPUT);
    Unselect();
    SPI.begin();

    #ifdef SPI_HAS_TRANSACTION
        SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    #endif
}

void CAN_GetInfo(FW_Info * aInfo)
{
    aInfo->mErrors = Register_Read(MCP_ERROR_FLAG);
    aInfo->mResult = sResult;
    aInfo->mRxErrors = Register_Read(MCP_RX_ERROR_COUNTER);
    aInfo->mTxErrors = Register_Read(MCP_TX_ERROR_COUNTER);

    if (EthCAN_OK != aInfo->mResult) { MSG_ERROR("Result = ", aInfo->mResult); }

    if (0 < aInfo->mErrors  ) { MSG_ERROR("Errors    = ", aInfo->mErrors  ); }
    if (0 < aInfo->mRxErrors) { MSG_ERROR("Rx Errors = ", aInfo->mRxErrors); }
    if (0 < aInfo->mTxErrors) { MSG_ERROR("Tx Errors = ", aInfo->mTxErrors); }
}

// Critical path
void CAN_Loop()
{
    Loop_Rx();
    Loop_Tx();
}

EthCAN_Result CAN_Config_Reset()
{
    Reset();

    EthCAN_Result lResult = SetMode(MCP_CAN_CTRL_MODE_CONFIG);
    if (EthCAN_OK == lResult)
    {
        SetRate(EthCAN_RATE_DEFAULT);

        ClearBuffers();

        Register_Set(MCP_CAN_INTE, MCP_RXxIF_MASK);

        Register_Modify(MCP_RXB[0] + MCP_B_CTRL, MCP_RXB_CTRL_MF_OFF | MCP_RXB_CTRL_BUKT_MASK, MCP_RXB_CTRL_BUKT_MASK);
        Register_Modify(MCP_RXB[1] + MCP_B_CTRL, MCP_RXB_CTRL_MF_OFF                         , 0);

        lResult = SetMode(MCP_CAN_CTRL_MODE_NORMAL);
    }

    sResult = lResult;

    return lResult;
}

void CAN_Config_Set(const FW_Config & aConfig)
{
    Reset();

    sResult = SetMode(MCP_CAN_CTRL_MODE_CONFIG);
    if (EthCAN_OK == sResult)
    {
        SetRate(static_cast<EthCAN_Rate>(aConfig.mRate));

        ClearBuffers();

        Register_Set(MCP_CAN_INTE, MCP_RXxIF_MASK);

        if (0 == (aConfig.mFlags & EthCAN_FLAG_CAN_FILTERS_ON))
        {
            Register_Modify(MCP_RXB[0] + MCP_B_CTRL, MCP_RXB_CTRL_MF_OFF | MCP_RXB_CTRL_BUKT_MASK, MCP_RXB_CTRL_BUKT_MASK);
            Register_Modify(MCP_RXB[1] + MCP_B_CTRL, MCP_RXB_CTRL_MF_OFF                         , 0);
        }
        else
        {
            bool lAdvanced = EthCAN_FLAG_CAN_ADVANCED == (aConfig.mFlags & EthCAN_FLAG_CAN_ADVANCED);

            unsigned int i;

            for (i = 0; i < MCP_RXM_QTY; i++)
            {
                Id_Write(MCP_RXMxID[i], aConfig.mMasks[i], lAdvanced);
            }

            for (i = 0; i < MCP_RXF_QTY; i++)
            {
                Id_Write(MCP_RXFxID[i], aConfig.mFilters[i], lAdvanced);
            }

            Register_Modify(MCP_RXB[0] + MCP_B_CTRL, MCP_RXB_CTRL_BUKT_MASK, MCP_RXB_CTRL_BUKT_MASK);
            Register_Modify(MCP_RXB[1] + MCP_B_CTRL, 0                     , 0);
        }

        sResult = SetMode(MCP_CAN_CTRL_MODE_NORMAL);
    }
}

// Static functions
////////////////////////////////////////////////////////////////////////////

void ClearBuffers()
{
    uint8_t i;

    for(i = 0; i < MCP_TXB_QTY; i++)
    {
        for (unsigned int j = 0; j < 14; j++)
        {
            Register_Set(MCP_TXB[i] + j, 0);
        }
    }

    for (i = 0; i < MCP_RXB_QTY; i++)
    {
        Register_Set(MCP_RXB[i] + MCP_B_CTRL, 0);
    }
}

// Critical path
uint32_t Id_FromMCP(const MCP_Id & aIn)
{
    uint32_t lResult;
    
    lResult   = aIn.mId_10_3 ;
    lResult <<= 3;
    lResult  |= aIn.mId_2_0;

    if (aIn.mExtended)
    {
        lResult <<= 2;
        lResult  |= aIn.mExtId_17_16;
        lResult <<= 8;
        lResult  |= aIn.mExtId_15_8;
        lResult <<= 8;
        lResult  |= aIn.mExtId_7_0;

        lResult |= EthCAN_ID_EXTENDED;
    }

    return lResult;
}

void Id_ToMCP(MCP_Id * aOut, uint32_t aIn)
{
    aOut->mValue = 0;

    if (EthCAN_ID_EXTENDED == (aIn & EthCAN_ID_EXTENDED))
    {
        aOut->mExtended = true;

        aOut->mExtId_17_16 = (aIn >> 16) & 0x03;
        aOut->mExtId_15_8  = (aIn >>  8) & 0xff;
        aOut->mExtId_7_0   =  aIn        & 0xff;

        aOut->mId_10_3 = (aIn >> 21) & 0xff;
        aOut->mId_2_0  = (aIn >> 18) & 0x07;
    }
    else
    {
        aOut->mId_10_3 = (aIn >> 3) & 0xff;
        aOut->mId_2_0  =  aIn       & 0x07;
    }
}

// Critical path
void Id_Write(uint8_t aReg, uint32_t aIn)
{
    MCP_Id lId;

    Id_ToMCP(&lId, aIn);

    Registers_Set(aReg, lId.mData, sizeof(lId));
}

void Id_Write(uint8_t aReg, uint32_t aIn, bool aAdvanced)
{
    if (aAdvanced)
    {
        Registers_Set(aReg, reinterpret_cast<uint8_t *>(&aIn), sizeof(aIn));
    }
    else
    {
        Id_Write(aReg, aIn);
    }
}

// Critical path
void Loop_Rx()
{
    static uint8_t sIndex = 0;

    uint8_t lStatus;

    Select();
    {
        SPI.transfer(MCP_READ_STATUS);

        lStatus = SPI.transfer(0x00);
    }
    Unselect();

    switch (lStatus & MCP_STATUS_RXxIF_MASK)
    {
    case 0: break;

    case MCP_STATUS_RXxIF(0): ReadFrame(0); sIndex = 1; break;
    case MCP_STATUS_RXxIF(1): ReadFrame(1); sIndex = 0; break;

    case MCP_STATUS_RXxIF_MASK:
        if (0 == sIndex)
        {
            ReadFrame(0);
            ReadFrame(1);
        }
        else
        {
            ReadFrame(1);
            ReadFrame(0);
        }
        break;
    }
}

void Loop_Tx()
{
    static uint8_t sPrio;

    if (NULL == sFrame)
    {
        sFrame = Buffer_Pop(BUFFER_TYPE_TX_CAN);
    }

    if (NULL != sFrame)
    {
        uint8_t lCtrl[MCP_TXB_QTY];

        for (unsigned int i = 0; i < MCP_TXB_QTY; i ++)
        {
            lCtrl[i]= Register_Read(MCP_TXB[i] + MCP_B_CTRL);
        }

        if (0 == (lCtrl[0] & MCP_TXB_CTRL_REQ_M))
        {
            if (0 == (lCtrl[1] & MCP_TXB_CTRL_REQ_M))
            {
                sPrio = 3;
                WriteFrame(1);
            }
            else
            {
                WriteFrame(0);
            }
        }
        else
        {
            if (0 == (lCtrl[1] & MCP_TXB_CTRL_REQ_M))
            {
                if (0 < sPrio)
                {
                    sPrio--;
                }
                WriteFrame(1);
            }
        }
    }
}

void ReadFrame(uint8_t aIndex)
{
    EthCAN_Frame * lFrame = Buffer_Pop(BUFFER_TYPE_FREE);
    if (NULL != lFrame)
    {
        uint8_t lCtrl;
        MCP_Id  lId;
        uint8_t lSize_byte;

        Select();
        {
            SPI.transfer(MCP_READ);
            SPI.transfer(MCP_RXB[aIndex] + MCP_B_CTRL);

            lCtrl = SPI.transfer(0x00);

            lId.mData[0] = SPI.transfer(0x00);
            lId.mData[1] = SPI.transfer(0x00);
            lId.mData[2] = SPI.transfer(0x00);
            lId.mData[3] = SPI.transfer(0x00);

            lSize_byte = SPI.transfer(0x00) & MCP_B_DATA_LENGTH_MASK;
        }
        Unselect();

        Select();
        {
            SPI.transfer(MCP_READ);
            SPI.transfer(MCP_RXB[aIndex] + MCP_B_DATA);

            for(uint8_t i = 0; i < lSize_byte; i++)
            {
                lFrame->mData[i] = SPI.transfer(0x00);
            }
        }
        Unselect();

        Select();
        {
            SPI.transfer(MCP_BITMOD);
            SPI.transfer(MCP_CAN_INTF);
            SPI.transfer(MCP_RXxIF(aIndex));
            SPI.transfer(0);
        }
        Unselect();

        lFrame->mDataSize_byte = lSize_byte;
        lFrame->mId            = Id_FromMCP(lId);

        if (MCP_RXB_CTRL_RTR == (lCtrl & MCP_RXB_CTRL_RTR))
        {
            lFrame->mDataSize_byte |= CAN_FLAG_RTR;
        }

        Buffer_Push(lFrame, BUFFER_TYPE_TX_SERIAL);
    }
}

void Register_Modify(uint8_t aReg, uint8_t aMask, uint8_t aVal)
{
    Select();
    {
        SPI.transfer(MCP_BITMOD);
        SPI.transfer(aReg);
        SPI.transfer(aMask);
        SPI.transfer(aVal);
    }
    Unselect();
}

int8_t Register_Read(uint8_t aReg)
{
    uint8_t lResult;

    Select();
    {
        SPI.transfer(MCP_READ);
        SPI.transfer(aReg);

        lResult = SPI.transfer(0x00);
    }
    Unselect();

    return lResult;
}

void Register_Set(uint8_t aReg, uint8_t aVal)
{
    Select();
    {
        SPI.transfer(MCP_WRITE);
        SPI.transfer(aReg);
        SPI.transfer(aVal);
    }
    Unselect();
}

void Registers_Set(uint8_t aReg, const uint8_t * aIn, uint8_t aSize_byte)
{
    Select();
    {
        SPI.transfer(MCP_WRITE);
        SPI.transfer(aReg);

        for(unsigned int i = 0; i < aSize_byte; i++)
        {
            SPI.transfer(aIn[i]);
        }
    }
    Unselect();
}

void Reset()
{
    Select();
    {
        SPI.transfer(MCP_RESET);
    }
    Unselect();

    delay(10);
}

EthCAN_Result SetMode(uint8_t aMode)
{
    Register_Modify(MCP_CAN_CTRL, MCP_CAN_CTRL_MODE_MASK, aMode);

    uint8_t lCtrl = Register_Read(MCP_CAN_CTRL);
    if (aMode != (lCtrl & MCP_CAN_CTRL_MODE_MASK))
    {
        MSG_ERROR("SetMode - EthCAN_ERROR_CAN - ", lCtrl);
        return EthCAN_ERROR_CAN;
    }

    return EthCAN_OK;
}

void SetRate(EthCAN_Rate aRate)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        Register_Set(MCP_CNF[i], CONFIG_BYTE[aRate][i]);
    }
}

void WriteFrame(uint8_t aIndex)
{
    uint8_t lTxA = MCP_TXB[aIndex];

    Registers_Set(lTxA + MCP_B_DATA, sFrame->mData, EthCAN_FRAME_DATA_SIZE(*sFrame));

    Register_Set(lTxA + MCP_B_DATA_LENGTH, sFrame->mDataSize_byte);

    Id_Write(lTxA + MCP_B_ID, sFrame->mId);

    Register_Modify(lTxA + MCP_B_CTRL, MCP_TXB_CTRL_REQ_M, MCP_TXB_CTRL_REQ_M);

    Buffer_Push(sFrame, BUFFER_TYPE_FREE);
    sFrame = NULL;
}
