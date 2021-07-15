
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Tool/Setup.h

#include "Component.h"

// ===== C ==================================================================
#include <string.h>

// ===== Includes ===========================================================
#include <EthCAN/Display.h>

// ===== EthCAN_Tool ========================================================
#include "Setup.h"

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

static void ReleaseDevices (unsigned int* aStep, EthCAN::Device* aDevices[2]);
static void RetrieveDevices(unsigned int* aStep, EthCAN::System* aSystem, unsigned int aIndex[2], EthCAN::Device* aDevices[2]);

static EthCAN_Result RetrieveConfigs     (unsigned int* aStep, EthCAN::Device* aDevices[2], EthCAN_Config aConfigs[2]);
static EthCAN_Result SetConfigs          (unsigned int* aStep, EthCAN::Device* aDevices[2], EthCAN_Config aConfigs[2]);
static EthCAN_Result StoreConfigAndReset (unsigned int* aStep, EthCAN::Device* aDevice, uint8_t aStoreFlags);
static EthCAN_Result StoreConfigsAndReset(unsigned int* aStep, EthCAN::Device* aDevices[2], uint8_t aStoreFlags);

// Functions
/////////////////////////////////////////////////////////////////////////////

#define BEGIN                                      \
    EthCAN_Result lResult = EthCAN_RESULT_INVALID; \
    unsigned int lStep = 1

#define END return lResult

#define STEP(D) printf("%u. " D "...\n", lStep); lStep++

EthCAN_Result Setup_AccessPoint(EthCAN::Device* aDevice, uint32_t aAddress, uint32_t aNetMask, const char* aSSID, const char* aPassword)
{
    BEGIN;
    {
        EthCAN_Config lConfig;

        STEP("Retrieving the configuration");

        lResult = aDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lResult)
        {
            STEP("Modifying the configuration");

            lConfig.mIPv4_Address = aAddress;
            lConfig.mIPv4_Gateway = 0;
            lConfig.mIPv4_NetMask = aNetMask;
            lConfig.mWiFi_Flags |= EthCAN_FLAG_WIFI_AP;
            strcpy_s(lConfig.mWiFi_Name, aSSID);
            strcpy_s(lConfig.mWiFi_Password, aPassword);

            STEP("Setting the configuration");

            lResult = aDevice->Config_Set(&lConfig);
            if (EthCAN_OK == lResult)
            {
                lResult = StoreConfigAndReset(&lStep, aDevice, EthCAN_FLAG_STORE_IPv4 | EthCAN_FLAG_STORE_WIFI);
                if (EthCAN_OK == lResult)
                {
                    printf(
                        "IMPORTANT: Because the configured EthCAN is now a WiFi access point, you need\n"
                        "           to connect your computer to the configured WiFi network if you want\n"
                        "           to communicate with it through the network.\n");
                }
            }
        }
    }
    END;
}

EthCAN_Result Setup_Bridge(EthCAN::System* aSystem, unsigned int aIndex[2])
{
    BEGIN;
    {
        EthCAN_Config lConfigs[2];
        EthCAN::Device* lDevices[2];

        RetrieveDevices(&lStep, aSystem, aIndex, lDevices);

        lResult = RetrieveConfigs(&lStep, lDevices, lConfigs);
        if (EthCAN_OK == lResult)
        {
            EthCAN_Info lInfos[2];

            for (unsigned int i = 0; i < 2; i++)
            {
                // TODO EthCAN_Tool Verify the device is connected to a network

                STEP("Modifying the configuration");

                lConfigs[i].mCAN_Flags &= ~EthCAN_FLAG_CAN_FILTERS_ON;
                lConfigs[i].mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
                lConfigs[i].mServer_Port = EthCAN_UDP_PORT;

                STEP("Retrieving the information");

                lResult = lDevices[i]->GetInfo(lInfos + i);
                if (EthCAN_OK != lResult)
                {
                    break;
                }
            }

            if (EthCAN_OK == lResult)
            {
                STEP("Modifying the configurations");

                lConfigs[0].mServer_IPv4 = lInfos[1].mIPv4_Address;
                lConfigs[1].mServer_IPv4 = lInfos[0].mIPv4_Address;

                lResult = SetConfigs(&lStep, lDevices, lConfigs);
                if (EthCAN_OK == lResult)
                {
                    lResult = StoreConfigsAndReset(&lStep, lDevices, EthCAN_FLAG_STORE_CAN | EthCAN_FLAG_STORE_SERVER);
                }
            }
        }

        ReleaseDevices(&lStep, lDevices);
    }
    END;
}

