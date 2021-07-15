
// product EthCAN

/// \author    KMS - Martin Dubois, P.Eng.
/// \copyright Copyright &copy; 2021 KMS
/// \file      Includes/EthCAN/Device.h
/// \brief     Device

#pragma once

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Result.h>
    #include <EthCAN_Types.h>
}

#include <EthCAN/Object.h>

namespace EthCAN
{

    /// \brief Device
    class Device : public Object
    {

    public:

        /// \brief The type of the function called when receiving a message
        /// \param aDevice  The Device instance
        /// \param aContext The context the Receiver_Start method got
        /// \param aFrame   The received data
        /// \retval false Stop the receiver
        /// \retval true  Continue to receive message
        /// \see Receiver_Start
        typedef bool (*Receiver)(Device* aDevice, void* aContext, const EthCAN_Frame& aFrame);

        /// \brief Reset the configuration
        /// \param aFlags See EthCAN_FLAG_NO_RESPONSE
        /// \retval EthCAN_OK
        /// \see Config_Set
        virtual EthCAN_Result Config_Erase(uint8_t aFlags = 0) = 0;

        /// \brief Get the current configuration
        /// \param aOut The output buffer
        /// \retval EthCAN_OK
        /// \see Config_Set
        virtual EthCAN_Result Config_Get(EthCAN_Config* aOut) = 0;

        /// \brief Reset the configuration
        /// \param aFlags See EthCAN_FLAG_NO_RESPONSE
        /// \retval EthCAN_OK
        /// \see Config_Set
        virtual EthCAN_Result Config_Reset(uint8_t aFlags = 0) = 0;

        /// \brief Set the configuration
        /// \param aInOut The configuration
        /// \param aFlags See EthCAN_FLAG_NO_RESPONSE
        /// \retval EthCAN_OK
        /// \see Config_Get Config_Store
        /// \see GetName
        virtual EthCAN_Result Config_Set(EthCAN_Config * aInOut, uint8_t aFlags = 0) = 0;

        /// \brief Store the configuration to the non-volatile storage
        /// \param aFlags See EthCAN_FLAG_NO_RESPONSE
        /// \retval EthCAN_OK
        /// \see Config_Set
        virtual EthCAN_Result Config_Store(uint8_t aFlags = 0) = 0;

        /// \brief Retrieve the host address used to communicate with this device
        /// \retval The host address
        virtual uint32_t GetHostAddress() const = 0;

        /// \brief Get a line with main infomation
        /// \param aOut       The output buffer
        /// \param aSize_byte The output buffer size (in byte)
        /// \retval EthCAN_OK
        /// \retval EthCAN_ERROR_INVALID_OUTPUT_BUFFER
        virtual EthCAN_Result GetInfoLine(char* aOut, unsigned int aSize_byte) const = 0;

        /// \brief Get the information
        /// \param aOut The output buffer
        /// \retval EthCAN_OK
        /// \see GetEth
        /// \see GetIPv4
        virtual EthCAN_Result GetInfo(EthCAN_Info* aOut) = 0;

        /// \brief Is connected by Ethernet ?
        /// \retval false No, it is not connected by Ethernet
        /// \retval true  Yes, it is connected by Ethernet
        virtual bool IsConnectedEth() const = 0;

        /// \brief Is connected by USB ?
        /// \retval false No, it is not connected by USB
        /// \retval true  Yes, it is connected by USB
        virtual bool IsConnectedUSB() const = 0;

        /// \brief Start the receiver thread
        /// \param aFunction The function to call
        /// \param aContext  The value to pass to the function
        /// \retval EthCAN_OK
        /// \see Receiver Receiver_Stop
        virtual EthCAN_Result Receiver_Start(Receiver aFunction, void* aContext) = 0;

        /// \brief Stop the received thread
        /// \retval EthCAN_OK
        /// \see Receiver_Start
        virtual EthCAN_Result Receiver_Stop() = 0;

        /// \brief Reset the device
        /// \param aFlags See EthCAN_FLAG_NO_RESPONSE
        /// \retval EthCAN_OK
        virtual EthCAN_Result Reset(uint8_t aFlags = 0) = 0;

        /// \brief Send data
        /// \param aIn    The data to send
        /// \param aFlags See EthCAN_FLAG_NO_RESPONSE
        /// \retval EthCAN_OK
        virtual EthCAN_Result Send(const EthCAN_Frame& aIn, uint8_t aFlags = 0) = 0;

    // Internal

        bool Loop();

    protected:

        Device();

        // ===== Object =====================================================
        virtual ~Device();

    };

}
