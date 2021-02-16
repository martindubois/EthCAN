
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Firmware0.ino

#include <ETH.h>

extern "C"
{
    #include "Includes/EthCAN_Protocol.h"
    #include "Includes/EthCAN_Result.h"
    #include "Includes/EthCAN_Types.h"
}

#include "CAN.h"
#include "Config.h"
#include "Info.h"
#include "UDP.h"
#include "USB.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

// ===== Entry points =======================================================
static void OnWifiEvent(WiFiEvent_t aEvent);

// Entry points
/////////////////////////////////////////////////////////////////////////////

void setup()
{
    Serial.begin(115200);

    Serial.println("INFO - EthCAN - Firmware0");

    WiFi.onEvent(OnWifiEvent);

    Serial.println("Loading configuration ...");

    Config_Reset();
    Config_Load();

    if (0 != gConfig.mIPv4_Addr)
    {
        Serial.println("Configuring static IPv4 address...");
        WiFi.config(gConfig.mIPv4_Addr, gConfig.mIPv4_Gateway, gConfig.mIPv4_Mask);
    }

    if ('\0' == gConfig.mWiFi_Name[0])
    {
        Serial.println("Enabling wired Ethernet...");
        ETH.begin();
    }
    else
    {
        if (0 == (gConfig.mWiFi_Flags & EthCAN_FLAG_WIFI_AP))
        {
            Serial.println("Connecting to WiFi...");
            WiFi.setHostname(gConfig.mName);
            WiFi.begin(gConfig.mWiFi_Name, gConfig.mWiFi_Password);
        }
        else
        {
            Serial.println("Configuring the WiFi access poing...");
            WiFi.softAP(gConfig.mWiFi_Name, gConfig.mWiFi_Password);
        }
    }


    Info_Init();

    CAN_Setup();
    UDP_Setup();
    USB_Setup();

    Serial.println("Setup completed");
}

void loop()
{
    CAN_Loop();
    UDP_Loop();
    USB_Loop();
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

// ===== Entry point ========================================================

void OnWifiEvent(WiFiEvent_t aEvent)
{
    switch (aEvent)
    {
    case SYSTEM_EVENT_ETH_START: Serial.println("INFO - Event - ETH_START");
        Serial.print("INFO - Ethernet address : ");
        Serial.println(ETH.macAddress());
        ETH.setHostname(gConfig.mName);
        break;

    case SYSTEM_EVENT_ETH_GOT_IP: Serial.println("INFO - Event - ETH_GOT_IP");
        Serial.print("INFO - IPv4 address : ");
        Serial.println(ETH.localIP());
        gInfo.mIPv4_Addr    = ETH.localIP();
        gInfo.mIPv4_Gateway = ETH.gatewayIP();
        gInfo.mIPv4_Mask    = ETH.subnetMask();
        break;

    case SYSTEM_EVENT_ETH_CONNECTED   : Serial.println("INFO - Event - ETH_CONNECTED");
        Serial.print("INFO - Link speed : ");
        Serial.print(ETH.linkSpeed());
        Serial.println(" Mbps");
        break;

    case SYSTEM_EVENT_ETH_DISCONNECTED: Serial.println("INFO - Event - ETH_DISCONNECTED"); break;
    case SYSTEM_EVENT_ETH_STOP        : Serial.println("INFO - EVent - ETH_STOP"); break;
    }
}
