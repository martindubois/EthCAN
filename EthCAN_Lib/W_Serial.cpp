
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_Serial.cpp

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
        throw EthCAN_ERROR_SERIAL_CONFIG;
    }

    assert(lDCB.DCBlength == sizeof(lDCB));
    assert(lDCB.fBinary);

    lDCB.BaudRate = CBR_115200;
    lDCB.ByteSize = 8;
    lDCB.Parity   = NOPARITY;
    lDCB.StopBits = 1;

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
        throw EthCAN_ERROR_SERIAL_CONFIG;
    }

    lTimeouts.ReadIntervalTimeout = 100;
    lTimeouts.ReadTotalTimeoutConstant = 500;
    lTimeouts.ReadTotalTimeoutMultiplier = 20;

    if (!SetCommTimeouts(mHandle, &lTimeouts))
    {
        throw EthCAN_ERROR_SERIAL_CONFIG;
    }
}

unsigned int Serial::Raw_Receive(void* aOut, unsigned int aOutSize_byte)
{
    assert(NULL != aOut);
    assert(0 < aOutSize_byte);

    DWORD lResult_byte;

    if (!ReadFile(mHandle, aOut, aOutSize_byte, &lResult_byte, NULL))
    {
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
        throw EthCAN_ERROR_SERIAL_SEND;
    }
}