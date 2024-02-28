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


Abstract: This is a stub class definition of CStreamReader

*/

#include "libmcenv_streamreader.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "lib3mf/lib3mf_dynamic.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CStreamReader 
**************************************************************************************************************************/
CStreamReader::CStreamReader(LibMCData::PStorage pStorage, LibMCData::PStorageStream pStorageStream)
	: m_pStorage (pStorage), m_pStorageStream (pStorageStream), m_nSize (0), m_nReadPosition (0)
{
	if (pStorage.get () == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pStorageStream.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sUUID = pStorageStream->GetUUID();
	m_sName = pStorageStream->GetName();
	m_sMIMEType = pStorageStream->GetMIMEType();
	m_nSize = pStorageStream->GetSize();
}

CStreamReader::~CStreamReader()
{
	m_pStorageStream = nullptr;
	m_pStorage = nullptr;
}

std::string CStreamReader::GetUUID()
{
	return m_sUUID;
}

std::string CStreamReader::GetName()
{
	return m_sName;
}

std::string CStreamReader::GetMIMEType()
{
	return m_sMIMEType;
}

LibMCEnv_uint64 CStreamReader::GetSize()
{
	return m_nSize;
}

LibMCEnv_uint64 CStreamReader::GetReadPosition()
{
	return m_nReadPosition;
}

void CStreamReader::Seek(const LibMCEnv_uint64 nReadPosition)
{
	if (m_nReadPosition > m_nSize)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSTREAMSEEKPOSITION);

	m_nReadPosition = m_nSize;

}

void CStreamReader::ReadData(const LibMCEnv_uint64 nSizeToRead, LibMCEnv_uint64 nDataBufferSize, LibMCEnv_uint64* pDataNeededCount, LibMCEnv_uint8 * pDataBuffer)
{
	if (nSizeToRead == 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSTREAMREADSIZE);

	if (nSizeToRead > (1ULL << 60))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSTREAMREADSIZE);

	if (m_nReadPosition + nSizeToRead > m_nSize)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_STREAMREADEXCEEDSSTREAMSIZE);

	if (pDataNeededCount != nullptr)
		*pDataNeededCount = nSizeToRead;

	if (pDataBuffer != nullptr) {

		if (nSizeToRead > nDataBufferSize)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		void* pReadCallback = nullptr;
		void* pSeekCallback = nullptr;
		void* pUserData = nullptr;

		m_pStorageStream->GetCallbacks (pReadCallback, pSeekCallback, pUserData);

		if (pReadCallback == nullptr)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSTREAMREADCALLBACK);
		if (pSeekCallback == nullptr)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSTREAMSEEKCALLBACK);
		if (pUserData == nullptr)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSTREAMCALLBACKUSERDATA);

		LibMCData::StreamReadCallback readCallback = (LibMCData::StreamReadCallback)pReadCallback;
		LibMCData::StreamSeekCallback seekCallback = (LibMCData::StreamSeekCallback)pSeekCallback;

		uint32_t nSeekErrorCode = 0;
		seekCallback(m_nReadPosition, pUserData, &nSeekErrorCode);
		if (nSeekErrorCode)
			throw ELibMCEnvInterfaceException (nSeekErrorCode);

		uint32_t nReadErrorCode = 0;
		readCallback((void*) pDataBuffer, nSizeToRead, pUserData, &nReadErrorCode);
		if (nReadErrorCode)
			throw ELibMCEnvInterfaceException(nSeekErrorCode);

		m_nReadPosition += nSizeToRead;

	}


}

void CStreamReader::ReadAllData(LibMCEnv_uint64 nDataBufferSize, LibMCEnv_uint64* pDataNeededCount, LibMCEnv_uint8 * pDataBuffer)
{
	m_nReadPosition = 0;

	ReadData(m_nSize, nDataBufferSize, pDataNeededCount, pDataBuffer);
}

