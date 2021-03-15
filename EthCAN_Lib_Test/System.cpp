
// Author    KMS - Martin Dubois, P.Eng.
// copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/System.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Device.h>
#include <EthCAN/System.h>

// ===== Common =============================================================
#include "../Common/Version.h"

// ===== EthCAN_Lib_Test ====================================================
#include "OS.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

static const uint8_t ETH_NOT_FOUND[6] = { 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

static const uint32_t IPv4_NOT_FOUND = 0x03040506;

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(System_Base)
{
    EthCAN::System* lS0;
    uint8_t         lV[4];

    // Create
    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    // GetVersion
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER, EthCAN::System::GetVersion(NULL));
    KMS_TEST_COMPARE(EthCAN_OK, EthCAN::System::GetVersion(lV));
    KMS_TEST_COMPARE(VERSION_MAJOR        , lV[0]);
    KMS_TEST_COMPARE(VERSION_MINOR        , lV[1]);
    KMS_TEST_COMPARE(VERSION_BUILD        , lV[2]);
    KMS_TEST_COMPARE(VERSION_COMPATIBILITY, lV[3]);

    // Detect
    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());

    // Device_Find_Eth
    KMS_TEST_ASSERT(NULL == lS0->Device_Find_Eth(NULL));
    KMS_TEST_ASSERT(NULL == lS0->Device_Find_Eth(ETH_NOT_FOUND));

    // Device_Find_IPv4
    KMS_TEST_ASSERT(NULL == lS0->Device_Find_IPv4(IPv4_NOT_FOUND));

    // Device_Find_Name
    KMS_TEST_ASSERT(NULL == lS0->Device_Find_Name(NULL));
    KMS_TEST_ASSERT(NULL == lS0->Device_Find_Name("DoesNotExist"));

    // Device_Find_USB
    KMS_TEST_ASSERT(NULL == lS0->Device_Find_USB());

    // Device_Get
    KMS_TEST_ASSERT(NULL == lS0->Device_Get(0));

    // Device_GetCount
    KMS_TEST_COMPARE(0, lS0->Device_GetCount());

    // Object::Debug
    lS0->Debug(stdout);

    // Object::IncRefCount
    lS0->IncRefCount();

    // Object::Release
    lS0->Release();
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(System_SetupA)
{
    EthCAN::Device* lD0;
    EthCAN_Info lInfo;
    EthCAN::System* lS0;

    OS_Sleep(7000);

    // Create
    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    // Detect
    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());

    // Device_GetCount
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    // Device_Get
    lD0 = lS0->Device_Get(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfo(&lInfo));

    lD0->Release();

    // Device_Find_Name
    lD0 = lS0->Device_Find_Name(lInfo.mName);
    KMS_TEST_ASSERT(NULL != lD0);

    lD0->Release();

    // Object::Release
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(System_SetupB)
{
    EthCAN::Device* lD0;
    EthCAN_Info lInfo;
    EthCAN::System* lS0;

    // Create
    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    // Detect
    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());

    // Device_GetCount
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    // Device_Get
    lD0 = lS0->Device_Get(0);
    KMS_TEST_ASSERT_RETURN(NULL != lD0);

    KMS_TEST_COMPARE(EthCAN_OK, lD0->GetInfo(&lInfo));

    lD0->Release();

    // Device_Find_Eth
    lD0 = lS0->Device_Find_Eth(lInfo.mEth_Address);
    KMS_TEST_ASSERT(NULL != lD0);

    lD0->Release();

    // Device_Find_IPv4
    lD0 = lS0->Device_Find_IPv4(lInfo.mIPv4_Address);
    KMS_TEST_ASSERT(NULL != lD0);

    lD0->Release();

    // Object::Release
    lS0->Release();
}
KMS_TEST_END

KMS_TEST_BEGIN(System_SetupC)
{
    EthCAN::Device* lD0;
    EthCAN::System* lS0;

    // Create
    lS0 = EthCAN::System::Create();
    KMS_TEST_ASSERT(NULL != lS0);

    lS0->SetTraceStream(stdout);

    // Detect
    KMS_TEST_COMPARE(EthCAN_OK, lS0->Detect());

    // Device_GetCount
    KMS_TEST_ASSERT(0 < lS0->Device_GetCount());

    // Device_Find_USB
    lD0 = lS0->Device_Find_USB(0);
    KMS_TEST_ASSERT_CLEANUP(NULL != lD0, End);

    lD0->Release();

End:
    // Object::Release
    lS0->Release();
}
KMS_TEST_END
