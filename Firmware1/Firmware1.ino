
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware1/Firmware1.ino

#include "Component.h"

// ===== Firmware1 ==========================================================
#include "Common/Version.h"

#include "Buffer.h"
#include "CAN.h"
#include "Cmd.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#ifdef _TEST_
    #define COM_SPEED_bps (115200)
#else
    #define COM_SPEED_bps FIRMWARE_BAUD_RATE_bps
#endif

#define LED_POWER (3)

// Entry points
/////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(COM_SPEED_bps);

    MSG_INFO("EthCAN - Firmware1 - ", VERSION_STR);

    pinMode(LED_POWER, OUTPUT);

    Buffer_Begin();

    Cmd_Setup();

    CAN_Begin();

    for (unsigned int lRetry = 0; lRetry < 4; lRetry ++)
    {
        if (EthCAN_OK == CAN_Config_Reset())
        {
            break;
        }

        MSG_WARNING("Retrying CAN initialisation... ");

        delay(100);
    }

    digitalWrite(LED_POWER, HIGH);
}

void loop()
{
    CAN_Loop();
    Cmd_Loop();
}
