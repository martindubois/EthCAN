
// Product EthCan

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Protocol.h
/// \brief     EthCAN_Header, EthCAN_RequestCode

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define EthCAN_FLAG_NO_RESPONSE (0x80)

#define EthCAN_FLAG_BUSY (0x80)

#define EthCAN_PACKET_SIZE_MAX_byte (256)

#define EthCAN_TCP_PORT (3489)

#define EthCAN_UDP_PORT (3489)

#define EthCAN_USB_SPEED_bps (115200)

#define EthCAN_SYNC (0xff)

// Data type
/////////////////////////////////////////////////////////////////////////////

/// \brief EthCAN_Header
typedef struct
{
    uint16_t mId; ///< The request id

    uint8_t mCode; ///< See EthCan_RequestCode
    uint8_t mDataSize_byte; ///< The data size (in byte)
    uint8_t mFlags; ///< See EthCAN_FLAG_...
    uint8_t mTotalSize_byte; ///< The total size (in byte)
    uint8_t mResult; ///< See EthCAN_Result

    uint8_t mReserved0;
}
EthCAN_Header;

/// \brief EthCAN_RequestCode
typedef enum
{
    EthCAN_REQUEST_CONFIG_ERASE,
    EthCAN_REQUEST_CONFIG_GET,
    EthCAN_REQUEST_CONFIG_RESET,
    EthCAN_REQUEST_CONFIG_SET,
    EthCAN_REQUEST_CONFIG_STORE,
    EthCAN_REQUEST_DO_NOTHING,
    EthCAN_REQUEST_INFO_GET,
    EthCAN_REQUEST_RESET,
    EthCAN_REQUEST_SEND,

    EthCAN_REQUEST_QTY,

    EthCAN_REQUEST_INVALID = 0xff
}
EthCAN_RequestCode;
