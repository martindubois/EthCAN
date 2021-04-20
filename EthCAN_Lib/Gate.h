
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Gate.h

#pragma once

#ifdef _KMS_LINUX_
    #include <pthread.h>
#endif

class Gate
{

public:

    Gate(); // L and W_Thread.cpp

    ~Gate(); // L and W_Thread.cpp

    void Enter(); // L and W_Thread.cpp
    void Leave(); // L and W_Thread.cpp

    #ifdef _KMS_LINUX_
        pthread_mutex_t * GetInternal();
    #endif

private:

    Gate(const Gate &);

    const Gate & operator = (const Gate &);

    #ifdef _KMS_LINUX_

        pthread_mutex_t mInternal;

    #endif

    #ifdef _KMS_WINDOWS_

        CRITICAL_SECTION mInternal;

    #endif

};
