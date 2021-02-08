
// Product EthCan

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Types.h
/// \brief     EthCAN_Config, EthCAN_Frame, EthCAN_Info, EthCAN_Rate

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define EthCAN_FLAG_EXTENDED (0x01)

// Data type
/////////////////////////////////////////////////////////////////////////////

/// \brief EthCAN_Config
typedef struct
{
    char mName[16]; ///< The device name

    uint32_t mCAN_Filters[6]; ///< The CAN filters
    uint32_t mCAN_Masks[2]; ///< The CAN mask
    uint8_t  mCAN_Rate; ///< See EthCAN_Rate

    uint8_t mReserved0[31];

    uint32_t mIPv4_Addr; ///< The static IPv4 address
    uint32_t mIPv4_Mask; ///< The static IPv4 mask

    uint8_t  mMessageDest_IPv4[4]; ///< The message destination IPv4 address
    uint16_t mMessageDest_Port; ///< The message destination port

    uint8_t  mReserved1[128 - 46];
}
EthCAN_Config;

/// \brief EthCAN_Frame
typedef struct
{
    uint32_t mDestId; ///< The destination id

    uint8_t mFlags; ///< See EthCAN_FLAG_...
    uint8_t mDataSize_byte; ///< The data size (in byte)

    uint8_t mReserved0[2];

    uint8_t mData[8]; ///< The data
}
EthCAN_Frame;

/// \brief EthCAN_Info
typedef struct
{
    char mName[16]; ///< Name

    uint8_t mEth_Addr[6]; ///< The Ethernet address

    uint8_t mReserved0[2];

    uint32_t mIPv4_Addr; ///< The current IPv4 address
    uint32_t mIPv4_Mask; ///< The current IPv4 mask

    uint8_t mFirmware0_Major; ///< The firmware 0 major
    uint8_t mFirmware0_Minor; ///< The firmware 0 minor
    uint8_t mFirmware0_Build; ///< The firmware 0 build
    uint8_t mFirmware0_Compat; ///< The firmware 0 compatibility

    uint8_t mFirmware1_Major; ///< The firmware 1 major
    uint8_t mFirmware1_Minor; ///< The firmware 1 minor
    uint8_t mFirmware1_Build; ///< The firmware 1 build
    uint8_t mFirmware1_Compat; ///< The firmware 1 compatibility

    uint8_t mHardware_Major; ///< The hardware major
    uint8_t mHardware_Minor; ///< The hardware minor
    uint8_t mHardware_Rev; ///< The hardware revision
    uint8_t mHardware_Compat; ///< The hardware compatibility

    uint32_t mMessageId; ///< The last message id
    uint32_t mRequestId; ///< The last request id

    uint8_t mReserved1[ 128 - 52 ];
}
EthCAN_Info;

/// \brief EthCAN_Rate
typedef enum
{
    EthCAN_RATE_5_Kb,
    EthCAN_RATE_10_Kb,
    EthCAN_RATE_20_Kb,
    EthCAN_RATE_25_Kb,
    EthCAN_RATE_31_2_Kb,
    EthCAN_RATE_33_Kb,
    EthCAN_RATE_40_Kb,
    EthCAN_RATE_50_Kb,
    EthCAN_RATE_80_Kb,
    EthCAN_RATE_83_3_Kb,
    EthCAN_RATE_95_Kb,
    EthCAN_RATE_100_Kb,
    EthCAN_RATE_125_Kb,
    EthCAN_RATE_200_Kb,
    EthCAN_RATE_250_Kb,
    EthCAN_RATE_500_Kb,
    EthCAN_RATE_666_Kb,
    EthCAN_RATE_1_Mb,
}
EthCAN_Rate;
