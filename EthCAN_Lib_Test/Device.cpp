
// Author    KMS - Martin Dubois, P.Eng.
// copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/Device.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>
#include <EthCAN/Display.h>
#include <EthCAN/System.h>

// Static variable
/////////////////////////////////////////////////////////////////////////////

static unsigned int sCounters_byte[2];
static unsigned int sCounters_frame[2];

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

static void ResetCounters();

static bool Send          (EthCAN::Device* aDevs[2], const EthCAN_Frame& aFrame);
static bool SendAndIgnore (EthCAN::Device* aDevs[2], const EthCAN_Frame& aFrame);
static bool SendAndReceive(EthCAN::Device* aDevs[2], const EthCAN_Frame& aFrame);

static bool VerifyCounters(unsigned int aExpected_byte, unsigned int aExpected_frame);

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(Device_SetupA)
{
    EthCAN_Config lCfg;
    EthCAN::Device* lD0;
    EthCAN_Info lInfo;
    char lLine[128];
    EthCAN::System* lS0;

    memset(&lCfg, 0, sizeof(lCfg));

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    lS0->Debug(stdout);

    lD0 = lS0->Device_Get(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    // Config_Get
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Get(&lCfg));

    // Config_Reset
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Reset());

    // Config_Set
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Set(&lCfg));

    // Config_Store
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Store());

    // GetInfo
    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfo(&lInfo));

    // GetInfoLine
    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfoLine(lLine, sizeof(lLine)));

    // IsConnectedEth
    KMS_TEST_ASSERT(lD0->IsConnectedEth());

    // IsConnectedUSB
    KMS_TEST_ASSERT(!lD0->IsConnectedUSB());

    // Receiver_Start
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Receiver_Start(Receiver, NULL));

    lD0->Release();
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupB)
{
    EthCAN_Config lCfg;
    EthCAN::Device* lD0;
    EthCAN_Frame lFrame;
    EthCAN_Info lInfo;
    EthCAN::System* lS0;

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    lS0->Debug(stdout);

    lD0 = lS0->Device_Get(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    // Config_Get
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER, lD0->Config_Get(NULL));
    KMS_TEST_COMPARE(EthCAN_OK                 , lD0->Config_Get(&lCfg));

    // Config_Reset
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Reset());

    // Config_Set
    KMS_TEST_COMPARE(EthCAN_ERROR_BUFFER, lD0->Config_Set(NULL));
    KMS_TEST_COMPARE(EthCAN_OK          , lD0->Config_Set(&lCfg));

    lCfg.mIPv4_NetMask = 0x03040506;
    KMS_TEST_COMPARE(EthCAN_ERROR_IPv4_MASK, lD0->Config_Set(&lCfg));

    lCfg.mIPv4_Address = 0x03040506;
    KMS_TEST_COMPARE(EthCAN_ERROR_IPv4_ADDRESS, lD0->Config_Set(&lCfg));

    lCfg.mCAN_Rate = 0xff;
    KMS_TEST_COMPARE(EthCAN_ERROR_CAN_RATE, lD0->Config_Set(&lCfg));

    // Config_Store
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Store());

    // GetInfo
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER, lD0->GetInfo(NULL));
    KMS_TEST_COMPARE(EthCAN_OK                 , lD0->GetInfo(&lInfo));

    // GetInfoLine
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER, lD0->GetInfoLine(NULL, 0));

    // IsConnectedEth
    KMS_TEST_ASSERT(lD0->IsConnectedEth());

    // IsConnectedUSB
    KMS_TEST_ASSERT(!lD0->IsConnectedUSB());

    // Receiver_Start
    KMS_TEST_COMPARE(EthCAN_ERROR_FUNCTION, lD0->Receiver_Start(NULL, NULL));
    KMS_TEST_COMPARE(EthCAN_OK            , lD0->Receiver_Start(Receiver, NULL));
    KMS_TEST_COMPARE(EthCAN_ERROR_RUNNING , lD0->Receiver_Start(Receiver, NULL));

    // Receiver_Stop
    KMS_TEST_COMPARE(EthCAN_OK               , lD0->Receiver_Stop());
    KMS_TEST_COMPARE(EthCAN_ERROR_NOT_RUNNING, lD0->Receiver_Stop());

    memset(&lFrame, 0, sizeof(lFrame));

    // Send
    KMS_TEST_COMPARE(EthCAN_ERROR_INPUT_BUFFER, lD0->Send(*reinterpret_cast<EthCAN_Frame*>(NULL)));
    KMS_TEST_COMPARE(EthCAN_OK                , lD0->Send(lFrame));

    // Reset - After all other test
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Reset());

    lD0->Release();
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupC)
{
    EthCAN_Config   lCfg;
    EthCAN::Device* lDevs[2];
    EthCAN_Frame    lFrame;
    EthCAN::System* lS0;

    unsigned int i;

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_COMPARE(2, lS0->Device_GetCount());

    for (i = 0; i < 2; i++)
    {
        lDevs[i] = lS0->Device_Get(i);

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Get(&lCfg));

        lCfg.mCAN_Filters[0] = EthCAN_ID_EXTENDED | 0x0aaaaaaa;
        lCfg.mCAN_Filters[5] =                      0x00000555;
        lCfg.mCAN_Masks  [0] = EthCAN_ID_EXTENDED | 0x1fffffff;
        lCfg.mCAN_Masks  [1] =                      0x000007ff;

        lCfg.mCAN_Flags = EthCAN_FLAG_CAN_FILTERS_ON;
        lCfg.mCAN_Rate  = EthCAN_RATE_1_Mb;

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Set(&lCfg));

        EthCAN::Display(stdout, lCfg);

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Start(Receiver, reinterpret_cast<void *>(static_cast<uint64_t>(i))));
    }

    memset(&lFrame, 0, sizeof(lFrame));

    lFrame.mDataSize_byte = 2;
    lFrame.mId = 0x555;

    KMS_TEST_ASSERT(SendAndReceive(lDevs, lFrame))

    lFrame.mDataSize_byte = 3;
    lFrame.mId = 0x444;

    KMS_TEST_ASSERT(SendAndIgnore(lDevs, lFrame));

    lFrame.mDataSize_byte = 4;
    lFrame.mId = EthCAN_ID_EXTENDED | 0x0aaaaaaa;

    KMS_TEST_ASSERT(SendAndReceive(lDevs, lFrame));

    lFrame.mDataSize_byte = 5;
    lFrame.mId = EthCAN_ID_EXTENDED | 0x0bbbbbbb;

    KMS_TEST_ASSERT(SendAndIgnore(lDevs, lFrame));

    for (i = 0; i < 2; i++)
    {
        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Stop());

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Get(&lCfg));

        lCfg.mCAN_Flags = 0;
        lCfg.mCAN_Rate = EthCAN_RATE_1_Mb;

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Set(&lCfg));

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Start(Receiver, reinterpret_cast<void *>(static_cast<uint64_t>(i))));
    }

    lFrame.mDataSize_byte = 6;
    lFrame.mId = 0x00000333;

    KMS_TEST_ASSERT(SendAndReceive(lDevs, lFrame));

    lFrame.mDataSize_byte = 7;
    lFrame.mId = EthCAN_ID_EXTENDED | 0x0ccccccc;

    KMS_TEST_ASSERT(SendAndReceive(lDevs, lFrame));

    for (i = 0; i < 2; i++)
    {
        lDevs[i]->Release();
    }

    lS0->Release();
}
KMS_TEST_END

