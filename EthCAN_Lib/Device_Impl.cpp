
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Device_Impl.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
}

#include <EthCAN/Display.h>
#include <EthCAN/System.h>

// ===== EthCAN_Lib =========================================================
#include "OS.h"
#include "Serial.h"
#include "Thread.h"
#include "UDPSocket.h"

#include "Device_Impl.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define MSG_LOOP_ITERATION (1)
#define MSG_SERIAL_DATA    (2)

// Public
/////////////////////////////////////////////////////////////////////////////

Device_Impl::Device_Impl()
    : mBusyUntil_ms(0)
    , mContext(NULL)
    , mId_Client(0)
    , mId_Server(0)
    , mReceiver(NULL)
    , mSerial(NULL)
    , mSocket_Client(NULL)
    , mSocket_Server(NULL)
    , mThread(NULL)
{
    memset(&mInfo, 0, sizeof(mInfo));
}

bool Device_Impl::Is(const uint8_t aEth[6]) const
{
    assert(NULL != aEth);

    return 0 == memcmp(mInfo.mEth_Address, aEth, sizeof(mInfo.mEth_Address));
}

bool Device_Impl::Is(uint32_t aIPv4) const
{
    return mInfo.mIPv4_Address == aIPv4;
}

bool Device_Impl::Is(const char* aName) const
{
    assert(NULL != aName);

    return 0 == strcmp(mInfo.mName, aName);
}

void Device_Impl::SetInfo(const EthCAN_Info& aInfo, Serial* aSerial)
{
    assert(NULL != &aInfo);

    if (NULL == aSerial)
    {
        if (NULL == mSocket_Client)
        {
            mSocket_Client = new UDPSocket();
            assert(NULL != mSocket_Client);
        }
    }
    else
    {
        mSerial = aSerial;

        mSerial->Receiver_Start(this, MSG_SERIAL_DATA);
    }

    mInfo = aInfo;
}

// ===== EthCAN::Device =====================================================

#define BEGIN                                      \
    EthCAN_Result lResult = EthCAN_RESULT_INVALID; \
    try

#define END                       \
    catch (EthCAN_Result eResult) \
    {                             \
        lResult = eResult;        \
    }                             \
    return lResult;

EthCAN_Result Device_Impl::Config_Erase(uint8_t aFlags)
{
    BEGIN
    {
        if (0 != Request(EthCAN_REQUEST_CONFIG_ERASE, aFlags, NULL, 0, NULL, 0))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Erase - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
        }
        else
        {
            lResult = EthCAN_OK;
        }
    }
    END
}

EthCAN_Result Device_Impl::Config_Get(EthCAN_Config* aOut)
{
    if (NULL == aOut) { return EthCAN_ERROR_OUTPUT_BUFFER; }

    BEGIN
    {
        if (sizeof(EthCAN_Config) != Request(EthCAN_REQUEST_CONFIG_GET, 0, NULL, 0, aOut, sizeof(EthCAN_Config)))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Get - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
        }
        else
        {
            lResult = EthCAN_OK;
        }
    }
    END
}

EthCAN_Result Device_Impl::Config_Reset(uint8_t aFlags)
{
    BEGIN
    {
        if (0 != Request(EthCAN_REQUEST_CONFIG_RESET, aFlags, NULL, 0, NULL, 0))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Reset - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
        }
        else
        {
            lResult = EthCAN_OK;
        }
    }
    END
}

EthCAN_Result Device_Impl::Config_Set(EthCAN_Config* aInOut, uint8_t aFlags)
{
    if (NULL == aInOut) { return EthCAN_ERROR_BUFFER; }

    BEGIN
    {
        Config_Verify(*aInOut);

        if (sizeof(EthCAN_Config) != Request(EthCAN_REQUEST_CONFIG_SET, aFlags, aInOut, sizeof(EthCAN_Config), aInOut, sizeof(EthCAN_Config)))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Set - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
        }
        else
        {
            lResult = EthCAN_OK;
        }
    }
    END
}

EthCAN_Result Device_Impl::Config_Store(uint8_t aFlags)
{
    BEGIN
    {
        if (0 != Request(EthCAN_REQUEST_CONFIG_STORE, aFlags, NULL, 0, NULL, 0))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Store - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
        }
        else
        {
            lResult = EthCAN_OK;
        }
    }
    END
}

uint32_t Device_Impl::GetHostAddress() const
{
    return UDPSocket::GetIPv4(mInfo.mIPv4_Address, mInfo.mIPv4_NetMask);
}

