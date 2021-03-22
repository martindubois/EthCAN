
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_Serial.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Serial.h"

// Private
/////////////////////////////////////////////////////////////////////////////

void Serial::Connect()
{
    mHandle = CreateFile(mLink, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == mHandle)
    {
        throw EthCAN_ERROR_SERIAL_OPEN;
    }

    DCB lDCB;
    COMMTIMEOUTS lTimeouts;

    if ((!GetCommState(mHandle, &lDCB)) || (!GetCommTimeouts(mHandle, &lTimeouts)))
    {
        TRACE_ERROR(mTrace, "Serial::Connect - EthCAN_ERROR_SERIAL_CONFIG");
        throw EthCAN_ERROR_SERIAL_CONFIG;
    }

    assert(lDCB.DCBlength == sizeof(lDCB));
    assert(lDCB.fBinary);

    // TODO USB
    //      Test at 230400 bps

    lDCB.BaudRate = CBR_115200;
    lDCB.ByteSize = 8;
    lDCB.Parity   = NOPARITY;
    lDCB.StopBits = ONESTOPBIT;

    lDCB.fAbortOnError   = FALSE;
    lDCB.fDsrSensitivity = FALSE;
    lDCB.fDtrControl     = DTR_CONTROL_DISABLE;
    lDCB.fErrorChar      = FALSE;
    lDCB.fInX            = FALSE;
    lDCB.fNull           = FALSE;
    lDCB.fParity         = FALSE;
    lDCB.fOutX           = FALSE;
    lDCB.fOutxCtsFlow    = FALSE;
    lDCB.fOutxDsrFlow    = FALSE;
    lDCB.fRtsControl     = RTS_CONTROL_DISABLE;

    if (!SetCommState(mHandle, &lDCB))
    {
        TRACE_ERROR(mTrace, "Serial::Connect - EthCAN_ERROR_SERIAL_CONFIG");
        throw EthCAN_ERROR_SERIAL_CONFIG;
    }

    lTimeouts.ReadIntervalTimeout = 1;
    lTimeouts.ReadTotalTimeoutConstant = 1;
    lTimeouts.ReadTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(mHandle, &lTimeouts))
    {
        TRACE_ERROR(mTrace, "Serial::Connect - EthCAN_ERROR_SERIAL_CONFIG");
        throw EthCAN_ERROR_SERIAL_CONFIG;
    }
}

void Serial::Disconnect()
{
    assert(INVALID_HANDLE_VALUE != mHandle);

    BOOL lRet = CloseHandle(mHandle);
    assert(lRet);
}

unsigned int Serial::Raw_Receive(void* aOut, unsigned int aOutSize_byte)
{
    assert(NULL != aOut);
    assert(0 < aOutSize_byte);

    DWORD lResult_byte;

    if (!ReadFile(mHandle, aOut, aOutSize_byte, &lResult_byte, NULL))
    {
        TRACE_ERROR(mTrace, "Serial::Raw_Receive - EthCAN_ERROR_SERIAL_RECEIVE");
        throw EthCAN_ERROR_SERIAL_RECEIVE;
    }

    return lResult_byte;
}

void Serial::Raw_Send(const void* aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn);
    assert(0 < aInSize_byte);

    DWORD lInfo_byte;

    if ((!WriteFile(mHandle, aIn, aInSize_byte, &lInfo_byte, NULL)) || (aInSize_byte != lInfo_byte))
    {
        TRACE_ERROR(mTrace, "Serial::Raw_Receive - EthCAN_ERROR_SERIAL_SEND");
        throw EthCAN_ERROR_SERIAL_SEND;
    }
}
