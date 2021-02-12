
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
    { "Filters", Config_CAN_Filters, "Filters {0_hex} {1} {2} {3} {4} {5}  See EthCAN_Config::mCAN_Filters"},
    { "Masks"  , Config_CAN_Masks  , "Masks [Mask0_hex] ...                See EthCAN_Config::mCAN_Masks"  },
    { "Rate"   , Config_CAN_Rate   , "Rate [Rate_Kbps]                     See EthCAN_Config::mCAN_Rate"   },

    { NULL, NULL, NULL, NULL }
};

static void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Flags  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_IPv4   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Name   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Server (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_COMMANDS[] =
{
    { "CAN"    , NULL          , "CAN ..."                                                   , CONFIG_CAN_COMMANDS },
    { "Display", Config_Display, "Display                          See Device::Display"      , NULL },
    { "Flags"  , Config_Flags  , "Flags [Flags_hex]                See EthCAN_Config::mFlags", NULL },
    { "IP"     , Config_IPv4   , "IP {Addr} {Mask}                 See EthCAN_Config"        , NULL },
    { "Name"   , Config_Name   , "Name [Name]                      See EthCAN_Config::mName" , NULL },
    { "Server" , Config_Server , "Server {IPv4} [Port]             See EthCAN_Config"        , NULL },

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
    { "Store", Device_Config_Store, "Store                         See Device::Config_Store", NULL },

    { NULL, NULL, NULL, NULL }
};

static void Device_Receiver_Start(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Receiver_Stop (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo DEVICE_RECEIVER_COMMANDS[] =
{
    { "Start", Device_Receiver_Start, "Start                       See Device::Receiver_Start", NULL },
    { "Stop" , Device_Receiver_Stop , "Stop                        See Device::Receiver_Start", NULL },

    { NULL, NULL, NULL, NULL }
};

static void Device_Debug  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_GetInfo(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Release(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Reset  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Device_Send   (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo DEVICE_COMMANDS[] =
{
    { "Config"  , NULL          , "Config ..."                                      , DEVICE_CONFIG_COMMANDS },
    { "Debug"   , Device_Debug  , "Debug                        See Object::Debug"  , NULL },
    { "GetInfo" , Device_GetInfo, "GetInfo                      See Device::GetInfo", NULL },
    { "Receiver", NULL          , "Receiver ..."                                    , DEVICE_RECEIVER_COMMANDS },
    { "Release" , Device_Release, "Release                      See Object::Release", NULL },
    { "Reset"   , Device_Reset  , "Reset                        See Device::Reset"  , NULL },
    { "Send"    , Device_Send   , "Send [Ext] [Id] [Count] ...  See Device::Send"   , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Debug     (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Detect    (KmsLib::ToolBase* aToolBase, const char* aArg);
static void GetVersion(KmsLib::ToolBase* aToolBase, const char* aArg);
static void List      (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Select    (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo COMMANDS[] =
{
    { "Config"    , NULL      , "Config ..."                                          , CONFIG_COMMANDS },
    { "Debug"     , Debug     , "Debug                         See Object::Debug"     , NULL },
    { "Detect"    , Detect    , "Detect                        See System::Detect"    , NULL },
    { "Device"    , NULL      , "Device ..."                                          , DEVICE_COMMANDS },
    { "GetVersion", GetVersion, "GetVersion                    See System::GetVersion", NULL },
    { "List"      , List      , "List                          List detected device"  , NULL },
    { "Select"    , Select    , "Select [D-Idx]                See System::Device_Get", NULL },

    KMS_LIB_TOOL_BASE_FUNCTIONS

    { NULL, NULL, NULL, NULL }
};

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void DisplayResult(KmsLib::ToolBase* aToolBase, EthCAN_Result aResult);

static bool Parse_IPv4(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aOut);

static bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

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

    if (NULL != sDevice)
    {
        sDevice->Release();
    }

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

void Config_Flags(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    if (aToolBase->Parse(&lArg, &sConfig.mFlags, 0, 0x00000f01, true, 0))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Config_IPv4(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    if (   Parse_IPv4(aToolBase, &lArg, &sConfig.mIPv4_Addr)
        && Parse_IPv4(aToolBase, &lArg, &sConfig.mIPv4_Mask))
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

    if (Parse_IPv4(aToolBase, &lArg, &sConfig.mServer_IPv4)
        && aToolBase->Parse(&lArg, &lPort, 0, 0xffff, false, 0))
    {
        sConfig.mServer_Port = lPort;

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Device_Config_Get(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Config_Get(&sConfig));
    }
}

void Device_Config_Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Config_Reset());
    }
}

void Device_Config_Set(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Config_Set(&sConfig));
    }
}

void Device_Config_Store(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Config_Store());
    }
}

void Device_Receiver_Start(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Receiver_Start(Receiver, NULL));
    }
}

void Device_Receiver_Stop(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Receiver_Stop());
    }
}

void Device_Debug(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        sDevice->Debug();

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Device_GetInfo(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        EthCAN_Info lInfo;

        EthCAN_Result lRet = sDevice->GetInfo(&lInfo);
        if (EthCAN_OK == lRet)
        {
            EthCAN::Device::Display(NULL, lInfo);
        }

        DisplayResult(aToolBase, lRet);
    }
}

void Device_Release(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        sDevice->Release();
        sDevice = NULL;

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Device_Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
        DisplayResult(aToolBase, sDevice->Reset());
    }
}

void Device_Send(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (NULL == sDevice)
    {
        aToolBase->SetError(__LINE__, "No device selected");
    }
    else
    {
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
            lInfo.mIPv4_Addr & 0xff, lInfo.mIPv4_Addr >> 8 & 0xff, lInfo.mIPv4_Addr >> 16 & 0xff, lInfo.mIPv4_Addr >> 24 & 0xff,
            lInfo.mName);

        lDev->Release();
    }

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Select(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    unsigned int lCount = sSystem->Device_GetCount();
    if (0 == lCount)
    {
        aToolBase->SetError(__LINE__, "No detected device");
    }
    else
    {
        const char* lArg = aArg;
        unsigned int lIndex;

        if (aToolBase->Parse(&lArg, &lIndex, 0, lCount - 1, false, 0))
        {
            if (NULL != sDevice)
            {
                sDevice->Release();
            }

            sDevice = sSystem->Device_Get(lIndex);
            assert(NULL != sDevice);

            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
        }
    }
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

bool Parse_IPv4(KmsLib::ToolBase* aToolBase, const char** aArg, uint32_t* aOut)
{
    unsigned int lValue[4];

    if (4 != sscanf_s(*aArg, "%u.%u.%u.%u", lValue + 0, lValue + 1, lValue + 2, lValue + 3))
    {
        aToolBase->SetError(__LINE__, "Invalid IPv4 format");
        return false;
    }

    uint32_t lOut = 0;

    for (unsigned int i = 0; i < 4; i ++)
    {
        if (255 < lValue[i])
        {
            aToolBase->SetError(__LINE__, "Invalid IPv4 value");
            return false;
        }

        lOut |= lValue[i] << (8 * i);
    }

    *aOut = lOut;

    while (((**aArg >= '0') && (**aArg <= '9')) || (**aArg == '.'))
    {
        (*aArg)++;
    }

    return true;
}

bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame)
{
    EthCAN::Device::Display(NULL, aFrame);
    return true;
}
