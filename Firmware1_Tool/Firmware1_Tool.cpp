
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021-2022 KMS
// Product   EthCAN
// File      EthCAN_Tool/Firmware1_Tool.cpp

#include "Component.h"

// ===== C ==================================================================
#include <stdio.h>

// ===== Windows ============================================================
#include <Windows.h>

// ===== Import/Includes ====================================================
#include <KmsLib/ToolBase.h>
#include <KmsTool.h>

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
}

#include <EthCAN/Display.h>

// ===== Common =============================================================
#include "../Common/Firmware.h"
#include "../Common/Version.h"

// Commands
/////////////////////////////////////////////////////////////////////////////

static void Config_Clear  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Filters(KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Flags  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Masks  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Rate   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Reset  (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Config_Set    (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo CONFIG_COMMANDS[] =
{
    { "Clear"  , Config_Clear  , "Clear                            Clear the configuraiton"  , NULL },
    { "Display", Config_Display, "Display                          Display the configuration", NULL },
    { "Filters", Config_Filters, "Filters [0_hex] ... [5_hex]      Change the filters"       , NULL },
    { "Flags"  , Config_Flags  , "Flags [Flags_hex]                Change flags"             , NULL },
    { "Masks"  , Config_Masks  , "Masks [0_hex] [1_hex]            Change the masks"         , NULL },
    { "Rate"   , Config_Rate   , "Rate [Rate]                      Change the rate"          , NULL },
    { "Reset"  , Config_Reset  , "Config Reset                     Reset the configuration"  , NULL },
    { "Set"    , Config_Set    , "Config Set                       Set the configuration"    , NULL },

    { NULL, NULL, NULL, NULL }
};

static void Connect   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Disconnect(KmsLib::ToolBase* aToolBase, const char* aArg);
static void GetInfo   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Receive   (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Reset     (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Send      (KmsLib::ToolBase* aToolBase, const char* aArg);
static void Test      (KmsLib::ToolBase* aToolBase, const char* aArg);

static const KmsLib::ToolBase::CommandInfo COMMANDS[] =
{
    { "Config"    , NULL      , "Config ..."                                              , CONFIG_COMMANDS },
    { "Connect"   , Connect   , "Connect {COMx:}               Connect to the device"     , NULL },
    { "Disconnect", Disconnect, "Disconnect                    Disconnect from the device", NULL },
    { "GetInfo"   , GetInfo   , "GetInfo                       Get device information"    , NULL },
    { "Receive"   , Receive   , "Receive                       Receive frames"            , NULL },
    { "Reset"     , Reset     , "Reset                         Reset the device"          , NULL },
    { "Send"      , Send      , "Send [Id_hex] [Size_byte_hex] [0_hex] ... [7_hex]\n"
                                "                              Send a frame"              , NULL },
    { "Test"      , Test      , "Test                          Test"                      , NULL },

    KMS_LIB_TOOL_BASE_FUNCTIONS

    { NULL, NULL, NULL, NULL }
};

// Data types
/////////////////////////////////////////////////////////////////////////////

// --> INIT <==+<======+=======+
//      |      |       |       |
//      +--> SYNC --> FRAME    |
//             |               |
//             +--> RESPONSE --+
typedef enum
{
    STATE_FRAME,
    STATE_INIT,
    STATE_RESPONSE,
    STATE_SYNC,
}
State;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Display(const FW_Info& aIn);

static void Port_Close();
static bool Port_OpenAndConfigure(KmsLib::ToolBase* aToolBase, const char* aPortName);

static unsigned int Receive(void* aOut, unsigned int aOutSize_byte);

static void Receive_Frame(uint8_t aByte);
static void Receive_Init (uint8_t aByte);
static bool Receive_Sync (uint8_t aByte, bool aDataExpected);

static bool Send(KmsLib::ToolBase* aToolBase, EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte);

static bool SendAndReceive(KmsLib::ToolBase* aToolBase, EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte);

static bool Write(KmsLib::ToolBase* aToolBase, const void* aIn, unsigned int aInSize_byte);

// ===== Internal commands ==================================================
static bool Config_Reset(KmsLib::ToolBase* aToolBase);
static bool Config_Set  (KmsLib::ToolBase* aToolBase, const FW_Config& aConfig);
static bool GetInfo     (KmsLib::ToolBase* aToolBase, FW_Info* aInfo);
static bool Reset       (KmsLib::ToolBase* aToolBase);
static bool Send        (KmsLib::ToolBase* aToolBase, const EthCAN_Frame& aFrame);

// Static variables
/////////////////////////////////////////////////////////////////////////////

static FW_Config sConfig;

static unsigned int sFrameCount;
static uint8_t      sFrame[sizeof(EthCAN_Frame)];

static HANDLE sPort = INVALID_HANDLE_VALUE;

static State sState;

// Entry point
/////////////////////////////////////////////////////////////////////////////

int main(int aCount, const char** aVector)
{
    KMS_TOOL_BANNER("EthCAN", "Firmware1_Tool", VERSION_STR, VERSION_TYPE);

    int lResult = 0;

    KmsLib::ToolBase lToolBase(COMMANDS);

    try
    {
        lToolBase.ParseArguments(aCount, aVector);

        lResult = lToolBase.ParseCommands();
    }
    catch (...)
    {
        lResult = lToolBase.SetError(__LINE__, "Unexpected exception (NOT TESTED)", KmsLib::ToolBase::REPORT_FATAL_ERROR);
    }

    Port_Close();

    return lResult;
}

// Command
/////////////////////////////////////////////////////////////////////////////

#define USE_DEVICE                                      \
    if (INVALID_HANDLE_VALUE == sPort)                  \
    {                                                   \
        aToolBase->SetError(__LINE__, "Not connected"); \
        return;                                         \
    }

void Config_Clear(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    memset(&sConfig, 0, sizeof(sConfig));

    sConfig.mRate = EthCAN_RATE_DEFAULT;

    Config_Display(aToolBase, "");
}

void Config_Display(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    unsigned int i;

    printf("Configuration\n");
    printf("    Filters :\n");
    
    for (i = 0; i < EthCAN_FILTER_QTY; i++)
    {
        printf("        ");  EthCAN::Display_Id(stdout, sConfig.mFilters[i]);
    }

    printf("    Flags   : 0x%02x\n", sConfig.mFlags);
    printf("    Masks   :\n");

    for (i = 0; i < EthCAN_MASK_QTY; i++)
    {
        printf("        "); EthCAN::Display_Id(stdout, sConfig.mMasks[i]);
    }

    printf("    Rate    : "); EthCAN::Display(stdout, static_cast<EthCAN_Rate>(sConfig.mRate));

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Config_Filters(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    for (unsigned int i = 0; i < EthCAN_FILTER_QTY; i++)
    {
        if (!aToolBase->Parse(&lArg, sConfig.mFilters + i, 0, 0x9fffffff, true, 0))
        {
            return;
        }
    }

    Config_Display(aToolBase, "");
}

void Config_Flags(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    unsigned int lFlags;

    if (aToolBase->Parse(&lArg, &lFlags, 0, 0xff, true, 0))
    {
        sConfig.mFlags = lFlags;

        Config_Display(aToolBase, "");
    }
}

void Config_Masks(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    for (unsigned int i = 0; i < 2; i++)
    {
        if (!aToolBase->Parse(&lArg, sConfig.mMasks + i, 0, 0x9fffffff, true, 0))
        {
            return;
        }
    }

    Config_Display(aToolBase, "");
}

void Config_Rate(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;
    unsigned int lRate;

    if (aToolBase->Parse(&lArg, &lRate, 0, EthCAN_RATE_QTY - 1, false, EthCAN_RATE_500_Kb))
    {
        sConfig.mRate = lRate;

        Config_Display(aToolBase, "");
    }
}

void Config_Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    if (Config_Reset(aToolBase))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Reseted");
    }
}

void Config_Set(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    if (Config_Set(aToolBase, sConfig))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Configured");
    }
}

void Connect(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    if (Port_OpenAndConfigure(aToolBase, aArg))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Connected");
    }
}

void Disconnect(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    Port_Close();

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Disconnected");
}

void GetInfo(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    FW_Info lInfo;

    if (GetInfo(aToolBase, &lInfo))
    {
        Display(lInfo);

        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
    }
}

void Receive(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    unsigned int lRet_byte = Receive(NULL, static_cast<unsigned int>(0));
    assert(0 == lRet_byte);
    (void)lRet_byte;

    KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Done");
}

void Reset(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    if (Reset(aToolBase))
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Reseted");
    }
}

void Send(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    const char* lArg = aArg;

    unsigned int lCount;
    EthCAN_Frame lFrame;

    if (   aToolBase->Parse(&lArg, &lFrame.mId, 0, 0x9fffffff, true, 0)
        && aToolBase->Parse(&lArg, &lCount    , 0, 0x48      , true, 0))
    {
        lFrame.mDataSize_byte = lCount;

        lCount &= ~CAN_FLAG_RTR;
        if (8 < lCount)
        {
            aToolBase->SetError(__LINE__, "Invalid data size");
            return;
        }

        for (unsigned int i = 0; i < lCount; i++)
        {
            unsigned int lValue;

            if (!aToolBase->Parse(&lArg, &lValue, 0, 0xff, true, 0))
            {
                return;
            }

            lFrame.mData[i] = lValue;
        }

        USE_DEVICE;

        if (Send(aToolBase, lFrame))
        {
            KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "Sent");
        }
    }
}

