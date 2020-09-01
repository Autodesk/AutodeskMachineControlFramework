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

#define __AMCIMPL_API_SESSION
#define __AMCIMPL_API_CONSTANTS

#include "amc_api_sessionhandler.hpp"
#include "amc_api_session.hpp"
#include "amc_api_auth.hpp"

#include "libmc_interfaceexception.hpp"

#include "amc_api_jsonrequest.hpp"
#include "amc_api_constants.hpp"

#include "common_utils.hpp"

using namespace AMC;


CAPISessionHandler::CAPISessionHandler()
{
}

CAPISessionHandler::~CAPISessionHandler()
{
}

PAPIAuth CAPISessionHandler::createAuthentication(const std::string& sAuthorizationJSON)
{
	if (!sAuthorizationJSON.empty()) {
		
		AMC::CAPIJSONRequest request((const uint8_t*)sAuthorizationJSON.c_str(), sAuthorizationJSON.length());

		std::string sSessionUUID = request.getUUID(AMC_API_KEY_TOKEN_SESSION, LIBMC_ERROR_INVALIDSESSIONUUID);
		std::string sToken = request.getSHA256(AMC_API_KEY_TOKEN_TOKEN, LIBMC_ERROR_INVALIDSESSIONTOKEN);

		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIterator = m_SessionMap.find (sSessionUUID);
		if (iIterator == m_SessionMap.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);		
		auto pSession = iIterator->second;

		if (pSession->getToken () != sToken)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONTOKEN);

		return std::make_shared<CAPIAuth>(pSession->getUUID(), pSession->getKey(), pSession->getUserName(), pSession->isAuthenticated());
	}
	else {
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);
	}

}


PAPIAuth CAPISessionHandler::createNewAuthenticationSession()
{
	auto pSession = std::make_shared<CAPISession>();

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	m_SessionMap.insert (std::make_pair (pSession->getUUID(), pSession));

	return std::make_shared<CAPIAuth>(pSession->getUUID(), pSession->getKey(), pSession->getUserName(), pSession->isAuthenticated());

}


PAPIAuth CAPISessionHandler::createEmptyAuthenticationSession()
{
	
	std::string sEmptyUUID = "00000000-0000-0000-0000-000000000000";
	std::string sEmptyKey = "0000000000000000000000000000000000000000000000000000000000000000";
	
	return std::make_shared<CAPIAuth>(sEmptyUUID, sEmptyKey, "", false);
}


void CAPISessionHandler::authorizeSession(const std::string& sSessionUUID, const std::string& sSaltedPassword, const std::string& sClientKey)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	pSession->authorizeSessionByPassword(sSaltedPassword, sClientKey);

}


void CAPISessionHandler::setUserDetailsForSession(const std::string& sSessionUUID, const std::string& sUsername, const std::string& sHashedPassword)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	pSession->setUserDetails(sUsername, sHashedPassword);
}

std::string CAPISessionHandler::getSessionToken(const std::string& sSessionUUID)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	return pSession->getToken();

}

bool CAPISessionHandler::sessionIsAuthenticated(const std::string& sSessionUUID)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	return pSession->isAuthenticated();

}
