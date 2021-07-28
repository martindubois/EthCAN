
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      Firmware1/LED.cpp

#include <Arduino.h>

#include "Component.h"

// ===== Firmware1 ==========================================================
#include "LED.h"

// Constants
// //////////////////////////////////////////////////////////////////////////

static const uint8_t PIN[LED_QTY] = { 3 };

// Variables
// //////////////////////////////////////////////////////////////////////////

static bool sState[LED_QTY];

// Functions
// //////////////////////////////////////////////////////////////////////////

void LED_Begin()
{
    for (uint8_t i = 0; i < LED_QTY; i++)
    {
        pinMode(PIN[i], OUTPUT);
    }
}

void LED_Off(uint8_t aIndex)
{
    digitalWrite(PIN[aIndex], LOW);
    sState[aIndex] = false;
}

void LED_On(uint8_t aIndex)
{
    digitalWrite(PIN[aIndex], HIGH);
    sState[aIndex] = true;
}

void LED_Toggle(uint8_t aIndex)
{
    if (sState[aIndex])
    {
        LED_Off(aIndex);
    }
    else
    {
        LED_On(aIndex);
    }
}
