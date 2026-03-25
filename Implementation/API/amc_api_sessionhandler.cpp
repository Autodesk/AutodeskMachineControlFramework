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
#include "amc_userinformation.hpp"

#include "libmc_interfaceexception.hpp"

#include "amc_api_jsonrequest.hpp"
#include "amc_api_constants.hpp"
#include "amc_ui_frontendstate.hpp"

#include "common_utils.hpp"

using namespace AMC;

#define APISESSIONHANDLER_DEFAULT_AUTH_TIMEOUT_SECONDS 1800
#define APISESSIONHANDLER_DEFAULT_UNAUTH_TIMEOUT_SECONDS 300

CAPISessionHandler::CAPISessionHandler(AMCCommon::PChrono pGlobalChrono, LibMCData::PDataModel pDataModel)
	: m_pGlobalChrono(pGlobalChrono),
	m_pDataModel(pDataModel),
	m_nSessionTimeoutSeconds(APISESSIONHANDLER_DEFAULT_AUTH_TIMEOUT_SECONDS),
	m_nUnauthSessionTimeoutSeconds(APISESSIONHANDLER_DEFAULT_UNAUTH_TIMEOUT_SECONDS)
{
}

CAPISessionHandler::~CAPISessionHandler()
{
}

PAPIAuth CAPISessionHandler::createAuthentication(const std::string& sAuthorizationJSON, AMCCommon::PChrono pGlobalChrono)
{
	if (!sAuthorizationJSON.empty()) {
		
		AMC::CAPIJSONRequest request((const uint8_t*)sAuthorizationJSON.c_str(), sAuthorizationJSON.length());

		std::string sSessionUUID = request.getUUID(AMC_API_KEY_TOKEN_SESSION, LIBMC_ERROR_INVALIDSESSIONUUID);
		std::string sToken = request.getSHA256(AMC_API_KEY_TOKEN_TOKEN, LIBMC_ERROR_INVALIDSESSIONTOKEN);

		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIterator = m_SessionMap.find (sSessionUUID);
		if (iIterator == m_SessionMap.end())
			return nullptr;

		auto pSession = iIterator->second;

		if (pSession->getToken () != sToken)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONTOKEN);

		uint64_t nNow = 0;
		if (m_pGlobalChrono)
			nNow = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		pSession->updateLastActivity(nNow);

		if (m_pDataModel) {
			try {
				auto pLoginHandler = m_pDataModel->CreateLoginHandler();
				pLoginHandler->UpdateLoginSessionActivity(sSessionUUID, nNow);
			}
			catch (...) {
			}
		}

		return std::make_shared<CAPIAuth>(pSession->getUUID(), pSession->getKey(), pSession->createUserInformation(), pSession->isAuthenticated(), pSession->getFrontendState ());
	}
	else {
		return nullptr;
	}

}


PAPIAuth CAPISessionHandler::createNewAuthenticationSession(PUIFrontendDefinition pFrontendDefinition)
{
	cleanupExpiredSessions();

	auto pSession = std::make_shared<CAPISession>(pFrontendDefinition, m_pGlobalChrono);

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	m_SessionMap.insert (std::make_pair (pSession->getUUID(), pSession));

	if (m_pDataModel) {
		try {
			auto pLoginHandler = m_pDataModel->CreateLoginHandler();
			pLoginHandler->CreateLoginSession(pSession->getUUID(), pSession->getLastActivityMicroseconds());
		}
		catch (...) {
		}
	}

	return std::make_shared<CAPIAuth>(pSession->getUUID(), pSession->getKey(), pSession->createUserInformation(), pSession->isAuthenticated(), pSession->getFrontendState());

}


PAPIAuth CAPISessionHandler::createEmptyAuthenticationSession()
{
	
	std::string sEmptyUUID = "00000000-0000-0000-0000-000000000000";
	std::string sEmptyKey = "0000000000000000000000000000000000000000000000000000000000000000";
	
	return std::make_shared<CAPIAuth>(sEmptyUUID, sEmptyKey, CUserInformation::makeEmpty (), false, nullptr);
}


