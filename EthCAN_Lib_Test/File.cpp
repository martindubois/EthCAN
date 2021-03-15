
// Author    KMS - Martin Dubois, P.Eng.
// copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib_Test/File.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/File.h>

// Constants
/////////////////////////////////////////////////////////////////////////////

#define FILE_NAME "File_Base_Test.bin"

// Tests
/////////////////////////////////////////////////////////////////////////////

KMS_TEST_BEGIN(File_Base)
{
    EthCAN_Config lC0;
    const char* lNoFileName = NULL;

    // File_Load
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER    , EthCAN::File_Load(NULL, ""));
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_BUFFER    , EthCAN::File_Load(NULL, stdout));
    KMS_TEST_COMPARE(EthCAN_ERROR_FILE_NAME        , EthCAN::File_Load(&lC0, lNoFileName));
    KMS_TEST_COMPARE(EthCAN_ERROR_INPUT_FILE_OPEN  , EthCAN::File_Load(&lC0, "DoesNoExist"));
    KMS_TEST_COMPARE(EthCAN_ERROR_INPUT_STREAM_READ, EthCAN::File_Load(&lC0, stdout));

    // File_Save
    KMS_TEST_COMPARE(EthCAN_ERROR_FILE_NAME          , EthCAN::File_Save(lNoFileName, lC0));
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_FILE_OPEN   , EthCAN::File_Save("//", lC0));
    KMS_TEST_COMPARE(EthCAN_ERROR_OUTPUT_STREAM_WRITE, EthCAN::File_Save(stdin, lC0));

    #ifdef _KMS_WINDOWS_
        KMS_TEST_COMPARE(EthCAN_ERROR_REFERENCE, EthCAN::File_Save("NoFile", *reinterpret_cast<EthCAN_Config*>(NULL)));
    #endif

    KMS_TEST_COMPARE(EthCAN_OK, EthCAN::File_Save(FILE_NAME, lC0));

    // File_Load - With something to load
    KMS_TEST_COMPARE(EthCAN_OK, EthCAN::File_Load(&lC0, FILE_NAME))
}
KMS_TEST_END
