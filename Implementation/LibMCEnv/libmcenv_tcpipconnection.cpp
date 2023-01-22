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


Abstract: This is a stub class definition of CTCPIPConnection

*/

#include "libmcenv_tcpipconnection.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_tcpippacket.hpp"


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CTCPIPConnection 
**************************************************************************************************************************/

CTCPIPConnection::CTCPIPConnection(const std::string& sIPAddress, uint32_t nPort, uint32_t nTimeOutInMS)
	: m_sIPAddress (sIPAddress), m_nPort (nPort), m_nTimeOutInMS (nTimeOutInMS)
{
	CTCPIPSocketConnection::initializeNetworking();
	Reconnect();
}

CTCPIPConnection::~CTCPIPConnection()
{

}

std::string CTCPIPConnection::GetIPAddress()
{
	return m_sIPAddress;
}

LibMCEnv_uint32 CTCPIPConnection::GetPort()
{
	return m_nPort;
}

LibMCEnv_uint32 CTCPIPConnection::GetTimeout()
{
	return m_nTimeOutInMS;
}

bool CTCPIPConnection::IsConnected()
{
	if (m_pSocketConnection.get() != nullptr) {
		return m_pSocketConnection->isConnected();
	}
	else
		return false;
}

void CTCPIPConnection::Disconnect()
{
	if (m_pSocketConnection.get() != nullptr) {
		m_pSocketConnection->disconnect ();
		m_pSocketConnection = nullptr;
	}
}

void CTCPIPConnection::Reconnect()
{
	Disconnect();

	m_pSocketConnection = std::make_shared<CTCPIPSocketConnection> (m_sIPAddress, m_nPort);
}

void CTCPIPConnection::SendBuffer(const LibMCEnv_uint64 nBufferBufferSize, const LibMCEnv_uint8* pBufferBuffer)
{
	if (m_pSocketConnection.get() == nullptr)
		throw ELibMCEnvInterfaceException (LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	if (!m_pSocketConnection->isConnected())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	if ((nBufferBufferSize == 0) || (pBufferBuffer == nullptr))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSENDBUFFER);
	

	m_pSocketConnection->sendBuffer(pBufferBuffer, nBufferBufferSize);

}

bool CTCPIPConnection::WaitForData(const LibMCEnv_uint32 nTimeOutInMS)
{
	if (m_pSocketConnection.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	if (!m_pSocketConnection->isConnected())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	return m_pSocketConnection->waitForData(nTimeOutInMS);
}

ITCPIPPacket * CTCPIPConnection::ReceiveFixedPacket(const LibMCEnv_uint32 nPacketSize, const LibMCEnv_uint32 nTimeOutInMS)
{
	if (m_pSocketConnection.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	if (!m_pSocketConnection->isConnected())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	auto resultPacket = std::make_unique<CTCPIPPacket> ();
	m_pSocketConnection->receiveBuffer(resultPacket->internalGetData(), nPacketSize, true, nTimeOutInMS);

	return resultPacket.release();
}

ITCPIPPacket * CTCPIPConnection::ReceiveData(const LibMCEnv_uint32 nDataSize, const LibMCEnv_uint32 nTimeOutInMS)
{
	if (m_pSocketConnection.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	if (!m_pSocketConnection->isConnected())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SOCKETNOTCONNECTED);

	auto resultPacket = std::make_unique<CTCPIPPacket>();
	m_pSocketConnection->receiveBuffer(resultPacket->internalGetData(), nDataSize, false, nTimeOutInMS);

	return resultPacket.release();

}