EthCAN_Result Device_Impl::GetInfoLine(char* aOut, unsigned int aSize_byte) const
{
    if (NULL == aOut   ) { return EthCAN_ERROR_OUTPUT_BUFFER; }
    if (55 > aSize_byte) { return EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL; }

    const char* lCon;
    if (IsConnectedEth())
    {
        if (IsConnectedUSB())
        {
            lCon = "Both";
        }
        else
        {
            lCon = "Eth ";
        }
    }
    else
    {
        lCon = "USB ";
    }

    char lEth [24];

    EthCAN::GetName_EthAddress(lEth , sizeof(lEth ), mInfo.mEth_Address);

    sprintf_s(aOut SIZE_INFO(aSize_byte), "%s %s %3u.%3u.%3u.%3u %s",
        lCon, lEth,
        mInfo.mIPv4_Address & 0xff, mInfo.mIPv4_Address >> 8 & 0xff, mInfo.mIPv4_Address >> 16 & 0xff, mInfo.mIPv4_Address >> 24 & 0xff,
        mInfo.mName);

    return EthCAN_OK;
}

EthCAN_Result Device_Impl::GetInfo(EthCAN_Info* aInfo)
{
    if (NULL == aInfo) { return EthCAN_ERROR_OUTPUT_BUFFER; }

    BEGIN
    {
        if (sizeof(mInfo) != Request(EthCAN_REQUEST_INFO_GET, 0, NULL, 0, aInfo, sizeof(*aInfo)))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Get - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
        }
        else
        {
            // TODO Security
            //      Validate the received information match the already known information.

            lResult = EthCAN_OK;
        }
    }
    END
}

bool Device_Impl::IsConnectedEth() const
{
    return (NULL != mSocket_Client);
}

bool Device_Impl::IsConnectedUSB() const
{
    return (NULL != mSerial);
}

EthCAN_Result Device_Impl::Receiver_Start(Receiver aReceiver, void* aContext)
{
    if (NULL == aReceiver) { return EthCAN_ERROR_FUNCTION; }
    if (NULL != mReceiver) { return EthCAN_ERROR_RUNNING; }

    assert(NULL == mContext);
    assert(NULL == mSocket_Server);
    assert(NULL == mThread);

    mLostCount = 0;

    EthCAN_Config lConfig;

    EthCAN_Result lResult = Config_Get(&lConfig);
    if (EthCAN_OK == lResult)
    {
        try
        {
            if (IsConnectedEth())
            {
                assert(0 != mInfo.mIPv4_Address);
                assert(0 != mInfo.mIPv4_NetMask);

                mSocket_Server = new UDPSocket();
                assert(NULL != mSocket_Server);

                lConfig.mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
                lConfig.mServer_IPv4 = UDPSocket::GetIPv4(mInfo.mIPv4_Address, mInfo.mIPv4_NetMask);
                lConfig.mServer_Port = mSocket_Server->GetPort();
                assert(0 != lConfig.mServer_IPv4);
                assert(0 != lConfig.mServer_Port);

                // We send a dummy request to the device in order to indicate
                // to firewall this UDP communication is OK.
                EthCAN_Header lHeader;

                Request_Init(&lHeader, EthCAN_REQUEST_DO_NOTHING, EthCAN_FLAG_NO_RESPONSE, 0);

                mSocket_Server->Send(&lHeader, sizeof(lHeader), mInfo.mIPv4_Address);
            }
            else
            {
                assert(IsConnectedUSB());

                lConfig.mServer_Flags |= EthCAN_FLAG_SERVER_USB;
                lConfig.mServer_IPv4 = 0;
                lConfig.mServer_Port = 0;
            }

            lResult = Config_Set(&lConfig);
            if (EthCAN_OK == lResult)
            {
                mContext = aContext;
                mReceiver = aReceiver;
            }

            if (IsConnectedEth())
            {
                mThread = new Thread(this, MSG_LOOP_ITERATION);
                assert(NULL != mThread);
            }
        }
        catch (EthCAN_Result eResult)
        {
            TRACE_ERROR(stderr, "Device_Impl::Receiver_Start - Exception");
            lResult = eResult;
        }
    }

    if (EthCAN_OK != lResult)
    {
        TRACE_ERROR(stderr, "Device_Impl::Receiver_Start");

        if (NULL != mSocket_Server)
        {
            delete mSocket_Server;
            mSocket_Server = NULL;
        }

        mContext = NULL;
        mReceiver = NULL;
    }

    return lResult;
}