void Test(KmsLib::ToolBase* aToolBase, const char* aArg)
{
    USE_DEVICE;

    FW_Info lInfo;

    bool lPassed = GetInfo(aToolBase, &lInfo);
    if (lPassed)
    {
        Display(lInfo);

        if (   (VERSION_MAJOR != lInfo.mFirmware[0])
            || (VERSION_MINOR != lInfo.mFirmware[1])
            || (VERSION_BUILD != lInfo.mFirmware[2])
            || (VERSION_COMPATIBILITY != lInfo.mFirmware[3]))
        {
            aToolBase->SetError(__LINE__, "Invalid firmware version\n");
            lPassed = false;
        }

        if (EthCAN_OK != lInfo.mResult)
        {
            aToolBase->SetError(__LINE__, "CAN initialisation failed\n");
            lPassed = false;
        }
    }

    if (lPassed)
    {
        EthCAN_Frame lFrame;

        memset(&lFrame, 0, sizeof(lFrame));

        lFrame.mId = 2;
        lFrame.mDataSize_byte = 8;

        lPassed = Send(aToolBase, lFrame);
    }

    if (lPassed)
    {
        lPassed = Config_Reset(aToolBase);
    }

    if (lPassed)
    {
        FW_Config lConfig;

        memset(&lConfig, 0, sizeof(lConfig));

        lConfig.mRate = EthCAN_RATE_DEFAULT;

        lPassed = Config_Set(aToolBase, lConfig);
    }

    if (lPassed)
    {
        lPassed = Reset(aToolBase);
    }

    if (lPassed)
    {
        KmsLib::ToolBase::Report(KmsLib::ToolBase::REPORT_OK, "PASSED");
    }
    else
    {
        aToolBase->SetError(__LINE__, "FAILED");
    }
}

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

