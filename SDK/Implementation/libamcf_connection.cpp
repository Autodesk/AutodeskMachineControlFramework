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

#include "../../Implementation/Common/common_utils.hpp"

// Include custom headers here.
#define AMCF_MINRETRYCOUNT 1
#define AMCF_MAXRETRYCOUNT 1024
#define AMCF_DEFAULTRETRYCOUNT 3

#define AMCF_MINTIMEOUT 1
#define AMCF_MAXTIMEOUT 65536
#define AMCF_DEFAULTTIMEOUT 1000



using namespace LibAMCF::Impl;

/*************************************************************************************************************************
 Class definition of CConnection 
**************************************************************************************************************************/

CConnection::CConnection(const std::string& sBaseURL)
	: m_sBaseURL(sBaseURL), m_nTimeOut (AMCF_DEFAULTTIMEOUT), m_nRetryCount (AMCF_DEFAULTRETRYCOUNT)
{
	if (m_sBaseURL.empty ())
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDBASEURL);

	if (m_sBaseURL.back() != '/')
		m_sBaseURL = m_sBaseURL + "/";

	m_pRequestHandler = std::make_shared<CAsyncRequestHandler>();

}

std::string CConnection::GetBaseURL()
{
	return m_sBaseURL;
}

void CConnection::SetTimeouts(const LibAMCF_uint32 nTimeout, const LibAMCF_uint32 nRetryCount)
{
	if (nTimeout < AMCF_MINTIMEOUT)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDTIMEOUT);
	if (nTimeout > AMCF_MAXTIMEOUT)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDTIMEOUT);
	if (nRetryCount < AMCF_MINRETRYCOUNT)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDRETRYCOUNT);
	if (nRetryCount > AMCF_MAXRETRYCOUNT)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDRETRYCOUNT);

	m_nTimeOut = nTimeout;
	m_nRetryCount = nRetryCount;
}

LibAMCF_uint32 CConnection::GetTimeout()
{
	return m_nTimeOut;
}

LibAMCF_uint32 CConnection::GetRetryCount()
{
	return m_nRetryCount;
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

	CAsyncAuthenticationRequest(const std::string& sBaseURL, const std::string& sUserName, const std::string& sPassword, uint32_t nTimeOut, uint32_t nRetryCount)
		: CAsyncRequest(), m_sBaseURL(sBaseURL), m_sUserName(sUserName), m_sPassword(sPassword), m_nTimeout (nTimeOut), m_nRetryCount (nRetryCount)
	{

	}

	PAsyncResult onExecute() override {

		CRestHandler_Post sessionRequest ("retrievesession", m_sBaseURL + "api/auth", "", m_nTimeout, m_nRetryCount);
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

		CRestHandler_Post tokenRequest("retrievetoken", m_sBaseURL + "api/auth/" + sSessionUUID, "", m_nTimeout, m_nRetryCount);
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

	auto pRequest = std::make_shared<CAsyncAuthenticationRequest>(m_sBaseURL, sUserName, sPassword, m_nTimeOut, m_nRetryCount);
	m_pRequestHandler->executeRequest(pRequest, [this](CAsyncResult * pResult) {

		auto pTokenData = dynamic_cast<CAsyncTokenData*> (pResult);
		if (pTokenData != nullptr)
			SetAuthToken(pTokenData->getToken ());
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
	std::lock_guard<std::mutex> lockGuard(m_AuthTokenMutex);
	return m_sAuthTokenInternal;
}

void CConnection::SetAuthToken(std::string& sToken)
{
	std::lock_guard<std::mutex> lockGuard(m_AuthTokenMutex);
	m_sAuthTokenInternal = sToken;
}

IStreamUpload * CConnection::CreateUpload(const std::string & sName, const std::string & sMimeType, const std::string & sUsageContext)
{
	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);
}



