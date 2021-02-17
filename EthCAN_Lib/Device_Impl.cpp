
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Device_Impl.cpp

#include "Component.h"

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
}

#include <EthCAN/System.h>

// ===== EthCAN_Lib =========================================================
#include "Serial.h"
#include "Thread.h"
#include "UDPSocket.h"

#include "Device_Impl.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define MSG_LOOP_ITERATION (1)
#define MSG_SERIAL_DATA    (2)

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Display(FILE* aOut, EthCAN_RequestCode aIn);

// Public
/////////////////////////////////////////////////////////////////////////////

Device_Impl::Device_Impl()
    : mContext(NULL)
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

    return 0 == memcmp(mInfo.mEth_Addr, aEth, sizeof(mInfo.mEth_Addr));
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

bool Device_Impl::Is(Serial* aSerial) const
{
    assert(NULL != aSerial);

    return (mSerial == aSerial);
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

EthCAN_Result Device_Impl::Config_Get(EthCAN_Config* aOut)
{
    if (NULL == aOut)
    {
        return EthCAN_ERROR_INVALID_OUTPUT_BUFFER;
    }

    BEGIN
    {
        if (sizeof(EthCAN_Config) != Request(EthCAN_REQUEST_CONFIG_GET, 0, NULL, 0, aOut, sizeof(EthCAN_Config)))
        {
            fprintf(stderr, "Device_Impl::Config_Get - EthCAN_ERROR_INVALID_DATA_SIZE\n");
            lResult = EthCAN_ERROR_INVALID_DATA_SIZE;
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
            fprintf(stderr, "Device_Impl::Config_Reset - EthCAN_ERROR_INVALID_DATA_SIZE\n");
            lResult = EthCAN_ERROR_INVALID_DATA_SIZE;
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
    if (NULL == aInOut)
    {
        return EthCAN_ERROR_INVALID_BUFFER;
    }

    BEGIN
    {
        Config_Verify(*aInOut);

        if (sizeof(EthCAN_Config) != Request(EthCAN_REQUEST_CONFIG_SET, aFlags, aInOut, sizeof(EthCAN_Config), aInOut, sizeof(EthCAN_Config)))
        {
            fprintf(stderr, "Device_Impl::Config_Set - EthCAN_ERROR_INVALID_DATA_SIZE\n");
            lResult = EthCAN_ERROR_INVALID_DATA_SIZE;
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
            fprintf(stderr, "Device_Impl::Config_Store - EthCAN_ERROR_INVALID_DATA_SIZE\n");
            lResult = EthCAN_ERROR_INVALID_DATA_SIZE;
        }
        else
        {
            lResult = EthCAN_OK;
        }
    }
    END
}

EthCAN_Result Device_Impl::GetInfo(EthCAN_Info* aInfo)
{
    if (NULL == aInfo)
    {
        return EthCAN_ERROR_INVALID_OUTPUT_BUFFER;
    }

    *aInfo = mInfo;

    return EthCAN_OK;
}

bool Device_Impl::IsConnectedEth()
{
    return (NULL != mSocket_Client);
}

bool Device_Impl::IsConnectedUSB()
{
    return (NULL != mSerial);
}

EthCAN_Result Device_Impl::Receiver_Start(Receiver aReceiver, void* aContext)
{
    if (NULL == aReceiver) { return EthCAN_ERROR_INVALID_FUNCTION; }
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
                mSocket_Server = new UDPSocket();

                lConfig.mServer_Flags &= ~EthCAN_FLAG_SERVER_USB;
                lConfig.mServer_IPv4 = mSocket_Server->GetIPv4(mInfo.mIPv4_Address, mInfo.mIPv4_NetMask);
                lConfig.mServer_Port = mSocket_Server->GetPort();
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
            }
        }
        catch (EthCAN_Result eResult)
        {
            fprintf(stderr, "Device_Impl::Receiver_Start - %s\n", EthCAN::System::GetResultName(eResult));
            lResult = eResult;
        }
    }

    if (EthCAN_OK != lResult)
    {
        fprintf(stderr, "Device_Impl::Receiver_Start - %s\n", EthCAN::System::GetResultName(lResult));

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
    if (NULL == &aIn)
    {
        return EthCAN_ERROR_INVALID_INPUT_BUFFER;
    }

    BEGIN
    {
        unsigned int lSize_byte = Request(EthCAN_REQUEST_SEND, aFlags, &aIn, sizeof(aIn), NULL, 0);
        assert(0 == lSize_byte);
        (void)lSize_byte;

        lResult = EthCAN_OK;
    }
    END

    return lResult;
}

// ===== EthCAN::Object =====================================================

void Device_Impl::Debug(FILE* aOut) const
{
    FILE* lOut = (NULL == aOut) ? stdout : aOut;

    fprintf(lOut, "    Information\n");

    Display(lOut, mInfo);

    fprintf(lOut, "    Last ID Client  : %u\n", mId_Client);
    fprintf(lOut, "    Last ID Server  : %u\n", mId_Server);
    fprintf(lOut, "    Lost Count      : %u\n", mLostCount);
    fprintf(lOut, "    Receiver        : %s\n", NULL == mReceiver ? "Stopped" : "Started");
    fprintf(lOut, "    Req. Code       : "); ::Display(lOut, static_cast<EthCAN_RequestCode>(mReq_Code));
    fprintf(lOut, "    Req. Out Size   : %u bytes\n", mReq_OutSize_byte);
    fprintf(lOut, "    Req. Result     : %s\n", EthCAN::System::GetResultName(mReq_Result));
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

Device_Impl::~Device_Impl()
{
    if (NULL != mReceiver)
    {
        Receiver_Stop();

        assert(NULL == mReceiver);
    }

    if (NULL != mSocket_Client)
    {
        delete mSocket_Client;
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

void Device_Impl::Config_Verify(const EthCAN_Config& aIn)
{
    assert(NULL != &aIn);

    // TODO aIn.mCAN_Filter
    // TODO aIn.mCAN_Masks

    if (EthCAN_RATE_QTY <= aIn.mCAN_Rate)
    {
        throw EthCAN_ERROR_INVALID_CAN_RATE;
    }

    // TODO aIn.mFlags
    // TODO aIn.mIPv4_Address
    // TODO aIn.mIPv4_Gateway
    // TODO aIn.mIPv4_Subnet
    // TODO aIn.mName[16]
    // TODO aIn.mServer_IPv4
    // TODO aIn.mServer_Port
    // TODO aIn.mWiFi_Name
    // TODO aIn.mWiFi_Password
}

void Device_Impl::Eth_Receive()
{
    assert(NULL != mSocket_Client);

    uint8_t lBuffer[256];

    unsigned int lSize_byte = sizeof(EthCAN_Header) + mReq_OutSize_byte;
    assert(sizeof(lBuffer) >= lSize_byte);

    for (;;)
    {
        unsigned int lReceived_byte = mSocket_Client->Receive(lBuffer, sizeof(lBuffer), 500);
        if (sizeof(EthCAN_Header) > lReceived_byte)
        {
            throw EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER;
        }

        assert(sizeof(lBuffer) >= lReceived_byte);

        if (OnResponse(reinterpret_cast<EthCAN_Header*>(lBuffer), lReceived_byte))
        {
            break;
        }

        fprintf(stderr, "Device_Impl::Eth_Receive - Not tested\n");
    }
}

bool Device_Impl::OnLoopIteration()
{
    assert(0 != mInfo.mIPv4_Address);
    assert(NULL != mSocket_Server);

    uint8_t lBuffer[256];
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

    // TODO Device.Security

    bool lResult = (aHeader->mCode == mReq_Code) && (aHeader->mId == mId_Client);
    if (lResult)
    {
        if (aHeader->mTotalSize_byte != aSize_byte)
        {
            fprintf(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_INVALID_RESPONSE_SIZE\n");
            mReq_Result = EthCAN_ERROR_INVALID_RESPONSE_SIZE;
        }
        else
        {
            mReq_Result = static_cast<EthCAN_Result>(aHeader->mResult);
            if (EthCAN_OK == mReq_Result)
            {
                unsigned int lSize_byte = aSize_byte - sizeof(EthCAN_Header);
                if (aHeader->mDataSize_byte != lSize_byte)
                {
                    fprintf(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_INVALID_DATA_SIZE\n");
                    mReq_Result = EthCAN_ERROR_INVALID_DATA_SIZE;
                }
                else
                {
                    if (mReq_OutSize_byte < lSize_byte)
                    {
                        fprintf(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_UNEXPECTED_DATA\n");
                        mReq_Result = EthCAN_ERROR_UNEXPECTED_DATA;
                    }
                    else
                    {
                        if (0 < lSize_byte)
                        {
                            assert(NULL != mReq_Out);

                            memcpy(mReq_Out, aHeader + 1, lSize_byte);

                            mReq_OutSize_byte = lSize_byte;
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

            mSerial->GetThread()->Sem_Wait(1000);
        }
    }

    return mReq_OutSize_byte;
}

void Device_Impl::Request_Send(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte)
{
    assert(EthCAN_REQUEST_QTY > aCode);

    mId_Client++;

    uint8_t lBuffer[256];

    unsigned int lSize_byte = sizeof(EthCAN_Header) + aInSize_byte;
    assert(sizeof(lBuffer) >= lSize_byte);

    EthCAN_Header* lHeader = reinterpret_cast<EthCAN_Header*>(lBuffer);

    memset(lHeader, 0, sizeof(EthCAN_Header));

    // TODO Device.Security

    lHeader->mCode           = aCode;
    lHeader->mDataSize_byte  = aInSize_byte;
    lHeader->mFlags          = aFlags;
    lHeader->mId             = mId_Client;
    lHeader->mTotalSize_byte = lSize_byte;

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
        fprintf(stderr, "Device_Impl::Request_Send - EthCAN_ERROR_NOT_CONNECTED\n");
        throw EthCAN_ERROR_NOT_CONNECTED;
    }
}

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

void Display(FILE* aOut, EthCAN_RequestCode aIn)
{
    FILE* lOut = NULL == aOut ? stdout : aOut;

    switch (aIn)
    {
    case EthCAN_REQUEST_CONFIG_GET  : fprintf(lOut, "EthCAN_REQUEST_CONFIG_GET\n"); break;
    case EthCAN_REQUEST_CONFIG_RESET: fprintf(lOut, "EthCAN_REQUEST_CONFIG_RESET\n"); break;
    case EthCAN_REQUEST_CONFIG_SET  : fprintf(lOut, "EthCAN_REQUEST_CONFIG_SET\n"); break;
    case EthCAN_REQUEST_CONFIG_STORE: fprintf(lOut, "EthCAN_REQUEST_CONFIG_STORE\n"); break;
    case EthCAN_REQUEST_INFO_GET    : fprintf(lOut, "EthCAN_REQUEST_INFO_GET\n"); break;
    case EthCAN_REQUEST_RESET       : fprintf(lOut, "EthCAN_REQUEST_RESET\n"); break;
    case EthCAN_REQUEST_SEND        : fprintf(lOut, "EthCAN_REQUEST_SEND\n"); break;

    default: fprintf(lOut, "Invalid request (%u)\n", aIn);
    }
}
