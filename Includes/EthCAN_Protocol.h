
// Product EthCan

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Protocol.h
/// \brief     EthCAN_Mesage, EthCAN_MessageCode, EthCAN_Request, EthCAN_RequestCode, EthCAN_Response

#pragma once

// Constants
/////////////////////////////////////////////////////////////////////////////

#define EthCAN_UDP_PORT (3489)

// Data type
/////////////////////////////////////////////////////////////////////////////

/// \brief EthCAN_Message
typedef struct
{
    uint16_t mId; ///< Message id

    uint8_t mCode; ///< See EthCAN_MessageCode
    uint8_t mDataSize_byte; ///< The data size (in byte)
    uint8_t mTotalSize_byte; ///< The total size (in byte)

    uint8_t mReserved0[ 32 - 5 ];
}
EthCAN_Message;

/// \brief EthCAN_MessageCode
typedef enum
{
    EthCAN_MESSAGE_RECEIVE,

    EthCAN_MESSAGE_QTY
}
EthCAN_MessageCode;

/// \brief EthCAN_Request
typedef struct
{
    uint16_t mId; ///< The request id

    uint8_t mCode; ///< See EthCan_RequestCode
    uint8_t mDataSize_byte; ///< The data size (in byte)
    uint8_t mTotalSize_byte; ///< The total size (in byte)

    uint8_t mReserved0[16 - 5];
}
EthCAN_Request;

/// \brief EthCAN_RequestCode
typedef enum
{
    EthCAN_REQUEST_CONFIG_GET,
    EthCAN_REQUEST_CONFIG_SET,
    EthCAN_REQUEST_INFO_GET,
    EthCAN_REQUEST_SEND,

    EthCAN_REQUEST_QTY
}
EthCAN_RequestCode;

/// \brief EthCAN_Response
typedef struct
{
    uint16_t mId; ///< The response id

    uint8_t mCode; ///< See EthCAN_RequestCode
    uint8_t mDataSize_byte; ///< The data size (in byte)
    uint8_t mTotalSize_byte; ///< The total size (in byte)

    uint8_t mReserved0[1];

    uint16_t mResult; ///< See EthCAN_Result

    uint8_t mReserved1[16 - 8];
}
EthCAN_Response;
