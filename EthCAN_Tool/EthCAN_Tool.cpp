
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Tool/EthCAN_Tool.cpp

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>

// ===== Import/Includes ====================================================
#include <KmsLib/ToolBase.h>
#include <KmsTool.h>

// ===== Includes ===========================================================
#include <EthCAN/Device.h>
#include <EthCAN/System.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// Commands
/////////////////////////////////////////////////////////////////////////////

static void Config_CAN_Filters(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_CAN_Masks  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_CAN_Rate   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_CAN_COMMANDS[] =
{
    { "Filters", Config_CAN_Filters, "Filters {0_hex} ... {5_hex}   See EthCAN_Config::mCAN_Filters"},
    { "Masks"  , Config_CAN_Masks  , "Masks {0_hex} {1_hex}         See EthCAN_Config::mCAN_Masks"  },
    { "Rate"   , Config_CAN_Rate   , "Rate [Rate_Kbps]              See EthCAN_Config::mCAN_Rate"   },

    { NULL, NULL, NULL, NULL }
};

static void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_IPv4   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Name   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Server (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_WiFi   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_COMMANDS[] =
{
    { "CAN"    , NULL          , "CAN ..."                                                , CONFIG_CAN_COMMANDS },
    { "Display", Config_Display, "Display                       See Device::Display"      , NULL },
    { "IP"     , Config_IPv4   , "IP {Address} {Gateway} {NetMask}\n"
                                 "                              See EthCAN_Config"        , NULL },
    { "Name"   , Config_Name   , "Name [Name]                   See EthCAN_Config::mName" , NULL },
    { "Server" , Config_Server , "Server {Address} [Port]       See EthCAN_Config"        , NULL },
    { "WiFi"   , Config_WiFi   , "WiFi [Name] [Password]        See EthCAN_Config"        , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Device_Config_Get  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Reset(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Set  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Store(KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo DEVICE_CONFIG_COMMANDS[] =
{
    { "Get"  , Device_Config_Get  , "Get                           See Device::Config_Get"  , NULL },
    { "Reset", Device_Config_Reset, "Reset                         See Device::Config_Reset", NULL },
    { "Set"  , Device_Config_Set  , "Set                           See Device::Config_Set"  , NULL },
    { "Store", Device_Config_Store, "Store [Flags_hex]             See Device::Config_Store", NULL },

    { NULL, NULL, NULL, NULL }
};

static void Device_Receiver_Start(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Receiver_Stop (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo DEVICE_RECEIVER_COMMANDS[] =
{
    { "Start", Device_Receiver_Start, "Start                         See Device::Receiver_Start", NULL },
    { "Stop" , Device_Receiver_Stop , "Stop                          See Device::Receiver_Start", NULL },

    { NULL, NULL, NULL, NULL }
};

static void Device_Debug  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_GetInfo(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Release(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Reset  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Send   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo DEVICE_COMMANDS[] =
{
    { "Config"  , NULL          , "Config ..."                                       , DEVICE_CONFIG_COMMANDS },
    { "Debug"   , Device_Debug  , "Debug                         See Object::Debug"  , NULL },
    { "GetInfo" , Device_GetInfo, "GetInfo                       See Device::GetInfo", NULL },
    { "Receiver", NULL          , "Receiver ..."                                     , DEVICE_RECEIVER_COMMANDS },
    { "Release" , Device_Release, "Release                       See Object::Release", NULL },
    { "Reset"   , Device_Reset  , "Reset                         See Device::Reset"  , NULL },
    { "Send"    , Device_Send   , "Send [Ext] [Id] [Count] ...   See Device::Send"   , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Select_Ethernet(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Select_Index   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Select_IP      (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Select_Name    (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Select_USB     (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo SELECT_COMMANDS[] =
{
    { "Ethernet", Select_Ethernet, "Ethernet {Address}            See System::Device_Find_Eth" , NULL },
    { "Index"   , Select_Index   , "Index [Index]                 See System::Device_Get"      , NULL },
    { "IP"      , Select_IP      , "IP {Address}                  See System::Device_Find_IPv4", NULL },
    { "Name"    , Select_Name    , "Name [Name]                   See System::Device_Find_Name", NULL },
    { "USB"     , Select_Index   , "USB [Index]                   See System::Device_Find_USB" , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Setup_AccessPoint(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Bridge     (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_DHCP       (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Link       (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Sniffer    (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_StaticIP   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_WiFi       (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo SETUP_COMMANDS[] =
{
    { "AccessPoint", Setup_AccessPoint, "AccessPoint {Address} {NetMask} [SSID] [Password]\n"
                                        "                              Setup the sel. EthCAN as an access point", NULL },
    { "Bridge"     , Setup_Bridge     , "Bridge [Index0] [Index1]      Setup a bridge between two EthCAN"       , NULL },
    { "DHCP"       , Setup_DHCP       , "DHCP                          Setup the selected EthCan to use DHCP"   , NULL },
    { "Link"       , Setup_Link       , "Link {Index0} {Index1} {IPv4} {Mask} [SSID] [Password]\n"
                                        "                              Setup a direct link between thwo EthCAN" , NULL },
    { "Sniffer"    , Setup_Sniffer    , "Sniffer                       Setup the selected EthCAN as sniffer"    , NULL },
    { "StaticIP"   , Setup_StaticIP   , "StaticIP {Address} {Gateway} {NetMask}\n"
                                        "                              Setup the sel. EthCAN with a static IP"  , NULL },
    { "WiFi"       , Setup_WiFi       , "WiFi [SSID] [Password]        Setup the WiFi for the selected EthCAN"  , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Debug     (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Detect    (KmsLib::ToolBase* aToolBase, const char* aArg);
static void GetVersion(KmsLib::ToolBase* aToolBase, const char* aArg);
static void List      (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo COMMANDS[] =
{
    { "Config"    , NULL      , "Config ..."                                          , CONFIG_COMMANDS },
    { "Debug"     , Debug     , "Debug                         See Object::Debug"     , NULL },
    { "Detect"    , Detect    , "Detect                        See System::Detect"    , NULL },
    { "Device"    , NULL      , "Device ..."                                          , DEVICE_COMMANDS },
    { "GetVersion", GetVersion, "GetVersion                    See System::GetVersion", NULL },
    { "List"      , List      , "List                          List detected device"  , NULL },
    { "Select"    , NULL      , "Select ..."                                          , SELECT_COMMANDS },
    { "Setup"     , NULL      , "Setup ..."                                           , SETUP_COMMANDS  },

    KMS_LIB_TOOL_BASE_FUNCTIONS

    { NULL, NULL, NULL, NULL }
};

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void DisplayResult(KmsLib::ToolBase* aToolBase, EthCAN_Result aResult);

static bool Parse_Address_Gateway_NetMask(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aAddress, uint32_t* aGateway, uint32_t* aNetMask);
static bool Parse_Address_NetMask(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aAddr, uint32_t* aMask);
static bool Parse_Ethernet     (KmsLib::ToolBase* aToolBase, const char** aArg, uint8_t aOut[6]);
static bool Parse_IPv4         (KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aOut);
static bool Parse_SSID_Password(KmsLib::ToolBase* aToolBase, const char** aArg, char* aName, char* aPassword);

static bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

static void ReleaseDevice();

static void Setup_IPv4(KmsLib::ToolBase* aToolBase, uint32_t aAddress, uint32_t aGateway, uint32_t aNetMask);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static EthCAN_Config sConfig;

static EthCAN::Device* sDevice = NULL;
static EthCAN::System* sSystem = NULL;

// Entry point
/////////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_TOOL_BANNER("DrvDMA", "DrvDMA_Tool", VERSION_STR, VERSION_TYPE);

    int lResult = 0;

    KmsLib::ToolBase lToolBase(COMMANDS);

    sSystem = EthCAN::System::Create();
    assert(NULL != sSystem);

    try
    {
        lToolBase.ParseArguments(aCount, aVector);

        lResult = lToolBase.ParseCommands();
    }
    catch (...)
    {
        lResult = lToolBase.SetError(__LINE__, "Unexpected exception (NOT TESTED)", KmsLib::ToolBase::REPORT_FATAL_ERROR);
    }

    ReleaseDevice();

    sSystem->Release();

    return lResult;
}

// Command
/////////////////////////////////////////////////////////////////////////////

void Config_CAN_Filters(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    for (unsigned int i = 0; i < 6; i ++)
    {
        if (!aToolBase->Parse(&lArg, sConfig.mCAN_Filters + i, 0, 0x9fffffff))
        {
            return;
        }
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Config_CAN_Masks(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    for (unsigned int i = 0; i < 2; i++)
    {
        if (!aToolBase->Parse(&lArg, sConfig.mCAN_Filters + i, 0, 0x9fffffff, 0))
        {
            return;
        }
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Config_CAN_Rate(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    unsigned int lRate;

    if (aToolBase->Parse(&lArg, &lRate, 5, 1000, false, 1000))
    {
        if      (  10 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_5_Kb; }
        else if (  20 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_10_Kb; }
        else if (  25 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_20_Kb; }
        else if (  31 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_25_Kb; }
        else if (  33 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_31_2_Kb; }
        else if (  40 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_33_Kb; }
        else if (  50 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_40_Kb; }
        else if (  80 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_50_Kb; }
        else if (  83 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_80_Kb; }
        else if (  95 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_83_3_Kb; }
        else if ( 100 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_95_Kb; }
        else if ( 125 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_100_Kb; }
        else if ( 200 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_125_Kb; }
        else if ( 250 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_200_Kb; }
        else if ( 500 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_250_Kb; }
        else if ( 666 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_500_Kb; }
        else if (1000 > lRate) { sConfig.mCAN_Rate = EthCAN_RATE_666_Kb; }
        else                   { sConfig.mCAN_Rate = EthCAN_RATE_1_Mb; }

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    EthCAN::Device::Display(NULL, sConfig);

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Config_IPv4(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    if (Parse_Address_Gateway_NetMask(aToolBase, &lArg, &sConfig.mIPv4_Address, &sConfig.mIPv4_Gateway, &sConfig.mIPv4_NetMask))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Config_Name(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    if (aToolBase->Parse(&lArg, sConfig.mName, sizeof(sConfig.mName), "EthCAN"))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Config_Server(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    unsigned int lPort;

    if (   Parse_IPv4(aToolBase, &lArg, &sConfig.mServer_IPv4)
        && aToolBase->Parse(&lArg, &lPort, 0, 0xffff, false, 0))
    {
        sConfig.mServer_Port = lPort;

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Config_WiFi(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    if (   aToolBase->Parse(&lArg, sConfig.mWiFi_Name    , sizeof(sConfig.mWiFi_Name    ), "")
        && aToolBase->Parse(&lArg, sConfig.mWiFi_Password, sizeof(sConfig.mWiFi_Password), ""))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

#define USE_SELECTED_DEVICE                                  \
    if (NULL == sDevice)                                     \
    {                                                        \
        aToolBase->SetError(__LINE__, "No device selected"); \
        return;                                              \
    }

void Device_Config_Get(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Config_Get(&sConfig));
}

void Device_Config_Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Config_Reset());
}

void Device_Config_Set(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Config_Set(&sConfig));
}

void Device_Config_Store(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, 0x1f, true, 0))
    {
        DisplayResult(aToolBase, sDevice->Config_Store(lFlags));
    }
}

void Device_Receiver_Start(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Receiver_Start(Receiver, NULL));
}

void Device_Receiver_Stop(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Receiver_Stop());
}

void Device_Debug(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    sDevice->Debug();

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Device_GetInfo(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    EthCAN_Info lInfo;

    EthCAN_Result lRet = sDevice->GetInfo(&lInfo);
    if (EthCAN_OK == lRet)
    {
        EthCAN::Device::Display(NULL, lInfo);
    }

    DisplayResult(aToolBase, lRet);
}

void Device_Release(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    sDevice->Release();
    sDevice = NULL;

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Device_Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Reset());
}

void Device_Send(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    bool lExt;
    unsigned int lCount;
    EthCAN_Frame lFrame;

    if (   aToolBase->Parse(&lArg, &lExt, true)
        && aToolBase->Parse(&lArg, &lFrame.mDestId, 0, 0x1fffffff, true, 0)
        && aToolBase->Parse(&lArg, &lCount, 1, 8, false, 1))
    {
        lFrame.mDataSize_byte = lCount;
        lFrame.mFlags = lExt ? EthCAN_FLAG_EXTENDED : 0;

        bool lParsed = true;

        for (unsigned int i = 0; lParsed && (i < lCount); i++)
        {
            unsigned int lData;

            lParsed = aToolBase->Parse(&lArg, &lData, 0, 0xff, true, 0);
            if (lParsed)
            {
                lFrame.mData[i] = lData;
            }
        }

        if (lParsed)
        {
            DisplayResult(aToolBase, sDevice->Send(lFrame));
        }
    }
}

#define USE_DEVICE                                           \
    if (0 >= sSystem->Device_GetCount())                     \
    {                                                        \
        aToolBase->SetError(__LINE__, "No detected device"); \
        return;                                              \
    }

void Select_Ethernet(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    const char* lArg = aArg;
    uint8_t lEth[6];

    if (Parse_Ethernet(aToolBase, &lArg, lEth))
    {
        ReleaseDevice();

        sDevice = sSystem->Device_Find_Eth(lEth);
        if (NULL == sDevice)
        {
            aToolBase->SetError(__LINE__, "Ethernet address not found");
        }
        else
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
        }
    }
}

void Select_Index(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    const char* lArg = aArg;
    unsigned int lIndex;

    if (aToolBase->Parse(&lArg, &lIndex, 0, sSystem->Device_GetCount() - 1, false, 0))
    {
        ReleaseDevice();

        sDevice = sSystem->Device_Get(lIndex);
        assert(NULL != sDevice);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Select_IP(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE

    const char* lArg = aArg;
    uint32_t lIPv4;

    if (Parse_IPv4(aToolBase, &lArg, &lIPv4))
    {
        ReleaseDevice();

        sDevice = sSystem->Device_Find_IPv4(lIPv4);
        if (NULL == sDevice)
        {
            aToolBase->SetError(__LINE__, "IPv4 address not found");
        }
        else
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
        }
    }
}

void Select_Name(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    const char* lArg = aArg;
    char lName[16];

    if (aToolBase->Parse(&lArg, lName, sizeof(lName), "EthCAN"))
    {
        ReleaseDevice();

        sDevice = sSystem->Device_Find_Name(lName);
        if (NULL == sDevice)
        {
            aToolBase->SetError(__LINE__, "Name not found");
        }
        else
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
        }
    }
}

void Select_USB(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    const char* lArg = aArg;
    unsigned int lIndex;

    if (aToolBase->Parse(&lArg, &lIndex, 0, sSystem->Device_GetCount() - 1, false, 0))
    {
        ReleaseDevice();

        sDevice = sSystem->Device_Find_USB(lIndex);
        if (NULL == sDevice)
        {
            aToolBase->SetError(__LINE__, "Not enough USB connected devices");
        }
        else
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
        }
    }
}

void Setup_AccessPoint(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    unsigned int lAddress;
    unsigned int lNetMask;
    char lName[32];
    char lPassword[32];

    if (   Parse_Address_NetMask(aToolBase, &lArg, &lAddress, &lNetMask)
        && Parse_SSID_Password  (aToolBase, &lArg, lName, lPassword))
    {
        EthCAN_Config lConfig;
        unsigned int lStep = 1;

        printf("%u. Retrieving the configuration...\n", lStep); lStep++;
        EthCAN_Result lRet = sDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lRet)
        {
            printf("%u. Modifying the configuration...\n", lStep); lStep++;
            lConfig.mIPv4_Address = lAddress;
            lConfig.mIPv4_Gateway = 0;
            lConfig.mIPv4_NetMask = lNetMask;
            lConfig.mWiFi_Flags |= EthCAN_FLAG_WIFI_AP;
            strcpy_s(lConfig.mWiFi_Name    , lName);
            strcpy_s(lConfig.mWiFi_Password, lPassword);

            printf("%u. Setting the configuration...\n", lStep); lStep++;
            lRet = sDevice->Config_Set(&lConfig);
            if (EthCAN_OK == lRet)
            {
                printf("%u. Storing the configuration...\n", lStep); lStep++;
                lRet = sDevice->Config_Store(EthCAN_FLAG_STORE_IPv4 | EthCAN_FLAG_STORE_WIFI);
                if (EthCAN_OK == lRet)
                {
                    printf("%u. Reseting the EthCAN...\n", lStep); lStep++;
                    lRet = sDevice->Reset();
                    if (EthCAN_OK == lRet)
                    {
                        printf(
                            "IMPORTANT: Because the configured EthCAN is now a WiFi access point, you need\n"
                            "           to connect your computer to the configured WiFi network if you want\n"
                            "           to communicate with it through the network.\n");
                    }
                }
            }
        }

        DisplayResult(aToolBase, lRet);
    }
}

void Setup_Bridge(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    unsigned int lCount = sSystem->Device_GetCount();
    if (2 > lCount)
    {
        aToolBase->SetError(__LINE__, "Not enough detected devices");
        return;
    }

    const char* lArg = aArg;
    unsigned int lIndex[2];

    if (   aToolBase->Parse(&lArg, lIndex + 0, 0, lCount - 2, false, 0)
        && aToolBase->Parse(&lArg, lIndex + 1, 1, lCount - 1, false, 1))
    {
        if (lIndex[0] == lIndex[1])
        {
            aToolBase->SetError(__LINE__, "Cannot bridge a device with itself");
            return;
        }

        EthCAN_Config lConfigs[2];
        EthCAN::Device* lDevices[2];
        EthCAN_Info lInfos[2];
        EthCAN_Result lRet = EthCAN_RESULT_INVALID;
        unsigned int lStep = 1;
        unsigned int i;

        for (i = 0; i < 2; i++)
        {
            printf("%u. Retrieving the device...\n", lStep); lStep++;
            lDevices[i] = sSystem->Device_Get(lIndex[0]);
            assert(NULL != lDevices[i]);

            // TODO Verify the device is connected to a network

            printf("%u. Retrieving the configuration...\n", lStep); lStep++;
            lRet = lDevices[i]->Config_Get(lConfigs + i);
            if (EthCAN_OK == lRet)
            {
                printf("%u. Modifying the configuration...\n", lStep); lStep++;
                lConfigs[i].mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
                lConfigs[i].mServer_Port = EthCAN_UDP_PORT;

                // TODO Setup filters and masks

                printf("%u. Retrieving the information...\n", lStep); lStep++;
                lRet = lDevices[i]->GetInfo(lInfos + i);
            }

            if (EthCAN_OK != lRet)
            {
                break;
            }
        }

        printf("%u. Modifying the configurations...\n", lStep); lStep++;
        lConfigs[0].mServer_IPv4 = lInfos[1].mIPv4_Address;
        lConfigs[1].mServer_IPv4 = lInfos[0].mIPv4_Address;

        for (i = 0; i < 2; i++)
        {
            if (EthCAN_OK == lRet)
            {
                printf("%u. Setting the configuration...\n", lStep); lStep++;
                lRet = lDevices[i]->Config_Set(lConfigs + i);
                if (EthCAN_OK == lRet)
                {
                    printf("%u. Storing the configuration...\n", lStep); lStep++;
                    lRet = lDevices[i]->Config_Store(EthCAN_FLAG_STORE_CAN | EthCAN_FLAG_STORE_SERVER);
                    if (EthCAN_OK == lRet)
                    {
                        printf("%u. Reseting the EthCAN...\n", lStep); lStep++;
                        lRet = lDevices[i]->Reset();
                    }
                }
            }

            lDevices[i]->Release();
        }

        DisplayResult(aToolBase, lRet);
    }
}

void Setup_DHCP(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    Setup_IPv4(aToolBase, 0, 0, 0);
}

void Setup_Link(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    unsigned int lCount = sSystem->Device_GetCount();
    if (2 > lCount)
    {
        aToolBase->SetError(__LINE__, "Not enough connected devices");
        return;
    }

    const char* lArg = aArg;
    unsigned int lIndex[2];
    unsigned int lAddress;
    unsigned int lNetMask;
    char lName[32];
    char lPassword[32];

    if (   aToolBase->Parse(&lArg, lIndex + 0, 0, lCount - 2, false)
        && aToolBase->Parse(&lArg, lIndex + 1, 1, lCount - 1, false)
        && Parse_Address_NetMask(aToolBase, &lArg, &lAddress, &lNetMask)
        && aToolBase->Parse(&lArg, lName    , sizeof(lName    ), "")
        && aToolBase->Parse(&lArg, lPassword, sizeof(lPassword), ""))
    {
        if (lIndex[0] == lIndex[1])
        {
            aToolBase->SetError(__LINE__, "Cannot link a device to itself\n");
            return;
        }

        EthCAN_Config lConfigs[2];
        EthCAN::Device* lDevices[2];
        EthCAN_Result lRet = EthCAN_RESULT_INVALID;
        unsigned int lStep = 1;
        unsigned int i;

        for (i = 0; i < 2; i++)
        {
            printf("%u. Retrieving the device...\n", lStep); lStep++;
            lDevices[i] = sSystem->Device_Get(lIndex[0]);
            assert(NULL != lDevices[i]);

            printf("%u. Retrieving the configuration...\n", lStep); lStep++;
            lRet = lDevices[i]->Config_Get(lConfigs + i);
            if (EthCAN_OK != lRet)
            {
                break;
            }

            printf("%u. Modifying the configuration...\n", lStep); lStep++;
            lConfigs[i].mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
            lConfigs[i].mServer_Port = EthCAN_UDP_PORT;
            lConfigs[i].mWiFi_Flags &= ~EthCAN_FLAG_WIFI_AP;
            strcpy_s(lConfigs[i].mWiFi_Name    , lName);
            strcpy_s(lConfigs[i].mWiFi_Password, lPassword);

            // TODO Setup filters and masks
        }

        printf("%u. Modifying the configurations...\n", lStep); lStep++;
        lConfigs[0].mIPv4_Address = lAddress;
        lConfigs[0].mIPv4_Gateway = 0;
        lConfigs[0].mIPv4_NetMask = lNetMask;
        lConfigs[0].mServer_IPv4  = lAddress + 0x01000000;
        lConfigs[1].mIPv4_Address = lAddress + 0x01000000;
        lConfigs[1].mIPv4_Gateway = 0;
        lConfigs[1].mIPv4_NetMask = lNetMask;
        lConfigs[1].mServer_IPv4  = lAddress;

        if ('\0' != lName[0])
        {
            lConfigs[0].mWiFi_Flags |= EthCAN_FLAG_WIFI_AP;
        }

        for (i = 0; i < 2; i++)
        {
            if (EthCAN_OK == lRet)
            {
                printf("%u. Setting the configuration...\n", lStep); lStep++;
                lRet = lDevices[i]->Config_Set(lConfigs + i);
                if (EthCAN_OK == lRet)
                {
                    printf("%u. Storing the configuration...\n", lStep); lStep++;
                    lRet = lDevices[i]->Config_Store(EthCAN_FLAG_STORE_CAN | EthCAN_FLAG_STORE_IPv4 | EthCAN_FLAG_STORE_SERVER | EthCAN_FLAG_STORE_WIFI);
                    if (EthCAN_OK == lRet)
                    {
                        printf("%u. Reseting the EthCAN...\n", lStep); lStep++;
                        lRet = lDevices[i]->Reset();
                        if (EthCAN_OK == lRet)
                        {
                            printf(
                                "IMPORTANT: Because the configured EthCAN are now on a specific WiFi, you need\n"
                                "           to connect your computer to this specific WiFi network if you want\n"
                                "           to communicate with them through the network.\n");
                        }
                    }
                }
            }

            lDevices[i]->Release();
        }

        DisplayResult(aToolBase, lRet);
    }
}

void Setup_Sniffer(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    EthCAN_Config lConfig;
    unsigned int lStep = 1;

    printf("%u. Retrieving the configuration...\n", lStep); lStep++;
    EthCAN_Result lRet = sDevice->Config_Get(&lConfig);
    if (EthCAN_OK == lRet)
    {
        printf("%u. Modifying the configuration...\n", lStep); lStep++;
        // TODO Configure filters and masks.

        printf("%u. Setting the configuration...\n", lStep); lStep++;
        lRet = sDevice->Config_Set(&lConfig);
        if (EthCAN_OK == lRet)
        {
            printf("%u. Starting the receiver...\n", lStep); lStep++;
            lRet = sDevice->Receiver_Start(Receiver, NULL);
        }
    }

    DisplayResult(aToolBase, lRet);
}

void Setup_StaticIP(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    uint32_t lAddress;
    uint32_t lGateway;
    uint32_t lNetMask;

    if (Parse_Address_Gateway_NetMask(aToolBase, &lArg, &lAddress, &lGateway, &lNetMask))
    {
        Setup_IPv4(aToolBase, lAddress, lGateway, lNetMask);
    }
}

void Setup_WiFi(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    char lName[32];
    char lPassword[32];

    if (Parse_SSID_Password(aToolBase, &lArg, lName, lPassword))
    {
        EthCAN_Config lConfig;
        unsigned int lStep = 1;

        printf("%u. Retrieving the configuration...\n", lStep); lStep++;
        EthCAN_Result lRet = sDevice->Config_Get(&lConfig);
        if (EthCAN_OK == lRet)
        {
            printf("%u. Modifying the configuration...\n", lStep); lStep++;
            lConfig.mWiFi_Flags &= ~EthCAN_FLAG_WIFI_AP;
            strcpy_s(lConfig.mWiFi_Name    , lName);
            strcpy_s(lConfig.mWiFi_Password, lPassword);

            printf("%u. Setting the configuration...\n", lStep); lStep++;
            lRet = sDevice->Config_Set(&lConfig);
            if (EthCAN_OK == lRet)
            {
                printf("%u. Storing the configuration...\n", lStep); lStep++;
                lRet = sDevice->Config_Store(EthCAN_FLAG_STORE_WIFI);
                if (EthCAN_OK == lRet)
                {
                    printf("%u. Reseting the EthCAN...\n", lStep); lStep++;
                    lRet = sDevice->Reset();
                }
            }
        }

        DisplayResult(aToolBase, lRet);
    }
}

void Debug(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    sSystem->Debug();
    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Detect(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    EthCAN_Result lRet = sSystem->Detect();
    if (EthCAN_OK == lRet)
    {
        printf("%u devices detected\n", sSystem->Device_GetCount());
    }

    DisplayResult(aToolBase, lRet);
}

void GetVersion(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    uint8_t lVersion[4];

    EthCAN_Result lRet = EthCAN::System::GetVersion(lVersion);
    if (EthCAN_OK == lRet)
    {
        printf("%u.%u.%u.%u\n", lVersion[0], lVersion[1], lVersion[2], lVersion[3]);
    }

    DisplayResult(aToolBase, lRet);
}

void List(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    unsigned int lCount = sSystem->Device_GetCount();
    printf("%u detected devices\n", lCount);

    printf(" # Con  Ethernet address   IPv4 address         Name\n");
    printf("== ==== ================= =============== =================\n");

    for (unsigned int i = 0; i < lCount; i++)
    {
        EthCAN::Device* lDev = sSystem->Device_Get(i);

        EthCAN_Info lInfo;

        EthCAN_Result lRet = lDev->GetInfo(&lInfo);
        assert(EthCAN_OK == lRet);

        const char* lCon;
        if (lDev->IsConnectedEth())
        {
            if (lDev->IsConnectedUSB())
            {
                lCon = "Both";
            }
            else
            {
                lCon = "Eth ";
            }
        }
        else
        {
            lCon = "USB ";
        }

        printf("%2u %s %02x:%02x:%02x:%02x:%02x:%02x %3u.%3u.%3u.%3u %s\n",
            i,
            lCon,
            lInfo.mEth_Addr[0], lInfo.mEth_Addr[1], lInfo.mEth_Addr[2], lInfo.mEth_Addr[3], lInfo.mEth_Addr[4], lInfo.mEth_Addr[5],
            lInfo.mIPv4_Address & 0xff, lInfo.mIPv4_Address >> 8 & 0xff, lInfo.mIPv4_Address >> 16 & 0xff, lInfo.mIPv4_Address >> 24 & 0xff,
            lInfo.mName);

        lDev->Release();
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

void DisplayResult(KmsLib::ToolBase* aToolBase, EthCAN_Result aResult)
{
    const char* lResultName = EthCAN::System::GetResultName(aResult);

    if (EthCAN::System::IsResultOK(aResult))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, lResultName);
    }
    else
    {
        aToolBase->SetError(aResult, lResultName);
    }
}

bool Parse_Address_Gateway_NetMask(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aAddress, uint32_t* aGateway, uint32_t* aNetMask)
{
    return Parse_IPv4(aToolBase, aArg, aAddress)
        && Parse_IPv4(aToolBase, aArg, aGateway)
        && Parse_IPv4(aToolBase, aArg, aNetMask);

}

bool Parse_Address_NetMask(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aAddress, uint32_t* aNetMask)
{
    return Parse_IPv4(aToolBase, aArg, aAddress)
        && Parse_IPv4(aToolBase, aArg, aNetMask);
}

bool Parse_Ethernet(KmsLib::ToolBase* aToolBase, const char** aArg, uint8_t aOut[6])
{
    unsigned int lValues[6];

    if (6 != sscanf_s(*aArg, "%x:%x:%x:%x:%x:%x", lValues + 0, lValues + 1, lValues + 2, lValues + 3, lValues + 4, lValues + 5))
    {
        aToolBase->SetError(__LINE__, "Invalid Ethernet address format");
        return false;
    }

    uint32_t lOut = 0;

    for (unsigned int i = 0; i < 6; i++)
    {
        if (255 < lValues[i])
        {
            aToolBase->SetError(__LINE__, "Invalid Ethernet address value");
            return false;
        }

        aOut[i] = lValues[i];
    }

    *aOut = lOut;

    while (((**aArg >= '0') && (**aArg <= '9'))
        || ((**aArg >= 'A') && (**aArg <= 'F'))
        || ((**aArg >= 'a') && (**aArg <= 'f'))
        || (**aArg == ':'))
    {
        (*aArg)++;
    }

    return true;
}

bool Parse_IPv4(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aOut)
{
    unsigned int lValues[4];

    if (4 != sscanf_s(*aArg, "%u.%u.%u.%u", lValues + 0, lValues + 1, lValues + 2, lValues + 3))
    {
        aToolBase->SetError(__LINE__, "Invalid IPv4 format");
        return false;
    }

    uint32_t lOut = 0;

    for (unsigned int i = 0; i < 4; i ++)
    {
        if (255 < lValues[i])
        {
            aToolBase->SetError(__LINE__, "Invalid IPv4 value");
            return false;
        }

        lOut |= lValues[i] << (8 * i);
    }

    *aOut = lOut;

    while (((**aArg >= '0') && (**aArg <= '9')) || (**aArg == '.'))
    {
        (*aArg)++;
    }

    return true;
}

bool Parse_SSID_Password(KmsLib::ToolBase* aToolBase, const char** aArg, char* aName, char* aPassword)
{
    return aToolBase->Parse(aArg, aName, 32, "EthCAN")
        && aToolBase->Parse(aArg, aName, 32, "EthCANPassword");
}

bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame)
{
    EthCAN::Device::Display(NULL, aFrame);
    return true;
}

void ReleaseDevice()
{
    if (NULL != sDevice)
    {
        sDevice->Release();
        sDevice = NULL;
    }
}

void Setup_IPv4(KmsLib::ToolBase* aToolBase, uint32_t aAddress, uint32_t aGateway, uint32_t aNetMask)
{
    EthCAN_Config lConfig;
    unsigned int lStep = 1;

    printf("%u. Retrieving the configuration...\n", lStep); lStep++;
    EthCAN_Result lRet = sDevice->Config_Get(&lConfig);
    if (EthCAN_OK == lRet)
    {
        printf("%u. Modifying the configuration...\n", lStep); lStep++;
        lConfig.mIPv4_Address = aAddress;
        lConfig.mIPv4_Gateway = aGateway;
        lConfig.mIPv4_NetMask = aNetMask;

        printf("%u. Setting the configuration...\n", lStep); lStep++;
        lRet = sDevice->Config_Set(&lConfig);
        if (EthCAN_OK == lRet)
        {
            printf("%u. Storing the configuration...\n", lStep); lStep++;
            lRet = sDevice->Config_Store(EthCAN_FLAG_STORE_IPv4);
            if (EthCAN_OK == lRet)
            {
                printf("%u. Reseting the EthCAN...\n", lStep); lStep++;
                lRet = sDevice->Reset();
                if (EthCAN_OK == lRet)
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

    DisplayResult(aToolBase, lRet);
}
