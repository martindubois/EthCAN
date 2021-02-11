
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Thread.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
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
            break;

        default: assert(false);
        }
    }
    Zone0_Leave();
}

// Internal
/////////////////////////////////////////////////////////////////////////////

int Thread::Run()
{
    assert(NULL != mReceiver);

    try
    {
        Zone0_Enter();
        {
            switch (mState)
            {
            case STATE_STARTING:
                mState = STATE_RUNNING;
                while (STATE_RUNNING == mState)
                {
                    Zone0_Leave();
                    {
                        if (!mReceiver->OnMessage(this, mMessage, NULL, 0))
                        {
                            break;
                        }
                    }
                    Zone0_Enter();
                }
                break;

            case STATE_STOPPING: break;

            default: assert(false);
            }

            mState = STATE_STOPPED;
        }
        Zone0_Leave();
    }
    catch (...)
    {
        return __LINE__;
    }

    return 0;
}
