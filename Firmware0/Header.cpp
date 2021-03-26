
// Author    KMS - Martin Dubois, P.Eng.
// Copyright (C) 2021 KMS
// Product   EthCan
// File      Firmware0/Header.cpp

// CODE REVIEW 2021-03-24 KMS - Martin Dubois, P.Eng.

#include <Arduino.h>

#include "Component.h"

// ===== Firmware0 ==========================================================

extern "C"
{
    #include "Includes/EthCAN_Result.h"
}

#include "Header.h"

// Functions
/////////////////////////////////////////////////////////////////////////////

void Header_Init(EthCAN_Header * aOut, const EthCAN_Header * aIn)
{
    aOut->mCode           = aIn->mCode;
    aOut->mDataSize_byte  = 0;
    aOut->mFlags          = 0;
    aOut->mId             = aIn->mId;
    aOut->mResult         = EthCAN_OK;
    aOut->mTotalSize_byte = sizeof(EthCAN_Header);

}

bool Header_Validate(const EthCAN_Header & aIn, unsigned int aSize_byte)
{
    if (sizeof(EthCAN_Header) > aSize_byte)
    {
        MSG_ERROR("Header_Validate - Packet too small - ", aSize_byte);
        return false;
    }

    if (sizeof(EthCAN_Header) > aIn.mTotalSize_byte)
    {
        MSG_ERROR("Header_Validate - Total size too small - ", aSize_byte);
        return false;
    }

    if (aSize_byte < aIn.mTotalSize_byte)
    {
        MSG_ERROR("Header_Validate - Total size too large - ", aIn.mTotalSize_byte);
        return false;
    }

    if (sizeof(EthCAN_Header) + aIn.mDataSize_byte > aIn.mTotalSize_byte)
    {
        MSG_ERROR("Header_Validate - Data size too large - ", aIn.mDataSize_byte);
        return false;
    }

    if (EthCAN_RESULT_REQUEST != aIn.mResult)
    {
        MSG_ERROR("Header_Validate - Invalid request result - ", aIn.mResult);
        return false;
    }

    return true;
}
