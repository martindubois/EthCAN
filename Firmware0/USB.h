
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/USB.h

#pragma once

// Functions
/////////////////////////////////////////////////////////////////////////////

extern void USB_Loop();
extern void USB_OnFrame(const EthCAN_Header & aHeader, const EthCAN_Frame & aFrame);
