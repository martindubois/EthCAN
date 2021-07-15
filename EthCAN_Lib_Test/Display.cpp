
// Author    KMS - Martin Dubois, P.Eng.
// copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/Display.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Display.h>

// Constants
/////////////////////////////////////////////////////////////////////////////

#if defined(_KMS_LINUX_) || defined(_KMS_OS_X_)
    #define NUL_DEV "/dev/null"
#endif

#ifdef _KMS_WINDOWS_
    #define NUL_DEV "NUL:"
#endif

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(Display_Base)
{
    FILE* lNull;

    EthCAN_Config lConfig;
    EthCAN_Frame  lFrame;
    EthCAN_Info   lInfo;

    uint8_t lData[6];
    char    lStr[32];

    KMS_TEST_COMPARE(0, fopen_s(&lNull, NUL_DEV, "wb"))

    #ifdef _KMS_WINDOWS_
        // NULL reference
        EthCAN::Display(NULL, *reinterpret_cast<EthCAN_Config*>(NULL));
        EthCAN::Display(NULL, *reinterpret_cast<EthCAN_Frame *>(NULL));
        EthCAN::Display(NULL, *reinterpret_cast<EthCAN_Info  *>(NULL));
    #endif

    // NULL pointer
    EthCAN::Display_EthAddress(NULL, NULL);
    EthCAN::Display_Version   (NULL, NULL);
    EthCAN::GetName_EthAddress (lStr, sizeof(lStr), NULL);
    EthCAN::GetName_IPv4Address(lStr, sizeof(lStr), NULL);

    // Zeroed
    memset(&lConfig, 0, sizeof(lConfig));
    memset(&lData  , 0, sizeof(lData));
    memset(&lFrame , 0, sizeof(lFrame));
    memset(&lInfo  , 0, sizeof(lInfo));
    EthCAN::Display(NULL, lConfig);
    EthCAN::Display(NULL, lFrame);
    EthCAN::Display(NULL, lInfo);
    EthCAN::GetName_EthAddress (lStr, sizeof(lStr), lData);
    EthCAN::GetName_IPv4Address(lStr, sizeof(lStr), lData);

    // All byte to 0x01
    memset(&lConfig, 0x01, sizeof(lConfig));
    memset(&lData  , 0x01, sizeof(lData));
    memset(&lFrame , 0x01, sizeof(lFrame));
    memset(&lInfo  , 0x01, sizeof(lInfo));
    EthCAN::Display(lNull, lConfig);
    EthCAN::Display(lNull, lFrame);
    EthCAN::Display(lNull, lInfo);
    EthCAN::GetName_EthAddress (lStr, sizeof(lStr), lData);
    EthCAN::GetName_IPv4Address(lStr, sizeof(lStr), lData);

    // All byte to 0xff
    memset(&lConfig, 0xff, sizeof(lConfig));
    memset(&lData  , 0xff, sizeof(lData));
    memset(&lFrame , 0xff, sizeof(lFrame));
    memset(&lInfo  , 0xff, sizeof(lInfo));
    EthCAN::Display(lNull, lConfig);
    EthCAN::Display(lNull, lFrame);
    EthCAN::Display(lNull, lInfo);
    EthCAN::GetName_EthAddress (lStr, sizeof(lStr), lData);
    EthCAN::GetName_IPv4Address(lStr, sizeof(lStr), lData);

    KMS_TEST_COMPARE(0, fclose(lNull));
}
KMS_TEST_END
