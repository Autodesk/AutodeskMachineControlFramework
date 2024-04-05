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


Abstract: This is a stub class definition of CUARTConnection

*/

#include "libmcdriver_scanlab_uartconnection.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CUARTConnection 
**************************************************************************************************************************/

CUARTConnection::CUARTConnection(PScanLabSDK pScanlabSDK, uint32_t nDesiredBaudRate, uint32_t nCardNo)
	: m_pScanlabSDK (pScanlabSDK), m_nDesiredBaudRate (nDesiredBaudRate), m_nCardNo (nCardNo)
{
	if (pScanlabSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	m_pScanlabSDK->checkGlobalErrorOfCard(m_nCardNo);
	m_nActualBaudRate = m_pScanlabSDK->n_uart_config (m_nCardNo, nDesiredBaudRate);
	m_pScanlabSDK->checkLastErrorOfCard(m_nCardNo);
}

CUARTConnection::~CUARTConnection()
{

}


LibMCDriver_ScanLab_uint32 CUARTConnection::GetBaudRate()
{
	return m_nActualBaudRate;
}

LibMCDriver_ScanLab_uint32 CUARTConnection::GetConfiguredBaudRate()
{
	return m_nDesiredBaudRate;
}

void CUARTConnection::ClearReceiveBuffer()
{
}

LibMCDriver_ScanLab_uint32 CUARTConnection::AvailableBytes()
{
	return 0;
}

void CUARTConnection::WriteString(const std::string & sValue)
{
	m_pScanlabSDK->checkGlobalErrorOfCard(m_nCardNo);

	for (auto ch : sValue) {
		m_pScanlabSDK->n_rs232_write_data(m_nCardNo, ch);
		m_pScanlabSDK->checkLastErrorOfCard(m_nCardNo);
	}
}

void CUARTConnection::WriteData(const LibMCDriver_ScanLab_uint64 nDataBufferSize, const LibMCDriver_ScanLab_uint8 * pDataBuffer)
{
	if (nDataBufferSize > 0) {
		if (pDataBuffer == nullptr)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

		for (size_t nIndex = 0; nIndex < nDataBufferSize; nIndex++) {			
			m_pScanlabSDK->n_rs232_write_data(m_nCardNo, pDataBuffer[nIndex]);
			m_pScanlabSDK->checkLastErrorOfCard(m_nCardNo);
		}
	}
	
}

void CUARTConnection::ReadData(const LibMCDriver_ScanLab_uint32 nByteCount, const LibMCDriver_ScanLab_uint32 nTimeOutInMS, LibMCDriver_ScanLab_uint64 nDataBufferSize, LibMCDriver_ScanLab_uint64* pDataNeededCount, LibMCDriver_ScanLab_uint8 * pDataBuffer)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CUARTConnection::ReadLine(const std::string & sSeparator, const LibMCDriver_ScanLab_uint32 nMaxLineLength, const LibMCDriver_ScanLab_uint32 nTimeOutInMS, LibMCDriver_ScanLab_uint64 nDataBufferSize, LibMCDriver_ScanLab_uint64* pDataNeededCount, LibMCDriver_ScanLab_uint8 * pDataBuffer)
{
	throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NOTIMPLEMENTED);
}

void CUARTConnection::Close()
{
}

