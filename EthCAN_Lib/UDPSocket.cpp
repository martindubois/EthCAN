
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/UDPSocket.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Includes ===========================================================
extern "C"
{
    #include <EthCAN_Protocol.h>
}

// ===== EthCAN_Lib =========================================================
#include "UDPSocket.h"

// Macros
/////////////////////////////////////////////////////////////////////////

#if defined(_KMS_LINUX_) || defined(_KMS_OS_X_)
    #define FIELD_ADDR_32 s_addr
#endif

#ifdef _KMS_WINDOWS_

    typedef int socklen_t;

    #define FIELD_ADDR_32 S_un.S_addr

#endif

// Public
/////////////////////////////////////////////////////////////////////////////

UDPSocket::UDPSocket() : mSocket(INVALID_SOCKET), mTimeout_ms(0)
{
    mFlags.mBroadcast_Enabled = false;

    Init();
}

UDPSocket::~UDPSocket()
{
    Uninit();
}

void UDPSocket::Broadcast(const void* aData, unsigned int aSize_byte)
{
    assert(NULL != aData);
    assert(0 < aSize_byte);

    if (!mFlags.mBroadcast_Enabled)
    {
        Broadcast_Enable();
    }

    Send(aData, aSize_byte, INADDR_BROADCAST);
}

uint16_t UDPSocket::GetPort() const
{
    assert(0 != mPort);

    return mPort;
}

unsigned int UDPSocket::Receive(void* aData, unsigned int aSize_byte, unsigned int aTimeout_ms, uint32_t* aFrom)
{
    assert(NULL != aData);
    assert(0 < aSize_byte);

    assert(INVALID_SOCKET != mSocket);

    if (mTimeout_ms != aTimeout_ms)
    {
        Timeout_Set(aTimeout_ms);
    }

    unsigned int lResult_byte;

    for (;;)
    {
        sockaddr_in lFrom;

        socklen_t lFromSize_byte = sizeof(lFrom);

        int lRet = recvfrom(mSocket, reinterpret_cast<char*>(aData), aSize_byte, 0, reinterpret_cast<sockaddr*>(&lFrom), &lFromSize_byte);
        if (0 == lRet)
        {
            TRACE_ERROR(stderr, "UDPSocket::Receive - EthCAN_ERROR_SOCKET_RECEIVE");
            throw EthCAN_ERROR_SOCKET_RECEIVE;
        }

        if (SOCKET_ERROR == lRet)
        {
            if (!Timeout_Verify())
            {
                TRACE_ERROR(stderr, "UDPSocket::Receive - EthCAN_ERROR_SOCKET");
                throw EthCAN_ERROR_SOCKET;
            }

            lResult_byte = 0;
            break;
        }

        assert(0 < lRet);

        lResult_byte = lRet;

        assert(aSize_byte >= lResult_byte);

        if (EthCAN_UDP_PORT == ntohs(lFrom.sin_port))
        {
            if (NULL != aFrom)
            {
                *aFrom = lFrom.sin_addr.FIELD_ADDR_32;
            }

            break;
        }

        // QUESTION  UDP.Retry
        //           Is the retry loop needed ?
        TRACE_WARNING(stderr, "WARNING  UDPSocket::Receive - Retry");
    }

    return lResult_byte;
}

void UDPSocket::Send(const void* aData, unsigned int aSize_byte, uint32_t aTo)
{
    assert(NULL != aData);
    assert(0 < aSize_byte);
    assert(0 != aTo);

    assert(INVALID_SOCKET != mSocket);

    sockaddr_in lTo;

    memset(&lTo, 0, sizeof(lTo));

    lTo.sin_addr.FIELD_ADDR_32 = aTo;
    lTo.sin_family             = AF_INET;
    lTo.sin_port               = htons(EthCAN_UDP_PORT);

    int lRet = sendto(mSocket, reinterpret_cast<const char*>(aData), aSize_byte, 0, reinterpret_cast<sockaddr*>(&lTo), sizeof(lTo));
    if (aSize_byte != lRet)
    {
        TRACE_ERROR(stderr, "UDPSocket::Send - EthCAN_ERROR_SOCKET_SEND");
        throw EthCAN_ERROR_SOCKET_SEND;
    }
}
