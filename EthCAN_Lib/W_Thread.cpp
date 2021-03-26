
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_Thread.cpp

// CODE COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Thread.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static DWORD WINAPI Run_Link(LPVOID aContext);

// Public
/////////////////////////////////////////////////////////////////////////////

void Thread::Sem_Signal()
{
    assert(NULL != mSemaphore);

    BOOL lRetB = ReleaseSemaphore(mSemaphore, 1, NULL);
    assert(lRetB);
}

void Thread::Sem_Wait(unsigned int aTimeout_ms)
{
    assert(NULL != mSemaphore);

    if (WAIT_OBJECT_0 != WaitForSingleObject(mSemaphore, aTimeout_ms))
    {
        throw EthCAN_ERROR_TIMEOUT;
    }
}

void Thread::Zone0_Enter()
{
    EnterCriticalSection(&mZone0);
}

void Thread::Zone0_Leave()
{
    LeaveCriticalSection(&mZone0);
}

// Private
/////////////////////////////////////////////////////////////////////////////

void Thread::Destroy()
{
    assert(NULL != mHandle);
    assert(NULL != mSemaphore);

    BOOL lRetB = CloseHandle(mHandle);
    assert(lRetB);

    lRetB = CloseHandle(mSemaphore);
    assert(lRetB);

    mHandle = NULL;
    mSemaphore = NULL;
}

void Thread::Start()
{
    InitializeCriticalSection(&mZone0);

    mSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
    if (NULL == mSemaphore)
    {
        TRACE_ERROR(stderr, "Thread::Start - EthCAN_ERROR_SEMAPHORE");
        throw EthCAN_ERROR_SEMAPHORE;
    }

    mHandle = CreateThread(NULL, 0, Run_Link, this, 0, &mThreadId);
    if (NULL == mHandle)
    {
        BOOL lRetB = CloseHandle(mSemaphore);
        assert(lRetB);

        mSemaphore = NULL;

        TRACE_ERROR(stderr, "Thread::Start - EthCAN_ERROR_THREAD");
        throw EthCAN_ERROR_THREAD;
    }
}

void Thread::Wait()
{
    assert(NULL != mHandle);

    if (WAIT_OBJECT_0 != WaitForSingleObject(mHandle, 3000))
    {
        TRACE_ERROR(stderr, "Thread::Wait - EthCAN_ERROR_THREAD");
        throw EthCAN_ERROR_THREAD;
    }
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

DWORD WINAPI Run_Link(LPVOID aContext)
{
    assert(NULL != aContext);

    Thread* lThis = reinterpret_cast<Thread *>(aContext);

    return lThis->Run();
}
