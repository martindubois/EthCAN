
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021-2022 KMS
// Product   EthCAN
// File      EthCAN_Lib/Device_Impl.cpp

// CODE REVIEW 2021-12-29 KMS - Martin Dubois, P. Eng.

// TEST COVERAGE 2021-12-29 KMS - Martin Dubois, P. Eng.

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
#include "Protocol_TCP.h"
#include "Protocol_UDP.h"
#include "Protocol_USB.h"
#include "Serial.h"
#include "Socket.h"
#include "Thread.h"

#include "Device_Impl.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define MSG_LOOP_ITERATION (1)
#define MSG_SERIAL_DATA    (2)

// Static function declarations
// //////////////////////////////////////////////////////////////////////////

static void ClearServerConfig(EthCAN_Config* aConfig);

// Public
/////////////////////////////////////////////////////////////////////////////

Device_Impl::Device_Impl()
    : mBusyUntil_ms(0)
    , mContext(NULL)
    , mId_Client(0)
    , mId_Server(0)
    , mIsConnectedEth(false)
    , mProtocol(NULL)
    , mProtocolId(PROTOCOL_INVALID)
    , mReceiver(NULL)
    , mReq_Code        (EthCAN_REQUEST_INVALID)
    , mReq_Out         (NULL)
    , mReq_OutSize_byte(0)
    , mThread(NULL)
    , mUDP_Server(NULL)
    , mUSB_Serial(NULL)
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
    if (NULL != mProtocol)
    {
        Protocol_Delete();
    }

    if (NULL == aSerial)
    {
        mIsConnectedEth = true;
        mProtocolId = PROTOCOL_UDP;
    }
    else
    {
        mProtocolId = PROTOCOL_USB;
        mUSB_Serial = aSerial;
    }

    Protocol_Create();

    mInfo = aInfo;
}

// ===== EthCAN::Device =====================================================

#define BEGIN                          \
    EthCAN_Result lResult = EthCAN_OK; \
    try

#define END                       \
    catch (EthCAN_Result eResult) \
    {                             \
        lResult = eResult;        \
    }                             \
    return lResult;

EthCAN_Result Device_Impl::CAN_Reset(uint8_t aFlags)
{
    BEGIN
    {
        unsigned int lSize_byte = Request(EthCAN_REQUEST_CAN_RESET, aFlags, NULL, 0, NULL, 0);
        assert(0 == lSize_byte);
        (void)lSize_byte;
    }
    END
}

EthCAN_Result Device_Impl::Config_Erase(uint8_t aFlags)
{
    BEGIN
    {
        if (0 != Request(EthCAN_REQUEST_CONFIG_ERASE, aFlags, NULL, 0, NULL, 0))
        {
            TRACE_ERROR(stderr, "Device_Impl::Config_Erase - EthCAN_ERROR_DATA_SIZE");
            lResult = EthCAN_ERROR_DATA_SIZE;
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
    }
    END
}

EthCAN_Result Device_Impl::Device_Reset(uint8_t aFlags)
{
    BEGIN
    {
        unsigned int lSize_byte = Request(EthCAN_REQUEST_DEVICE_RESET, aFlags, NULL, 0, NULL, 0);
        assert(0 == lSize_byte);
        (void)lSize_byte;
    }
    END
}

uint32_t Device_Impl::GetHostAddress() const
{
    if (!IsConnectedEth())
    {
        return EthCAN_ERROR_NOT_CONNECTED_ETH;
    }

    return Socket::GetIPv4(mInfo.mIPv4_Address, mInfo.mIPv4_NetMask);
}

EthCAN_Result Device_Impl::GetInfoLine(char* aOut, unsigned int aSize_byte) const
{
    assert(NULL != mProtocol);

    if (NULL == aOut   ) { return EthCAN_ERROR_OUTPUT_BUFFER; }
    if (60 > aSize_byte) { return EthCAN_ERROR_OUTPUT_BUFFER_TOO_SMALL; }

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

    sprintf_s(aOut SIZE_INFO(aSize_byte), "%s %s %3u.%3u.%3u.%3u %-17s %s",
        lCon, lEth,
        mInfo.mIPv4_Address & 0xff, mInfo.mIPv4_Address >> 8 & 0xff, mInfo.mIPv4_Address >> 16 & 0xff, mInfo.mIPv4_Address >> 24 & 0xff,
        mInfo.mName,
        mProtocol->GetName());

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

        // TODO Security
        //      Validate the received information match the already known information.
    }
    END
}

