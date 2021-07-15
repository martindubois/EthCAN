
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
#include <EthCAN/File.h>
#include <EthCAN/Display.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// ===== EthCAN_Tool ========================================================
#include "Setup.h"

// Commands
/////////////////////////////////////////////////////////////////////////////

static void Config_CAN_Filters(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_CAN_Flags  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_CAN_Masks  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_CAN_Rate   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_CAN_COMMANDS[] =
{
    { "Filters", Config_CAN_Filters, "Filters {0_hex} ... {5_hex}   See EthCAN_Config::mCAN_Filters"},
    { "Flags"  , Config_CAN_Flags  , "Flags {Flags_hex}             See EthCAN_Config::mCAN_Flags"  },
    { "Masks"  , Config_CAN_Masks  , "Masks {0_hex} {1_hex}         See EthCAN_Config::mCAN_Masks"  },
    { "Rate"   , Config_CAN_Rate   , "Rate [Rate_Kbps]              See EthCAN_Config::mCAN_Rate"   },

    { NULL, NULL, NULL, NULL }
};

static void Config_File_Load(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_File_Save(KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_FILE_COMMANDS[] =
{
    { "Load", Config_File_Load, "Load {Name}                     See EthCAN::File_Load" },
    { "Save", Config_File_Save, "Save {Name}                     See EthCAN::File_Save" },

    { NULL, NULL, NULL, NULL }
};

static void Config_Clear  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_IPv4   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Name   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Server (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_WiFi   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_COMMANDS[] =
{
    { "CAN"    , NULL          , "CAN ..."                                                , CONFIG_CAN_COMMANDS },
    { "Clear"  , Config_Clear  , "Clear..."                                               , NULL },
    { "Display", Config_Display, "Display                       See Device::Display"      , NULL },
    { "File"   , NULL          , "File ..."                                               , CONFIG_FILE_COMMANDS },
    { "IP"     , Config_IPv4   , "IP {Address} {Gateway} {NetMask}\n"
                                 "                              See EthCAN_Config"        , NULL },
    { "Name"   , Config_Name   , "Name [Name]                   See EthCAN_Config::mName" , NULL },
    { "Server" , Config_Server , "Server {Address} [Port]       See EthCAN_Config"        , NULL },
    { "WiFi"   , Config_WiFi   , "WiFi [Flags_hex] [Name] [Password]\n"
                                 "                              See EthCAN_Config"        , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Device_Config_Erase(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Get  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Reset(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Set  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Config_Store(KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo DEVICE_CONFIG_COMMANDS[] =
{
    { "Erase", Device_Config_Erase, "Erase [Flags_hex]             See Device::Config_Erase", NULL },
    { "Get"  , Device_Config_Get  , "Get                           See Device::Config_Get"  , NULL },
    { "Reset", Device_Config_Reset, "Reset [Flags_hex]             See Device::Config_Reset", NULL },
    { "Set"  , Device_Config_Set  , "Set [Flags_hex]               See Device::Config_Set"  , NULL },
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
    { "Reset"   , Device_Reset  , "Reset [Flags_hex]             See Device::Reset"  , NULL },
    { "Send"    , Device_Send   , "Send [Flags_hex] [Ext] [Id] [Count] ..."
                                  "                              See Device::Send"   , NULL },

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
    { "USB"     , Select_USB     , "USB [Index]                   See System::Device_Find_USB" , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Setup_AccessPoint(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Bridge     (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_DHCP       (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Link       (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Sniffer    (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_StaticIP   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_WiFi       (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Setup_Wireshark  (KmsLib::ToolBase* aToolBase, const char* aArg);

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
    { "Wireshark"  , Setup_Wireshark  , "Wireshark                     Setup the selected EthCAN as a sniffer to\n"
                                        "                              use with Wireshark"                      , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Debug     (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Detect    (KmsLib::ToolBase* aToolBase, const char* aArg);
static void GetVersion(KmsLib::ToolBase* aToolBase, const char* aArg);
static void List      (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Production(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Verbose   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo COMMANDS[] =
{
    { "Config"    , NULL      , "Config ..."                                          , CONFIG_COMMANDS },
    { "Debug"     , Debug     , "Debug                         See Object::Debug"     , NULL },
    { "Detect"    , Detect    , "Detect                        See System::Detect"    , NULL },
    { "Device"    , NULL      , "Device ..."                                          , DEVICE_COMMANDS },
    { "GetVersion", GetVersion, "GetVersion                    See System::GetVersion", NULL },
    { "List"      , List      , "List                          List detected device"  , NULL },
    { "Production", Production, "Production"                                          , NULL },
    { "Select"    , NULL      , "Select ..."                                          , SELECT_COMMANDS },
    { "Setup"     , NULL      , "Setup ..."                                           , SETUP_COMMANDS  },
    { "Verbose"   , Verbose   , "Verbose"                                             , NULL },

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
static bool Parse_SSID_Password(KmsLib::ToolBase* aToolBase, const char** aArg, char* aName, char* aPassword, const char *aNameDefault, const char *aPasswordDefault);

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
    KMS_TOOL_BANNER("EthCAN", "EthCAN_Tool", VERSION_STR, VERSION_TYPE);

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

    for (unsigned int i = 0; i < EthCAN_FILTER_QTY; i ++)
    {
        if (!aToolBase->Parse(&lArg, sConfig.mCAN_Filters + i, 0, 0x9fffffff, true))
        {
            return;
        }
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Config_CAN_Flags(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, EthCAN_FLAG_CAN_ADVANCED | EthCAN_FLAG_CAN_FILTERS_ON, true, 0))
    {
        sConfig.mCAN_Flags = lFlags;
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Config_CAN_Masks(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    for (unsigned int i = 0; i < EthCAN_MASK_QTY; i++)
    {
        if (!aToolBase->Parse(&lArg, sConfig.mCAN_Masks + i, 0, 0x9fffffff, true, 0))
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

void Config_File_Load(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    DisplayResult(aToolBase, EthCAN::File_Load(&sConfig, aArg));
}

void Config_File_Save(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    DisplayResult(aToolBase, EthCAN::File_Save(aArg, sConfig));
}

void Config_Clear(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    EthCAN_CONFIG_DEFAULT(&sConfig);

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Cleared");
}

void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    EthCAN::Display(NULL, sConfig);

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
    unsigned int lFlags;
    if (   aToolBase->Parse(&lArg, &lFlags, 0, 1, true, 0)
        && aToolBase->Parse(&lArg, sConfig.mWiFi_Name    , sizeof(sConfig.mWiFi_Name    ), "")
        && aToolBase->Parse(&lArg, sConfig.mWiFi_Password, sizeof(sConfig.mWiFi_Password), ""))
    {
        sConfig.mWiFi_Flags = lFlags;

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

#define USE_SELECTED_DEVICE                                  \
    if (NULL == sDevice)                                     \
    {                                                        \
        aToolBase->SetError(__LINE__, "No device selected"); \
        return;                                              \
    }

void Device_Config_Erase(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, 0x80, true, 0))
    {
        DisplayResult(aToolBase, sDevice->Config_Erase(lFlags));
    }
}

void Device_Config_Get(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    DisplayResult(aToolBase, sDevice->Config_Get(&sConfig));
}

void Device_Config_Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, 0x80, true, 0))
    {
        DisplayResult(aToolBase, sDevice->Config_Reset(lFlags));
    }
}

void Device_Config_Set(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, 0x80, true, 0))
    {
        DisplayResult(aToolBase, sDevice->Config_Set(&sConfig, lFlags));
    }
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
        EthCAN::Display(NULL, lInfo);
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

    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, 0x80, true, 0))
    {
        DisplayResult(aToolBase, sDevice->Reset(lFlags));
    }
}

void Device_Send(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    const char* lArg = aArg;
    bool lExt;
    unsigned int lCount;
    unsigned int lFlags;
    EthCAN_Frame lFrame;

    if (   aToolBase->Parse(&lArg, &lFlags, 0, 0x80, true, 0)
        && aToolBase->Parse(&lArg, &lExt, true)
        && aToolBase->Parse(&lArg, &lFrame.mId, 0, 0x1fffffff, true, 0)
        && aToolBase->Parse(&lArg, &lCount, 1, 8, false, 1))
    {
        lFrame.mDataSize_byte = lCount;

        if (lExt)
        {
            lFrame.mId |= EthCAN_ID_EXTENDED;
        }

        bool lParsed = true;

        for (unsigned int i = 0; lParsed && (i < static_cast<unsigned int>(EthCAN_FRAME_DATA_SIZE(lFrame))); i++)
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
            DisplayResult(aToolBase, sDevice->Send(lFrame, lFlags));
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
    char lName[EthCAN_NAME_SIZE_byte];

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
    char lName    [EthCAN_WIFI_NAME_SIZE_byte];
    char lPassword[EthCAN_WIFI_PASSWORD_SIZE_byte];

    if (   Parse_Address_NetMask(aToolBase, &lArg, &lAddress, &lNetMask)
        && Parse_SSID_Password  (aToolBase, &lArg, lName, lPassword, "EthCAN", "EthCANPassword"))
    {
        EthCAN_Result lRet = Setup_AccessPoint(sDevice, lAddress, lNetMask, lName, lPassword);

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

        EthCAN_Result lRet = Setup_Bridge(sSystem, lIndex);

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
    char lName    [EthCAN_WIFI_NAME_SIZE_byte];
    char lPassword[EthCAN_WIFI_PASSWORD_SIZE_byte];

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

        EthCAN_Result lRet = Setup_Link(sSystem, lIndex, lAddress, lNetMask, lName, lPassword);

        DisplayResult(aToolBase, lRet);
    }
}

void Setup_Sniffer(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    EthCAN_Result lRet = Setup_Sniffer(sDevice);

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
    char lName    [EthCAN_WIFI_NAME_SIZE_byte];
    char lPassword[EthCAN_WIFI_PASSWORD_SIZE_byte];

    if (Parse_SSID_Password(aToolBase, &lArg, lName, lPassword, "", ""))
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

void Setup_Wireshark(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_SELECTED_DEVICE;

    EthCAN_Config lConfig;
    unsigned int lStep = 1;

    printf("%u. Retrieving the configuration...\n", lStep); lStep++;
    EthCAN_Result lRet = sDevice->Config_Get(&lConfig);
    if (EthCAN_OK == lRet)
    {
        printf("%u. Modifying the configuration...\n", lStep); lStep++;
        // TODO lConfig.mCAN_Flags, lConfig.mCAN_Filters and lConfig.mCAN_Masks
        lConfig.mServer_Flags &= ~EthCAN_FLAG_WIFI_AP;
        // TODO lConfig.mServer_IPv4
        lConfig.mServer_Port = EthCAN_UDP_PORT;

        printf("%u. Setting the configuration...\n", lStep); lStep++;
        lRet = sDevice->Config_Set(&lConfig);
    }

    DisplayResult(aToolBase, lRet);
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
    uint8_t lVersion[EthCAN_VERSION_SIZE_byte];

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

        char lLine[128];

        EthCAN_Result lRet = lDev->GetInfoLine(lLine, sizeof(lLine));
        assert(EthCAN_OK == lRet);
        (void)lRet;

        printf("%2u %s\n", i, lLine);

        lDev->Release();
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Production(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    printf("1. Detecting devices...\n");
    Detect(aToolBase, NULL);

    EthCAN::Device* lDev = sSystem->Device_Find_USB();
    if (NULL == lDev)
    {
        aToolBase->SetError(__LINE__, "No USB connected device");
        return;
    }

    EthCAN_Info lInfo;

    EthCAN_Result lRet = lDev->GetInfo(&lInfo);
    if (EthCAN_OK == lRet)
    {
        printf("2. Erasing stored configuration...");
        lRet = lDev->Config_Erase();
        if (EthCAN_OK == lRet)
        {
            printf("Ethernet address : "); EthCAN::Display_EthAddress(NULL, lInfo.mEth_Address);
            printf("WiFi address     : "); EthCAN::Display_EthAddress(NULL, lInfo.mEth_Address);
        }
    }

    lDev->Release();

    DisplayResult(aToolBase, lRet);
}

void Verbose(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    sSystem->SetTraceStream(stdout);

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Verbose");
}

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

void DisplayResult(KmsLib::ToolBase* aToolBase, EthCAN_Result aResult)
{
    const char* lResultName = EthCAN::GetName(aResult);

    if (EthCAN_RESULT_OK(aResult))
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

bool Parse_SSID_Password(KmsLib::ToolBase* aToolBase, const char** aArg, char* aName, char* aPassword, const char* aNameDefault, const char* aPasswordDefault)
{
    return aToolBase->Parse(aArg, aName    , EthCAN_WIFI_NAME_SIZE_byte    , aNameDefault)
        && aToolBase->Parse(aArg, aPassword, EthCAN_WIFI_PASSWORD_SIZE_byte, aPasswordDefault);
}

bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame)
{
    EthCAN::Display(NULL, aFrame);
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
    EthCAN_Result lRet = Setup_IPv4(sDevice, aAddress, aGateway, aNetMask);

    DisplayResult(aToolBase, lRet);
}
