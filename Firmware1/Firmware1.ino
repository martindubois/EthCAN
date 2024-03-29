
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware1/Firmware1.ino

// CODE REVIEW 2021-03-24 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Firmware1 ==========================================================
#include "Common/Version.h"

#include "Buffer.h"
#include "CAN.h"
#include "Cmd.h"
#include "LED.h"

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

    LED_Begin();

    Buffer_Begin();

    Cmd_Setup();

    CAN_Begin();

    // TODO Firmware1
    //      Is this retry loop needed?
    for (uint8_t lRetry = 0; lRetry < 4; lRetry ++)
    {
        if (EthCAN_OK == CAN_Config_Reset())
        {
            break;
        }

        MSG_WARNING("Retrying CAN initialisation... ");

        delay(100);
        LED_Toggle(LED_POWER);
    }

    LED_On(LED_POWER);
}

void loop()
{
    CAN_Loop();
    Cmd_Loop();
}
