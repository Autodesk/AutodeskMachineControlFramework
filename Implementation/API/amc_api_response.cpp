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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#define __AMCIMPL_API_CONSTANTS

#include "amc_api_response.hpp"
#include "amc_api_constants.hpp"
#include "libmc_interfaceexception.hpp"

using namespace AMC;


CAPIResponse::CAPIResponse(uint32_t nHTTPCode, const std::string& sContentType)
	: m_sContentType (sContentType), m_nHTTPCode (nHTTPCode)
{

}

size_t CAPIResponse::getStreamSize() const
{
	return m_StreamData.size();
}

const uint8_t* CAPIResponse::getStreamData() const
{
	if (m_StreamData.size() > 0)
		return m_StreamData.data();

	return nullptr;
}

std::string CAPIResponse::getContentType() const
{
	return m_sContentType;
}

uint32_t CAPIResponse::getHTTPCode() const
{
	return m_nHTTPCode;
}




CAPIStringResponse::CAPIStringResponse(uint32_t nHTTPCode, const std::string& sContentType, const std::string& sStringValue)
	: CAPIResponse (nHTTPCode, sContentType)
{
	size_t len = sStringValue.length();
	if (len > 0) {
		m_StreamData.resize(len);
		const char* pChar = sStringValue.c_str();
		uint8_t* pData = m_StreamData.data();

		for (size_t idx = 0; idx < len; idx++) {
			*pData = (uint8_t)*pChar;
			pChar++; pData++;
		}

	}


}


CAPIFixedBufferResponse::CAPIFixedBufferResponse(const std::string& sContentType)
	: CAPIResponse (AMC_API_HTTP_SUCCESS, sContentType)
{
}

std::vector<uint8_t>& CAPIFixedBufferResponse::getBuffer()
{
	return m_StreamData;
}

CAPIFixedFloatBufferResponse::CAPIFixedFloatBufferResponse(const std::string& sContentType)
	: CAPIResponse(AMC_API_HTTP_SUCCESS, sContentType), m_nWriteIndex (0)
{

}

void CAPIFixedFloatBufferResponse::resizeTo(size_t nFloatCount)
{
	size_t nByteCount = sizeof(float) * nFloatCount;
	m_StreamData.resize(nByteCount);

	for (auto& it : m_StreamData)
		it = 0;
}

void CAPIFixedFloatBufferResponse::resetWriting()
{
	m_nWriteIndex = 0;
}

void CAPIFixedFloatBufferResponse::addFloat(float fValue)
{
	size_t nStartAddress = m_nWriteIndex * 4;
	if ((nStartAddress + 4) > m_StreamData.size())
		throw ELibMCInterfaceException(LIBMC_ERROR_FIXEDFLOATBUFFEROVERRUN);

	uint8_t* pTarget = &m_StreamData.at(nStartAddress);
	uint8_t* pSource = (uint8_t*)&fValue;

	for (size_t nIndex = 0; nIndex < 4; nIndex++)
		pTarget[nIndex] = pSource[nIndex];

	m_nWriteIndex++;



}
