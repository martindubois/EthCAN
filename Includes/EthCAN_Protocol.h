
// Product EthCan

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Protocol.h
/// \brief     EthCAN_Header, EthCAN_RequestCode

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define EthCAN_UDP_PORT (3489)

#define EthCAN_SYNC (0xff)

// Constants
/////////////////////////////////////////////////////////////////////////////

#define EthCAN_FLAG_NO_RESPONSE (0x01)

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

    uint16_t mResult; ///< See EthCAN_Result

    uint32_t mSalt; ///< Random value
    uint32_t mSign; ///< Hash
}
EthCAN_Header;

/// \brief EthCAN_RequestCode
typedef enum
{
    EthCAN_REQUEST_CONFIG_GET,
    EthCAN_REQUEST_CONFIG_RESET,
    EthCAN_REQUEST_CONFIG_SET,
    EthCAN_REQUEST_CONFIG_STORE,
    EthCAN_REQUEST_INFO_GET,
    EthCAN_REQUEST_RESET,
    EthCAN_REQUEST_SEND,

    EthCAN_REQUEST_QTY
}
EthCAN_RequestCode;
