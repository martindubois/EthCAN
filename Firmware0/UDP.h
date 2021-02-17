
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/UDP.h

#pragma once

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void UDP_Loop();
extern void UDP_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame, uint32_t aAddress, uint16_t aPort);
extern void UDP_Setup();
