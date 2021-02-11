
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Device_Impl.h

#pragma once

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
}

#include <EthCAN/Device.h>

// ===== EthCAN_Lib =========================================================
#include "IMessageReceiver.h"

class Device_Impl : public EthCAN::Device, public IMessageReceiver
{

public:

    Device_Impl();

    bool Is(const uint8_t aEth[6]) const;
    bool Is(uint32_t aIPv4) const;
    bool Is(const char* aName) const;
    bool Is(Serial* aSerial) const;

    void SetInfo(const EthCAN_Info& aInfo, Serial* aSerial);

    // ===== EthCAN::Device =================================================

    virtual EthCAN_Result Config_Get(EthCAN_Config* aOut);
    virtual EthCAN_Result Config_Reset(uint8_t aFlags = 0);
    virtual EthCAN_Result Config_Set(EthCAN_Config* aInOut, uint8_t aFlags = 0);
    virtual EthCAN_Result Config_Store(uint8_t aFlags = 0);

    virtual EthCAN_Result GetInfo(EthCAN_Info* aOut);

    virtual bool IsConnectedEth();
    virtual bool IsConnectedUSB();

    virtual EthCAN_Result Receiver_Start(Receiver aFunction, void* aContext);
    virtual EthCAN_Result Receiver_Stop();

    virtual EthCAN_Result Reset(uint8_t aFlags = 0);

    virtual EthCAN_Result Send(const EthCAN_Frame& aIn, uint8_t aFlags = 0);

    // ===== EthCAN::Object =================================================
    virtual void Debug(FILE* aOut = NULL) const;

    // ===== IMessageReceived ===============================================
    virtual bool OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte);

protected:

    // ===== EthCAN::Object =================================================
    virtual ~Device_Impl();

private:

    void Config_Verify(const EthCAN_Config& aIn);

    void Eth_Receive();

    bool OnLoopIteration();
    bool OnRequest (const EthCAN_Header* aHeader, unsigned int aSize_byte);
    bool OnResponse(const EthCAN_Header* aHeader, unsigned int aSize_byte);
    bool OnSerialData(const void* aData, unsigned int aSize_byte);

    unsigned int Request(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte);

    void Request_Process();
    void Request_Send(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte);

    void* mContext;

    uint32_t mId_Client;
    uint32_t mId_Server;

    EthCAN_Info mInfo;

    unsigned int mLostCount;

    Receiver mReceiver;

    uint8_t       mReq_Code;
    void*         mReq_Out;
    unsigned int  mReq_OutSize_byte;
    EthCAN_Result mReq_Result;

    Serial* mSerial;

    UDPSocket* mSocket_Client;
    UDPSocket* mSocket_Server;

    Thread* mThread;
};
