
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware1/Firmware1.ino

#include "Component.h"

// ===== Firmware1 ==========================================================
#include "Common/Version.h"

#include "CAN.h"
#include "Cmd.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#ifdef _TEST_
    #define COM_SPEED_bps (115200)
#else
    #define COM_SPEED_bps (1000000)
#endif

// Entry points
/////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(COM_SPEED_bps);

    MSG_INFO("EthCAN - Firmware1 - ", VERSION_STR);

    pinMode(3, OUTPUT);

    Cmd_Setup();

    CAN_Begin();

    uint8_t lVal = 0x01;

    for (unsigned int lRetry = 0; lRetry < 4; lRetry ++)
    {
        EthCAN_Result lRet = CAN_Config_Reset();
        Cmd_Result_CAN_Set(lRet);
        if (EthCAN_OK == lRet)
        {
            break;
        }

        delay(100);
        digitalWrite(3, lVal);
        lVal ^= 0x01;
    }
}

void loop()
{
    CAN_Loop();
    Cmd_Loop();
}