bool Device_Impl::IsConnectedEth() const { return mIsConnectedEth; }
bool Device_Impl::IsConnectedUSB() const { return NULL != mUSB_Serial; }

EthCAN::Device::ProtocolId Device_Impl::Protocol_Get() const
{
    assert(PROTOCOL_QTY > mProtocolId);

    return mProtocolId;
}

EthCAN_Result Device_Impl::Protocol_Reset()
{
    Protocol_Delete();
    Protocol_Create();

    return EthCAN_OK;
}

EthCAN_Result Device_Impl::Protocol_Set(ProtocolId aId)
{
    if (NULL != mReceiver) { return EthCAN_ERROR_RECEIVER_RUNNING; }

    assert(PROTOCOL_QTY > mProtocolId);

    if (mProtocolId != aId)
    {
        switch (aId)
        {
        case PROTOCOL_UDP:
        case PROTOCOL_TCP:
            if (!IsConnectedEth()) { return EthCAN_ERROR_NOT_CONNECTED_ETH; }
            break;

        case PROTOCOL_USB:
            if (!IsConnectedUSB()) { return EthCAN_ERROR_NOT_CONNECTED_USB; }
            break;

        default: return EthCAN_ERROR_ENUM;
        }

        Protocol_Delete();

        mProtocolId = aId;

        Protocol_Create();
    }

    return EthCAN_OK;
}

EthCAN_Result Device_Impl::Receiver_Config()
{
    EthCAN_Config lConfig;

    EthCAN_Result lResult = Config_Get(&lConfig);
    if (EthCAN_OK == lResult)
    {
        ClearServerConfig(&lConfig);

        try
        {
            switch (mProtocolId)
            {
            case PROTOCOL_UDP:
                assert(0 != mInfo.mIPv4_Address);
                assert(0 != mInfo.mIPv4_NetMask);

                if (NULL == mUDP_Server)
                {
                    lResult = EthCAN_ERROR_NOT_RUNNING;
                }
                else
                {
                    lConfig.mServer_IPv4 = Socket::GetIPv4(mInfo.mIPv4_Address, mInfo.mIPv4_NetMask);
                    lConfig.mServer_Port = mUDP_Server->GetPort();
                    assert(0 != lConfig.mServer_IPv4);
                    assert(0 != lConfig.mServer_Port);

                    UDP_Beep();
                }
                break;

            case PROTOCOL_USB: lConfig.mServer_Flags |= EthCAN_FLAG_SERVER_USB; break;
            case PROTOCOL_TCP: lConfig.mServer_Flags |= EthCAN_FLAG_SERVER_TCP; break;

            default: assert(false);
            }

            if (EthCAN_OK == lResult)
            {
                lResult = Config_Set(&lConfig);
            }
        }
        catch (EthCAN_Result eResult)
        {
            fprintf(stderr, "Device_Impl::Receiver_Config - Exception - %u\n", eResult);
            lResult = eResult;
        }
    }

    return lResult;
}

EthCAN_Result Device_Impl::Receiver_Start(Receiver aReceiver, void* aContext)
{
    if (NULL == aReceiver) { return EthCAN_ERROR_FUNCTION; }
    if (NULL != mReceiver) { return EthCAN_ERROR_RUNNING; }

    assert(NULL == mContext);
    assert(PROTOCOL_QTY > mProtocolId);

    mLostCount = 0;

    EthCAN_Result lResult = EthCAN_RESULT_INVALID;

    try
    {
        if (PROTOCOL_UDP == mProtocolId)
        {
            mUDP_Server = new Socket;
            assert(NULL != mUDP_Server);
        }

        lResult = Receiver_Config();
        if (EthCAN_OK == lResult)
        {
            mContext = aContext;
            mReceiver = aReceiver;
        }

        if (PROTOCOL_UDP == mProtocolId)
        {
            Thread_Create();
        }
    }
    catch (EthCAN_Result eResult)
    {
        fprintf(stderr, "Device_Impl::Receiver_Start - Exception - %u\n", eResult);
        lResult = eResult;
    }
 
    if (EthCAN_OK != lResult)
    {
        fprintf(stderr, "Device_Impl::Receiver_Start - Error - %u\n", lResult);

        if (NULL != mUDP_Server)
        {
            UDP_Server_Delete();
        }

        mContext = NULL;
        mReceiver = NULL;
    }

    return lResult;
}

