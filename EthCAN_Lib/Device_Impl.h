
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021-2022 KMS
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
#include "Gate.h"
#include "IMessageReceiver.h"

class Protocol;
class Serial;
class Socket;
class Thread;

class Device_Impl : public EthCAN::Device, public IMessageReceiver
{

public:

    Device_Impl();

    bool Is(const uint8_t aEth[6]) const;
    bool Is(uint32_t aIPv4) const;
    bool Is(const char* aName) const;

    void SetInfo(const EthCAN_Info& aInfo, Serial* aSerial);

    // ===== EthCAN::Device =================================================

    virtual EthCAN_Result CAN_Reset(uint8_t aFlags = 0);

    virtual EthCAN_Result Config_Erase(uint8_t aFlags = 0);
    virtual EthCAN_Result Config_Get(EthCAN_Config* aOut);
    virtual EthCAN_Result Config_Reset(uint8_t aFlags = 0);
    virtual EthCAN_Result Config_Set(EthCAN_Config* aInOut, uint8_t aFlags = 0);
    virtual EthCAN_Result Config_Store(uint8_t aFlags = 0);

    virtual EthCAN_Result Device_Reset(uint8_t aFlags = 0);

    virtual uint32_t      GetHostAddress() const;
    virtual EthCAN_Result GetInfo(EthCAN_Info* aOut);
    virtual EthCAN_Result GetInfoLine(char* aOut, unsigned int aSize_byte) const;

    virtual bool IsConnectedEth() const;
    virtual bool IsConnectedUSB() const;

    virtual ProtocolId    Protocol_Get() const;
    virtual EthCAN_Result Protocol_Reset();
    virtual EthCAN_Result Protocol_Set(ProtocolId aId);

    virtual EthCAN_Result Receiver_Config();
    virtual EthCAN_Result Receiver_Start(Receiver aFunction, void* aContext);
    virtual EthCAN_Result Receiver_Stop();

    virtual EthCAN_Result Send(const EthCAN_Frame& aIn, uint8_t aFlags = 0);

    // ===== EthCAN::Object =================================================
    virtual void Debug(FILE* aOut = NULL) const;

    // ===== IMessageReceived ===============================================
    virtual bool OnMessage(void * aSource, unsigned int aMessage, const void* aData, unsigned int aSize_byte);

protected:

    // ===== EthCAN::Object =================================================
    virtual ~Device_Impl();

private:

    void Busy_Mark_Z0();
    void Busy_Wait();

    void Config_Verify(const EthCAN_Config& aIn);

    bool OnData(const void* aData, unsigned int aSize_byte);
    bool OnLoopIteration();
    bool OnRequest (const EthCAN_Header* aHeader, unsigned int aSize_byte);
    bool OnResponse(const EthCAN_Header* aHeader, unsigned int aSize_byte);

    void Protocol_Create();
    void Protocol_Delete();

    unsigned int Request(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte, void* aOut, unsigned int aOutSize_byte);

    void Request_End();
    void Request_Init(EthCAN_Header * aHeader, uint8_t aCode, uint8_t aFlags, unsigned int aDataSize_byte);
    void Request_Send(uint8_t aCode, uint8_t aFlags, const void* aIn, unsigned int aInSize_byte);

    void Thread_Create();
    void Thread_Delete();

    void UDP_Beep();
    void UDP_Receive();
    void UDP_Server_Delete();

    uint64_t mBusyUntil_ms;

    void* mContext;

    uint32_t mId_Client;
    uint32_t mId_Server;

    EthCAN_Info mInfo;

    bool mIsConnectedEth;

    unsigned int mLostCount;

    Protocol* mProtocol;

    ProtocolId mProtocolId;

    Receiver mReceiver;

    Thread* mThread;

    Socket* mUDP_Server;

    Serial* mUSB_Serial;

    // ======================================================================
    Gate mZone0;

    uint8_t       mReq_Code;
    void*         mReq_Out;
    unsigned int  mReq_OutSize_byte;
    EthCAN_Result mReq_Result;

};
