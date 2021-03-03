
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Common/InternalProtocol.h

#pragma once

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
//        IntPro_Config_Set
// Output uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_Result)      EthCAN_OK

typedef struct
{
    uint32_t mFilters[6];
    uint32_t mMasks  [2];

    // 6 * 4 + 2 * 4
    // = 24  + 8
    // = 32

    uint8_t  mReserved0[40 - 32 - 2];

    // 1 + 1
    // 2

    uint8_t  mFlags;
    uint8_t  mRate;
}
IntPro_Config_Set;

// ===== EthCAN_REQUEST_INGO_GET ============================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_INFO_GET
// Output uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_Result)      EthCAN_OK
//        IntPro_Info_Get

typedef struct
{
    uint8_t mFirmware[4];

    uint8_t mResult_CAN;

    uint8_t mReserved0[3];
}
IntPro_Info_Get;

// ===== EthCAN_REQUEST_RESET ===============================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_RESET
// Output uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_Result)      EthCAN_OK

// ===== EthCAN_REQUEST_SEND ================================================
// Input  uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_RequestCode) EthCAN_REQUEST_SEND
//        EthCAN_Frame                 (Partial)
// Output uint8_t                      EthCAN_SYNC
//        uint8_t (EthCAN_Result)      EthCAN_OK
