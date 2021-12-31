
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol_USB.h

#pragma once

// ===== EthCAN_Lib =========================================================
class Serial;

#include "Protocol.h"

class Protocol_USB : public Protocol
{

public:

    Protocol_USB(Serial* aSerial);

    // ===== Protocol =======================================================

    virtual ~Protocol_USB();

    virtual unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom);

    virtual void Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4);

    virtual void Signal();

    virtual void Wait();

private:

    Serial* mSerial;

};
