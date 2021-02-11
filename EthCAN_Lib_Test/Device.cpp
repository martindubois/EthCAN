
// Author    KMS - Martin Dubois, P.Eng.
// copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/Device.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>
#include <EthCAN/System.h>

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static bool Receiver(EthCAN::Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(Device_Base)
{
    EthCAN_Config lCfg;
    EthCAN_Info lInfo;

    memset(&lCfg , 0, sizeof(lCfg ));
    memset(&lInfo, 0, sizeof(lInfo));

    lCfg.mCAN_Rate = 0xcc;

    // Display
    EthCAN::Device::Display(NULL, lCfg);
    EthCAN::Device::Display(NULL, lInfo);

    EthCAN::Device::Display(NULL, EthCAN_RATE_100_Kb);

    // GetRateName
    KMS_TEST_ASSERT(NULL != EthCAN::Device::GetRateName(EthCAN_RATE_10_Kb));
    KMS_TEST_ASSERT(NULL == EthCAN::Device::GetRateName(EthCAN_RATE_QTY));
}
KMS_TEST_END

KMS_TEST_BEGIN(Device_SetupA)
{
    EthCAN_Config lCfg;
    EthCAN::Device* lD0;
    EthCAN_Info lInfo;
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
    EthCAN::Device::Display(NULL, lCfg);

    // Config_Reset
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Reset());

    // Config_Set
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Set(&lCfg));

    // Config_Store
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Store());

    // GetInfo
    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfo(&lInfo));

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
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_OUTPUT_BUFFER, lD0->Config_Get(NULL));
    KMS_TEST_COMPARE(EthCAN_OK                         , lD0->Config_Get(&lCfg));
    EthCAN::Device::Display(NULL, lCfg);

    // Config_Reset
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Reset());

    // Config_Set
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_BUFFER, lD0->Config_Set(NULL));
    KMS_TEST_COMPARE(EthCAN_OK                  , lD0->Config_Set(&lCfg));
    EthCAN::Device::Display(NULL, lCfg);

    lCfg.mIPv4_Mask = 0x03040506;
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_IPv4_MASK, lD0->Config_Set(&lCfg));

    lCfg.mIPv4_Addr = 0x03040506;
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_IPv4_ADDRESS, lD0->Config_Set(&lCfg));

    lCfg.mCAN_Rate = 0xff;
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_CAN_RATE, lD0->Config_Set(&lCfg));

    // Config_Store
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Config_Store());

    // GetInfo
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_OUTPUT_BUFFER, lD0->GetInfo(NULL));
    KMS_TEST_COMPARE(EthCAN_OK                         , lD0->GetInfo(&lInfo));

    // IsConnectedEth
    KMS_TEST_ASSERT(lD0->IsConnectedEth());

    // IsConnectedUSB
    KMS_TEST_ASSERT(!lD0->IsConnectedUSB());

    // Receiver_Start
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_FUNCTION, lD0->Receiver_Start(NULL, NULL));
    KMS_TEST_COMPARE(EthCAN_OK                    , lD0->Receiver_Start(Receiver, NULL));
    KMS_TEST_COMPARE(EthCAN_ERROR_RUNNING         , lD0->Receiver_Start(Receiver, NULL));

    // Receiver_Stop
    KMS_TEST_COMPARE(EthCAN_OK               , lD0->Receiver_Stop());
    KMS_TEST_COMPARE(EthCAN_ERROR_NOT_RUNNING, lD0->Receiver_Stop());

    memset(&lFrame, 0, sizeof(lFrame));

    // Send
    KMS_TEST_COMPARE(EthCAN_ERROR_INVALID_INPUT_BUFFER, lD0->Send(*reinterpret_cast<EthCAN_Frame*>(NULL)));
    KMS_TEST_COMPARE(EthCAN_OK                        , lD0->Send(lFrame));

    // Reset - After all other test
    KMS_TEST_COMPARE(EthCAN_OK, lD0->Reset());

    lD0->Release();
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

    return true;
}
