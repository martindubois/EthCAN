
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

    Serial(const char* aLink, FILE* aTrace);

    ~Serial();

    Thread* GetThread();

    void Receiver_Start(IMessageReceiver* aReceiver, unsigned int aMessage);

    void Send(const void* aIn, unsigned int aSize_byte);

    // ===== IMessageReceived ===============================================
    virtual bool OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte);

private:

    void Buffer_Flush(unsigned int aSize_byte);
    void Buffer_Trace(unsigned int aSize_byte);

    void Connect   (); // L and W_Serial.cpp
    void Disconnect(); // L and W_Serial.cpp

    bool OnLoopIteration();

    unsigned int Raw_Receive(      void* aOut, unsigned int aOutSize_byte); // L and W_Serial.cpp
    void         Raw_Send   (const void* aIn , unsigned int aInSize_byte ); // L and W_Serial.cpp

    bool Receive_Data();
    bool Receive_Header();
    bool Receive_Trace();

    // --> TRACE <==+=====+
    //       |      |     |
    //       +--> HEADER  |
    //              |     |
    //              +--> DATA
    typedef enum
    {
        STATE_DATA,
        STATE_HEADER,
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

    FILE* mTrace = NULL;

    #ifdef _KMS_WINDOWS_
        HANDLE mHandle;
    #endif

};
