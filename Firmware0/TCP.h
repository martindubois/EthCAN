
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/TCP.h

#pragma once

// ===== Firmware0 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void TCP_Loop();
extern bool TCP_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame);
extern void TCP_Setup();
