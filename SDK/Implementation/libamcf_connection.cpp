/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is a stub class definition of CConnection

*/

#include "libamcf_connection.hpp"
#include "libamcf_interfaceexception.hpp"
#include "libamcf_operationresult.hpp"

#include <RapidJSON/rapidjson.h>
#include <RapidJSON/document.h>
#include <RapidJSON/writer.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "libamcf_resthandler.hpp"
#include "libamcf_streamupload.hpp"

#include "../../Implementation/Common/common_utils.hpp"

// Include custom headers here.




using namespace LibAMCF::Impl;

/*************************************************************************************************************************
 Class definition of CConnection 
**************************************************************************************************************************/

CConnection::CConnection(const std::string& sBaseURL)	
{
	m_pConnectionState = std::make_shared<CConnectionState>(sBaseURL);

}

std::string CConnection::GetBaseURL()
{
	return m_pConnectionState->getBaseURL ();
}

void CConnection::SetTimeouts(const LibAMCF_uint32 nTimeout, const LibAMCF_uint32 nRetryCount)
{
	m_pConnectionState->setTimeOutAndRetryCount(nTimeout, nRetryCount);
}

LibAMCF_uint32 CConnection::GetTimeout()
{
	return m_pConnectionState->getTimeOut ();
}

LibAMCF_uint32 CConnection::GetRetryCount()
{
	return m_pConnectionState->getRetryCount ();
}


class CAsyncTokenData : public CAsyncSuccess {
private:
	std::string m_sToken;

public:
	CAsyncTokenData(std::string sToken)
		: m_sToken (sToken)
	{

	}

	std::string getToken()
	{
		return m_sToken;
	}

};

class CAsyncAuthenticationRequest : public CAsyncRequest
{

private:
	std::string m_sBaseURL;
	std::string m_sUserName;
	std::string m_sPassword;
	uint32_t m_nTimeout;
	uint32_t m_nRetryCount;

public:

	CAsyncAuthenticationRequest(PConnectionState pConnectionState, const std::string& sUserName, const std::string& sPassword)
		: CAsyncRequest(), m_sUserName(sUserName), m_sPassword(sPassword)
	{
		if (pConnectionState.get() == nullptr)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);

		m_sBaseURL = pConnectionState->getBaseURL();
		m_nTimeout = pConnectionState->getTimeOut();
		m_nRetryCount = pConnectionState->getRetryCount();
	}

	PAsyncResult onExecute() override {

		CRestHandler_JSONPost sessionRequest ("retrievesession", m_sBaseURL + "api/auth", "", m_nTimeout, m_nRetryCount);
		sessionRequest.addValue("username", m_sUserName);
		sessionRequest.sendRequest();

		auto sSessionUUID = sessionRequest.getResultValue("sessionuuid");
		auto sSessionKey = sessionRequest.getResultValue("sessionkey");
		auto sLoginSalt = sessionRequest.getResultValue("loginsalt");

		if (sSessionUUID.empty())
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_COULDNOTCREATESESSION);

		std::string sClientKey = AMCCommon::CUtils::calculateRandomSHA256String(32);
		auto sSaltedPassword = AMCCommon::CUtils::calculateSHA256FromString(sLoginSalt + m_sPassword);
		auto sClientKeyHash = AMCCommon::CUtils::calculateSHA256FromString(sClientKey + sSaltedPassword);
		auto sSessionKeyHash = AMCCommon::CUtils::calculateSHA256FromString(sSessionKey + sClientKeyHash);

		CRestHandler_JSONPost tokenRequest("retrievetoken", m_sBaseURL + "api/auth/" + sSessionUUID, "", m_nTimeout, m_nRetryCount);
		tokenRequest.addValue("clientkey", sClientKey);
		tokenRequest.addValue("password", sSessionKeyHash);
		tokenRequest.sendRequest();

		auto sToken = tokenRequest.getResultValue ("token");
		if (sToken.empty())
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_COULDNOTRECEIVETOKEN);

		return std::make_shared<CAsyncTokenData>(sToken);

	}

};



IOperationResult * CConnection::AuthenticateWithPassword(const std::string & sUserName, const std::string & sPassword)
{

	auto pRequest = std::make_shared<CAsyncAuthenticationRequest>(m_pConnectionState, sUserName, sPassword);
	auto pRequestHandler = m_pConnectionState->getRequestHandler();
	pRequestHandler->executeRequest(pRequest, [this](CAsyncResult * pResult) {

		auto pTokenData = dynamic_cast<CAsyncTokenData*> (pResult);
		if (pTokenData != nullptr) {
			auto sToken = pTokenData->getToken ();
			m_pConnectionState->setAuthToken(sToken);
		}
	});

	return new COperationResult (pRequest->getFuture ());

	
}

bool CConnection::IsAuthenticated()
{
	auto sAuthToken = GetAuthToken();
	return !sAuthToken.empty();
}

IOperationResult * CConnection::RefreshAuthentication()
{
	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);
}

IOperationResult * CConnection::Ping()
{
	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);
}

std::string CConnection::GetAuthToken()
{
	return m_pConnectionState->getAuthToken();
}


IStreamUpload * CConnection::CreateUpload(const std::string & sName, const std::string & sMimeType, const std::string & sUsageContext)
{
	return new CStreamUpload(m_pConnectionState, sName, sMimeType, sUsageContext);
}



