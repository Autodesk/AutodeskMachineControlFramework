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


Abstract: This is a stub class definition of CLoginHandler

*/

#include "libmcdata_loginhandler.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CLoginHandler 
**************************************************************************************************************************/

CLoginHandler::CLoginHandler(AMCData::PSQLHandler pSQLHandler)
    : m_pSQLHandler (pSQLHandler)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "invalid param: pSQLHandler");

}


bool CLoginHandler::UserExists(const std::string & sUsername)
{
    std::string sQuery = "SELECT uuid FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    return pStatement->nextRow();

}

void CLoginHandler::GetUserDetails(const std::string & sUsername, std::string & sSalt, std::string & sHashedPassword)
{
    if (sUsername.empty ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT salt, passwordhash FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    if (!pStatement->nextRow ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND, "user not found: " + sUsername);
    
    sSalt = pStatement->getColumnString(1);
    sHashedPassword = pStatement->getColumnString(2);

    pStatement = nullptr;
}


void CLoginHandler::GetUserProperties(const std::string& sUsername, std::string& sUUID, std::string& sDescription, std::string& sRole, std::string& sLanguageIdentifier)
{
    if (sUsername.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT uuid, description, role, language FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND, "user not found: " + sUsername);

    sUUID = pStatement->getColumnString(1);
    sDescription = pStatement->getColumnString(2);
    sRole = pStatement->getColumnString(3);
    sLanguageIdentifier = pStatement->getColumnString(4);

    if (pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTUNIQUE, "user not unique: " + sUsername);

    pStatement = nullptr;
}

void CLoginHandler::GetUserPropertiesByUUID(const std::string& sUUID, std::string& sUsername, std::string& sDescription, std::string& sRole, std::string& sLanguageIdentifier)
{
    if (sUsername.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT login, description, role, language FROM users WHERE uuid=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUUID);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND, "user uuid not found: " + sUUID);

    sUsername = pStatement->getColumnString(1);
    sDescription = pStatement->getColumnString(2);
    sRole = pStatement->getColumnString(3);
    sLanguageIdentifier = pStatement->getColumnString(4);

    if (pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTUNIQUE, "user uuid not unique: " + sUUID);

    pStatement = nullptr;
}


std::string CLoginHandler::GetUsernameByUUID(const std::string& sUUID)
{
    if (sUUID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERUUID);

    std::string sQuery = "SELECT login FROM users WHERE uuid=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, AMCCommon::CUtils::normalizeUUIDString (sUUID));
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserUUID(const std::string& sUsername)
{
    if (sUsername.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT uuid FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserDescription(const std::string& sUsername)
{
    if (sUsername.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT description FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserDescriptionByUUID(const std::string& sUUID)
{
    if (sUUID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERUUID);

    std::string sQuery = "SELECT description FROM users WHERE uuid=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, AMCCommon::CUtils::normalizeUUIDString(sUUID));
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserRole(const std::string& sUsername)
{
    if (sUsername.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT role FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserRoleByUUID(const std::string& sUUID)
{
    if (sUUID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERUUID);

    std::string sQuery = "SELECT role FROM users WHERE uuid=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, AMCCommon::CUtils::normalizeUUIDString(sUUID));
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserLanguage(const std::string& sUsername)
{
    if (sUsername.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);

    std::string sQuery = "SELECT language FROM users WHERE login=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, sUsername);
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::GetUserLanguageByUUID(const std::string& sUUID)
{
    if (sUUID.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERUUID);

    std::string sQuery = "SELECT language FROM users WHERE uuid=? AND active=1";
    auto pStatement = m_pSQLHandler->prepareStatement(sQuery);
    pStatement->setString(1, AMCCommon::CUtils::normalizeUUIDString(sUUID));
    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERNOTFOUND);

    return pStatement->getColumnString(1);
}

std::string CLoginHandler::CreateUser(const std::string& sUsername, const std::string& sRole, const std::string& sSalt, const std::string& sHashedPassword, const std::string& sDescription)
{
    if (sUsername.empty ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERNAME);
    if (sRole.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERROLE);
    if (sSalt.empty ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERSALT);
    if (sHashedPassword.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYUSERPASSWORD);

    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sUsername))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDUSERNAME);
    if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sRole))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDUSERROLE);

    std::string sNormalizedSalt = AMCCommon::CUtils::normalizeSHA256String(sSalt);
    std::string sNormalizedHashedPassword = AMCCommon::CUtils::normalizeSHA256String(sHashedPassword);

    auto pTransaction = m_pSQLHandler->beginTransaction();
    auto pUserCheckStatement = pTransaction->prepareStatement("SELECT uuid FROM users WHERE login=? AND active=1");
    pUserCheckStatement->setString(1, sUsername);
    if (pUserCheckStatement->nextRow ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_USERALREADYEXISTS, "user already exists: " + sUsername);

    std::string sNewUserUUID = AMCCommon::CUtils::createUUID();
    auto pUserCreateStatement = pTransaction->prepareStatement("INSERT INTO users (uuid, login, description, salt, passwordhash, role, active) VALUES (?, ?, ?, ?, ?, ?, ?)");
    pUserCreateStatement->setString(1, sNewUserUUID);
    pUserCreateStatement->setString(2, sUsername);
    pUserCreateStatement->setString(3, sDescription);
    pUserCreateStatement->setString(4, sNormalizedSalt);
    pUserCreateStatement->setString(5, sNormalizedHashedPassword);
    pUserCreateStatement->setString(6, sRole);
    pUserCreateStatement->setInt(7, 1);

    pUserCreateStatement->execute();

    pTransaction->commit();

    return sNewUserUUID;
}

void CLoginHandler::SetUserLanguage(const std::string& sUsername, const std::string& sLanguageIdentifier)
{

}

void CLoginHandler::SetUserRole(const std::string& sUsername, const std::string& sLanguageIdentifier)
{

}

void CLoginHandler::SetUserDescription(const std::string& sUsername, const std::string& sDescription)
{

}

void CLoginHandler::SetUserPassword(const std::string& sUsername, const std::string& sSalt, const std::string& sHashedPassword)
{

}

void CLoginHandler::SetUserLanguageByUUID(const std::string& sUUID, const std::string& sLanguageIdentifier)
{

}

void CLoginHandler::SetUserRoleByUUID(const std::string& sUUID, const std::string& sLanguageIdentifier)
{

}

void CLoginHandler::SetUserDescriptionByUUID(const std::string& sUUID, const std::string& sDescription)
{

}

void CLoginHandler::SetUserPasswordByUUID(const std::string& sUUID, const std::string& sSalt, const std::string& sHashedPassword)
{

}

IUserList* CLoginHandler::GetActiveUsers()
{
    throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}



