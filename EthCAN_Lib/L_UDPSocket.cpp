
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/L_UDPSocket.cpp

#include "Component.h"

// ===== C ==================================================================
#include <errno.h>
#include <ifaddrs.h>

// ===== EthCAN_Lib =========================================================
#include "UDPSocket.h"

// Public
/////////////////////////////////////////////////////////////////////////////

uint32_t UDPSocket::GetIPv4(uint32_t aAddress, uint32_t aNetMask)
{
    assert(0 != aAddress);
    assert(0 != aNetMask);

    struct ifaddrs * lAddrs;

    int lRet = getifaddrs(&lAddrs);
    if (0 != lRet)
    {
        throw EthCAN_ERROR_NETWORK;
    }

    assert(NULL != lAddrs);

    uint32_t lResult = 0;

    const struct ifaddrs * lCurrent = lAddrs;
    while (NULL != lCurrent)
    {
        if (AF_INET == lCurrent->ifa_addr->sa_family)
        {
            const sockaddr_in * lAddress = reinterpret_cast<const sockaddr_in *>(lCurrent->ifa_addr);
            const sockaddr_in * lNetMask = reinterpret_cast<const sockaddr_in *>(lCurrent->ifa_netmask);

            if ((lAddress->sin_addr.s_addr & lNetMask->sin_addr.s_addr) == (aAddress & aNetMask))
            {
                lResult = lAddress->sin_addr.s_addr;
                break;
            }
        }
    }

    if (NULL == lCurrent)
    {
        const sockaddr_in * lAddress = reinterpret_cast<const sockaddr_in *>(lCurrent->ifa_addr);
        lResult = lAddress->sin_addr.s_addr;
    }

    freeifaddrs(lAddrs);

    return lResult;
}

void UDPSocket::Thread_Init()
{
}

void UDPSocket::Thread_Uninit()
{
}

// Private
/////////////////////////////////////////////////////////////////////////////

void UDPSocket::Broadcast_Enable()
{
    int lVal = 1;

    if (0 != setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, &lVal, sizeof(lVal)))
    {
        throw EthCAN_ERROR_SOCKET_OPTION;
    }
}

void UDPSocket::Close()
{
    assert(INVALID_SOCKET != mSocket);

    int lRet = close(mSocket);
    assert(0 == lRet);
}

void UDPSocket::Init()
{
    assert(INVALID_SOCKET == mSocket);

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == mSocket)
    {
        fprintf(stderr, "ERROR  UDPSocket::Init - EthCAN_ERROR_SOCKET\n");
        throw EthCAN_ERROR_SOCKET;
    }

    sockaddr_in lAddr;

    memset(&lAddr, 0, sizeof(lAddr));

    lAddr.sin_family = AF_INET;

    if (0 != bind(mSocket, reinterpret_cast<sockaddr *>(&lAddr), sizeof(lAddr)))
    {
        Close();

        throw EthCAN_ERROR_SOCKET_BIND;
    }

    socklen_t lSize_byte = sizeof(lAddr);

    if (   (0 != getsockname(mSocket, reinterpret_cast<sockaddr *>(&lAddr), &lSize_byte))
        || (sizeof(lAddr) != lSize_byte))
    {
        Close();

        throw EthCAN_ERROR_SOCKET_NAME;
    }

    assert(0 != lAddr.sin_port);

    mPort = lAddr.sin_port;
}

void UDPSocket::Timeout_Set(unsigned int aTimeout_ms)
{
    assert(INVALID_SOCKET != mSocket);

    struct timeval lTimeout;

    lTimeout.tv_sec = aTimeout_ms / 1000;
    lTimeout.tv_usec = aTimeout_ms % 1000 * 1000;

    if (0 != setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, &lTimeout, sizeof(lTimeout)))
    {
        throw EthCAN_ERROR_SOCKET_OPTION;
    }

    mTimeout_ms = aTimeout_ms;
}

bool UDPSocket::Timeout_Verify()
{
    return EAGAIN == errno;
}

void UDPSocket::Uninit()
{
    Close();
}
