
// Author    KMS - Martin Dubois, P. Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_UDPSocket.cpp

// TEST COVERAGE 2021-03-10 KMS - Martin Dubois, P.Eng.

#include "Component.h"

// ===== Windows ============================================================
#include <iphlpapi.h>

// ===== EthCAN_Lib =========================================================
#include "Socket.h"

// Public
/////////////////////////////////////////////////////////////////////////////

uint32_t Socket::GetIPv4(uint32_t aAddress, uint32_t aNetMask)
{
    assert(0 != aAddress);
    assert(0 != aNetMask);

    uint8_t lBuffer[4096];
    ULONG lSize_byte = sizeof(lBuffer);
    MIB_IPADDRTABLE* lTable = reinterpret_cast<MIB_IPADDRTABLE *>(lBuffer);

    if ((ERROR_SUCCESS != GetIpAddrTable(lTable, &lSize_byte, FALSE)) || (0 >= lTable->dwNumEntries))
    {
        TRACE_ERROR(stderr, "Socket::GetIPv4 - EthCAN_ERROR_NETWORK");
        throw EthCAN_ERROR_NETWORK;
    }

    for (unsigned int i = 0; i < lTable->dwNumEntries; i++)
    {
        if ((lTable->table[i].dwAddr & lTable->table[i].dwMask) == (aAddress & aNetMask))
        {
            return lTable->table[i].dwAddr;
        }
    }

    TRACE_WARNING(stderr, "Socket::GetIPv4 - Using default address");
    return lTable->table[0].dwAddr;
}

void Socket::Thread_Init()
{
    WORD    lVersion = MAKEWORD(2, 2);
    WSADATA lData;

    int lRet = WSAStartup(lVersion, &lData);
    assert(0 == lRet);
}

void Socket::Thread_Uninit()
{
    int lRet = WSACleanup();
    assert(0 == lRet);
}

// Private
/////////////////////////////////////////////////////////////////////////////

void Socket::Broadcast_Enable()
{
    BOOL lBool = true;

    if (0 != setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&lBool), sizeof(lBool)))
    {
        TRACE_ERROR(stderr, "Socket::Broadcast_Enable - EthCAN_ERROR_SOCKET_OPTION");
        throw EthCAN_ERROR_SOCKET_OPTION;
    }
}

void Socket::Close()
{
    assert(INVALID_SOCKET != mSocket);

    int lRet = closesocket(mSocket);
    assert(0 == lRet);
}

void Socket::Init(int aType, int aProtocol)
{
    assert(INVALID_SOCKET == mSocket);

    Thread_Init();

    mSocket = socket(AF_INET, aType, aProtocol);
    if (INVALID_SOCKET == mSocket)
    {
        TRACE_ERROR(stderr, "Socket::Init - EthCAN_ERROR_SOCKET");
        throw EthCAN_ERROR_SOCKET;
    }

    sockaddr_in lAddr;

    memset(&lAddr, 0, sizeof(lAddr));

    lAddr.sin_family = AF_INET;

    if (0 != bind(mSocket, reinterpret_cast<sockaddr *>(&lAddr), sizeof(lAddr)))
    {
        Close();

        TRACE_ERROR(stderr, "Socket::Init - EthCAN_ERROR_SOCKET_BIND");
        throw EthCAN_ERROR_SOCKET_BIND;
    }

    int lSize_byte = sizeof(lAddr);

    if (   (0 != getsockname(mSocket, reinterpret_cast<sockaddr *>(&lAddr), &lSize_byte))
        || (sizeof(lAddr) != lSize_byte))
    {
        Close();

        TRACE_ERROR(stderr, "Socket::Init - EthCAN_ERROR_NAME");
        throw EthCAN_ERROR_SOCKET_NAME;
    }

    assert(0 != lAddr.sin_port);

    mPort = ntohs(lAddr.sin_port);
}

void Socket::Timeout_Set(unsigned int aTimeout_ms)
{
    assert(INVALID_SOCKET != mSocket);

    DWORD lTimeout_ms = aTimeout_ms;

    if (0 != setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&lTimeout_ms), sizeof(lTimeout_ms)))
    {
        TRACE_ERROR(stderr, "Socket::Timeout_Set - EthCAN_ERROR_OPTION");
        throw EthCAN_ERROR_SOCKET_OPTION;
    }

    mTimeout_ms = aTimeout_ms;
}

bool Socket::Timeout_Verify()
{
    switch (WSAGetLastError())
    {
    case WSAETIMEDOUT: return true;
    }

    TRACE_ERROR(stderr, "Socket::Timeout_Verify - false");
    return false;
}

void Socket::Uninit()
{
    Close();

    Thread_Uninit();
}
