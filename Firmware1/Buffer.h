
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/Buffer.h

#pragma once

// ====== Firmware1 =========================================================
extern "C"
{
    #include "Includes/EthCAN_Types.h"
}

// Constants
/////////////////////////////////////////////////////////////////////////////

#define BUFFER_TYPE_FREE      (0)
#define BUFFER_TYPE_TX_SERIAL (1)
#define BUFFER_TYPE_TX_CAN    (2)

#define BUFFER_TYPE_QTY (3)

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void Buffer_Begin();
extern void Buffer_Push(EthCAN_Frame * aFrame, uint8_t aType);

extern EthCAN_Frame * Buffer_Pop(uint8_t aType);