EthCAN_Result Setup_IPv4(EthCAN::Device* aDevice, uint32_t aAddress, uint32_t aGateway, uint32_t aNetMask)
{
    assert(NULL != aDevice);

    BEGIN;
    {
        EthCAN_Config lConfig;

        STEP("Retrieving the configuration");

        lResult = aDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lResult)
        {
            STEP("Modifying the configuration");

            lConfig.mIPv4_Address = aAddress;
            lConfig.mIPv4_Gateway = aGateway;
            lConfig.mIPv4_NetMask = aNetMask;

            STEP("Setting the configuration");

            lResult = aDevice->Config_Set(&lConfig);
            if (EthCAN_OK == lResult)
            {
                lResult = StoreConfigAndReset(&lStep, aDevice, EthCAN_FLAG_STORE_IPv4);
                if (EthCAN_OK == lResult)
                {
                    // TODO EthCAN_Tool
                    //      Copy message from the guide
                    printf(
                        "IMPORTANT: Because the IPv4 address of the EthCAN most probably changed, if you\n"
                        "           want to communicate with it through the network, you need to restart\n"
                        "           the device detection by executing the command \"Detect\".\n");
                }
            }
        }
    }
    END;
}

EthCAN_Result Setup_Link(EthCAN::System* aSystem, unsigned int aIndex[2], uint32_t aAddress, uint32_t aNetMask, const char* aSSID, const char* aPassword)
{
    assert(NULL != aSSID);
    assert(NULL != aPassword);

    BEGIN;
    {
        EthCAN_Config lConfigs[2];
        EthCAN::Device* lDevices[2];

        RetrieveDevices(&lStep, aSystem, aIndex, lDevices);

        lResult = RetrieveConfigs(&lStep, lDevices, lConfigs);
        if (EthCAN_OK == lResult)
        {
            for (unsigned int i = 0; i < 2; i++)
            {
                STEP("Modifying the configuration");

                lConfigs[i].mCAN_Flags    &= ~EthCAN_FLAG_CAN_FILTERS_ON;
                lConfigs[i].mIPv4_Gateway  = 0;
                lConfigs[i].mIPv4_NetMask  = aNetMask;
                lConfigs[i].mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
                lConfigs[i].mServer_Port   = EthCAN_UDP_PORT;
                lConfigs[i].mWiFi_Flags   &= ~EthCAN_FLAG_WIFI_AP;
                strcpy_s(lConfigs[i].mWiFi_Name    , aSSID);
                strcpy_s(lConfigs[i].mWiFi_Password, aPassword);
            }

            STEP("Modifying the configurations");

            lConfigs[0].mIPv4_Address = aAddress;
            lConfigs[0].mServer_IPv4  = aAddress + 0x01000000;
            lConfigs[1].mIPv4_Address = aAddress + 0x01000000;
            lConfigs[1].mServer_IPv4  = aAddress;

            if ('\0' != aSSID[0])
            {
                lConfigs[0].mWiFi_Flags |= EthCAN_FLAG_WIFI_AP;
            }

            lResult = SetConfigs(&lStep, lDevices, lConfigs);
            if (EthCAN_OK == lResult)
            {
                lResult = StoreConfigsAndReset(&lStep, lDevices, EthCAN_FLAG_STORE_CAN | EthCAN_FLAG_STORE_IPv4 | EthCAN_FLAG_STORE_SERVER | EthCAN_FLAG_STORE_WIFI);
                if (EthCAN_OK == lResult)
                {
                    printf(
                        "IMPORTANT: Because the configured EthCAN are now on a specific WiFi, you need\n"
                        "           to connect your computer to this specific WiFi network if you want\n"
                        "           to communicate with them through the network.\n");
                }
            }
        }
        ReleaseDevices(&lStep, lDevices);
    }
    END;
}

EthCAN_Result Setup_Sniffer(EthCAN::Device* aDevice)
{
    assert(NULL != aDevice);

    BEGIN;
    {
        EthCAN_Config lConfig;

        STEP("Retrieving the configuration");

        lResult = aDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lResult)
        {
            STEP("Modifying the configuration");

            lConfig.mCAN_Flags &= ~EthCAN_FLAG_CAN_FILTERS_ON;

            STEP("Setting the configuration");

            lResult = aDevice->Config_Set(&lConfig);
            if (EthCAN_OK == lResult)
            {
                STEP("Starting the receiver");

                lResult = aDevice->Receiver_Start(Receiver, NULL);
            }
        }
    }
    END;
}

