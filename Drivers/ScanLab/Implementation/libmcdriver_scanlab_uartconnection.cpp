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
#include <chrono>

using namespace LibMCDriver_ScanLab::Impl;

/*************************************************************************************************************************
 Class definition of CUARTConnection 
**************************************************************************************************************************/

CUARTConnection::CUARTConnection(PScanLabSDK pScanlabSDK, uint32_t nDesiredBaudRate, uint32_t nCardNo, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pScanlabSDK (pScanlabSDK), m_nDesiredBaudRate (nDesiredBaudRate), m_nCardNo (nCardNo), m_pDriverEnvironment (pDriverEnvironment)
{
	if (pScanlabSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);
	if (pDriverEnvironment.get () == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	m_pScanlabSDK->checkGlobalErrorOfCard(m_nCardNo);
	m_nActualBaudRate = m_pScanlabSDK->n_uart_config (m_nCardNo, nDesiredBaudRate);
	m_pScanlabSDK->checkLastErrorOfCard(m_nCardNo);

	ClearReceiveBuffer();
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
	m_ReceiveBuffer.clear();

	for (uint32_t nReadIndex = 0; nReadIndex < 512; nReadIndex++) {
		uint32_t nRawValue = m_pScanlabSDK->n_rs232_read_data(m_nCardNo);
		uint32_t nNumberOfNotReadCharacters = (nRawValue >> 16) & 0xff;
		uint32_t nNumberOfBufferOverruns = (nRawValue >> 24) & 0xff;
		if ((nNumberOfBufferOverruns == 0) && (nNumberOfNotReadCharacters == 0))
			break;
	}

}

LibMCDriver_ScanLab_uint32 CUARTConnection::AvailableBytes()
{
	pollRTCReceiveBuffer();
	return (uint32_t) m_ReceiveBuffer.size ();
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
	if (pDataNeededCount != nullptr)
		*pDataNeededCount = nByteCount;

	if ((nByteCount > 0) && (pDataBuffer)) {
		pollRTCReceiveBuffer();

		auto startTime = std::chrono::high_resolution_clock::now();

		while (m_ReceiveBuffer.size () < nByteCount) {
			pollRTCReceiveBuffer();

			auto duration = std::chrono::high_resolution_clock::now() - startTime;
			auto millseconds = std::chrono::duration_cast<std::chrono::milliseconds> (duration);
			if (millseconds.count() > nTimeOutInMS)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RS232READTIMEOUT);

			if (m_ReceiveBuffer.size() < nByteCount)
				m_pDriverEnvironment->Sleep(1);
		}

		std::vector<uint8_t> newBuffer;
		for (size_t nIndex = 0; nIndex < nByteCount; nIndex++)
			pDataBuffer[nIndex] = m_ReceiveBuffer.at(nIndex);

		newBuffer.reserve(m_ReceiveBuffer.size () - nByteCount);
		for (size_t nIndex = nByteCount; nIndex < m_ReceiveBuffer.size(); nIndex++)
			newBuffer.push_back(m_ReceiveBuffer.at(nIndex));

		m_ReceiveBuffer = newBuffer;

	}

}

std::string CUARTConnection::ReadLine(const std::string& sSeparator, const LibMCDriver_ScanLab_uint32 nMaxLineLength, const LibMCDriver_ScanLab_uint32 nTimeOutInMS)
{
	std::string sLine;

	if (sSeparator.empty ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	auto startTime = std::chrono::high_resolution_clock::now();
	size_t nMaxReceiveSize = nMaxLineLength + sSeparator.length();

	bool bIsFinished = false;
	while (!bIsFinished) {
		pollRTCReceiveBuffer();

		// Check for MaxLineLength!
		if (m_ReceiveBuffer.size () > nMaxReceiveSize)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RS232READLINEOVERRUN);

		// Check for separator
		if (m_ReceiveBuffer.size() >= sSeparator.length()) {

			bool bSeparatorMatches = true;
			size_t nStartCharacter = m_ReceiveBuffer.size() - sSeparator.length();
			for (size_t nIndex = 0; nIndex < sSeparator.length(); nIndex++) {
				if (m_ReceiveBuffer.at(nStartCharacter + nIndex) != sSeparator.at(nIndex)) {
					bSeparatorMatches = false;
					break;
				}
			}

			if (bSeparatorMatches) {
				bIsFinished = true;
				m_ReceiveBuffer.at(nStartCharacter) = 0;
				sLine = (char*)m_ReceiveBuffer.data();
			}
		}

		// Check for timeout!
		auto duration = std::chrono::high_resolution_clock::now() - startTime;
		auto millseconds = std::chrono::duration_cast<std::chrono::milliseconds> (duration);
		if (millseconds.count() > nTimeOutInMS)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RS232READTIMEOUT);

		if (!bIsFinished)
			m_pDriverEnvironment->Sleep(1);

	}

	m_ReceiveBuffer.clear();

	return sLine;
}


void CUARTConnection::pollRTCReceiveBuffer()
{
	m_pScanlabSDK->checkGlobalErrorOfCard(m_nCardNo);

	for (uint32_t nReadIndex = 0; nReadIndex < 256; nReadIndex++) {

		// See Scanlab SDK Documentation for details about RS232 Reading (Chapter 9.2.3)...
		uint32_t nRawValue = m_pScanlabSDK->n_rs232_read_data(m_nCardNo);
		uint8_t nDataValue = nRawValue & 0xff;
		bool bIsNew = (nRawValue & 0x0100) != 0;
		uint32_t nNumberOfNotReadCharacters = (nRawValue >> 16) & 0xff;

		uint32_t nNumberOfBufferOverruns = (nRawValue >> 24) & 0xff;
		if (nNumberOfBufferOverruns > 0)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RS232RINGBUFFEROVERRUN);

		if (!bIsNew)
			break;

		m_ReceiveBuffer.push_back(nDataValue);

		if (nNumberOfNotReadCharacters == 0)
			break;
	}

}
