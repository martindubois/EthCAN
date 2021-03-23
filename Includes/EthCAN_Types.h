
// Product EthCan

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Types.h
/// \brief     EthCAN_Config, EthCAN_Frame, EthCAN_Info, EthCAN_Rate

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define CAN_FLAG_RTR         (0x40)
#define CAN_HEADER_SIZE_byte (5)
#define CAN_OFFSET_DATA_SIZE (4)

#define EthCAN_FILTER_QTY (6)

#define EthCAN_FLAG_CAN_ADVANCED   (0x01)
#define EthCAN_FLAG_CAN_FILTERS_ON (0x02)

#define EthCAN_FLAG_STORE_CAN    (0x01)
#define EthCAN_FLAG_STORE_IPv4   (0x02)
#define EthCAN_FLAG_STORE_SERVER (0x04)
#define EthCAN_FLAG_STORE_WIFI   (0x08)

#define EthCAN_FLAG_STORE_ALL (0x0f)

#define EthCAN_FLAG_SERVER_USB (0x01)

#define EthCAN_FLAG_WIFI_AP (0x01)

#define EthCAN_ID_EXTENDED (0x80000000)

#define EthCAN_MASK_QTY (2)

#define EthCAN_VERSION_SIZE_byte (4)

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

    uint32_t mCAN_Filters[EthCAN_FILTER_QTY]; ///< The CAN filters
    uint32_t mCAN_Masks  [EthCAN_MASK_QTY  ]; ///< The CAN mask

    uint8_t  mCAN_Flags; ///< See EthCAN_FLAG_CAN_ADVANCED
    uint8_t  mCAN_Rate;  ///< See EthCAN_Rate

    uint8_t mReserved0[2];

    uint32_t mIPv4_Address; ///< The static IPv4 address - If 0, the device use DHCP
    uint32_t mIPv4_Gateway; ///< The gateway address
    uint32_t mIPv4_NetMask; ///< The static IPv4 mask

    uint32_t mServer_IPv4;  ///< The server IPv4 address
    uint16_t mServer_Port;  ///< The server port - If 0, the server is disabled
    uint8_t  mServer_Flags; ///< Is the USB server enabled?

    // 16 + 6 * 4 + 2 * 4 + 2 * 1 + 2 + 3 * 4 + 4 + 2 + 1
    // = 16 + 24  + 8     + 2     + 2 + 12    + 6     + 1
    // = 40       + 10            + 14        + 7
    // = 50                       + 21
    // = 71

    uint8_t mReserved1[192 - 71 - 65];

    // 1 + 32 + 32
    // = 65 bytes

    char mWiFi_Flags;        ///< Is the WiFi access poing enabled?
    char mWiFi_Name    [32]; ///< The WiFi name - If empty, the WiFi is disabled
    char mWiFi_Password[32]; ///< The WiFi password
}
EthCAN_Config;

#define EthCAN_NAME_DEFAULT ("EthCAN")

#define EthCAN_CONFIG_DEFAULT(C)               \
    memset((C), 0, sizeof(EthCAN_Config));     \
    strcpy_s((C)->mName, EthCAN_NAME_DEFAULT); \
    (C)->mCAN_Rate = EthCAN_RATE_DEFAULT

/// \brief EthCAN_Frame
typedef struct
{
    uint32_t mId; ///< The id

    uint8_t mDataSize_byte; ///< The data size (in byte) and possibly CAN_FLAG_RTR

    uint8_t mData[8]; ///< The data

    uint8_t mReserved0[3];
}
EthCAN_Frame;

#define EthCAN_FRAME_DATA_SIZE(F)  ((F).mDataSize_byte & ~ CAN_FLAG_RTR)
#define EthCAN_FRAME_EXTENDED(F)   (EthCAN_ID_EXTENDED == ((F).mId & EthCAN_ID_EXTENDED))
#define EthCAN_FRAME_ID(F)         ((F).mId & ~ EthCAN_ID_EXTENDED)
#define EthCAN_FRAME_RTR(F)        (CAN_FLAG_RTR == ((F).mDataSize_byte & CAN_FLAG_RTR))
#define EthCAN_FRAME_TOTAL_SIZE(F) (CAN_HEADER_SIZE_byte + EthCAN_FRAME_DATA_SIZE(F))

/// \brief EthCAN_Info
typedef struct
{
    char mName[16]; ///< Name

    uint8_t mEth_Address[6]; ///< The MAC address on the wired Ethernet

    uint8_t mReserved0[2];

    uint32_t mIPv4_Address; ///< The current IPv4 address
    uint32_t mIPv4_Gateway; ///< The current IPv4 gateway
    uint32_t mIPv4_NetMask; ///< The current IPv4 mask

    uint8_t mFirmware0_Version[4];  ///< The firmware 0 version
    uint8_t mFirmware1_Version[4];  ///< The firmware 1 version (Valid if mFirmware1_Result == EthCAN_OK)
    uint8_t mHardware_Version [4];  ///< The hardware version

    uint8_t mFirmware1_Result; ///< The result of the info retrieval from the firmware 1

    // 16 + 6 + 2 + 3 * 4 + 3 * 4 + 1
    // = 22   + 2 + 12    + 12    + 1
    // = 24       + 24            + 1
    // = 48                       + 1
    // = 49

    uint8_t mReserved2[192 - 49 - 78];

    // 2 * 1 + 13 * 4 + 2 * 1 + 2 + 2 * 4 + 2 * 2 + 2 * 1 + 6
    // = 2   + 52    + 2     + 2 + 8     + 4     + 2     + 6
    // = 54          + 4         + 12            + 8
    // = 58                      + 20
    // = 78

    uint8_t mCAN_Errors; ///< CAN error flags (Valid if mFirmware1_Result == EthCAN_OK)
    uint8_t mCAN_Result; ///< CAN initialisation result (Valid if mFirmware1_Result == EthCAN_OK)

    uint32_t mCounter_Debug[4]; ///< Counters used for debuggings
    uint32_t mCounter_Errors  ; ///< Errors
    uint32_t mCounter_Events  ; ///< Events
    uint32_t mCounter_Fx_byte ; ///< Byte formwarded
    uint32_t mCounter_Fx_frame; ///< Frame forwarded
    uint32_t mCounter_Requests; ///< Request received
    uint32_t mCounter_Rx_byte ; ///< Byte receiver from the CAN bus
    uint32_t mCounter_Rx_frame; ///< Frame received from the CAN bus
    uint32_t mCounter_Tx_byte ; ///< Byte transmitted to the CAN bus
    uint32_t mCounter_Tx_frame; ///< Frame transmitted to the CAN bus

    uint8_t mCounter_RxErrors; ///< Errors as reported by the hardware (Valid if mFirmware1_Result == EthCAN_OK)
    uint8_t mCounter_TxErrors; ///< Errors as reported by the hardware (Valid if mFirmware1_Result == EthCAN_OK)

    uint8_t mReserver3[2];

    uint32_t mLast_Forward_Id  ; ///< The last message id
    uint32_t mLast_Rx_Id       ; ///< Last received CAN id
    uint16_t mLast_Error_Line  ; ///< Last error line
    uint16_t mLast_Request_Id  ; ///< Last request id
    uint8_t  mLast_Error_Code  ; ///< Last error
    uint8_t  mLast_Request_Code; ///< Last request code

    uint8_t mReserved4[6];

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

#define EthCAN_RATE_DEFAULT (EthCAN_RATE_500_Kb)
