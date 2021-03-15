
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/File.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/File.h>

namespace EthCAN
{

    // Functions
    /////////////////////////////////////////////////////////////////////////

    EthCAN_Result File_Load(EthCAN_Config* aOut, const char* aFileName)
    {
        if (NULL == aOut     ) { return EthCAN_ERROR_OUTPUT_BUFFER; }
        if (NULL == aFileName) { return EthCAN_ERROR_FILE_NAME; }

        FILE* lFile;

        errno_t lErr = fopen_s(&lFile, aFileName, "rb");
        if (0 != lErr)
        {
            return EthCAN_ERROR_INPUT_FILE_OPEN;
        }

        EthCAN_Result lResult = File_Load(aOut, lFile);

        lErr = fclose(lFile);
        if (0 != lErr)
        {
            TRACE_ERROR(stderr, "File_Load - EthCAN_ERROR_INPUT_FILE_CLOSE");
            lResult = EthCAN_ERROR_INPUT_FILE_CLOSE;
        }

        return lResult;
    }

    EthCAN_Result File_Load(EthCAN_Config* aOut, FILE* aFile)
    {
        if (NULL == aOut ) { return EthCAN_ERROR_OUTPUT_BUFFER; }
        if (NULL == aFile) { return EthCAN_ERROR_INPUT_STREAM; }

        if (1 != fread(aOut, sizeof(EthCAN_Config), 1, aFile))
        {
            return EthCAN_ERROR_INPUT_STREAM_READ;
        }

        return EthCAN_OK;
    }

    EthCAN_Result File_Save(const char* aFileName, const EthCAN_Config& aIn)
    {
        if (NULL == aFileName) { return EthCAN_ERROR_FILE_NAME; }
        if (NULL == &aIn     ) { return EthCAN_ERROR_REFERENCE; }

        FILE* lFile;

        errno_t lErr = fopen_s(&lFile, aFileName, "wb");
        if (0 != lErr)
        {
            return EthCAN_ERROR_OUTPUT_FILE_OPEN;
        }

        EthCAN_Result lResult = File_Save(lFile, aIn);

        lErr = fclose(lFile);
        if (0 != lErr)
        {
            TRACE_ERROR(stderr, "File_Save - EthCAN_ERROR_OUTPUT_FILE_CLOSE");
            lResult = EthCAN_ERROR_OUTPUT_FILE_CLOSE;
        }

        return lResult;
    }

    EthCAN_Result File_Save(FILE* aFile, const EthCAN_Config& aIn)
    {
        if (NULL == aFile) { return EthCAN_ERROR_OUTPUT_STREAM; }
        if (NULL == &aIn ) { return EthCAN_ERROR_REFERENCE; }

        if (1 != fwrite(&aIn, sizeof(aIn), 1, aFile))
        {
            return EthCAN_ERROR_OUTPUT_STREAM_WRITE;
        }

        return EthCAN_OK;
    }

}