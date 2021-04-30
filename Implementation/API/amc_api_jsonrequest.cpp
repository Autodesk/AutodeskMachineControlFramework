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

#include "amc_api_jsonrequest.hpp"
#include "libmc_interfaceexception.hpp"

#include "RapidJSON/document.h"
#include "Common/common_utils.hpp"

using namespace AMC;

class AMC::CAPIJSONImpl
{
public:
	rapidjson::Document m_Document;

};


CAPIJSONRequest::CAPIJSONRequest(const uint8_t* pBodyData, const size_t nBodySize)
	: m_pImpl (__construct (pBodyData, nBodySize))
{
}

CAPIJSONRequest::~CAPIJSONRequest()
{

}

CAPIJSONImpl* CAPIJSONRequest::__construct(const uint8_t* pBodyData, const size_t nBodySize)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pImpl = std::make_unique <CAPIJSONImpl>();
	pImpl->m_Document.Parse((const char*)pBodyData, nBodySize);	
	if (pImpl->m_Document.HasParseError ()) 
		throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTPARSEJSONREQUEST);

	return pImpl.release();
}


bool CAPIJSONRequest::hasValue(const std::string& sKeyName)
{
	return m_pImpl->m_Document.HasMember(sKeyName.c_str ());
}

std::string CAPIJSONRequest::getRawString(const std::string& sKeyName, uint32_t nErrorCode)
{
	
	if (!hasValue (sKeyName))
		throw ELibMCInterfaceException(nErrorCode);
	if (!m_pImpl->m_Document[sKeyName.c_str()].IsString())
		throw ELibMCInterfaceException(nErrorCode);

	return m_pImpl->m_Document[sKeyName.c_str()].GetString ();
}

std::string CAPIJSONRequest::getUUID(const std::string& sKeyName, uint32_t nErrorCode)
{
	auto sRawString = getRawString(sKeyName, nErrorCode);
	return AMCCommon::CUtils::normalizeUUIDString(sRawString);
}

std::string CAPIJSONRequest::getNameString(const std::string& sKeyName, uint32_t nErrorCode)
{
	auto sRawString = getRawString(sKeyName, nErrorCode);

	if (!AMCCommon::CUtils::UTF8StringIsValid(sRawString))
		throw ELibMCInterfaceException(nErrorCode);

	for (char& c : sRawString) {
		if (c < 32)
			throw ELibMCInterfaceException(nErrorCode);
	}

	return AMCCommon::CUtils::trimString (sRawString);
}


uint64_t CAPIJSONRequest::getUint64(const std::string& sKeyName, const uint64_t nMinValue, const uint64_t nMaxValue, uint32_t nErrorCode)
{
	if (!hasValue(sKeyName))
		throw ELibMCInterfaceException(nErrorCode);
	if (!m_pImpl->m_Document[sKeyName.c_str()].IsUint64())
		throw ELibMCInterfaceException(nErrorCode);

	uint64_t nValue = m_pImpl->m_Document[sKeyName.c_str()].GetUint64();

	if ((nValue < nMinValue) || (nValue > nMaxValue))
		throw ELibMCInterfaceException(nErrorCode);

	return nValue;
}

std::string CAPIJSONRequest::getSHA256(const std::string& sKeyName, uint32_t nErrorCode)
{
	auto sNameString = getNameString(sKeyName, nErrorCode);

	return AMCCommon::CUtils::normalizeSHA256String(sNameString);


}
