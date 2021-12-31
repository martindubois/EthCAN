
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/Socket.h

#pragma once

#if defined(_KMS_LINUX_) || defined(_KMS_OS_X_)

    // Constants
    /////////////////////////////////////////////////////////////////////////

    #define INVALID_SOCKET (-1)

    #define SOCKET_ERROR (-1)

    // Data type
    /////////////////////////////////////////////////////////////////////////

    typedef int SOCKET;

#endif

class Socket
{

public:

    static uint32_t GetIPv4(uint32_t aAddress, uint32_t aNetMask); // L and W_UDPSocket.cpp

    static void Thread_Init();
    static void Thread_Uninit();

    Socket();
    Socket(uint32_t aIPv4);

    ~Socket();

    void Broadcast(const void* aData, unsigned int aSize_byte);

    uint16_t GetPort() const;

    unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms);
    unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom);

    void Send(const void* aData, unsigned int aSize_byte);
    void Send(const void* aData, unsigned int aSize_byte, uint32_t aTo);

private:

    void Broadcast_Enable(); // L and W_UDPSocket.cpp

    void Close(); // L and W_UDPSocket.cpp

    void Init(int aType, int aProtocol); // L and W_UDPSocket.cpp

    void Timeout_Set(unsigned int aTimeout_ms); // L and W_UDPSocket.cpp

    bool Timeout_Verify(); // L and W_UDPSocket.cpp

    void Uninit(); // L and W_UDPSocket.cpp

    struct
    {
        unsigned mBroadcast_Enabled : 1;

        unsigned mReserved0 : 31;
    }
    mFlags;

    uint16_t mPort;

    SOCKET mSocket;

    unsigned int mTimeout_ms;

};
