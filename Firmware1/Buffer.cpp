
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware/Buffer.cpp

#include <Arduino.h>

#include "Component.h"

// ===== Firmware1 ==========================================================
#include "Buffer.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define BUFFER_QTY (8)

// Data types
/////////////////////////////////////////////////////////////////////////////

typedef struct
{
    EthCAN_Frame mFrame;
    uint8_t mIndex;
    uint8_t mNext;
}
Buffer;

typedef struct
{
    uint8_t mIndex;
    uint8_t mLast;
    uint8_t mNext;
}
Queue;

// Static variables
/////////////////////////////////////////////////////////////////////////////

static Buffer sBuffers[BUFFER_QTY];
static Queue  sQueues [BUFFER_TYPE_QTY];

// Functions
/////////////////////////////////////////////////////////////////////////////

void Buffer_Begin()
{
    unsigned int i;

    memset(sBuffers, 0, sizeof(sBuffers));
    memset(sQueues , 0, sizeof(sQueues));

    for (i = 0; i < BUFFER_QTY; i ++)
    {
        sBuffers[i].mIndex = i;
        sBuffers[i].mNext = i + 1;
    }

    sQueues[BUFFER_TYPE_FREE].mLast = BUFFER_QTY - 1;
    
    for (i = 1; i < BUFFER_TYPE_QTY; i ++)
    {
        sQueues[i].mLast = BUFFER_QTY;
        sQueues[i].mNext = BUFFER_QTY;
    }
}

// Critical path
void Buffer_Push(EthCAN_Frame * aFrame, unsigned int aType)
{
    Buffer * lBuffer = reinterpret_cast<Buffer *>(aFrame);
    uint8_t  lIndex  = lBuffer->mIndex;
    Queue  * lQueue  = sQueues + aType;

    lBuffer->mNext = BUFFER_QTY;

    if (BUFFER_QTY <= lQueue->mNext)
    {
        lQueue->mLast = lIndex;
        lQueue->mNext = lIndex;
    }
    else
    {
        sBuffers[lQueue->mLast].mNext = lIndex;
        lQueue->mLast = lIndex;
    }
}

// Critical path
EthCAN_Frame * Buffer_Pop(unsigned int aType)
{
    Queue * lQueue = sQueues + aType;

    if (BUFFER_QTY <= lQueue->mNext)
    {
        return NULL;
    }

    Buffer * lBuffer = sBuffers + lQueue->mNext;
        
    lQueue->mNext = lBuffer->mNext;

    return &lBuffer->mFrame;
}
