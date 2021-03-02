
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Thread.h

#pragma once

#ifdef _KMS_LINUX_
    #include <pthread.h>
#endif

// ===== EthCAN_Lib =========================================================
#include "IMessageReceiver.h"

class Thread
{

public:

    Thread(IMessageReceiver * aReceiver, unsigned int aMessage);

    ~Thread();

    void Sem_Signal(); // L and W_Thread.cpp
    void Sem_Wait(unsigned int aTimeout_ms); // L and W_Thread.cpp

    void Zone0_Enter(); // L and W_Thread.cpp
    void Zone0_Leave(); // L and W_Thread.cpp

// internal

    int Run();

private:

    void Start(); // L and W_Thread.cpp

    void Wait(); // L and W_Thread.cpp

    // --> STARTING --> RUNNING -------+
    //      |                |         |
    //      +--> STOPPING <--+         |
    //                 |               |
    //                 +--> STOPPED <--+
    typedef enum
    {
        STATE_RUNNING,
        STATE_STARTING,
        STATE_STOPPED,
        STATE_STOPPING,
    }
    State;

    void* mContext;

    unsigned int       mMessage;
    IMessageReceiver * mReceiver;

    State mState; // Zone 0

    #ifdef _KMS_LINUX_

        pthread_cond_t mCond;

        unsigned int mCount;

        pthread_t mThread;

        pthread_mutex_t mZone0;

    #endif

    #ifdef _KMS_WINDOWS_

        HANDLE mHandle;

        HANDLE mSemaphore;

        DWORD mThreadId;

        CRITICAL_SECTION mZone0;

    #endif

};
