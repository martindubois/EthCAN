
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Header.h

#pragma once

// ===== Firmware0 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void Header_Init(EthCAN_Header * aOut, const EthCAN_Header * aIn);

extern bool Header_Validate(const EthCAN_Header & aIn, unsigned int aSize_byte);
