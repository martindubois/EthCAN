
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/System_Impl.h

#pragma once

// ===== Includes ===========================================================
#include <EthCAN/System.h>

// ===== EthCAN_Lib =========================================================
#include "IMessageReceiver.h"

class Device_Impl;

class System_Impl : public EthCAN::System, public IMessageReceiver
{

public:

    System_Impl();

    // ===== EthCAN::Device =================================================

    virtual EthCAN_Result Detect();

    virtual EthCAN::Device* Device_Find_Eth(const uint8_t aEth[6]);
    virtual EthCAN::Device* Device_Find_IPv4(const uint32_t aIPv4);
    virtual EthCAN::Device* Device_Find_Name(const char* aName);
    virtual EthCAN::Device* Device_Find_USB(unsigned int aIndex = 0);

    virtual EthCAN::Device* Device_Get(unsigned int aIndex);
    virtual unsigned int    Device_GetCount() const;

    // ===== EthCAN::Object =================================================
    virtual void Debug(FILE* aOut = NULL) const;

    // ===== IMessageReceived ===============================================
    virtual bool OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte);

protected:

    // ===== EthCAN::Object =================================================
    virtual ~System_Impl();

private:

    void Detect_Eth();
    void Detect_Receive(UDPSocket* aSocket);
    void Detect_USB();

    Device_Impl* Device_Add();
    Device_Impl* Device_Find_Serial(Serial* aSerial);

    void Devices_Release();

    bool OnDetectData(const void* aData, unsigned int aSize_byte, uint32_t aFrom, Serial* aSerial);
    bool OnInfo(const EthCAN_Info& aInfo, Serial* aSerial);
    bool OnSerialData(void * aSource, const void* aData, unsigned int aSize_byte);
    void OnSerialLink(const char* aLink);
    void OnSerialPort(Serial* aSerial);

    void Request_Init(EthCAN_Header* aOut);

    unsigned int mDeviceCount;

    Device_Impl** mDevices;

    unsigned int mRequestId;

};
