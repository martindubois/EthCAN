
// Author    KMS - Martin Dubois, P,Eng.
// Copyright (C) 2021 KMS
// Product   EthCAN
// File      EthCAN_Lib/W_UDPSocket.cpp

#include "Component.h"

// ===== Windows ============================================================
#include <iphlpapi.h>

// ===== EthCAN_Lib =========================================================
#include "UDPSocket.h"

// Static variables
/////////////////////////////////////////////////////////////////////////////

static unsigned int sInstanceCount = 0;

// Static function declarations
/////////////////////////////////////////////////////////////////////////////

static void Thread_Init();
static void Thread_Uninit();

// Public
/////////////////////////////////////////////////////////////////////////////

uint32_t UDPSocket::GetIPv4(uint32_t aAddress, uint32_t aNetMask) const
{
    assert(0 != aAddress);
    assert(0 != aNetMask);

    uint8_t lBuffer[4096];
    ULONG lSize_byte = sizeof(lBuffer);
    MIB_IPADDRTABLE* lTable = reinterpret_cast<MIB_IPADDRTABLE *>(lBuffer);

    if ((ERROR_SUCCESS != GetIpAddrTable(lTable, &lSize_byte, FALSE)) || (0 >= lTable->dwNumEntries))
    {
        throw EthCAN_ERROR_NETWORK;
    }

    for (unsigned int i = 0; i < lTable->dwNumEntries; i++)
    {
        if ((lTable->table[i].dwAddr & lTable->table[i].dwMask) == (aAddress & aNetMask))
        {
            return lTable->table[i].dwAddr;
        }
    }

    return lTable->table[0].dwAddr;
}

// Private
/////////////////////////////////////////////////////////////////////////////

void UDPSocket::Broadcast_Enable()
{
    BOOL lBool = true;

    if (0 != setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&lBool), sizeof(lBool)))
    {
        throw EthCAN_ERROR_SOCKET_OPTION;
    }
}

void UDPSocket::Close()
{
    assert(INVALID_SOCKET != mSocket);

    int lRet = closesocket(mSocket);
    assert(0 == lRet);
}

void UDPSocket::Init()
{
    assert(INVALID_SOCKET == mSocket);

    sInstanceCount++;
    if (1 == sInstanceCount)
    {
        Thread_Init();
    }

    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == mSocket)
    {
        throw EthCAN_ERROR_INVALID_SOCKET;
    }

    sockaddr_in lAddr;

    memset(&lAddr, 0, sizeof(lAddr));

    lAddr.sin_family = AF_INET;

    if (0 != bind(mSocket, reinterpret_cast<sockaddr *>(&lAddr), sizeof(lAddr)))
    {
        Close();

        throw EthCAN_ERROR_SOCKET_BIND;
    }

    int lSize_byte = sizeof(lAddr);

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

    DWORD lTimeout_ms = aTimeout_ms;

    if (0 != setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&lTimeout_ms), sizeof(lTimeout_ms)))
    {
        throw EthCAN_ERROR_SOCKET_OPTION;
    }

    mTimeout_ms = aTimeout_ms;
}

bool UDPSocket::Timeout_Verify()
{
    switch (WSAGetLastError())
    {
    case WSAETIMEDOUT: return true;
    }

    return false;
}

void UDPSocket::Uninit()
{
    Close();

    sInstanceCount--;
    if (0 == sInstanceCount)
    {
        Thread_Uninit();
    }
}

// Static functions
/////////////////////////////////////////////////////////////////////////////

void Thread_Init()
{
    WORD    lVersion = MAKEWORD(2, 2);
    WSADATA lData;

    int lRet = WSAStartup(lVersion, &lData);
    assert(0 == lRet);
}

void Thread_Uninit()
{
    int lRet = WSACleanup();
    assert(0 == lRet);
}
