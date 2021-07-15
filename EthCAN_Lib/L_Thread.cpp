
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/L_Thread.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Thread.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void * Run_Link(void * aContext);

// Public
/////////////////////////////////////////////////////////////////////////////

void Thread::Sem_Signal()
{
    int lRet = pthread_cond_signal(&mCond);
    assert(0 == lRet);
}

void Thread::Sem_Wait(unsigned int aTimeout_ms)
{
    int lRet = 0;

    mZone0.Enter();
    {
        timespec lAbsTime;

        clock_gettime(CLOCK_REALTIME, &lAbsTime);
        lAbsTime.tv_sec += aTimeout_ms / 1000 + 1;

        while ((0 >= mCount) && (0 == lRet))
        {
            lRet = pthread_cond_timedwait(&mCond, mZone0.GetInternal(), &lAbsTime);
        }
    }
    mZone0.Leave();

    if (0 != lRet)
    {
        throw EthCAN_ERROR_TIMEOUT;
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

void Thread::Destroy()
{
    int lRet = pthread_cond_destroy(&mCond);
    assert(0 == lRet);
}

void Thread::Start()
{
    mCount = 0;

    int lRet = pthread_cond_init(&mCond, NULL);
    assert(0 == lRet);

    lRet = pthread_create(&mThread, NULL, Run_Link, this);
    assert(0 == lRet);
}

void Thread::Wait()
{
    int lRet = pthread_join(mThread, NULL);
    assert(0 == lRet);
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void * Run_Link(void * aContext)
{
    assert(NULL != aContext);

    Thread* lThis = reinterpret_cast<Thread *>(aContext);

    return reinterpret_cast<void *>(lThis->Run());
}
