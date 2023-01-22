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


Abstract: This is a stub class definition of CModbusTCPConnection

*/

#include "libmcenv_modbustcpconnection.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmcenv_modbustcpinstance.hpp"
#include "libmcenv_modbustcpdigitaliostatus.hpp"
#include "libmcenv_modbustcpregisterstatus.hpp"


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CModbusTCPConnection 
**************************************************************************************************************************/

CModbusTCPConnection::CModbusTCPConnection(const std::string& sIPAddress, uint32_t nPort, uint32_t nTimeOutInMS)
{
	m_pModbusConnectionInstance = std::make_shared<CModbusTCPConnectionInstance>(sIPAddress, nPort, nTimeOutInMS);
}

CModbusTCPConnection::~CModbusTCPConnection()
{

}

std::string CModbusTCPConnection::GetIPAddress()
{
	return m_pModbusConnectionInstance->getIPAddress();
}

LibMCEnv_uint32 CModbusTCPConnection::GetPort()
{
	return m_pModbusConnectionInstance->getPort();
}

LibMCEnv_uint32 CModbusTCPConnection::GetTimeout()
{
	return m_pModbusConnectionInstance->getTimeoutInMs();
}

bool CModbusTCPConnection::IsConnected()
{
	return m_pModbusConnectionInstance->isConnected ();
}

void CModbusTCPConnection::Disconnect()
{
	m_pModbusConnectionInstance->disconnect();
}

void CModbusTCPConnection::Reconnect()
{
	m_pModbusConnectionInstance->reconnect();
}

LibMCEnv_uint16 CModbusTCPConnection::DiagnosisCall(const LibMCEnv_uint16 nSubFunction, const LibMCEnv_uint16 nDataField)
{
	uint16_t nDataResponse = 0;
	m_pModbusConnectionInstance->diagnosisCall(nSubFunction, nDataField, nDataResponse);
	return nDataResponse;
}

IModbusTCPDigitalIOStatus* CModbusTCPConnection::ReadCoilStatus(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nBitCount)
{
	auto digitalIOStatus = std::make_unique<CModbusTCPDigitalIOStatus>(nStartAddress);

	m_pModbusConnectionInstance->readCoilStatus(nStartAddress, nBitCount, digitalIOStatus->getInternalData());

	return digitalIOStatus.release();
}

IModbusTCPDigitalIOStatus* CModbusTCPConnection::ReadInputStatus(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nBitCount)
{
	auto digitalIOStatus = std::make_unique<CModbusTCPDigitalIOStatus>(nStartAddress);

	m_pModbusConnectionInstance->readInputStatus(nStartAddress, nBitCount, digitalIOStatus->getInternalData());

	return digitalIOStatus.release();

}

IModbusTCPRegisterStatus* CModbusTCPConnection::ReadHoldingRegisters(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nRegisterCount)
{
	auto registerStatus = std::make_unique<CModbusTCPRegisterStatus>(nStartAddress);

	m_pModbusConnectionInstance->readHoldingRegisters(nStartAddress, nRegisterCount, registerStatus->getInternalData());

	return registerStatus.release();
}

IModbusTCPRegisterStatus* CModbusTCPConnection::ReadInputRegisters(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nRegisterCount)
{
	auto registerStatus = std::make_unique<CModbusTCPRegisterStatus>(nStartAddress);

	m_pModbusConnectionInstance->readInputRegisters(nStartAddress, nRegisterCount, registerStatus->getInternalData());

	return registerStatus.release();

}



void CModbusTCPConnection::ForceMultipleCoils(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint64 nBufferBufferSize, const LibMCEnv_uint8 * pBufferBuffer)
{
	std::vector<bool> coilStatus;
	if ((nBufferBufferSize == 0) || (pBufferBuffer == 0))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPCOILCOUNT);

	if (nBufferBufferSize > MODBUSTCP_MAX_COILCOUNT)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPCOILCOUNT);

	coilStatus.resize(nBufferBufferSize);
	for (uint32_t nIndex = 0; nIndex < nBufferBufferSize; nIndex)
		coilStatus.at(nIndex) = (pBufferBuffer[nIndex] != 0);

	m_pModbusConnectionInstance->forceMultipleCoils(nStartAddress, coilStatus);
}

void CModbusTCPConnection::PresetMultipleRegisters(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint64 nBufferBufferSize, const LibMCEnv_uint16 * pBufferBuffer)
{
	std::vector<uint16_t> registerStatus;
	if ((nBufferBufferSize == 0) || (pBufferBuffer == 0))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPREGISTERCOUNT);

	if (nBufferBufferSize > MODBUSTCP_MAX_REGISTERCOUNT)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSTCPREGISTERCOUNT);

	registerStatus.resize(nBufferBufferSize);
	for (uint32_t nIndex = 0; nIndex < nBufferBufferSize; nIndex)
		registerStatus.at(nIndex) = pBufferBuffer[nIndex];

	m_pModbusConnectionInstance->presetMultipleRegisters(nStartAddress, registerStatus);

}

