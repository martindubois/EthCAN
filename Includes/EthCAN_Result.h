
// Product EthCAN

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN_Result.h
/// \brief     EthCAN_Result

#pragma once

// Data type
/////////////////////////////////////////////////////////////////////////////

/// \brief EthCAN_Result
typedef enum
{
    EthCAN_OK,
    EthCAN_OK_PENDING,

    EthCAN_OK_QTY,

    EthCAN_ERROR = 0x0010,
    EthCAN_ERROR_BUFFER,
    EthCAN_ERROR_BUSY,
    EthCAN_ERROR_CAN,
    EthCAN_ERROR_CAN_RATE,                  // 20
    EthCAN_ERROR_CONFIG,
    EthCAN_ERROR_DATA_SIZE,
    EthCAN_ERROR_DATA_UNEXPECTED,
    EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER,
    EthCAN_ERROR_EXCEPTION,
    EthCAN_ERROR_FILE_NAME,
    EthCAN_ERROR_FUNCTION,
    EthCAN_ERROR_INPUT_BUFFER,
    EthCAN_ERROR_INPUT_FILE_CLOSE,
    EthCAN_ERROR_INPUT_FILE_OPEN,           // 30
    EthCAN_ERROR_INPUT_STREAM,
    EthCAN_ERROR_INPUT_STREAM_READ,
    EthCAN_ERROR_IPv4_ADDRESS,
    EthCAN_ERROR_IPv4_MASK,
    EthCAN_ERROR_NETWORK,
    EthCAN_ERROR_NOT_CONNECTED,
    EthCAN_ERROR_NOT_CONNECTED_ETH,
    EthCAN_ERROR_NOT_RUNNING,
    EthCAN_ERROR_OUTPUT_BUFFER,
    EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL,   // 40
    EthCAN_ERROR_OUTPUT_FILE_CLOSE,
    EthCAN_ERROR_OUTPUT_FILE_OPEN,
    EthCAN_ERROR_OUTPUT_STREAM,
    EthCAN_ERROR_OUTPUT_STREAM_WRITE,
    EthCAN_ERROR_PENDING,
    EthCAN_ERROR_REFERENCE,
    EthCAN_ERROR_RESPONSE_SIZE,
    EthCAN_ERROR_RUNNING,
    EthCAN_ERROR_SEMAPHORE,
    EthCAN_ERROR_SERIAL,                    // 50
    EthCAN_ERROR_SERIAL_CONFIG,
    EthCAN_ERROR_SERIAL_OPEN,
    EthCAN_ERROR_SERIAL_RECEIVE,
    EthCAN_ERROR_SERIAL_SEND,
    EthCAN_ERROR_SOCKET,
    EthCAN_ERROR_SOCKET_BIND,
    EthCAN_ERROR_SOCKET_NAME,
    EthCAN_ERROR_SOCKET_OPTION,
    EthCAN_ERROR_SOCKET_RECEIVE,
    EthCAN_ERROR_SOCKET_SEND,               // 60
    EthCAN_ERROR_THREAD,
    EthCAN_ERROR_TIMEOUT,

    EthCAN_ERROR_QTY,

    EthCAN_RESULT_INVALID = 0xf0,
    EthCAN_RESULT_NO_ERROR,
    EthCAN_RESULT_REQUEST,
}
EthCAN_Result;

#define EthCAN_RESULT_OK(R) ((R) < EthCAN_OK_QTY)