void Display(const FW_Info& aIn)
{
    assert(NULL != &aIn);

    printf("Firmware   : "); EthCAN::Display_Version(stdout, aIn.mFirmware);
    printf("Result CAN : "); EthCAN::Display(stdout, static_cast<EthCAN_Result>(aIn.mResult));
}

void Port_Close()
{
    if (INVALID_HANDLE_VALUE != sPort)
    {
        BOOL lRet = CloseHandle(sPort);
        sPort = INVALID_HANDLE_VALUE;

        assert(lRet);
        (void)lRet;
    }
}

bool Port_OpenAndConfigure(KmsLib::ToolBase* aToolBase, const char* aPortName)
{
    Port_Close();

    sPort = CreateFile(aPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == sPort)
    {
        aToolBase->SetError(__LINE__, "CreateFile( , , , , , ,  ) failed");
        return false;
    }

    sFrameCount = 0;
    sState = STATE_INIT;

    DCB lDCB;
    COMMTIMEOUTS lTimeouts;

    if ((!GetCommState(sPort, &lDCB)) || (!GetCommTimeouts(sPort, &lTimeouts)))
    {
        aToolBase->SetError(__LINE__, "GetCommState( ,  ) or GetCommTimeouts( ,  ) failed");
        return false;
    }

    assert(lDCB.DCBlength == sizeof(lDCB));
    assert(lDCB.fBinary);

    lDCB.BaudRate = CBR_115200;
    lDCB.ByteSize = 8;
    lDCB.Parity   = NOPARITY;
    lDCB.StopBits = ONESTOPBIT;

    lDCB.fAbortOnError   = FALSE;
    lDCB.fDsrSensitivity = FALSE;
    lDCB.fDtrControl     = DTR_CONTROL_DISABLE;
    lDCB.fErrorChar      = FALSE;
    lDCB.fInX            = FALSE;
    lDCB.fNull           = FALSE;
    lDCB.fParity         = FALSE;
    lDCB.fOutX           = FALSE;
    lDCB.fOutxCtsFlow    = FALSE;
    lDCB.fOutxDsrFlow    = FALSE;
    lDCB.fRtsControl     = RTS_CONTROL_DISABLE;

    if (!SetCommState(sPort, &lDCB))
    {
        printf("%u\n", GetLastError());
        aToolBase->SetError(__LINE__, "SetCommState( ,  ) failed");
        return false;
    }

    lTimeouts.ReadIntervalTimeout        = 100;
    lTimeouts.ReadTotalTimeoutConstant   = 500;
    lTimeouts.ReadTotalTimeoutMultiplier = 20;

    if (!SetCommTimeouts(sPort, &lTimeouts))
    {
        aToolBase->SetError(__LINE__, "SetCommTimeouts( ,  ) failed");
        return false;
    }

    return true;
}

