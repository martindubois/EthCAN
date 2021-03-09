
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/System.cpp

#include "Component.h"

// ===== EthCAN_Lib =========================================================
#include "Device_Impl.h"
#include "Serial.h"
#include "Thread.h"
#include "UDPSocket.h"

#include "System_Impl.h"

// Constants
/////////////////////////////////////////////////////////////////////////////

#define MSG_SERIAL_DATA (1)

// Public
/////////////////////////////////////////////////////////////////////////////

System_Impl::System_Impl() : mDeviceCount(0), mDevices(NULL), mRequestId(0)
{
}

// ===== EthCAN::System =====================================================

EthCAN_Result System_Impl::Detect()
{
    EthCAN_Result lResult = EthCAN_RESULT_INVALID;

    try
    {
        Devices_Release();

        Detect_Eth();
        Detect_USB();

        lResult = EthCAN_OK;
    }
    catch (EthCAN_Result eResult)
    {
        fprintf(stderr, "ERROR  System_Impl::Detect - %u\n", eResult);
        lResult = eResult;
    }

    return lResult;
}

EthCAN::Device* System_Impl::Device_Find_Eth(const uint8_t aEth[6])
{
    if (NULL != aEth)
    {
        for (unsigned int i = 0; i < mDeviceCount; i++)
        {
            assert(NULL != mDevices);
            assert(NULL != mDevices[i]);

            if (mDevices[i]->Is(aEth))
            {
                mDevices[i]->IncRefCount();

                return mDevices[i];
            }
        }
    }

    return NULL;
}

EthCAN::Device* System_Impl::Device_Find_IPv4(uint32_t aIPv4)
{
    for (unsigned int i = 0; i < mDeviceCount; i++)
    {
        assert(NULL != mDevices);
        assert(NULL != mDevices[i]);

        if (mDevices[i]->Is(aIPv4))
        {
            mDevices[i]->IncRefCount();

            return mDevices[i];
        }
    }

    return NULL;
}

EthCAN::Device* System_Impl::Device_Find_Name(const char* aName)
{
    if (NULL != aName)
    {
        for (unsigned int i = 0; i < mDeviceCount; i++)
        {
            assert(NULL != mDevices);
            assert(NULL != mDevices[i]);

            if (mDevices[i]->Is(aName))
            {
                mDevices[i]->IncRefCount();

                return mDevices[i];
            }
        }
    }

    return NULL;
}

EthCAN::Device* System_Impl::Device_Find_USB(unsigned int aIndex)
{
    unsigned int lIndex = 0;

    for (unsigned int i = 0; i < mDeviceCount; i++)
    {
        assert(NULL != mDevices);
        assert(NULL != mDevices[i]);

        if (mDevices[i]->IsConnectedUSB())
        {
            if (aIndex = lIndex)
            {
                mDevices[i]->IncRefCount();

                return mDevices[i];
            }

            lIndex++;
        }
    }

    return NULL;
}

EthCAN::Device* System_Impl::Device_Get(unsigned int aIndex)
{
    if (mDeviceCount <= aIndex)
    {
        return NULL;
    }

    assert(NULL != mDevices);
    assert(NULL != mDevices[aIndex]);

    mDevices[aIndex]->IncRefCount();

    return mDevices[aIndex];
}

unsigned int System_Impl::Device_GetCount() const
{
    return mDeviceCount;
}

// ===== EthCAN::Object =====================================================

void System_Impl::Debug(FILE* aOut) const
{
    FILE* lOut = (NULL == aOut) ? stdout : aOut;

    fprintf(lOut,
        "Last request id : %u\n"
        "%u devices\n",
        mRequestId,
        mDeviceCount);

    for (unsigned int i = 0; i < mDeviceCount; i++)
    {
        assert(NULL != mDevices);
        assert(NULL != mDevices[i]);

        fprintf(lOut, "Device %u\n", i);

        mDevices[i]->Debug(lOut);
    }
}

// ===== IMessageReceiver ===================================================

bool System_Impl::OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte)
{
    bool lResult = false;

    switch (aMessage)
    {
    case MSG_SERIAL_DATA: lResult = OnSerialData(aSource, aData, aSize_byte); break;

    default: assert(false);
    }

    return true;
}

// Protected
/////////////////////////////////////////////////////////////////////////////

// ===== EthCAN::Object =====================================================

System_Impl::~System_Impl()
{
    Devices_Release();
}

// Private
/////////////////////////////////////////////////////////////////////////////

void System_Impl::Detect_Eth()
{
    UDPSocket lSocket;

    EthCAN_Header lHeader;

    Request_Init(&lHeader);

    for (unsigned int i = 0; i < 3; i++)
    {
        lSocket.Broadcast(&lHeader, sizeof(lHeader));

        Detect_Receive(&lSocket);
    }
}

void System_Impl::Detect_Receive(UDPSocket* aSocket)
{
    assert(NULL != aSocket);

    for (;;)
    {
        char lBuffer[256];

        uint32_t lFrom;

        unsigned int lSize_byte = aSocket->Receive(lBuffer, sizeof(lBuffer), 300, &lFrom);
        if (0 == lSize_byte)
        {
            break;
        }

        OnDetectData(lBuffer, lSize_byte, lFrom, NULL);
    }
}

