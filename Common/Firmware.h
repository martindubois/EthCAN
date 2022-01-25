
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021-2022 KMS
// Product   EthCAN
// File      Common/Firmware.h

// This file defines data the 2 firmwares exchange.

// CODE REVIEW 2021-03-23 KMS - Martin Dubois, P.Eng.

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define FIRMWARE_BAUD_RATE_bps (1000000)

// Data types
/////////////////////////////////////////////////////////////////////////////

// ===== EthCAN_REQUEST_CONFIG_RESET ========================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_CONFIG_RESET
// Output uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_Result)      EthCAN_OK

// ===== EthCAN_REQUEST_CONFIG_SET ==========================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_CONFIG_SET
//        FW_Config

typedef struct
{
    uint32_t mFilters[EthCAN_FILTER_QTY];
    uint32_t mMasks  [EthCAN_MASK_QTY];

    // 6 * 4 + 2 * 4
    // = 24  + 8
    // = 32

    uint8_t  mReserved0[48 - 32 - 2];

    // 1 + 1
    // 2

    uint8_t  mFlags;
    uint8_t  mRate;
}
FW_Config;

// ===== EthCAN_REQUEST_INGO_GET ============================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_INFO_GET
// Output uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_Result)      EthCAN_OK
//        FW_Info

typedef struct
{
    uint8_t mFirmware[EthCAN_VERSION_SIZE_byte];

    uint16_t mDebug[2];

    uint8_t mReserved0[16 - 8 - 4];

    uint8_t mErrors;
    uint8_t mResult;
    uint8_t mRxErrors;
    uint8_t mTxErrors;
}
FW_Info;

// ===== EthCAN_REQUEST_SEND ================================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_SEND
//        EthCAN_Frame                 (Partial)
