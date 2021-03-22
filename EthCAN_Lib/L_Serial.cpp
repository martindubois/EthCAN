
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/L_Serial.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Serial.h"

// Private
/////////////////////////////////////////////////////////////////////////////

void Serial::Connect()
{
    // TODO Linux.USB
    throw EthCAN_ERROR_SERIAL_OPEN;
}

void Serial::Disconnect()
{
    // TODO Linux.USB
}

unsigned int Serial::Raw_Receive(void* aOut, unsigned int aOutSize_byte)
{
    assert(NULL != aOut);
    assert(0 < aOutSize_byte);

    // TODO Linux.USB
    throw EthCAN_ERROR_SERIAL_RECEIVE;
}

void Serial::Raw_Send(const void* aIn, unsigned int aInSize_byte)
{
    assert(NULL != aIn);
    assert(0 < aInSize_byte);

    // TODO Linux.USB
    throw EthCAN_ERROR_SERIAL_SEND;
}
