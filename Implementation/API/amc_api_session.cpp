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

#include "amc_api_session.hpp"
#include "amc_parameterhandler.hpp"
#include "common_utils.hpp"
#include "amc_userinformation.hpp"

#include "libmc_interfaceexception.hpp"

using namespace AMC;

#define APISESSION_RANDOMKEYITERATIONS 16

CAPISession::CAPISession(AMCCommon::PChrono pGlobalChrono)
	: m_sUUID(AMCCommon::CUtils::createUUID()),
	m_sKey(AMCCommon::CUtils::calculateRandomSHA256String(APISESSION_RANDOMKEYITERATIONS)),
	m_sToken(AMCCommon::CUtils::calculateRandomSHA256String(APISESSION_RANDOMKEYITERATIONS)),
	m_sUserName(""),
	m_sUserUUID(AMCCommon::CUtils::createUUID ()),
	m_bAuthenticated(false)
{

	m_pClientVariableHandler = std::make_shared<CParameterHandler>("", pGlobalChrono);
	
}
	
			
CAPISession::~CAPISession()
{

}

std::string CAPISession::getUUID()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sUUID;
}

std::string CAPISession::getKey()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sKey;
}

std::string CAPISession::getUserName()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sUserName;
}

std::string CAPISession::getUserUUID()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sUserUUID;
}

std::string CAPISession::getUserDescription()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sUserDescription;
}

std::string CAPISession::getUserLanguageIdentifier()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sUserLanguageIdentifier;
}

std::string CAPISession::getUserRoleIdentifier()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sUserRoleIdentifier;
}

bool CAPISession::isAuthenticated()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_bAuthenticated;
}

std::string CAPISession::getToken()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sToken;	
}

		
void CAPISession::authorizeSessionByPassword(const std::string& sSaltedPasswordHash, const std::string& sClientKey)
{
	if (m_bAuthenticated)
		throw ELibMCInterfaceException(LIBMC_ERROR_USERALREADYAUTHORIZED);

	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	if (m_sUserName.empty()) 
		throw ELibMCInterfaceException (LIBMC_ERROR_INVALIDLOGIN);

	auto sNormalizedClientKey = AMCCommon::CUtils::normalizeSHA256String(sClientKey);

	auto sClientKeyHash = AMCCommon::CUtils::calculateSHA256FromString(sNormalizedClientKey + m_sHashedPassword);
	auto sSessionKeyHash = AMCCommon::CUtils::calculateSHA256FromString(m_sKey + sClientKeyHash);

	if (AMCCommon::CUtils::normalizeSHA256String(sSaltedPasswordHash) == sSessionKeyHash) {
		m_bAuthenticated = true;
	}
	else {
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDLOGIN);
	}

}

void CAPISession::setUserDetails(const std::string& sUserName, const std::string& sHashedPassword, const std::string& sUserUUID, const std::string& sUserDescription, const std::string& sUserRoleIdentifier, const std::string& sUserLanguageIdentifier)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);

	m_sUserName = sUserName;
	m_sHashedPassword = sHashedPassword;
	m_sUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
		
	m_sUserDescription = sUserDescription;
	m_sUserRoleIdentifier = sUserRoleIdentifier;
	m_sUserLanguageIdentifier = sUserLanguageIdentifier;
}

PParameterHandler CAPISession::getClientVariableHandler()
{
	return m_pClientVariableHandler;
}

PUserInformation CAPISession::createUserInformation()
{
	return std::make_shared<CUserInformation> (m_sUserUUID, m_sUserName, m_sUserDescription, m_sUserRoleIdentifier, m_sUserLanguageIdentifier);
}