void CAPISessionHandler::authorizeSession(const std::string& sSessionUUID, const std::string& sSaltedPassword, const std::string& sClientKey)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	pSession->authorizeSessionByPassword(sSaltedPassword, sClientKey);

	uint64_t nNow = 0;
	if (m_pGlobalChrono)
		nNow = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();
	pSession->updateLastActivity(nNow);

}


void CAPISessionHandler::setUserDetailsForSession(const std::string& sSessionUUID, const std::string& sUsername, const std::string& sHashedPassword, const std::string& sUserUUID, const std::string& sUserDescription, const std::string& sUserRoleIdentifier, const std::string& sUserLanguageIdentifier)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	pSession->setUserDetails(sUsername, sHashedPassword, sUserUUID, sUserDescription, sUserRoleIdentifier, sUserLanguageIdentifier);

	if (m_pDataModel) {
		try {
			auto pLoginHandler = m_pDataModel->CreateLoginHandler();
			pLoginHandler->UpdateLoginSessionAuthentication(sSessionUUID, sUserUUID, sUsername, sUserRoleIdentifier);
		}
		catch (...) {
		}
	}
}

void CAPISessionHandler::getUserDetailsForSession(const std::string& sSessionUUID, std::string& sUsername, std::string& sUserUUID, std::string& sUserDescription, std::string& sUserRoleIdentifier, std::string& sUserLanguageIdentifier)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	auto iIterator = m_SessionMap.find(sSessionUUID);
	if (iIterator == m_SessionMap.end())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDSESSIONUUID);

	auto pSession = iIterator->second;
	sUsername = pSession->getUserName();
	sUserUUID = pSession->getUserUUID();
	sUserDescription = pSession->getUserDescription();
	sUserRoleIdentifier = pSession->getUserRoleIdentifier();
	sUserLanguageIdentifier = pSession->getUserLanguageIdentifier();

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

void CAPISessionHandler::setSessionTimeout(uint64_t nAuthSeconds, uint64_t nUnauthSeconds)
{
	m_nSessionTimeoutSeconds = nAuthSeconds;
	m_nUnauthSessionTimeoutSeconds = nUnauthSeconds;
}

void CAPISessionHandler::deactivateSession(const std::string& sSessionUUID)
{
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIterator = m_SessionMap.find(sSessionUUID);
		if (iIterator != m_SessionMap.end())
			m_SessionMap.erase(iIterator);
	}

	if (m_pDataModel) {
		try {
			auto pLoginHandler = m_pDataModel->CreateLoginHandler();
			pLoginHandler->DeactivateLoginSession(sSessionUUID);
		}
		catch (...) {
		}
	}
}

void CAPISessionHandler::cleanupExpiredSessions()
{
	if (!m_pGlobalChrono)
		return;

	uint64_t nNow = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();
	uint64_t nAuthTimeoutMicroseconds = m_nSessionTimeoutSeconds * 1000000ULL;
	uint64_t nUnauthTimeoutMicroseconds = m_nUnauthSessionTimeoutSeconds * 1000000ULL;

	std::vector<std::string> expiredUUIDs;

	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		for (auto iIterator = m_SessionMap.begin(); iIterator != m_SessionMap.end(); ) {
			auto pSession = iIterator->second;
			uint64_t nLastActivity = pSession->getLastActivityMicroseconds();
			uint64_t nTimeout = pSession->isAuthenticated() ? nAuthTimeoutMicroseconds : nUnauthTimeoutMicroseconds;

			if (nLastActivity > 0 && nNow > nLastActivity && (nNow - nLastActivity) > nTimeout) {
				expiredUUIDs.push_back(iIterator->first);
				iIterator = m_SessionMap.erase(iIterator);
			}
			else {
				++iIterator;
			}
		}
	}

	if (m_pDataModel && !expiredUUIDs.empty()) {
		try {
			auto pLoginHandler = m_pDataModel->CreateLoginHandler();
			for (const auto& sUUID : expiredUUIDs) {
				pLoginHandler->DeactivateLoginSession(sUUID);
			}
		}
		catch (...) {
		}
	}
}
