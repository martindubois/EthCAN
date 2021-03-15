
// Product EthCAN

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN/System.h
/// \brief     System

#pragma once

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Result.h>
    #include <EthCAN_Protocol.h>
    #include <EthCAN_Types.h>
}

#include <EthCAN/Object.h>

// ===== EthCAN_Lib =========================================================
class Serial;
class UDPSocket;

namespace EthCAN
{

    class Device;

    /// \brief System
    class System : public Object
    {

    public:

        /// \brief Create a System instance
        /// \retval NULL   Error
        /// \retval Others The address of the new instance
        static System* Create();

        /// \brief Get the library version
        /// \param aOut The output buffer
        /// \retval EthCAN_OK
        static EthCAN_Result GetVersion(uint8_t aOut[4]);

        /// \brief Is the result code OK ?
        /// \param aIn See EthCAN_Result
        /// \retval false No, the result code is not OK
        /// \retval true  Yes, the result code is OK
        static bool IsResultOK(EthCAN_Result aIn);

        /// \brief Detect connected devices
        /// \retval EthCAN_OK
        virtual EthCAN_Result Detect() = 0;

        /// \brief Find a device by Ethernet address
        /// \param aEth The Ethernet address
        /// \retval NULL  Not found
        /// \retval Other The address of the Device instance
        virtual Device * Device_Find_Eth(const uint8_t aEth[6]) = 0;

        /// \brief Find a device by IPv4 address
        /// \param aIPv4 The IPv4 address
        /// \retval NULL  Not found
        /// \retval Other The address of the Device instance
        virtual Device * Device_Find_IPv4(const uint32_t aIPv4) = 0;

        /// \brief Find a device by name
        /// \param aName The name
        /// \retval NULL  Not found
        /// \retval Other The address of the Device instance
        virtual Device * Device_Find_Name(const char * aName) = 0;

        /// \brief Find a device connected using USB link
        /// \param aIndex The USB connected device index
        /// \retval NULL  Not found
        /// \retval Other The address of the Device instance
        virtual Device * Device_Find_USB(unsigned int aIndex = 0) = 0;

        /// \brief Get a device by index
        /// \param aIndex The device index
        /// \retval NULL  Invalid index
        /// \retval Other The address of the Device instance
        virtual Device * Device_Get(unsigned int aIndex) = 0;

        /// \brief Get the detected device count
        /// \return The detected device count
        virtual unsigned int Device_GetCount() const = 0;

        /// \brief Set the trace stream
        /// \param aTrace The trace stream
        virtual void SetTraceStream(FILE* aTrace) = 0;

    protected:

        System();

        // ===== Object =====================================================
        virtual ~System();

    };

}
