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


Abstract: This is a stub class definition of CUserManagementHandler

*/

#include "libmcenv_usermanagementhandler.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CUserManagementHandler 
**************************************************************************************************************************/

CUserManagementHandler::CUserManagementHandler(LibMCData::PLoginHandler pLoginHandler, AMC::PAccessControl pAccessControl, AMC::PLanguageHandler pLanguageHandler)
	: m_pLoginHandler (pLoginHandler), m_pAccessControl (pAccessControl), m_pLanguageHandler (pLanguageHandler)
{

	if (pLoginHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pAccessControl.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pLanguageHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CUserManagementHandler::~CUserManagementHandler()
{

}


bool CUserManagementHandler::UserExists(const std::string & sUsername)
{
	if (sUsername.empty ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	return m_pLoginHandler->UserExists(sUsername);
}

void CUserManagementHandler::GetUserProperties(const std::string & sUsername, std::string & sUUID, std::string & sDescription, std::string & sRole, std::string & sLanguageIdentifier)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	m_pLoginHandler->GetUserProperties(sUsername, sUUID, sDescription, sRole, sLanguageIdentifier);
}

void CUserManagementHandler::GetUserPropertiesByUUID(const std::string & sUUID, std::string & sUsername, std::string & sDescription, std::string & sRole, std::string & sLanguageIdentifier)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	m_pLoginHandler->GetUserPropertiesByUUID(sNormalizedUUID, sUsername, sDescription, sRole, sLanguageIdentifier);
}

std::string CUserManagementHandler::GetUsernameByUUID(const std::string & sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	return m_pLoginHandler->GetUsernameByUUID(sNormalizedUUID);
}

std::string CUserManagementHandler::GetUserUUID(const std::string & sUsername)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	return m_pLoginHandler->GetUserUUID(sUsername);
}

std::string CUserManagementHandler::GetUserDescription(const std::string & sUsername)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	return m_pLoginHandler->GetUserDescription(sUsername);
}

std::string CUserManagementHandler::GetUserDescriptionByUUID(const std::string & sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	return m_pLoginHandler->GetUserDescriptionByUUID(sUUID);
}

std::string CUserManagementHandler::GetUserRole(const std::string & sUsername)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	return m_pLoginHandler->GetUserRole(sUsername);
}

std::string CUserManagementHandler::GetUserRoleByUUID(const std::string & sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	return m_pLoginHandler->GetUserRoleByUUID(sNormalizedUUID);
}

std::string CUserManagementHandler::GetUserLanguage(const std::string & sUsername)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	return m_pLoginHandler->GetUserLanguage(sUsername);
}

std::string CUserManagementHandler::GetUserLanguageByUUID(const std::string & sUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	return m_pLoginHandler->GetUserLanguageByUUID(sNormalizedUUID);
}

std::string CUserManagementHandler::CreateUser(const std::string & sUsername, const std::string & sRole, const std::string & sSalt, const std::string & sHashedPassword, const std::string & sDescription)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERROLE);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sRole))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERROLE, "invalid user role: " + sRole);

	std::string sNormalizedSalt = AMCCommon::CUtils::normalizeSHA256String(sSalt);
	std::string sNormalizedHash = AMCCommon::CUtils::normalizeSHA256String(sHashedPassword);

	return m_pLoginHandler->CreateUser(sUsername, sRole, sNormalizedSalt, sNormalizedHash, sDescription);
}

void CUserManagementHandler::SetUserLanguage(const std::string & sUsername, const std::string & sLanguageIdentifier)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	if (sLanguageIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLANGUAGE);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sLanguageIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLANGUAGE, "invalid user language: " + sLanguageIdentifier);

	if (!m_pLanguageHandler->hasLanguage(sLanguageIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLANGUAGE, "invalid user language: " + sLanguageIdentifier);

	m_pLoginHandler->SetUserLanguage(sUsername, sLanguageIdentifier);

}

void CUserManagementHandler::SetUserRole(const std::string & sUsername, const std::string & sRole)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	if (sRole.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERROLE);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sRole))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERROLE, "invalid user role: " + sRole);

	if (!m_pAccessControl->hasRole(sRole))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERROLE, "invalid user role: " + sRole);

	m_pLoginHandler->SetUserRole(sUsername, sRole);
}

void CUserManagementHandler::SetUserDescription(const std::string & sUsername, const std::string & sDescription)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	m_pLoginHandler->SetUserDescription(sUsername, sDescription);
}

void CUserManagementHandler::SetUserPassword(const std::string & sUsername, const std::string & sSalt, const std::string & sHashedPassword)
{
	if (sUsername.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLOGIN);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sUsername))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLOGIN, "invalid user login: " + sUsername);

	std::string sNormalizedSalt = AMCCommon::CUtils::normalizeSHA256String(sSalt);
	std::string sNormalizedHash = AMCCommon::CUtils::normalizeSHA256String(sHashedPassword);

	m_pLoginHandler->SetUserPassword(sUsername, sNormalizedSalt, sNormalizedHash);
}

void CUserManagementHandler::SetUserLanguageByUUID(const std::string & sUUID, const std::string & sLanguageIdentifier)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	if (sLanguageIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERLANGUAGE);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sLanguageIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLANGUAGE, "invalid user language: " + sLanguageIdentifier);

	if (!m_pLanguageHandler->hasLanguage(sLanguageIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERLANGUAGE, "invalid user language: " + sLanguageIdentifier);

	m_pLoginHandler->SetUserLanguageByUUID(sNormalizedUUID, sLanguageIdentifier);

}

void CUserManagementHandler::SetUserRoleByUUID(const std::string & sUUID, const std::string & sRole)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	if (sRole.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYUSERROLE);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sRole))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERROLE, "invalid user role: " + sRole);

	if (!m_pAccessControl->hasRole(sRole))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERROLE, "invalid user role: " + sRole);

	m_pLoginHandler->SetUserRoleByUUID(sNormalizedUUID, sRole);
}

void CUserManagementHandler::SetUserDescriptionByUUID(const std::string & sUUID, const std::string & sDescription)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	m_pLoginHandler->SetUserDescriptionByUUID(sNormalizedUUID, sDescription);
}

void CUserManagementHandler::SetUserPasswordByUUID(const std::string & sUUID, const std::string & sSalt, const std::string & sHashedPassword)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
	std::string sNormalizedSalt = AMCCommon::CUtils::normalizeSHA256String(sSalt);
	std::string sNormalizedHash = AMCCommon::CUtils::normalizeSHA256String(sHashedPassword);

	m_pLoginHandler->SetUserPasswordByUUID(sNormalizedUUID, sNormalizedSalt, sNormalizedHash);
}

