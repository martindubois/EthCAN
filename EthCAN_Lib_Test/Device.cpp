
// Author    KMS - Martin Dubois, P.Eng.
// copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/Device.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>
#include <EthCAN/Display.h>
#include <EthCAN/System.h>

// ===== EthCAN_Lib_Test ====================================================
#include "OS.h"

// Static variable
/////////////////////////////////////////////////////////////////////////////

static unsigned int sCounters_byte[2];
static unsigned int sCounters_frame[2];

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

static void ResetCounters();

static bool Send          (EthCAN::Device* aDevs[2], uint32_t aId, unsigned int aSize_byte);
static bool SendAndIgnore (EthCAN::Device* aDevs[2], uint32_t aId, unsigned int aSize_byte);
static bool SendAndReceive(EthCAN::Device* aDevs[2], uint32_t aId, unsigned int aSize_byte);

static bool VerifyCounters(unsigned int aExpected_byte, unsigned int aExpected_frame);

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(Device_SetupA)
{
    EthCAN_Config lCfg;
    EthCAN::Device* lD0;
    EthCAN_Frame lFrame;
    EthCAN_Info lInfo;
    char lLine[128];
    EthCAN::System* lS0;

    memset(&lCfg  , 0, sizeof(lCfg));
    memset(&lFrame, 0, sizeof(lFrame));

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    lS0->Debug(stdout);

    lD0 = lS0->Device_Get(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    // Config_Erase
    // KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Erase());

    // Config_Get
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER, lD0->Config_Get(NULL));
    KMS_TEST_COMPARE(EthCAN_OK                 , lD0->Config_Get(&lCfg));

    // Config_Reset
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Reset());

    // Config_Set
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Set(&lCfg));

    // Config_Store
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Store());

    // GetInfo
    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfo(&lInfo));

    // GetHostAddress

    // GetInfoLine
    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfoLine(lLine, sizeof(lLine)));

    // IsConnectedEth, IsConnectedUSB
    KMS_TEST_ASSERT(lD0->IsConnectedEth() || lD0->IsConnectedUSB());

    // Receiver_Start
    KMS_TEST_COMPARE(EthCAN_ERROR_FUNCTION, lD0->Receiver_Start(NULL, NULL));
    KMS_TEST_COMPARE(EthCAN_OK            , lD0->Receiver_Start(Receiver, NULL));
    KMS_TEST_COMPARE(EthCAN_ERROR_RUNNING , lD0->Receiver_Start(Receiver, NULL));

    // Receiver_Stop
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Receiver_Stop());
    KMS_TEST_COMPARE(EthCAN_ERROR_NOT_RUNNING, lD0->Receiver_Stop());

    // Send
    KMS_TEST_COMPARE(EthCAN_ERROR_INPUT_BUFFER, lD0->Send(*reinterpret_cast<EthCAN_Frame*>(NULL)));
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Send(lFrame));

    // Reset - After all other test
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Reset());

    lD0->Release();
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupB)
{
    EthCAN::Device* lD0;
    EthCAN::System* lS0;

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    lD0 = lS0->Device_Get(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    // IsConnectedEth
    KMS_TEST_ASSERT(lD0->IsConnectedEth());

    // IsConnectedUSB
    KMS_TEST_ASSERT(!lD0->IsConnectedUSB());

    lD0->Release();
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupC)
{
    EthCAN::Device* lD0;
    EthCAN::System* lS0;

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    lD0 = lS0->Device_Find_USB(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    // IsConnectedEth
    KMS_TEST_ASSERT(!lD0->IsConnectedEth());

    // IsConnectedUSB
    KMS_TEST_ASSERT(lD0->IsConnectedUSB());

    lD0->Release();
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupD)
{
    EthCAN_Config   lCfg;
    EthCAN::Device* lDevs[2];
    EthCAN::System* lS0;

    printf(" 1. Detecting...\n");

    unsigned int i;

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT_RETURN(NULL != lS0);

    KMS_TEST_COMPARE_RETURN(EthCAN_OK, lS0->Detect());
    KMS_TEST_COMPARE_RETURN(2, lS0->Device_GetCount());

    printf(" 2. Reseting...\n");

    for (i = 0; i < 2; i++)
    {
        printf("(%u)\n", i);

        lDevs[i] = lS0->Device_Get(i);

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Reset());
    }

    printf(" 3. Configuring...\n");

    for (i = 0; i < 2; i++)
    {
        printf("(%u)\n", i);

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Get(&lCfg));

        lCfg.mCAN_Filters[0] = EthCAN_ID_EXTENDED | 0x0aaaaaaa;
        lCfg.mCAN_Filters[5] =                      0x00000555;
        lCfg.mCAN_Masks  [0] = EthCAN_ID_EXTENDED | 0x1fffffff;
        lCfg.mCAN_Masks  [1] =                      0x000007ff;

        lCfg.mCAN_Flags = EthCAN_FLAG_CAN_FILTERS_ON;
        lCfg.mCAN_Rate  = EthCAN_RATE_1_Mb;

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Config_Set(&lCfg));

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Start(Receiver, reinterpret_cast<void *>(static_cast<uint64_t>(i))));

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Config_Get(&lCfg));

        EthCAN::Display(stdout, lCfg);
    }

    printf(" 4. Sending to 0x555 STD...\n");
    KMS_TEST_ASSERT(SendAndReceive(lDevs, 0x555, 2));

    printf(" 5. Sending to 0x444 STD...\n");
    KMS_TEST_ASSERT(SendAndIgnore(lDevs, 0x444, 3));

    printf(" 6. Sending to 0xaaaaaaa EXT...\n");
    KMS_TEST_ASSERT(SendAndReceive(lDevs, EthCAN_ID_EXTENDED | 0xaaaaaaa, 4));

    printf(" 7. Sending to 0xbbbbbbb EXT...\n");
    KMS_TEST_ASSERT(SendAndIgnore(lDevs, EthCAN_ID_EXTENDED | 0xbbbbbbb, 5));

    printf(" 8. Configuring...\n");

    for (i = 0; i < 2; i++)
    {
        printf("(%u)\n", i);

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Stop());

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Get(&lCfg));

        lCfg.mCAN_Flags = 0;
        lCfg.mCAN_Rate = EthCAN_RATE_1_Mb;

        KMS_TEST_COMPARE_RETURN(EthCAN_OK, lDevs[i]->Config_Set(&lCfg));

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Start(Receiver, reinterpret_cast<void *>(static_cast<uint64_t>(i))));

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Config_Get(&lCfg));

        EthCAN::Display(stdout, lCfg);
    }

    printf(" 9. Sending to 0x333 STD...\n");
    KMS_TEST_ASSERT(SendAndReceive(lDevs, 0x333, 6));

    printf("10. Sending to 0xccccccc EXT...\n");
    KMS_TEST_ASSERT(SendAndReceive(lDevs, EthCAN_ID_EXTENDED | 0xccccccc, 7));

    printf("11. Releasing...\n");

    for (i = 0; i < 2; i++)
    {
        EthCAN_Info lInfo;

        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->Receiver_Stop());
        KMS_TEST_COMPARE(EthCAN_OK, lDevs[i]->GetInfo(&lInfo));

        EthCAN::Display(stdout, lInfo);

        lDevs[i]->Release();
    }

    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupE)
{
    EthCAN::Device* lD0;
    char lLine[64];
    EthCAN::System* lS0;

    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    lD0 = lS0->Device_Find_USB(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    KMS_TEST_ASSERT(lD0->IsConnectedEth());
    KMS_TEST_ASSERT(lD0->IsConnectedUSB());

    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfoLine(lLine, sizeof(lLine)));

    lD0->Release();
    lS0->Release();
}
KMS_TEST_END