Device_Impl* System_Impl::Device_Add()
{
    if (0 == mDeviceCount % 16)
    {
        Device_Impl** lDevices = new Device_Impl * [mDeviceCount + 16];
        assert(NULL != lDevices);

        if (0 < mDeviceCount)
        {
            assert(NULL != mDevices);

            for (unsigned int i = 0; i < mDeviceCount; i++)
            {
                lDevices[i] = mDevices[i];
            }

            delete[] mDevices;
        }

        mDevices = lDevices;
    }

    assert(NULL != mDevices);

    Device_Impl* lResult = new Device_Impl();
    assert(NULL != lResult);

    mDevices[mDeviceCount] = lResult;
    mDeviceCount++;

    return lResult;
}

Device_Impl* System_Impl::Device_Find_Serial(Serial* aSerial)
{
    assert(NULL != aSerial);

    for (unsigned int i = 0; i < mDeviceCount; i++)
    {
        assert(NULL != mDevices);
        assert(NULL != mDevices[i]);

        if (mDevices[i]->Is(aSerial))
        {
            mDevices[i]->IncRefCount();

            return mDevices[i];
        }
    }

    return NULL;
}

void System_Impl::Devices_Release()
{
    if (0 < mDeviceCount)
    {
        assert(NULL != mDevices);

        for (unsigned int i = 0; i < mDeviceCount; i++)
        {
            assert(NULL != mDevices[i]);

            mDevices[i]->Release();
        }

        mDeviceCount = 0;

        delete[] mDevices;
    }
}

bool System_Impl::OnDetectData(const void* aData, unsigned int aSize_byte, uint32_t aFrom, Serial* aSerial)
{
    assert(NULL != aData);
    assert(0 < aSize_byte);

    if (sizeof(EthCAN_Header) <= aSize_byte)
    {
        const EthCAN_Header* lHeader = reinterpret_cast<const EthCAN_Header*>(aData);

        if (   (lHeader->mCode == EthCAN_REQUEST_INFO_GET)
            && (lHeader->mDataSize_byte == sizeof(EthCAN_Info))
            && (lHeader->mId == mRequestId)
            && (lHeader->mResult == EthCAN_OK)
            && (lHeader->mTotalSize_byte == sizeof(EthCAN_Header) + sizeof(EthCAN_Info)))
        {
            const EthCAN_Info* lInfo = reinterpret_cast<const EthCAN_Info*>(lHeader + 1);

            if ((0 == aFrom) || (lInfo->mIPv4_Address == aFrom))
            {
                return OnInfo(*lInfo, aSerial);
            }
        }
    }

    return false;
}

bool System_Impl::OnInfo(const EthCAN_Info& aIn, Serial* aSerial)
{
    assert(NULL != &aIn);

    Device_Impl* lDevice = dynamic_cast<Device_Impl*>(Device_Find_Eth(aIn.mEth_Address));
    if (NULL == lDevice)
    {
        lDevice = Device_Add();
    }

    lDevice->SetInfo(aIn, aSerial);
    lDevice->Release();

    return true;
}

bool System_Impl::OnSerialData(void * aSource, const void* aData, unsigned int aSize_byte)
{
    assert(NULL != aSource);
    assert(NULL != aData);
    assert(0 < aSize_byte);

    if (OnDetectData(aData, aSize_byte, 0, reinterpret_cast<Serial*>(aSource)))
    {
        Serial* lSerial = reinterpret_cast<Serial *>(aSource);

        lSerial->GetThread()->Sem_Signal();
    }

    return true;
}

void System_Impl::OnSerialLink(const char* aLink)
{
    assert(NULL != aLink);

    Serial* lSerial = NULL;

    try
    {
        Serial* lSerial = new Serial(aLink);
        assert(NULL != lSerial);

        OnSerialPort(lSerial);
    }
    catch (...)
    {
        fprintf(stderr, "WARNING  System_Impl::OnSerialLink - Excpetion\n");

        if (NULL != lSerial)
        {
            delete lSerial;
        }
    }
}

void System_Impl::OnSerialPort(Serial* aSerial)
{
    assert(NULL != aSerial);

    aSerial->Receiver_Start(this, MSG_SERIAL_DATA);

    EthCAN_Header lHeader;

    Request_Init(&lHeader);

    aSerial->Send(&lHeader, sizeof(lHeader));

    try
    {
        aSerial->GetThread()->Sem_Wait(5000);
    }
    catch (...)
    {
        fprintf(stderr, "WARNING  System_Impl::OnSerialPort - Exception\n");
        delete aSerial;
    }
}

void System_Impl::Request_Init(EthCAN_Header* aOut)
{
    assert(NULL != aOut);

    mRequestId++;

    memset(aOut, 0, sizeof(*aOut));

    // TODO Device.Security

    aOut->mCode = EthCAN_REQUEST_INFO_GET;
    aOut->mId = mRequestId;
    aOut->mTotalSize_byte = sizeof(*aOut);
}
