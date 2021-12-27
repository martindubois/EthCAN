
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Firmware0.ino

// CODE REVIEW 2021-03-24 KMS - Martin Dubois, P.Eng.

#include <ETH.h>

#include <esp_wifi.h>

#include "Component.h"

// ===== Firmware0 ==========================================================

#include "Common/Version.h"

#include "CAN.h"
#include "Config.h"
#include "Info.h"
#include "TCP.h"
#include "UDP.h"
#include "USB.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================
static void OnWiFiEvent(WiFiEvent_t aEvent);

// Entry points
/////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(EthCAN_USB_SPEED_bps);

    MSG_INFO("EthCAN - Firmware0 - ", VERSION_STR);

    WiFi.onEvent(OnWiFiEvent);

    Config_Load();

    if (0 != gConfig.mIPv4_Address)
    {
        WiFi.config(gConfig.mIPv4_Address, gConfig.mIPv4_Gateway, gConfig.mIPv4_NetMask);
    }

    if ('\0' == gConfig.mWiFi_Name[0])
    {
        ETH.begin();
    }
    else
    {
        if (0 == (gConfig.mWiFi_Flags & EthCAN_FLAG_WIFI_AP))
        {
            WiFi.setHostname(gConfig.mName);
            WiFi.begin(gConfig.mWiFi_Name, gConfig.mWiFi_Password);

            // TODO Firmware0.WiFi
            //      Is this loop needed?
            for (unsigned int lRetry = 0; lRetry < 10; lRetry ++)
            {
                if (WL_CONNECTED == WiFi.status())
                {
                    break;
                }

                delay(500);
            }
        }
        else
        {
            WiFi.softAP(gConfig.mWiFi_Name, gConfig.mWiFi_Password);
        }
    }

    Info_Init(gConfig.mName);

    CAN_Setup();
    TCP_Setup();
    UDP_Setup();
}

void loop()
{
    CAN_Loop();
    Config_Loop();
    TCP_Loop();
    UDP_Loop();
    USB_Loop();
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// ===== Entry point ========================================================

void OnWiFiEvent(WiFiEvent_t aEvent)
{
    Info_Count_Events();

    uint8_t lAddress[6];

    switch (aEvent)
    {
    case SYSTEM_EVENT_ETH_CONNECTED:
        MSG_INFO("Link speed : ", ETH.linkSpeed());
        break;

    case SYSTEM_EVENT_ETH_GOT_IP:
        Info_Set_IPv4(ETH.localIP(), ETH.gatewayIP(), ETH.subnetMask());
        break;

    case SYSTEM_EVENT_ETH_START:
        ETH.setHostname(gConfig.mName);
        esp_eth_get_mac(lAddress);
        Info_Set_EthAddress(lAddress);
        break;

    case SYSTEM_EVENT_ETH_DISCONNECTED:
    case SYSTEM_EVENT_ETH_STOP        :
    case SYSTEM_EVENT_STA_CONNECTED   :
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        esp_wifi_get_mac(WIFI_IF_STA, lAddress);
        Info_Set_IPv4(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask());
        Info_Set_EthAddress(lAddress);
        break;

    case SYSTEM_EVENT_STA_START :
    case SYSTEM_EVENT_WIFI_READY:
        break;

    default: MSG_WARNING(aEvent);
    }
}
