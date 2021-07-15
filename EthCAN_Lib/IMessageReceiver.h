
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/IMessageReceiver.h

#pragma once

class IMessageReceiver
{

public:

    virtual bool OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte) = 0;

};
