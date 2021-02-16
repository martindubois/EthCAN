
// Product EthCan

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Types.h
/// \brief     EthCAN_Config, EthCAN_Frame, EthCAN_Info, EthCAN_Rate

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define EthCAN_ID_EXTENDED (0x80000000)

#define EthCAN_FLAG_EXTENDED (0x01)

#define EthCAN_FLAG_STORE_CAN    (0x01)
#define EthCAN_FLAG_STORE_IPv4   (0x02)
#define EthCAN_FLAG_STORE_SERVER (0x04)
#define EthCAN_FLAG_STORE_WIFI   (0x08)

#define EthCAN_FLAG_STORE_ALL (0x0f)

#define EthCAN_FLAG_SERVER_USB (0x01)

#define EthCAN_FLAG_WIFI_AP (0x01)

// Data type
/////////////////////////////////////////////////////////////////////////////

/// \brief EthCAN_Config
/// \note If the WiFi is configured, the wired Ethernet is disabled.
/// \note Static IPv4 address is configured for WiFi and wired Ethernet.
/// \note If the USB server is enabled, information about the networked
///       server is ignored.
typedef struct
{
    char mName[16]; ///< The device name - Used for the DHCP request and to find the device

    uint32_t mCAN_Filters[6]; ///< The CAN filters
    uint32_t mCAN_Masks[2];   ///< The CAN mask
    uint8_t  mCAN_Rate;       ///< See EthCAN_Rate

    uint8_t mReserved0[3];

    uint32_t mIPv4_Addr;    ///< The static IPv4 address - If 0, the device use DHCP
    uint32_t mIPv4_Gateway; ///< The gateway address
    uint32_t mIPv4_Mask;    ///< The static IPv4 mask

    uint32_t mServer_IPv4;  ///< The server IPv4 address
    uint16_t mServer_Port;  ///< The server port - If 0, the server is disabled
    uint8_t  mServer_Flags; ///< Is the USB server enabled?

    // 16 + 24 + 8 + 1 + 3 + 4 + 4 + 4 + 4 + 2 + 1
    // = 48    + 9     + 7     + 8     + 6     + 1
    // = 57            + 15            + 7
    // = 72                            + 7
    // = 79

    uint8_t mReserved1[192 - 79 - 65];

    // = 65 bytes

    char mWiFi_Flags;        ///< Is the WiFi access poing enabled?
    char mWiFi_Name    [32]; ///< The WiFi name - If empty, the WiFi is disabled
    char mWiFi_Password[32]; ///< The WiFi password
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

    uint32_t mIPv4_Addr;    ///< The current IPv4 address
    uint32_t mIPv4_Gateway; ///< The current IPv4 gateway
    uint32_t mIPv4_Mask;    ///< The current IPv4 mask

    uint8_t mFirmware0_Major;  ///< The firmware 0 major
    uint8_t mFirmware0_Minor;  ///< The firmware 0 minor
    uint8_t mFirmware0_Build;  ///< The firmware 0 build
    uint8_t mFirmware0_Compat; ///< The firmware 0 compatibility

    uint8_t mFirmware1_Major;  ///< The firmware 1 major
    uint8_t mFirmware1_Minor;  ///< The firmware 1 minor
    uint8_t mFirmware1_Build;  ///< The firmware 1 build
    uint8_t mFirmware1_Compat; ///< The firmware 1 compatibility

    uint8_t mHardware_Major;  ///< The hardware major
    uint8_t mHardware_Minor;  ///< The hardware minor
    uint8_t mHardware_Rev;    ///< The hardware revision
    uint8_t mHardware_Compat; ///< The hardware compatibility

    uint32_t mMessageId;     ///< The last message id
    uint32_t mRequestId_UDP; ///< The last request id
    uint32_t mRequestId_USB; ///< The last request id

    // 16 + 6 + 2 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4
    // = 22   + 6     + 8     + 8     + 8     + 8
    // = 28           + 16            + 16
    // = 44                           + 16
    // = 60

    uint8_t mReserved1[ 128 - 60 ];
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

    EthCAN_RATE_QTY
}
EthCAN_Rate;