EthCAN_Result Device_Impl::Receiver_Stop()
{
    assert(PROTOCOL_QTY > mProtocolId);

    if (NULL == mReceiver) { return EthCAN_ERROR_NOT_RUNNING; }

    EthCAN_Config lConfig;

    EthCAN_Result lResult = Config_Get(&lConfig);
    if (EthCAN_OK == lResult)
    {
        ClearServerConfig(&lConfig);

        lResult = Config_Set(&lConfig);
    }

    try
    {
        if (PROTOCOL_UDP == mProtocolId)
        {
            Thread_Delete();
            UDP_Server_Delete();
        }
    }
    catch (EthCAN_Result eResult)
    {
        fprintf(stderr, "Device_Impl::Receiver_Stop - Exception - %u\n", eResult);
        lResult = eResult;
    }

    mContext = NULL;
    mReceiver = NULL;

    return lResult;
}

EthCAN_Result Device_Impl::Send(const EthCAN_Frame& aIn, uint8_t aFlags)
{
    #ifdef _KMS_WINDOWS_
        if (NULL == &aIn) { return EthCAN_ERROR_INPUT_BUFFER; }
    #endif

    BEGIN
    {
        unsigned int lSize_byte = Request(EthCAN_REQUEST_SEND, aFlags, &aIn, sizeof(aIn), NULL, 0);
        assert(0 == lSize_byte);
        (void)lSize_byte;
    }
    END
}

// ===== EthCAN::Object =====================================================

void Device_Impl::Debug(FILE* aOut) const
{
    assert(NULL != mProtocol);

    FILE* lOut = (NULL == aOut) ? stdout : aOut;

    fprintf(lOut, "    Information\n");

    EthCAN::Display(lOut, mInfo);

    fprintf(lOut, "    Last ID Client  : %u\n", mId_Client);
    fprintf(lOut, "    Last ID Server  : %u\n", mId_Server);
    fprintf(lOut, "    Lost Count      : %u\n", mLostCount);
    fprintf(lOut, "    Protocol ID     : %s\n", mProtocol->GetName());
    fprintf(lOut, "    Receiver        : %s\n", NULL == mReceiver ? "Stopped" : "Started");
    fprintf(lOut, "    Req. Code       : "); EthCAN::Display(lOut, static_cast<EthCAN_RequestCode>(mReq_Code));
    fprintf(lOut, "    Req. Out Size   : %u bytes\n", mReq_OutSize_byte);
    fprintf(lOut, "    Req. Result     : %s\n", EthCAN::GetName(mReq_Result));
    fprintf(lOut, "    Thread          : %s\n", NULL == mThread ? "Not started" : "Started");
    fprintf(lOut, "    UDP Server      : %s\n", NULL == mUDP_Server ? "Not connected" : "Connected");
    fprintf(lOut, "    USB Serial      : %s\n", NULL == mUSB_Serial ? "Not connected" : "Connected");

    Object::Debug(lOut);
}

// ===== IMessageReceiver ===================================================

