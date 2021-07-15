
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Thread.h

#pragma once

#if defined(_KMS_LINUX_) || defined(_KMS_OS_X_)
    #include <pthread.h>
#endif

// ===== EthCAN_Lib =========================================================
#include "Gate.h"
#include "IMessageReceiver.h"

class Thread
{

public:

    Thread(IMessageReceiver * aReceiver, unsigned int aMessage);

    ~Thread();

    void Sem_Signal(); // L and W_Thread.cpp
    void Sem_Wait(unsigned int aTimeout_ms); // L and W_Thread.cpp

    Gate mZone0;

// internal

    int Run();

private:

    void Destroy(); // L and W_Thread.cpp

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

    #if defined(_KMS_LINUX_) || defined(_KMS_OS_X_)

        pthread_cond_t mCond;

        unsigned int mCount;

        pthread_t mThread;

    #endif

    #ifdef _KMS_WINDOWS_

        HANDLE mHandle;

        HANDLE mSemaphore;

        DWORD mThreadId;

    #endif

};
