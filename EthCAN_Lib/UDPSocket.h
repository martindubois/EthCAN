
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/UDPSocket.h

#pragma once

class UDPSocket
{

public:

    UDPSocket();

    ~UDPSocket();

    void Broadcast(const void* aData, unsigned int aSize_byte);

    uint32_t GetIPv4(uint32_t aAddress, uint32_t aNetMask) const;

    uint16_t GetPort() const;

    unsigned int Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom = NULL);

    void Send(const void* aData, unsigned int aSize_byte, uint32_t aTo);

private:

    void Broadcast_Enable(); // L and W_UDPSocket.cpp

    void Close(); // L and W_UDPSocket.cpp

    void Init(); // L and W_UDPSocket.cpp

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
