
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol_UDP.h

#pragma once

// ===== EthCAN_Lib =========================================================
#include "Socket.h"

#include "Protocol.h"

class Protocol_UDP : public Protocol
{

public:

    Protocol_UDP();

    // ===== Protocol =======================================================

    virtual unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom);

    virtual void Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4);

    virtual void Signal();

    virtual void Wait();

private:

    Socket mClient;

};