bool Send(KmsLib::ToolBase* aToolBase, EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte)
{
    assert(EthCAN_REQUEST_QTY > aCode);

    uint8_t lRequest[2];

    lRequest[0] = EthCAN_SYNC;
    lRequest[1] = aCode;

    bool lResult = Write(aToolBase, lRequest, sizeof(lRequest));
    if (lResult)
    {
        if (0 < aInSize_byte)
        {
            assert(NULL != aIn);

            lResult = Write(aToolBase, aIn, aInSize_byte);
        }
    }

    return lResult;
}

bool SendAndReceive(KmsLib::ToolBase* aToolBase, EthCAN_RequestCode aCode, const void * aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte)
{
    assert(NULL != aToolBase);

    bool lResult = Send(aToolBase, aCode, aIn, aInSize_byte);
    if (lResult)
    {
        unsigned int lOutSize_byte = Receive(aOut, aOutSize_byte);
        if (aOutSize_byte != lOutSize_byte)
        {
            aToolBase->SetError(__LINE__, "The device did not send the expected data");
            lResult = false;
        }
    }

    return lResult;
}

// Return the size of the data put in the output buffer
unsigned int Receive(void* aOut, unsigned int aOutSize_byte)
{
    assert(INVALID_HANDLE_VALUE != sPort);

    uint8_t lByte;
    DWORD lInfo_byte;

    uint8_t*     lOut         = reinterpret_cast<uint8_t *>(aOut);
    unsigned int lResult_byte = 0;

    while (ReadFile(sPort, &lByte, sizeof(lByte), &lInfo_byte, NULL) && (sizeof(lByte) == lInfo_byte))
    {
        switch (sState)
        {
        case STATE_FRAME: Receive_Frame(lByte); break;
        case STATE_INIT : Receive_Init (lByte); break;

        case STATE_RESPONSE:
            assert(NULL != aOut);
            assert(0 < aOutSize_byte);

            lOut[lResult_byte] = lByte;
            lResult_byte++;
            if (aOutSize_byte <= lResult_byte)
            {
                sState = STATE_INIT;
                return lResult_byte;
            }
            break;

        case STATE_SYNC:
            if (!Receive_Sync(lByte, NULL != aOut))
            {
                return lResult_byte;
            }
            break;

        default: assert(false);
        }
    }

    return lResult_byte;
}

