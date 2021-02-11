
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Object.cpp

#include "Component.h"

// ===== Includes ===========================================================
#include <EthCAN/Object.h>

namespace EthCAN
{

    // Public
    /////////////////////////////////////////////////////////////////////////

    void Object::Debug(FILE* aOut) const
    {
        FILE* lOut = (NULL == aOut) ? stdout : aOut;

        fprintf(lOut, "    Ref. count : %u\n", mRefCount);
    }

    void Object::IncRefCount()
    {
        assert(0 < mRefCount);

        mRefCount++;
    }

    void Object::Release()
    {
        mRefCount--;

        if (0 == mRefCount)
        {
            delete this;
        }
    }

    // Protected
    /////////////////////////////////////////////////////////////////////////

    Object::Object(unsigned int aRefCount) : mRefCount(aRefCount)
    {
        assert(0 < aRefCount);
    }

    Object::~Object()
    {
    }

}
