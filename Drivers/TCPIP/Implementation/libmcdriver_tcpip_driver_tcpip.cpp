/*++

Copyright (C) 2022 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_TCPIP

*/

#include "libmcdriver_tcpip_driver_tcpip.hpp"
#include "libmcdriver_tcpip_driver_tcpippacket.hpp"
#include "libmcdriver_tcpip_interfaceexception.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

#define TCPIPDRIVER_MAXSENDCOUNT (1024UL * 1024UL * 1024UL)

using namespace LibMCDriver_TCPIP::Impl;


/*************************************************************************************************************************
 Class definition of CDriver_TCPIP 
**************************************************************************************************************************/
CDriver_TCPIP::CDriver_TCPIP(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName(sName), m_pDriverEnvironment(pDriverEnvironment), m_bSimulationMode(false)
{

}

CDriver_TCPIP::~CDriver_TCPIP()
{

}

void CDriver_TCPIP::Configure(const std::string& sConfigurationString)
{
	CDriver_TCPIPSocketConnection::initializeNetworking();
}

std::string CDriver_TCPIP::GetName()
{
	return m_sName;
}

std::string CDriver_TCPIP::GetType()
{
	return "tcpip";
}

void CDriver_TCPIP::GetVersion(LibMCDriver_TCPIP_uint32& nMajor, LibMCDriver_TCPIP_uint32& nMinor, LibMCDriver_TCPIP_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_TCPIP_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_TCPIP_VERSION_MINOR;
	nMicro = LIBMCDRIVER_TCPIP_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_TCPIP::QueryParameters()
{
	QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession ());
}

void CDriver_TCPIP::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
	if (pDriverUpdateInstance.get() == nullptr)
		return;
}



void CDriver_TCPIP::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_TCPIP::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_TCPIP::Connect(const std::string& sIPAddress, const LibMCDriver_TCPIP_uint32 nPort, const LibMCDriver_TCPIP_uint32 nTimeout)
{
	m_pSocketConnection = nullptr;

	m_pSocketConnection = std::make_shared< CDriver_TCPIPSocketConnection>(sIPAddress, nPort);
}

void CDriver_TCPIP::Disconnect()
{
	if (m_pSocketConnection)
		m_pSocketConnection->disconnect();
	m_pSocketConnection = nullptr;

}

void CDriver_TCPIP::SendBuffer(const LibMCDriver_TCPIP_uint64 nBufferBufferSize, const LibMCDriver_TCPIP_uint8* pBufferBuffer)
{

	if (m_pSocketConnection.get () == nullptr)
		throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_DRIVERNOTCONNECTED);

	if ((nBufferBufferSize > 0) && (pBufferBuffer != nullptr)) {
		if (nBufferBufferSize > TCPIPDRIVER_MAXSENDCOUNT)
			throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_SENDCOUNTEXCEEDSMAXIMUM);

		m_pSocketConnection->sendBuffer(pBufferBuffer, (size_t) nBufferBufferSize);
	}

}


bool CDriver_TCPIP::WaitForData(const LibMCDriver_TCPIP_uint32 nTimeOutInMS)
{
	if (m_pSocketConnection.get() == nullptr)
		throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_DRIVERNOTCONNECTED);

	return m_pSocketConnection->waitForData(nTimeOutInMS);
}


IDriver_TCPIPPacket* CDriver_TCPIP::ReceivePacket(const LibMCDriver_TCPIP_uint32 nPacketSize, const LibMCDriver_TCPIP_uint32 nTimeOutInMS) 
{
	if (m_pSocketConnection.get() == nullptr)
		throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_DRIVERNOTCONNECTED);

	std::unique_ptr<CDriver_TCPIPPacket> pPacket(new CDriver_TCPIPPacket());
	m_pSocketConnection->receiveBuffer(pPacket->getBufferDataReference (), nPacketSize, true);

	return pPacket.release();

}

bool CDriver_TCPIP::IsConnected()
{
	if (m_pSocketConnection.get() != nullptr) {
		return m_pSocketConnection->isConnected();
	}

	return false;
}
