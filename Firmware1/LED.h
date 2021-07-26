
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/LED.h

#pragma once

// Constants
// //////////////////////////////////////////////////////////////////////////

#define LED_POWER (0)

#define LED_QTY (1)

// Functions
// //////////////////////////////////////////////////////////////////////////

extern void LED_Begin ();
extern void LED_Off   (uint8_t aIndex);
extern void LED_On    (uint8_t aIndex);
extern void LED_Toggle(uint8_t aIndex);