// Static functions
/////////////////////////////////////////////////////////////////////////////

bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame)
{
    assert(NULL != aDevice);
    assert(NULL == aContext);
    assert(NULL != &aFrame);

    uint64_t lIndex = reinterpret_cast<uint64_t>(aContext);

    sCounters_byte[lIndex] += EthCAN_FRAME_DATA_SIZE(aFrame);

    sCounters_frame[lIndex] ++;

    return true;
}

void ResetCounters()
{
    memset(&sCounters_byte , 0, sizeof(sCounters_byte ));
    memset(&sCounters_frame, 0, sizeof(sCounters_frame));
}

bool Send(EthCAN::Device* aDevs[2], const EthCAN_Frame& aFrame)
{
    ResetCounters();

    bool lResult = true;

    for (unsigned int i = 0; i < 2; i++)
    {
        EthCAN_Result lRet = aDevs[i]->Send(aFrame);
        if (EthCAN_OK != lRet)
        {
            printf("ERROR  Device::Send(  )  failed - "); EthCAN::Display(stdout, lRet);
            lResult = false;
        }
    }

    return lResult;
}

bool SendAndIgnore(EthCAN::Device* aDevs[2], const EthCAN_Frame& aFrame)
{
    bool lResult = Send(aDevs, aFrame);

    if (!VerifyCounters(0, 0))
    {
        lResult = false;
    }

    return lResult;
}

bool SendAndReceive(EthCAN::Device* aDevs[2], const EthCAN_Frame& aFrame)
{
    bool lResult = Send(aDevs, aFrame);

    if (!VerifyCounters(EthCAN_FRAME_DATA_SIZE(aFrame), 1))
    {
        lResult = false;
    }

    return lResult;
}

bool VerifyCounters(unsigned int aExpected_byte, unsigned int aExpected_frame)
{
    bool lResult = true;

    for (unsigned int i = 0; i < 2; i++)
    {
        if (sCounters_byte[i] != aExpected_byte)
        {
            printf("ERROR  Counter %i = %u bytes but %u bytes was expected\n", i, sCounters_byte[i], aExpected_byte);
            lResult = false;
        }

        if (1 != sCounters_byte[i])
        {
            printf("ERROR  Counter %i = %u frames but %u frames was expected\n", i, sCounters_frame[i], aExpected_frame);
            lResult = false;
        }
    }

    return lResult;
}