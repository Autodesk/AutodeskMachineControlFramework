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

#include "amc_api_auth.hpp"
#include "libmc_exceptiontypes.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_userinformation.hpp"

#include "common_utils.hpp"

using namespace AMC;


CAPIAuth::CAPIAuth(const std::string& sSessionUUID, const std::string& sSessionKey, PUserInformation pUserInformation, bool bIsAuthorized, PParameterHandler pClientVariableHandler, AMCCommon::PChrono pGlobalChrono)
	: m_sSessionUUID(AMCCommon::CUtils::normalizeUUIDString(sSessionUUID)), 
	m_sSessionKey (AMCCommon::CUtils::normalizeSHA256String(sSessionKey)),
	m_pUserInformation(pUserInformation),
	m_bIsAuthorized (bIsAuthorized)

{
	if (pUserInformation.get() == nullptr)
		throw ELibMCCustomException (LIBMC_ERROR_INVALIDPARAM, "CAPIAuth::CAPIAuth - pUserInformation is null");

	if (pClientVariableHandler.get () != nullptr) {
		m_pClientVariableHandler = pClientVariableHandler;
	}
	else
	{
		m_pClientVariableHandler = std::make_shared<CParameterHandler>("", pGlobalChrono);
	}
}

CAPIAuth::~CAPIAuth()
{

}


bool CAPIAuth::contextUUIDIsAuthorized(std::string& sContextUUID)
{
	return true;
}

std::string CAPIAuth::getSessionUUID()
{
	return m_sSessionUUID;
}

std::string CAPIAuth::getSessionKey()
{
	return m_sSessionKey;
}


bool CAPIAuth::userIsAuthorized()
{
	return m_bIsAuthorized;
}

PUserInformation CAPIAuth::getUserInformation()
{
	return m_pUserInformation;
}

std::string CAPIAuth::getUserName() 
{
	return m_pUserInformation->getLogin();
}

std::string CAPIAuth::getUserUUID()
{
	return m_pUserInformation->getUUID();
}


PParameterHandler CAPIAuth::getClientVariableHandler()
{
	return m_pClientVariableHandler;
}

std::string CAPIAuth::createStreamDownloadTicket(const std::string& sStreamUUID, const std::string& sDownloadFileName)
{
	std::string sTicketUUID = AMCCommon::CUtils::createUUID();
	std::string sNormalizedStreamUUID = AMCCommon::CUtils::normalizeUUIDString(sStreamUUID);

	m_DownloadTickets.insert(std::make_pair (sTicketUUID, std::make_pair (sNormalizedStreamUUID, sDownloadFileName)));

	return sTicketUUID;
}
