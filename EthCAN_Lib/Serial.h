
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Serial.h

#pragma once

// ===== EthCAN_Lib =========================================================
#include "IMessageReceiver.h"

class Thread;

class Serial : public IMessageReceiver
{

public:

    Serial(const char* aLink);

    ~Serial();

    void Close(); // L and W_Serial.cpp

    Thread* GetThread();

    void Receiver_Start(IMessageReceiver* aReceiver, unsigned int aMessage);

    void Receiver_Stop();;

    void Send(const void* aIn, unsigned int aSize_byte);

    // ===== IMessageReceived ===============================================
    virtual bool OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte);

private:

    void Connect(); // L and W_Serial.cpp

    bool OnLoopIteration();

    unsigned int Raw_Receive(      void* aOut, unsigned int aOutSize_byte); // L and W_Serial.cpp
    void         Raw_Send   (const void* aIn , unsigned int aInSize_byte ); // L and W_Serial.cpp

    // --> TRACE <--+
    //       |      |
    //       +--> DATA <--+
    //              |     |
    //              +-----+
    typedef enum
    {
        STATE_DATA,
        STATE_TRACE,
    }
    State;

    uint8_t mBuffer[256];

    unsigned int mBufferLevel;

    void* mContext;

    char mLink[64];

    unsigned int      mMessage ; // Zone 0
    IMessageReceiver* mReceiver; // Zone 0

    State mState;

    Thread* mThread;

    #ifdef _KMS_WINDOWS_
        HANDLE mHandle;
    #endif

};