bool Device_Impl::OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte)
{
    bool lResult = false;

    switch (aMessage)
    {
    case MSG_LOOP_ITERATION: lResult = OnLoopIteration(); break;
    case MSG_SERIAL_DATA   : lResult = OnData(aData, aSize_byte); break;

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

    if (NULL != mProtocol)
    {
        Protocol_Delete();
        assert(NULL == mProtocol);
    }

    if (NULL != mUSB_Serial)
    {
        delete mUSB_Serial;
    }
}

// Private
/////////////////////////////////////////////////////////////////////////////

// TODO Device
//      Optimize busy time.

void Device_Impl::Busy_Mark_Z0()
{
    uint64_t lNow_ms = OS_GetTickCount();

    switch (mReq_Code)
    {
    case EthCAN_REQUEST_CAN_RESET:
    case EthCAN_REQUEST_CONFIG_RESET:
    case EthCAN_REQUEST_CONFIG_SET:
        mBusyUntil_ms = lNow_ms + 3000;
        break;

    case EthCAN_REQUEST_DEVICE_RESET:
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

bool Device_Impl::OnData(const void* aData, unsigned int aSize_byte)
{
    assert(NULL != aData);
    assert(sizeof(EthCAN_Header) <= aSize_byte);

    assert(NULL != mProtocol);

    const EthCAN_Header* lHeader = reinterpret_cast<const EthCAN_Header*>(aData);

    if (EthCAN_RESULT_REQUEST == lHeader->mResult)
    {
        OnRequest(lHeader, aSize_byte);
    }
    else
    {
        if (OnResponse(lHeader, aSize_byte))
        {
            mProtocol->Signal();
        }
    }

    return true;
}

bool Device_Impl::OnLoopIteration()
{
    assert(0 != mInfo.mIPv4_Address);
    assert(NULL != mProtocol);

    bool lResult = true;

    try
    {
        uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];
        EthCAN_Header* lHeader = reinterpret_cast<EthCAN_Header*>(lBuffer);
        unsigned int lSize_byte;

        switch (mProtocolId)
        {
        case PROTOCOL_TCP:
            lSize_byte = mProtocol->Receive(lBuffer, sizeof(EthCAN_Header), 500, NULL);
            if (sizeof(EthCAN_Header) == lSize_byte)
            {
                if (lSize_byte < lHeader->mTotalSize_byte)
                {
                    lSize_byte += mProtocol->Receive(lBuffer + lSize_byte, lHeader->mTotalSize_byte - lSize_byte, 500, NULL);
                }

                lResult = OnData(lBuffer, lSize_byte);
            }
            break;

        case PROTOCOL_UDP:
            assert(NULL != mUDP_Server);

            uint32_t lFrom;
            lSize_byte = mUDP_Server->Receive(lBuffer, sizeof(lBuffer), 500, &lFrom);
            if ((mInfo.mIPv4_Address == lFrom)
                && (sizeof(EthCAN_Header) + sizeof(EthCAN_Frame)) == lSize_byte)
            {
                lResult = OnRequest(lHeader, lSize_byte);
            }
            break;

        default: assert(false);
        }
    }
    catch (EthCAN_Result eResult)
    {
        fprintf(stderr, "Device_Impl::OnLoopIteration - Exception - %u\n", eResult);
        lResult = false;
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
                fprintf(stderr, "Device_Impl::OnRequest - Lost request (%u)\n", lCount);
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

    bool lResult = false;

    mZone0.Enter();
    {
        if (EthCAN_REQUEST_QTY > mReq_Code)
        {
            // TODO Device.Security

            lResult = (aHeader->mCode == mReq_Code) && (aHeader->mId == mId_Client);
            if (lResult)
            {
                if (aHeader->mTotalSize_byte != aSize_byte)
                {
                    fprintf(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_RESPONSE_SIZE (%u bytes)\n", aSize_byte);
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
                            fprintf(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_DATA_SIZE (%u bytes)\n", lSize_byte);
                            mReq_Result = EthCAN_ERROR_DATA_SIZE;
                        }
                        else
                        {
                            if (mReq_OutSize_byte < lSize_byte)
                            {
                                fprintf(stderr, "Device_Impl::OnResponse - EthCAN_ERROR_DATA_UNEXPECTED (%u bytes)\n", lSize_byte);
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
                                    Busy_Mark_Z0();
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            fprintf(stderr, "Device_Impl::OnResponse - Ignored answer\n");
        }
    }
    mZone0.Leave();

    return lResult;
}

void Device_Impl::Protocol_Create()
{
    assert(NULL == mProtocol);

    switch (mProtocolId)
    {
    case PROTOCOL_UDP:
        mProtocol = new Protocol_UDP;
        break;

    case PROTOCOL_USB:
        assert(NULL != mUSB_Serial);
        mProtocol = new Protocol_USB(mUSB_Serial);
        mUSB_Serial->Receiver_Start(this, MSG_SERIAL_DATA);
        break;

    case PROTOCOL_TCP:
        mProtocol = new Protocol_TCP(mInfo.mIPv4_Address);

        Thread_Create();
        assert(NULL != mThread);

        dynamic_cast<Protocol_TCP*>(mProtocol)->Init(mThread);
        break;

    default: assert(false);
    }

    assert(NULL != mProtocol);
}

void Device_Impl::Protocol_Delete()
{
    assert(NULL != mProtocol);
    assert(PROTOCOL_QTY > mProtocolId);

    if (PROTOCOL_TCP == mProtocolId)
    {
        Thread_Delete();
    }

    delete mProtocol;
    mProtocol = NULL;
}

unsigned int Device_Impl::Request(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte)
{
    assert(EthCAN_REQUEST_QTY > aCode);

    assert(NULL != mProtocol);
    assert(PROTOCOL_QTY > mProtocolId);

    mZone0.Enter();

    if (EthCAN_REQUEST_INVALID != mReq_Code)
    {
        fprintf(stderr, "Device_Impl::Request - EthCAN_ERROR_BUSY - %u\n", mReq_Code);

        mZone0.Leave();

        throw EthCAN_ERROR_BUSY;
    }

    assert(NULL == mReq_Out);
    assert(0 == mReq_OutSize_byte);

    mReq_Code         = aCode;
    mReq_Out          = aOut;
    mReq_OutSize_byte = aOutSize_byte;
    mReq_Result       = EthCAN_RESULT_REQUEST;

    mZone0.Leave();

    unsigned int lResult_byte = 0;

    try
    {
        Busy_Wait();

        Request_Send(aCode, aFlags, aIn, aInSize_byte);

        if (0 == (aFlags & EthCAN_FLAG_NO_RESPONSE))
        {
            switch (mProtocolId)
            {
            case PROTOCOL_UDP: UDP_Receive(); break;

            default: mProtocol->Wait();
            }

            // TODO Device
            //      Verify mReq_Result

            lResult_byte = mReq_OutSize_byte;
        }
    }
    catch (EthCAN_Result eE)
    {
        fprintf(stderr, "ERROR  Device_Impl::Request - Exception - %u\n", eE);
        Request_End();
        throw;
    }

    Request_End();

    return lResult_byte;
}

void Device_Impl::Request_End()
{
    assert(EthCAN_REQUEST_INVALID != mReq_Code);

    mZone0.Enter();
    {
        mReq_Code         = EthCAN_REQUEST_INVALID;
        mReq_Out          = NULL;
        mReq_OutSize_byte = 0;
    }
    mZone0.Leave();
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

    assert(NULL != mProtocol);

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

    mProtocol->Send(lBuffer, lSize_byte, mInfo.mIPv4_Address);
}

void Device_Impl::Thread_Create()
{
    assert(NULL == mThread);

    mThread = new Thread(this, MSG_LOOP_ITERATION);
    assert(NULL != mThread);
}

void Device_Impl::Thread_Delete()
{
    assert(NULL != mThread);

    delete mThread;
    mThread = NULL;
}

// This method sends a dummy request to the device in order to indicate to
// firewall this UDP communication is OK.
void Device_Impl::UDP_Beep()
{
    assert(NULL != mUDP_Server);

    EthCAN_Header lHeader;

    Request_Init(&lHeader, EthCAN_REQUEST_DO_NOTHING, EthCAN_FLAG_NO_RESPONSE, 0);

    mUDP_Server->Send(&lHeader, sizeof(lHeader), mInfo.mIPv4_Address);
}

void Device_Impl::UDP_Receive()
{
    assert(NULL != mProtocol);

    uint8_t lBuffer[EthCAN_PACKET_SIZE_MAX_byte];

    for (;;)
    {
        uint32_t lFrom;

        unsigned int lReceived_byte = mProtocol->Receive(lBuffer, sizeof(lBuffer), 2000, &lFrom);
        if (sizeof(EthCAN_Header) > lReceived_byte)
        {
            fprintf(stderr, "Device_Impl::UDP_Receive - EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER (%u bytes)\n", lReceived_byte);
            throw EthCAN_ERROR_DEVICE_DOES_NOT_ANSWER;
        }

        assert(sizeof(lBuffer) >= lReceived_byte);

        if (OnResponse(reinterpret_cast<EthCAN_Header*>(lBuffer), lReceived_byte))
        {
            break;
        }

        TRACE_DEBUG(stderr, "Device_Impl::UDP_Receive - Retry");
    }
}

void Device_Impl::UDP_Server_Delete()
{
    assert(NULL != mUDP_Server);

    delete mUDP_Server;
    mUDP_Server = NULL;
}

// Static functions
// //////////////////////////////////////////////////////////////////////////

void ClearServerConfig(EthCAN_Config* aConfig)
{
    assert(NULL != aConfig);

    aConfig->mServer_Flags &= ~(EthCAN_FLAG_SERVER_USB | EthCAN_FLAG_SERVER_TCP);
    aConfig->mServer_IPv4 = 0;
    aConfig->mServer_Port = 0;
}
