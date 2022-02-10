/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Autodesk Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else

#define INVALID_SOCKET 0

#endif //_WIN32

#include <stdlib.h>
#include <stdio.h>

#include "libmcdriver_tcpip_sockets.hpp"
#include "libmcdriver_tcpip_interfaceexception.hpp"
#include <iostream>

using namespace LibMCDriver_TCPIP::Impl;

#define TCPIPSOCKET_MAXRECEIVECOUNT 1024 * 1024 * 256
#define TCPIPSOCKET_MAXSENDCOUNT 1024 * 1024 * 256

CDriver_TCPIPSocketConnection::CDriver_TCPIPSocketConnection(const std::string& sIPAddress, uint32_t nPort)
    : m_Socket (INVALID_SOCKET)
{
#ifdef _WIN32
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; 

    std::string sPort = std::to_string(nPort);

    iResult = getaddrinfo(sIPAddress.c_str(), sPort.c_str(), &hints, &result);
    if (iResult != 0) {
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_COULDNOTGETADDRESSINFO, "could not get address info: " + std::to_string (iResult));
    }

    if (result == nullptr)
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_COULDNOTGETADDRESSINFO, "invalid address info: " + sIPAddress + ":" + std::to_string (nPort));

    SOCKET ConnectSocket = INVALID_SOCKET;

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            freeaddrinfo(result);
            throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_COULDNOTCREATESOCKET, "socket failed with error: " + std::to_string(WSAGetLastError()));
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_COULDNOTCONNECT, "could not connect to " + sIPAddress + ":" + sPort);

    m_Socket = ConnectSocket;
#else
    throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_NOTIMPLEMENTED);
#endif //_WIN32


}

CDriver_TCPIPSocketConnection::~CDriver_TCPIPSocketConnection()
{
    disconnect();
}


void CDriver_TCPIPSocketConnection::sendBuffer(const uint8_t* pBuffer, size_t nCount)
{
    if (nCount > TCPIPSOCKET_MAXSENDCOUNT)
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_SENDCOUNTEXCEEDSMAXIMUM);

    if (nCount > 0) {
        if (m_Socket == INVALID_SOCKET)
            throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_DRIVERNOTCONNECTED);

        if (pBuffer == nullptr)
            throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_INVALIDPARAM);

#ifdef _WIN32

        int iResult = send(m_Socket, (const char*)pBuffer, (int) nCount, 0);
        if (iResult == SOCKET_ERROR) {
            disconnect();
            throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_SENDERROR, "could not send data to socket: " + std::to_string(WSAGetLastError()));
        } 
#else
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_NOTIMPLEMENTED);
#endif //_WIN32

    }
}


bool CDriver_TCPIPSocketConnection::waitForData(uint32_t timeOutInMS)
{
    if (m_Socket == INVALID_SOCKET)
        return false;

#ifdef _WIN32
    struct timeval timeout;
    struct fd_set fds;

    timeout.tv_sec = (timeOutInMS / 1000);
    timeout.tv_usec = (timeOutInMS % 1000) * 1000;

    FD_ZERO(&fds);
    FD_SET(m_Socket, &fds);

    int selectionResult = select (0, &fds, 0, 0, &timeout);

    return selectionResult > 0;
#else
    return false;
#endif //_WIN32

}


void CDriver_TCPIPSocketConnection::receiveBuffer(std::vector<uint8_t>& Buffer, size_t nCount, bool bMustReceiveAll)
{
    if (nCount > TCPIPSOCKET_MAXRECEIVECOUNT)
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_RECEIVECOUNTEXCEEDSMAXIMUM);

    if (nCount > 0) {
        if (m_Socket == INVALID_SOCKET)
            throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_DRIVERNOTCONNECTED);

        size_t oldSize = Buffer.size();
        Buffer.resize(oldSize + nCount);

        uint32_t totalBytesReceived = 0;

#ifdef _WIN32
        while (totalBytesReceived < nCount) {

            uint8_t* pData = &Buffer[oldSize + totalBytesReceived];
            int bytesReceived = recv(m_Socket, (char*)pData, (int)nCount, 0);

            if (bytesReceived == 0) {
                disconnect();
                throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_CONNECTIONCLOSED);
            }

            if (bytesReceived < 0)
                throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_RECEIVEERROR, "socket receive error: " + std::to_string(WSAGetLastError()));

            totalBytesReceived += bytesReceived;
            if (!bMustReceiveAll)
                break;

        }

        if (totalBytesReceived != nCount)
            Buffer.resize(oldSize + nCount);
#else
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_NOTIMPLEMENTED);
#endif // _WIN32
    }
}


void CDriver_TCPIPSocketConnection::disconnect()
{
#ifdef _WIN32
    if (m_Socket != INVALID_SOCKET) {
        closesocket(m_Socket);
    }
#endif //_WIN32

    m_Socket = INVALID_SOCKET;
}

bool CDriver_TCPIPSocketConnection::isConnected()
{
    return (m_Socket != INVALID_SOCKET);
}

void CDriver_TCPIPSocketConnection::initializeNetworking()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif //_WIN32

}

