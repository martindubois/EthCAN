
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/CAN.cpp

#include <Arduino.h>

#include "Component.h"

// ===== Arduino ============================================================
#include <SPI.h>

// ===== Firmware1 ==========================================================
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
    { 0x0f, 0xf1, 0x85 }, //  21.25 Kb
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
    { 0x00, 0xd0, 0x82 }, // 1 Mb
};

// ===== Pins ==============================================================
#define SPI_CS (9)

// Static function declarations
////////////////////////////////////////////////////////////////////////////

static void ClearRxBuffers();
static void ClearTxBuffers();

static uint32_t Id_FromMCP(const MCP_Id & aIn);
static void     Id_ToMCP  (MCP_Id * aOut, uint32_t aIn);
static void     Id_Write  (uint8_t aReg, uint32_t aIn);
static void     Id_Write  (uint8_t aReg, uint32_t aIn, bool aAdvanced);

static bool IsFrameAvailable();

static void Msg_Read(EthCAN_Frame * aFrame);
static void Msg_Read(EthCAN_Frame * aFrame, uint8_t aAddr);

static void   Register_Modify(uint8_t aReg, uint8_t aMask, uint8_t aVal);
static int8_t Register_Read  (uint8_t aReg);
static void   Register_Set   (uint8_t aReg, uint8_t aVal);

static void Registers_Read(uint8_t * aOut, uint8_t aReg, unsigned int aSize_byte);
static void Registers_Set (uint8_t aReg, const uint8_t * aIn, unsigned int aSize_byte);

static void Reset();

static EthCAN_Result SetMode(uint8_t aMode);

static void SetRate(EthCAN_Rate aRate);

static void Select();
static void Unselect();

static uint8_t Status_Read();

static void SPI_Begin();
static void SPI_End();

// Function
////////////////////////////////////////////////////////////////////////////

void CAN_Begin()
{
    MSG_DEBUG("CAN_Begin()");

    pinMode(SPI_CS, OUTPUT);
    Unselect();
    SPI.begin();
}

void CAN_Loop()
{
    if (IsFrameAvailable())
    {
        MSG_DEBUG("CAN_Loop - Frame");

        EthCAN_Frame lFrame;

        Msg_Read(&lFrame);

        Cmd_Send(lFrame);
    }
}

EthCAN_Result CAN_Config_Reset()
{
    MSG_DEBUG("CAN_Config_Reset()");

    Reset();

    EthCAN_Result lResult = SetMode(MCP_CAN_CTRL_MODE_CONFIG);
    if (EthCAN_OK == lResult)
    {
        SetRate(EthCAN_RATE_500_Kb);

        ClearTxBuffers();
        ClearRxBuffers();

        Register_Set(MCP_CAN_INTE, MCP_RX0IF | MCP_RX1IF);

        Register_Modify(MCP_RXB[0] + MCP_B_CTRL, MCP_RXB_CTRL_MF_OFF | MCP_RXB_CTRL_BUKT_MASK, MCP_RXB_CTRL_BUKT_MASK);
        Register_Modify(MCP_RXB[1] + MCP_B_CTRL, MCP_RXB_CTRL_MF_OFF                         , 0);

        lResult = SetMode(MCP_CAN_CTRL_MODE_NORMAL);
    }

    return lResult;
}

EthCAN_Result CAN_Config_Set(const IntPro_Config_Set & aConfig)
{
    MSG_DEBUG("CAN_Config_Set(  )");

    Reset();

    EthCAN_Result lResult = SetMode(MCP_CAN_CTRL_MODE_CONFIG);
    if (EthCAN_OK == lResult)
    {
        SetRate(aConfig.mRate);

        ClearTxBuffers();
        ClearRxBuffers();

        Register_Set(MCP_CAN_INTE, MCP_RX0IF | MCP_RX1IF);

        if (EthCAN_FLAG_CAN_FILTERS_OFF == (aConfig.mFlags & EthCAN_FLAG_CAN_FILTERS_OFF))
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

        lResult = SetMode(MCP_CAN_CTRL_MODE_NORMAL);
    }

    return lResult;
}

#define RETRY_COUNT (21)

EthCAN_Result CAN_Send(const EthCAN_Frame & aFrame)
{
    MSG_DEBUG("CAN_Send(  )");

    uint8_t lCtrl;
    uint8_t lRetry;
    uint8_t lTxA;
    uint8_t lTxB = 0;

    for (lRetry = 0; lRetry < RETRY_COUNT; lRetry++)
    {
        lTxA = MCP_TXB[lTxB];

        lCtrl = Register_Read(lTxA + MCP_B_CTRL);
        if (0 == (lCtrl & MCP_TXB_CTRL_REQ_M))
        {
            break;
        }

        lTxB = (lTxB + 1) % MCP_TXB_QTY;
    }
    
    if (RETRY_COUNT <= lRetry)
    {
        MSG_ERROR("CAN_Send - EthCAN_ERROR_TIMEOUT", "");
        return EthCAN_ERROR_TIMEOUT;
    }

    Registers_Set(lTxA + MCP_B_DATA, aFrame.mData, aFrame.mDataSize_byte);

    Register_Set(lTxA + MCP_B_DATA_LENGTH, aFrame.mDataSize_byte);

    Id_Write(lTxA + MCP_B_ID, aFrame.mId);

    Register_Modify(lTxA + MCP_B_CTRL, MCP_TXB_CTRL_REQ_M, MCP_TXB_CTRL_REQ_M);

    for (lRetry = 0; lRetry < RETRY_COUNT; lRetry++)
    {
        lCtrl = Register_Read(lTxA + MCP_B_CTRL);
        if (0 == (lCtrl & MCP_TXB_CTRL_REQ_M))
        {
            return EthCAN_OK;
        }
    }

    MSG_ERROR("CAN_Send - EthCAN_ERROR_TIMEOUT", "");
    return EthCAN_ERROR_TIMEOUT;
}