EthCAN_Result Device_Impl::Receiver_Stop()
{
    if (NULL == mReceiver) { return EthCAN_ERROR_NOT_RUNNING; }

    EthCAN_Config lConfig;

    EthCAN_Result lResult = Config_Get(&lConfig);
    if (EthCAN_OK == lResult)
    {
        lConfig.mServer_IPv4 = 0;
        lConfig.mServer_Port = 0;
        lConfig.mServer_Flags &= ~ EthCAN_FLAG_SERVER_USB;

        lResult = Config_Set(&lConfig);
    }

    try
    {
        if (NULL != mThread)
        {
            assert(NULL != mSocket_Server);

            delete mThread;
            delete mSocket_Server;
        }
    }
    catch (EthCAN_Result eResult)
    {
        TRACE_ERROR(stderr, "Device_Impl::Receiver_Stop - Exception");
        lResult = eResult;
    }

    mContext = NULL;
    mReceiver = NULL;
    mSocket_Server = NULL;
    mThread = NULL;

    return lResult;
}

EthCAN_Result Device_Impl::Reset(uint8_t aFlags)
{
    BEGIN
    {
        unsigned int lSize_byte = Request(EthCAN_REQUEST_RESET, aFlags, NULL, 0, NULL, 0);
        assert(0 == lSize_byte);
        (void)lSize_byte;

        lResult = EthCAN_OK;
    }
    END
}

EthCAN_Result Device_Impl::Send(const EthCAN_Frame& aIn, uint8_t aFlags)
{
    if (NULL == &aIn) { return EthCAN_ERROR_INPUT_BUFFER; }

    BEGIN
    {
        unsigned int lSize_byte = Request(EthCAN_REQUEST_SEND, aFlags, &aIn, sizeof(aIn), NULL, 0);
        assert(0 == lSize_byte);
        (void)lSize_byte;

        lResult = EthCAN_OK;
    }
    END
}

// ===== EthCAN::Object =====================================================

void Device_Impl::Debug(FILE* aOut) const
{
    FILE* lOut = (NULL == aOut) ? stdout : aOut;

    fprintf(lOut, "    Information\n");

    EthCAN::Display(lOut, mInfo);

    fprintf(lOut, "    Last ID Client  : %u\n", mId_Client);
    fprintf(lOut, "    Last ID Server  : %u\n", mId_Server);
    fprintf(lOut, "    Lost Count      : %u\n", mLostCount);
    fprintf(lOut, "    Receiver        : %s\n", NULL == mReceiver ? "Stopped" : "Started");
    fprintf(lOut, "    Req. Code       : "); EthCAN::Display(lOut, static_cast<EthCAN_RequestCode>(mReq_Code));
    fprintf(lOut, "    Req. Out Size   : %u bytes\n", mReq_OutSize_byte);
    fprintf(lOut, "    Req. Result     : %s\n", EthCAN::GetName(mReq_Result));
    fprintf(lOut, "    Serial          : %s\n", NULL == mSerial ? "Not connected" : "Connected");
    fprintf(lOut, "    Socket Client   : %s\n", NULL == mSocket_Client ? "Not connected" : "Connected");
    fprintf(lOut, "    Socket Server   : %s\n", NULL == mSocket_Server ? "Not connected" : "Connected");
    fprintf(lOut, "    Thread          : %s\n", NULL == mThread ? "Not started" : "Started");

    Object::Debug(lOut);
}

// ===== IMessageReceiver ===================================================

