
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/Cmd.h

#pragma once

// ===== Firmware1 ==========================================================
extern "C"
{
    #include "Includes/EthCAN_Types.h"
}

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void Cmd_Loop();
extern void Cmd_Send(const EthCAN_Frame & aFrame);
extern void Cmd_Setup();