// Static functions
////////////////////////////////////////////////////////////////////////////

void ClearRxBuffers()
{
    for (unsigned int i = 0; i < MCP_RXB_QTY; i++)
    {
        Register_Set(MCP_RXB[i] + MCP_B_CTRL, 0);
    }
}

void ClearTxBuffers()
{
    for(unsigned int i = 0; i < MCP_TXB_QTY; i++)
    {
        for (unsigned int j = 0; j < 14; j++)
        {
            Register_Set(MCP_TXB[i] + j, 0);
        }
    }
}

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

    if (EthCAN_ID_EXTENDED != (aIn & EthCAN_ID_EXTENDED))
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

bool IsFrameAvailable()
{
    uint8_t lStatus = Status_Read();

    return 0 != (lStatus & MCP_STATUS_RXxIF_MASK);
}

void Msg_Read(EthCAN_Frame * aFrame)
{
    uint8_t lStatus = Status_Read();
    if (0 != (lStatus & MCP_STATUS_RX0IF))
    {
        Msg_Read(aFrame, MCP_RXB[0]);

        Register_Modify(MCP_CAN_INTF, MCP_RX0IF, 0);
    }
    else if (0 != (lStatus & MCP_STATUS_RX1IF))
    {
        Msg_Read(aFrame, MCP_RXB[1]);

        Register_Modify(MCP_CAN_INTF, MCP_RX1IF, 0);
    }
}

void Msg_Read(EthCAN_Frame * aFrame, uint8_t aAddr)
{
    MCP_Id lId;

    Registers_Read(lId.mData, aAddr + MCP_B_ID, sizeof(lId));

    aFrame->mId = Id_FromMCP(lId);

    uint8_t lCtrl = Register_Read(aAddr + MCP_B_CTRL);

    aFrame->mDataSize_byte = Register_Read(aAddr + MCP_B_DATA_LENGTH) & MCP_B_DATA_LENGTH_MASK;

    if (MCP_RXB_CTRL_RTR == (lCtrl & MCP_RXB_CTRL_RTR))
    {
        aFrame->mDataSize_byte |= EthCAN_FLAG_CAN_RTR;
    }

    Registers_Read(aFrame->mData, aAddr + MCP_B_DATA, aFrame->mDataSize_byte);
}

void Register_Modify(uint8_t aReg, uint8_t aMask, uint8_t aVal)
{
    SPI_Begin();
    {
        SPI.transfer(MCP_BITMOD);
        SPI.transfer(aReg);
        SPI.transfer(aMask);
        SPI.transfer(aVal);
    }
    SPI_End();
}

int8_t Register_Read(uint8_t aReg)
{
    uint8_t lResult;

    SPI_Begin();
    {
        SPI.transfer(MCP_READ);
        SPI.transfer(aReg);

        lResult = SPI.transfer(0x00);
    }
    SPI_End();

    return lResult;
}

void Register_Set(uint8_t aReg, uint8_t aVal)
{
    SPI_Begin();
    {
        SPI.transfer(MCP_WRITE);
        SPI.transfer(aReg);
        SPI.transfer(aVal);
    }
    SPI_End();
}

void Registers_Read(uint8_t * aOut, uint8_t aReg, unsigned int aSize_byte)
{
    SPI_Begin();
    {
        SPI.transfer(MCP_READ);
        SPI.transfer(aReg);

        for(unsigned int i = 0; i < aSize_byte; i++)
        {
            aOut[i] = SPI.transfer(0x00);
        }
    }
    SPI_End();
}

void Registers_Set(uint8_t aReg, const uint8_t * aIn, unsigned int aSize_byte)
{
    SPI_Begin();
    {
        SPI.transfer(MCP_WRITE);
        SPI.transfer(aReg);

        for(unsigned int i = 0; i < aSize_byte; i++)
        {
            SPI.transfer(aIn[i]);
        }
    }
    SPI_End();
}

void Reset()
{
    SPI_Begin();
    {
        SPI.transfer(MCP_RESET);
    }
    SPI_End();

    delay(10);
}

EthCAN_Result SetMode(uint8_t aMode)
{
    Register_Modify(MCP_CAN_CTRL, MCP_CAN_CTRL_MODE_MASK, aMode);

    uint8_t lCtrl = Register_Read(MCP_CAN_CTRL);

    delay(10);

    if (aMode != (lCtrl & MCP_CAN_CTRL_MODE_MASK))
    {
        MSG_ERROR("SetMode - EthCAN_ERROR_CAN - ", lCtrl);
        return EthCAN_ERROR_CAN;
    }

    return EthCAN_OK;
}

void SetRate(EthCAN_Rate aRate)
{
    for (unsigned int i = 0; i < 3; i++)
    {
        Register_Set(MCP_CNF[i], CONFIG_BYTE[aRate][i]);
    }

    delay(10);
}

void Select()
{
    digitalWrite(SPI_CS, LOW);
}

void Unselect()
{
    digitalWrite(SPI_CS, HIGH);
}

uint8_t Status_Read()
{
    uint8_t lResult;

    SPI_Begin();
    {
        SPI.transfer(MCP_READ_STATUS);

        lResult = SPI.transfer(0x00);
    }
    SPI_End();

    return lResult;
}

void SPI_Begin()
{
    #ifdef SPI_HAS_TRANSACTION
        SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    #endif

    Select();
}

void SPI_End()
{
    Unselect();

    #ifdef SPI_HAS_TRANSACTION
        SPI.endTransaction();
    #endif
}
