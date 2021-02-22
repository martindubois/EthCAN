
// product EthCAN

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN/Display.h
/// \brief     EthCAN::Display EthCAN::Display_EthAddress EthCAN::Display_Id
///            EthCNA::Display_IPv4Address EthCAN::Display_Version
///            EthCAN::GetName EthCAN::GetName_EthAddress
///            EthCAN::GetName_IPv4Address

#pragma once

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
    #include <EthCAN_Result.h>
    #include <EthCAN_Types.h>
}

// ===== C ==================================================================
#include <stdio.h>

namespace EthCAN
{

    // Functions
    /////////////////////////////////////////////////////////////////////////

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display(FILE* aOut, const EthCAN_Config& aIn);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display(FILE* aOut, const EthCAN_Frame& aIn);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display(FILE* aOut, const EthCAN_Info& aIn);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display(FILE* aOut, EthCAN_Rate aIn);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display(FILE* aOut, EthCAN_RequestCode aIn);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display(FILE* aOut, EthCAN_Result aIn);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display_EthAddress(FILE* aOut, const uint8_t aIn[6]);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display_Id(FILE* aOut, uint32_t aIPv4);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display_IPv4Address(FILE* aOut, uint32_t aIPv4);

    /// \brief Display
    /// \param aOut The output stream
    /// \param aIn  The data to display
    extern void Display_Version(FILE* aOut, const uint8_t aIn[4]);

    /// \brief Retrieve the name of a CAN rate
    /// \param aIn See EthCAN_Rate
    /// \retval NULL   Invalid value
    /// \retval Others The address of a constant string
    extern const char* GetName(EthCAN_Rate aIn);

    /// \brief Get the name associated to a result code
    /// \param aIn See EthCAN_Result
    /// \retval NULL  The result code is not valid
    /// \retval Other The address of a constant name
    extern const char* GetName(EthCAN_Result aIn);

    /// \brief Get the name associated to a result code
    /// \param aOut          The output buffer
    /// \param aOutSize_byte The output buffer size
    /// \param aIn           The Ethernet address
    /// \retval EthCAN_OK
    extern EthCAN_Result GetName_EthAddress(char* aOut, unsigned int aOutSize_byte, const uint8_t aIn[6]);

    /// \brief Get the name associated to a result code
    /// \param aOut          The output buffer
    /// \param aOutSize_byte The output buffer size
    /// \param aIn           The IPv4 address
    /// \retval EthCAN_OK
    extern EthCAN_Result GetName_IPv4Address(char* aOut, unsigned int aOutSize_byte, const uint8_t aIn[4]);

}
