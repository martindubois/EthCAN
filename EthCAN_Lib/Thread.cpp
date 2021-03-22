
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Thread.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "UDPSocket.h"

#include "Thread.h"

// Public
/////////////////////////////////////////////////////////////////////////////

Thread::Thread(IMessageReceiver* aReceiver, unsigned int aMessage) : mMessage(aMessage), mReceiver(aReceiver), mState(STATE_STARTING)
{
    assert(NULL != aReceiver);

    Start();
}

Thread::~Thread()
{
    Zone0_Enter();
    {
        switch (mState)
        {
        case STATE_RUNNING:
        case STATE_STARTING: 
            mState = STATE_STOPPING;

        case STATE_STOPPING:
            Zone0_Leave();
            {
                Wait();
            }
            Zone0_Enter();
            break;

        case STATE_STOPPED:
            // NOT TESTED  Thread
            //             Thread is already stopped.
            break;

        default: assert(false);
        }
    }
    Zone0_Leave();

    Destroy();
}

// Internal
/////////////////////////////////////////////////////////////////////////////

int Thread::Run()
{
    assert(NULL != mReceiver);

    UDPSocket::Thread_Init();

    try
    {
        Zone0_Enter();
        {
            switch (mState)
            {
            case STATE_STARTING:
                mState = STATE_RUNNING;
                bool lContinue;
                lContinue = true;
                while (lContinue && (STATE_RUNNING == mState))
                {
                    Zone0_Leave();
                    {
                        lContinue = mReceiver->OnMessage(this, mMessage, NULL, 0);
                    }
                    Zone0_Enter();
                }
                break;

            case STATE_STOPPING:
                TRACE_WARNING(stderr, "Thread::Run - Stopped before starting");
                break;

            default: assert(false);
            }

            mState = STATE_STOPPED;
        }
        Zone0_Leave();
    }
    catch (...)
    {
        TRACE_ERROR(stderr, "Thread::Run - Exception");
        return __LINE__;
    }

    UDPSocket::Thread_Uninit();

    return 0;
}
