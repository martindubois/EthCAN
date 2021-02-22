
// product EthCAN

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN/File.h
/// \brief     EthCAN::File_Load EthCAN::File_Save

#pragma once

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Types.h>
}

// ===== C ==================================================================
#include <stdio.h>

namespace EthCAN
{

    // Functions
    /////////////////////////////////////////////////////////////////////////

    /// \brief Load data from file
    /// \param aOut      The output buffer
    /// \param aFileName The complet file name
    /// \retval EthCAN_OK
    extern EthCAN_Result File_Load(EthCAN_Config* aOut, const char* aFileName);

    /// \brief Load data from file
    /// \param aOut  The output buffer
    /// \param aFile The binary input stream
    /// \retval EthCAN_OK
    extern EthCAN_Result File_Load(EthCAN_Config* aOut, FILE* aFile);

    /// \brief Save data to file
    /// \param aFileName The complet file name
    /// \param aIn       The data to save
    /// \retval EthCAN_OK
    extern EthCAN_Result File_Save(const char* aFileName, const EthCAN_Config& aIn);

    /// \brief Save data to file
    /// \param aFile The binary output stream
    /// \param aIn   The data to save
    /// \retval EthCAN_OK
    extern EthCAN_Result File_Save(FILE* aFile, const EthCAN_Config& aIn);

}