// Static functions
/////////////////////////////////////////////////////////////////////////////

bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame)
{
    assert(NULL != aDevice);
    assert(NULL != &aFrame);

    uint64_t lIndex = reinterpret_cast<uint64_t>(aContext);
    assert(2 > lIndex);

    printf("===== Device %u =====\n", static_cast<unsigned int>(lIndex));
    EthCAN::Display(stdout, aFrame);

    sCounters_byte[lIndex] += EthCAN_FRAME_DATA_SIZE(aFrame);

    sCounters_frame[lIndex] ++;

    return true;
}

void ResetCounters()
{
    memset(&sCounters_byte , 0, sizeof(sCounters_byte ));
    memset(&sCounters_frame, 0, sizeof(sCounters_frame));
}

bool Send(EthCAN::Device* aDevs[2], uint32_t aId, unsigned int aSize_byte)
{
    assert(NULL != aDevs);

    ResetCounters();

    EthCAN_Frame lFrame;

    memset(&lFrame, 0, sizeof(lFrame));

    lFrame.mId = aId;
    lFrame.mDataSize_byte = aSize_byte;

    bool lResult = true;

    for (unsigned int i = 0; i < 2; i++)
    {
        assert(NULL != aDevs[i]);

        EthCAN_Result lRet = aDevs[i]->Send(lFrame);
        if (EthCAN_OK != lRet)
        {
            printf("ERROR  Device::Send(  )  failed - "); EthCAN::Display(stdout, lRet);
            lResult = false;
        }
    }

    return lResult;
}

bool SendAndIgnore(EthCAN::Device* aDevs[2], uint32_t aId, unsigned int aSize_byte)
{
    bool lResult = Send(aDevs, aId, aSize_byte);

    if (!VerifyCounters(0, 0))
    {
        lResult = false;
    }

    return lResult;
}

bool SendAndReceive(EthCAN::Device* aDevs[2], uint32_t aId, unsigned int aSize_byte)
{
    bool lResult = Send(aDevs, aId, aSize_byte);

    if (!VerifyCounters(aSize_byte, 1))
    {
        lResult = false;
    }

    return lResult;
}

bool VerifyCounters(unsigned int aExpected_byte, unsigned int aExpected_frame)
{
    OS_Sleep(2000);

    bool lResult = true;

    for (unsigned int i = 0; i < 2; i++)
    {
        if (sCounters_byte[i] != aExpected_byte)
        {
            printf("ERROR  Counter %i = %u bytes but %u bytes was expected\n", i, sCounters_byte[i], aExpected_byte);
            lResult = false;
        }

        if (aExpected_byte != sCounters_byte[i])
        {
            printf("ERROR  Counter %i = %u frames but %u frames was expected\n", i, sCounters_frame[i], aExpected_frame);
            lResult = false;
        }
    }

    return lResult;
}
