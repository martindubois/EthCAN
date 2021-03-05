
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/MCP2515.h

#pragma once

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef union
{
    uint8_t mData[4];

    struct
    {
        unsigned mId_10_3     : 8;
        unsigned mExtId_17_16 : 2;

        unsigned mReserved0 : 1;

        unsigned mExtended : 1;

        unsigned mReserved1 : 1;

        unsigned mId_2_0 : 3;

        unsigned mExtId_15_8 : 8;
        unsigned mExtId_7_0  : 8;
    };

    uint32_t mValue;
}
MCP_Id;

// Constants
/////////////////////////////////////////////////////////////////////////////

#define MCP_CNF_QTY (3)
#define MCP_RXB_QTY (2)
#define MCP_RXF_QTY (6)
#define MCP_RXM_QTY (2)
#define MCP_TXB_QTY (3)

// ===== Status register ====================================================
#define MCP_STATUS_RXxIF(I)   (0x01 << (I))
#define MCP_STATUS_RXxIF_MASK (0x03)

// ===== Register offsets ===================================================

#define MCP_B_CTRL        (0)
#define MCP_B_ID          (1)
#define MCP_B_DATA_LENGTH (5)
#define MCP_B_DATA        (6)

// MCB_B_DATA_LENGTH
#define MCP_B_DATA_LENGTH_MASK (0x0F)

// ===== Register addresses =================================================

static const uint8_t MCP_RXFxID[MCP_RXF_QTY] = { 0x00, 0x04, 0x08, 0x10, 0x14, 0x18 };

#define MCP_RX_BFP_CTRL (0x0c) // TODO
#define MCP_TX_RTS_CTRL (0x0d) // TODO

#define MCP_CAN_CTRL (0x0f)

#define MCP_TX_ERROR_COUNTER (0x1c)
#define MCP_RX_ERROR_COUNTER (0x1d)

static const uint8_t MCP_RXMxID[MCP_RXM_QTY] = { 0x20, 0x24 };

static const uint8_t MCP_CNF[MCP_CNF_QTY] = { 0x2A, 0x29, 0x28 };

#define MCP_CAN_INTE   (0x2B)
#define MCP_CAN_INTF   (0x2C)
#define MCP_ERROR_FLAG (0x2d)

static const uint8_t MCP_TXB[MCP_TXB_QTY] = { 0x30, 0x40, 0x50 };
// See MCP_B_...

static const uint8_t MCP_RXB[MCP_RXB_QTY] = { 0x60, 0x70 };
// See MCP_B_...

// CANCTRL
#define MCP_CAN_CTRL_MODE_NORMAL (0x00)
#define MCP_CAN_CTRL_MODE_CONFIG (0x80)
#define MCP_CAN_CTRL_MODE_MASK   (0xe0)

// MCP_CANINTE
#define MCP_RXxIF(I)   (0x01 << (I))
#define MCP_RXxIF_MASK (0x03)

// MCP_TXB_CTRL
#define MCP_TXB_CTRL_REQ_M (0x08)

// MCP_RXB_CTRL
#define MCP_RXB_CTRL_BUKT_MASK (0x04)
#define MCP_RXB_CTRL_RTR       (0x08)
#define MCP_RXB_CTRL_MF_OFF    (0x60)
#define MCP_RXB_CTRL_MF_MASK   (0x60)

// ===== SPI Instructions ===================================================
#define MCP_WRITE       (0x02)
#define MCP_READ        (0x03)
#define MCP_BITMOD      (0x05)
#define MCP_READ_STATUS (0xa0)
#define MCP_RESET       (0xc0)
