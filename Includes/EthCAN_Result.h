
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

    EthCAN_OK_QTY,

    EthCAN_ERROR = 256,
    EthCAN_ERROR_EXCEPTION,
    EthCAN_ERROR_INVALID_OUTPUT_BUFFER,

    EthCAN_ERROR_QTY,

    EthCAN_INVALID,
}
EthCAN_Result;