bool Device_Impl::OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte)
{
    bool lResult = false;

    switch (aMessage)
    {
    case MSG_LOOP_ITERATION: lResult = OnLoopIteration(); break;
    case MSG_SERIAL_DATA   : lResult = OnSerialData(aData, aSize_byte); break;

    default: assert(false);
    }

    return lResult;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

// NOT TESTED Device
//            Releasing a device while the receiver is started

Device_Impl::~Device_Impl()
{
    if (NULL != mReceiver)
    {
        Receiver_Stop();

        assert(NULL == mReceiver);
    }

    if (NULL != mSerial)
    {
        delete mSerial;
    }

    if (NULL != mSocket_Client)
    {
        delete mSocket_Client;
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

// TODO Device
//      Optimize busy time.

void Device_Impl::Busy_Mark()
{
    uint64_t lNow_ms = OS_GetTickCount();

    switch (mReq_Code)
    {
    case EthCAN_REQUEST_CONFIG_RESET:
    case EthCAN_REQUEST_CONFIG_SET:
        mBusyUntil_ms = lNow_ms + 3000;
        break;

    case EthCAN_REQUEST_RESET:
        mBusyUntil_ms = lNow_ms + (IsConnectedEth() ? 7000 : 3000);
        break;

    default: assert(false);
    }
}

void Device_Impl::Busy_Wait()
{
    if (0 < mBusyUntil_ms)
    {
        uint64_t lNow_ms = OS_GetTickCount();

        if (mBusyUntil_ms > lNow_ms)
        {
            uint64_t lDiff_ms = mBusyUntil_ms - lNow_ms;
            assert(7000 >= lDiff_ms);

            OS_Sleep(static_cast<unsigned int>(lDiff_ms));
        }

        mBusyUntil_ms = 0;
    }
}

void Device_Impl::Config_Verify(const EthCAN_Config& aIn)
{
    assert(NULL != &aIn);

    // TODO Device.Config_Verify
    //      aIn.mCAN_Filter
    //      aIn.mCAN_Flags
    //      aIn.mCAN_Masks

    if (EthCAN_RATE_QTY <= aIn.mCAN_Rate)
    {
        throw EthCAN_ERROR_CAN_RATE;
    }

    // TODO Device.Config_Verify
    //      aIn.mFlags
    //      aIn.mIPv4_Address
    //      aIn.mIPv4_Gateway
    //      aIn.mIPv4_Subnet
    //      aIn.mName[16]
    //      aIn.mServer_IPv4
    //      aIn.mServer_Port
    //      aIn.mWiFi_Name
    //      aIn.mWiFi_Password
}

void Device_Impl::Eth_Receive()
{
    assert(NULL != mSocket_Client);

    uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];

    unsigned int lSize_byte = sizeof(EthCAN_Header) + mReq_OutSize_byte;
    assert(sizeof(lBuffer) >= lSize_byte);

    for (;;)
    {
        unsigned int lReceived_byte = mSocket_Client->Receive(lBuffer, sizeof(lBuffer), 2000);
        if (sizeof(EthCAN_Header) > lReceived_byte)
        {
            TRACE_ERROR(stderr, "Device_Impl::Eth_Receive - EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER");
            throw EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER;
        }

        assert(sizeof(lBuffer) >= lReceived_byte);

        if (OnResponse(reinterpret_cast<EthCAN_Header*>(lBuffer), lReceived_byte))
        {
            break;
        }

        TRACE_DEBUG(stderr, "Device_Impl::Eth_Receive - Not tested");
    }
}

bool Device_Impl::OnLoopIteration()
{
    assert(0 != mInfo.mIPv4_Address);
    assert(NULL != mSocket_Server);

    uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];
    uint32_t lFrom;

    bool lResult = true;

    unsigned int lSize_byte = mSocket_Server->Receive(lBuffer, sizeof(lBuffer), 500, &lFrom);
    if ((mInfo.mIPv4_Address == lFrom)
        && (sizeof(EthCAN_Header) + sizeof(EthCAN_Frame)) == lSize_byte)
    {
        lResult = OnRequest(reinterpret_cast<EthCAN_Header*>(lBuffer), lSize_byte);
    }

    return lResult;
}

bool Device_Impl::OnRequest(const EthCAN_Header* aHeader, unsigned int aSize_byte)
{
    assert(NULL != aHeader);
    assert(sizeof(EthCAN_Header) <= aSize_byte);

    bool lResult = true;

    // TODO Device.Security

    if (   (aHeader->mCode           == EthCAN_REQUEST_SEND)
        && (aHeader->mDataSize_byte  == sizeof(EthCAN_Frame))
        && (aHeader->mFlags          == EthCAN_FLAG_NO_RESPONSE)
        && (aHeader->mResult         == EthCAN_RESULT_REQUEST)
        && (aHeader->mTotalSize_byte == aSize_byte))
    {
        if (0 != mId_Server)
        {
            uint32_t lCount = aHeader->mId - mId_Server;
            if (1 < lCount)
            {
                TRACE_WARNING(stderr, "Device_Impl::OnRequest - Lost request");
                mLostCount += lCount - 1;
            }
        }

        mId_Server = aHeader->mId;

        if (NULL != mReceiver)
        {
            lResult = mReceiver(this, mContext, *reinterpret_cast<const EthCAN_Frame*>(aHeader + 1));
        }
    }

    return lResult;
}

bool Device_Impl::OnResponse(const EthCAN_Header* aHeader, unsigned int aSize_byte)
{
    assert(NULL != aHeader);
    assert(sizeof(EthCAN_Header) <= aSize_byte);

    assert(EthCAN_REQUEST_QTY > mReq_Code);

    // TODO Device.Security

    bool lResult = (aHeader->mCode == mReq_Code) && (aHeader->mId == mId_Client);
    if (lResult)
    {
        if (aHeader->mTotalSize_byte != aSize_byte)
        {
            TRACE_ERROR(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_RESPONSE_SIZE");
            mReq_Result = EthCAN_ERROR_RESPONSE_SIZE;
        }
        else
        {
            mReq_Result = static_cast<EthCAN_Result>(aHeader->mResult);
            if (EthCAN_OK == mReq_Result)
            {
                unsigned int lSize_byte = aSize_byte - sizeof(EthCAN_Header);
                if (aHeader->mDataSize_byte != lSize_byte)
                {
                    TRACE_ERROR(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_DATA_SIZE");
                    mReq_Result = EthCAN_ERROR_DATA_SIZE;
                }
                else
                {
                    if (mReq_OutSize_byte < lSize_byte)
                    {
                        TRACE_ERROR(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_DATA_UNEXPECTED");
                        mReq_Result = EthCAN_ERROR_DATA_UNEXPECTED;
                    }
                    else
                    {
                        if (0 < lSize_byte)
                        {
                            assert(NULL != mReq_Out);

                            memcpy(mReq_Out, aHeader + 1, lSize_byte);
                        }

                        mReq_OutSize_byte = lSize_byte;

                        if (EthCAN_FLAG_BUSY == (aHeader->mFlags & EthCAN_FLAG_BUSY))
                        {
                            Busy_Mark();
                        }
                    }
                }
            }
        }
    }

    return lResult;
}

bool Device_Impl::OnSerialData(const void* aData, unsigned int aSize_byte)
{
    assert(NULL != aData);
    assert(sizeof(EthCAN_Header) <= aSize_byte);

    const EthCAN_Header* lHeader = reinterpret_cast<const EthCAN_Header *>(aData);

    if (EthCAN_RESULT_REQUEST == lHeader->mResult)
    {
        OnRequest(lHeader, aSize_byte);
    }
    else
    {
        if (OnResponse(lHeader, aSize_byte))
        {
            mSerial->GetThread()->Sem_Signal();
        }
    }

    return true;
}

unsigned int Device_Impl::Request(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte)
{
    assert(EthCAN_REQUEST_QTY > aCode);

    mReq_Code = aCode;
    mReq_Out = aOut;
    mReq_OutSize_byte = aOutSize_byte;

    Busy_Wait();

    Request_Send(aCode, aFlags, aIn, aInSize_byte);

    if (0 == (aFlags & EthCAN_FLAG_NO_RESPONSE))
    {
        if (IsConnectedEth())
        {
            Eth_Receive();
        }
        else
        {
            assert(NULL != mSerial);

            mSerial->GetThread()->Sem_Wait(2000);
        }
    }

    return mReq_OutSize_byte;
}

void Device_Impl::Request_Init(EthCAN_Header* aHeader, uint8_t aCode, uint8_t aFlags, unsigned int aDataSize_byte)
{
    assert(NULL != aHeader);
    assert(EthCAN_REQUEST_QTY > aCode);

    unsigned int lTotalSize_byte = sizeof(EthCAN_Header) + aDataSize_byte;
    assert(0xff >= lTotalSize_byte);

    memset(aHeader, 0, sizeof(*aHeader));

    // TODO Device.Security

    aHeader->mCode           = aCode;
    aHeader->mDataSize_byte  = aDataSize_byte;
    aHeader->mFlags          = aFlags;
    aHeader->mId             = mId_Client;
    aHeader->mResult         = EthCAN_RESULT_REQUEST;
    aHeader->mTotalSize_byte = lTotalSize_byte;
}

void Device_Impl::Request_Send(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte)
{
    assert(EthCAN_REQUEST_QTY > aCode);

    mId_Client++;

    uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];

    unsigned int lSize_byte = sizeof(EthCAN_Header) + aInSize_byte;
    assert(sizeof(lBuffer) >= lSize_byte);

    EthCAN_Header* lHeader = reinterpret_cast<EthCAN_Header*>(lBuffer);

    Request_Init(lHeader, aCode, aFlags, aInSize_byte);

    if (0 < aInSize_byte)
    {
        assert(NULL != aIn);

        memcpy(lHeader + 1, aIn, aInSize_byte);
    }

    if (IsConnectedEth())
    {
        assert(NULL != mSocket_Client);

        mSocket_Client->Send(lBuffer, lSize_byte, mInfo.mIPv4_Address);
    }
    else if (IsConnectedUSB())
    {
        assert(NULL != mSerial);

        mSerial->Send(lBuffer, lSize_byte);
    }
    else
    {
        TRACE_ERROR(stderr, "Device_Impl::Request_Send - EthCAN_ERROR_NOT_CONNECTED");
        throw EthCAN_ERROR_NOT_CONNECTED;
    }
}