EthCAN_Result Setup_WiFi(EthCAN::Device* aDevice, const char* aSSID, const char* aPassword)
{
    assert(NULL != aDevice);
    assert(NULL != aSSID);
    assert(NULL != aPassword);

    BEGIN;
    {
        EthCAN_Config lConfig;

        STEP("Retrieving the configuration");

        lResult = aDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lResult)
        {
            STEP("Modifying the configuration");

            lConfig.mWiFi_Flags &= ~EthCAN_FLAG_WIFI_AP;
            strcpy_s(lConfig.mWiFi_Name    , aSSID);
            strcpy_s(lConfig.mWiFi_Password, aPassword);

            STEP("Setting the configuration");

            lResult = aDevice->Config_Set(&lConfig);
            if (EthCAN_OK == lResult)
            {
                lResult = StoreConfigAndReset(&lStep, aDevice, EthCAN_FLAG_STORE_WIFI);
            }
        }
    }
    END;
}

EthCAN_Result Setup_Wireshark(EthCAN::Device* aDevice)
{
    assert(NULL != aDevice);

    BEGIN;
    {
        EthCAN_Config lConfig;

        STEP("Retrieving the configuration");

        lResult = aDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lResult)
        {
            STEP("Modifying the configuration");

            lConfig.mCAN_Flags    &= ~EthCAN_FLAG_CAN_FILTERS_ON;
            lConfig.mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
            lConfig.mServer_IPv4   = aDevice->GetHostAddress();
            lConfig.mServer_Port   = EthCAN_UDP_PORT;

            STEP("Setting the configuration");

            lResult = aDevice->Config_Set(&lConfig);
        }
    }
    END;
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame)
{
    EthCAN::Display(NULL, aFrame);
    return true;
}

#define STEP_I(D) printf("%u. " D "...\n", *aStep); (*aStep)++

void ReleaseDevices(unsigned int* aStep, EthCAN::Device* aDevices[2])
{
    assert(NULL != aDevices);

    for (unsigned int i = 0; i < 2; i++)
    {
        assert(NULL != aDevices[i]);

        STEP_I("Releasing device");

        aDevices[i]->Release();
    }
}

void RetrieveDevices(unsigned int* aStep, EthCAN::System* aSystem, unsigned int aIndex[2], EthCAN::Device* aDevices[2])
{
    assert(NULL != aSystem);
    assert(NULL != aIndex);
    assert(NULL != aDevices);

    for (unsigned int i = 0; i < 2; i++)
    {
        STEP_I("Retrieving the device");

        aDevices[i] = aSystem->Device_Get(aIndex[0]);
        assert(NULL != aDevices[i]);
    }
}

#define BEGIN_I EthCAN_Result lResult = EthCAN_RESULT_INVALID
#define END_I   return lResult

EthCAN_Result RetrieveConfigs(unsigned int* aStep, EthCAN::Device* aDevices[2], EthCAN_Config aConfigs[2])
{
    assert(NULL != aDevices);
    assert(NULL != aConfigs);

    BEGIN_I;
    {
        for (unsigned i = 0; i < 2; i++)
        {
            assert(NULL != aDevices[i]);

            STEP_I("Retrieving the configuration");

            lResult = aDevices[i]->Config_Get(aConfigs + i);
            if (EthCAN_OK != lResult)
            {
                break;
            }
        }
    }
    END_I;
}

EthCAN_Result SetConfigs(unsigned int* aStep, EthCAN::Device* aDevices[2], EthCAN_Config aConfigs[2])
{
    assert(NULL != aDevices);
    assert(NULL != aConfigs);

    BEGIN_I;
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            STEP_I("Setting the configuration");

            lResult = aDevices[i]->Config_Set(aConfigs + i);
            if (EthCAN_OK != lResult)
            {
                break;
            }
        }
    }
    END_I;
}

EthCAN_Result StoreConfigAndReset(unsigned int* aStep, EthCAN::Device* aDevice, uint8_t aStoreFlags)
{
    assert(NULL != aDevice);

    BEGIN_I;
    {
        STEP_I("Storing the configuration");

        lResult = aDevice->Config_Store(EthCAN_FLAG_STORE_IPv4 | EthCAN_FLAG_STORE_WIFI);
        if (EthCAN_OK == lResult)
        {
            STEP_I("Reseting the EthCAN");

            lResult = aDevice->Reset();
        }
    }
    END_I;
}

EthCAN_Result StoreConfigsAndReset(unsigned int* aStep, EthCAN::Device* aDevices[2], uint8_t aStoreFlags)
{
    assert(NULL != aDevices);

    BEGIN_I;
    {
        for (unsigned int i = 0; i < 2; i++)
        {
            lResult = StoreConfigAndReset(aStep, aDevices[i], aStoreFlags);
            if (EthCAN_OK != lResult)
            {
                break;
            }
        }
    }
    END_I;
}
