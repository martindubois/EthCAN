
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol_TCP.h

#pragma once

// ===== EthCAN_Lib =========================================================
#include "Socket.h"

class Thread;

#include "Protocol.h"

class Protocol_TCP : public Protocol
{

public:

    Protocol_TCP(uint32_t aIPv4);

    void Init(Thread* aThread);

    // ===== Protocol =======================================================

    virtual unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom);

    virtual void Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4);

    virtual void Signal();

    virtual void Wait();

private:

    Socket mSocket;

    Thread* mThread;

};
