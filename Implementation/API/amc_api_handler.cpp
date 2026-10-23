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


#include "amc_api_handler.hpp"
#include "libmc_interfaceexception.hpp"

namespace AMC {


	CAPIFieldDetails::CAPIFieldDetails()
		: m_sFieldName (), m_bIsFileData (false), m_bIsMandatory (false)
	{

	}

	CAPIFieldDetails::CAPIFieldDetails(const std::string& sFieldName, const bool bIsFileData, const bool bIsMandatory)
		: m_sFieldName (sFieldName), m_bIsFileData (bIsFileData), m_bIsMandatory (bIsMandatory)
	{

	}


	CAPIFormFields::CAPIFormFields()
	{

	}

	void CAPIFormFields::addDataField (const std::string& sName, std::shared_ptr<std::vector<uint8_t>> pData)
	{
		m_FileData.insert(std::make_pair (sName, pData));
	}

	bool CAPIFormFields::hasDataField(const std::string& sName)
	{
		auto iIter = m_FileData.find(sName);
		return (iIter != m_FileData.end());
	}

	std::shared_ptr<std::vector<uint8_t>> CAPIFormFields::getDataField(const std::string& sName)
	{
		auto iIter = m_FileData.find(sName);
		if (iIter == m_FileData.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDFIELDNAME);

		return iIter->second;
	}


	void CAPIFormFields::addStringField(const std::string& sName, const std::string& sValue)
	{
		m_StringData.insert(std::make_pair (sName, sValue));
	}

	bool CAPIFormFields::hasStringField(const std::string& sName)
	{
		auto iIter = m_StringData.find (sName);
		return (iIter != m_StringData.end());
	}

	std::string CAPIFormFields::getStringField(const std::string& sName)
	{
		auto iIter = m_StringData.find(sName);
		if (iIter == m_StringData.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDFIELDNAME);

		return iIter->second;
	}

	uint64_t CAPIFormFields::getUint64Field(const std::string& sName)
	{
		auto sString = getStringField(sName);
		return std::stoull(sString);
	}

	void CAPIFormFields::addRequestParameter(const std::string& sName, const std::string& sValue)
	{
		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDQUERYPARAMETER, sName);

		auto iIter = m_RequestParameters.find(sName);
		if (iIter != m_RequestParameters.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEQUERYPARAMETER, sName);

		m_RequestParameters.insert(std::make_pair(sName, sValue));

	}

	bool CAPIFormFields::hasRequestParameter(const std::string& sName)
	{
		auto iIter = m_RequestParameters.find(sName);
		if (iIter != m_RequestParameters.end())
			return true;

		return false;
	}

	std::string CAPIFormFields::getRequestParameter(const std::string& sName, bool bFailIfNotExistent)
	{
		auto iIter = m_RequestParameters.find(sName);
		if (iIter != m_RequestParameters.end())
			return iIter->second;

		if (bFailIfNotExistent)
			throw ELibMCInterfaceException(LIBMC_ERROR_QUERYPARAMETERNOTFOUND, sName);

		return "";


	}

	bool CAPIFormFields::getOptionalUint64RequestParameter(const std::string& sName, uint64_t& nValue)
	{
		std::string sValue = getRequestParameter(sName, false);
		if (sValue.empty() || (sValue.length() > 18) || (sValue.find_first_not_of("0123456789") != std::string::npos))
			return false;

		nValue = std::stoull(sValue);
		return true;
	}


	CAPIHandler::CAPIHandler(const std::string& sClientHash)
		: m_sClientHash (sClientHash)
	{
		
	}

	CAPIHandler::~CAPIHandler()
	{
		
	}
				
	void CAPIHandler::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession)
	{
		bNeedsToBeAuthorized = true;
		bCreateNewSession = false;
	}


	bool CAPIHandler::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
	{
		return false;
	}

	uint32_t CAPIHandler::getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType)
	{
		return 0;
	}

	CAPIFieldDetails CAPIHandler::getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex)
	{
		throw ELibMCInterfaceException (LIBMC_ERROR_INVALIDINDEX);
	}

		
	void CAPIHandler::writeJSONHeader(CJSONWriter & writer, const std::string & sProtocol)
	{
		writer.addString(AMC_API_KEY_PROTOCOL, sProtocol);
		writer.addString(AMC_API_KEY_VERSION, AMC_API_PROTOCOL_VERSION);		
	}

	
}

