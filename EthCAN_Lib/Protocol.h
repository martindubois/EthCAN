
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Protocol.h

#pragma once

class Protocol
{

public:

    virtual ~Protocol();

    const char* GetName() const;

    virtual unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom) = 0;

    virtual void Send(const void* aIn, unsigned int aSize_byte, uint32_t aIPv4) = 0;

    virtual void Signal() = 0;

    virtual void Wait() = 0;

protected:

    Protocol(const char* aName);

private:

    const char* mName;

};
