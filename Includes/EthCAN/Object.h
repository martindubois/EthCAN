
// Product EthCAN

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN/Object.h
/// \brief     Object

#pragma once

// ===== C ==================================================================
#include <stdio.h>

namespace EthCAN
{

    /// \brief Object
    class Object
    {

    public:

        /// \brief Display debug information
        /// \param aOut The output stream
        virtual void Debug(FILE* aOut = NULL) const;

        /// \brief Increment the reference count
        void IncRefCount();

        /// \brief Release the object
        virtual void Release();

    protected:

        Object(unsigned int aRefCount);

        virtual ~Object();

    private:

        unsigned int mRefCount;

    };

}