void Receive_Frame(uint8_t aByte)
{
    assert(sizeof(sFrame) > sFrameCount);

    const EthCAN_Frame* lFrame = reinterpret_cast<EthCAN_Frame*>(sFrame);

    sFrame[sFrameCount] = aByte;

    sFrameCount++;
    if (sizeof(sFrame) <= sFrameCount)
    {
        EthCAN::Display(stdout, *lFrame);
        sFrameCount = 0;
        sState = STATE_INIT;
    }
    else if (CAN_HEADER_SIZE_byte <= sFrameCount)
    {
        if (EthCAN_FRAME_DATA_SIZE(*lFrame) <= static_cast<uint8_t>(sFrameCount - CAN_HEADER_SIZE_byte))
        {
            EthCAN::Display(stdout, *lFrame);
            sFrameCount = 0;
            sState = STATE_INIT;
        }
    }
}

void Receive_Init(uint8_t aByte)
{
    if (EthCAN_SYNC == aByte)
    {
        sState = STATE_SYNC;
    }
    else
    {
        if (   ('\n' == aByte)
            || ('\r' == aByte)
            || (32 <= aByte) && (126 >= aByte))
        {
            printf("%c", aByte);
        }
        else
        {
            printf(" %02x", aByte);
        }
    }
}

// false Do not continue to wait for data
// true  Continue to wait for data
bool Receive_Sync(uint8_t aByte, bool aDataExpected)
{
    bool lResult = true;

    switch (aByte)
    {
    case EthCAN_RESULT_REQUEST: sState = STATE_FRAME;

    case EthCAN_OK:
        EthCAN::Display(stdout, static_cast<EthCAN_Result>(aByte));
        if (aDataExpected)
        {
            sState = STATE_RESPONSE;
        }
        else
        {
            sState = STATE_INIT;
            lResult = false;
        }
        break;

    case EthCAN_ERROR_TIMEOUT:
        EthCAN::Display(stdout, static_cast<EthCAN_Result>(aByte));
        sState = STATE_INIT;
        lResult = false;
        break;

    default:
        printf("Invalid byte after SYNC (0x%02x)\n", aByte);
        sState = STATE_INIT;
    }

    return lResult;
}

bool Write(KmsLib::ToolBase* aToolBase, const void* aIn, unsigned int aInSize_byte)
{
    assert(NULL != aToolBase);
    assert(NULL != aIn);
    assert(0 < aInSize_byte);

    assert(INVALID_HANDLE_VALUE != sPort);

    DWORD lInfo_byte;

    if (!WriteFile(sPort, aIn, aInSize_byte, &lInfo_byte, NULL))
    {
        aToolBase->SetError(__LINE__, "WriteFile( , , , ,  ) failed");
        return false;
    }

    if (aInSize_byte != lInfo_byte)
    {
        aToolBase->SetError(__LINE__, "WriteFile did not write the expected data size");
        return false;
    }

    return true;
}

// ===== Internal commands ==================================================

bool Config_Reset(KmsLib::ToolBase* aToolBase)
{
    return SendAndReceive(aToolBase, EthCAN_REQUEST_CONFIG_RESET, NULL, 0, NULL, 0);
}

bool Config_Set(KmsLib::ToolBase* aToolBase, const FW_Config& aConfig)
{
    return Send(aToolBase, EthCAN_REQUEST_CONFIG_SET, &aConfig, sizeof(aConfig));
}

bool GetInfo(KmsLib::ToolBase* aToolBase, FW_Info* aInfo)
{
    return SendAndReceive(aToolBase, EthCAN_REQUEST_INFO_GET, NULL, 0, aInfo, sizeof(*aInfo));
}

bool Reset(KmsLib::ToolBase* aToolBase)
{
    return SendAndReceive(aToolBase, EthCAN_REQUEST_CAN_RESET, NULL, 0, NULL, 0);
}

bool Send(KmsLib::ToolBase* aToolBase, const EthCAN_Frame& aFrame)
{
    return Send(aToolBase, EthCAN_REQUEST_SEND, &aFrame, EthCAN_FRAME_TOTAL_SIZE(aFrame));
}
