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


Abstract: This is a stub class definition of CUserDetailList

*/

#include "libmcenv_userdetaillist.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CUserDetailList 
**************************************************************************************************************************/

class LibMCEnv::Impl::CUserDetailImpl
{
private:
    std::string m_sUsername;
    std::string m_sUUID;
    std::string m_sDescription;
    std::string m_sRole;
    std::string m_sLanguageIdentifier;

public:

    CUserDetailImpl(const std::string& sUsername, const std::string& sUUID, const std::string& sDescription, const std::string& sRole, const std::string& sLanguageIdentifier)
        : m_sUsername (sUsername), m_sUUID (sUUID), m_sDescription (sDescription), m_sRole (sRole), m_sLanguageIdentifier (sLanguageIdentifier)
    {

    }

    virtual ~CUserDetailImpl()
    {

    }

    std::string getUsername()
    {
        return m_sUsername;
    }

    std::string getUUID()
    {
        return m_sUUID;
    }

    std::string getDescription()
    {
        return m_sDescription;
    }

    std::string getRole()
    {
        return m_sRole;
    }

    std::string getLanguageIdentifier ()
    {
        return m_sLanguageIdentifier;
    }

};


CUserDetailList::CUserDetailList()
{

}

CUserDetailList::~CUserDetailList()
{

}

void CUserDetailList::addUser(const std::string& sUsername, const std::string& sUUID, const std::string& sDescription, const std::string& sRole, const std::string& sLanguageIdentifier)
{
    m_UserDetailList.push_back(std::make_shared<CUserDetailImpl> (sUsername, sUUID, sDescription, sRole, sLanguageIdentifier));
}


LibMCEnv_uint32 CUserDetailList::Count()
{
    return (uint32_t) m_UserDetailList.size();
}

void CUserDetailList::GetUserProperties(const LibMCEnv_uint32 nUserIndex, std::string & sUsername, std::string & sUUID, std::string & sDescription, std::string & sRole, std::string & sLanguageIdentifier)
{
    if (nUserIndex >= m_UserDetailList.size())
	    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERINDEX);

    auto pUser = m_UserDetailList.at(nUserIndex);
    sUsername = pUser->getUsername();
    sUUID = pUser->getUUID();
    sDescription = pUser->getDescription();
    sRole = pUser->getRole();
    sLanguageIdentifier = pUser->getLanguageIdentifier();
}

std::string CUserDetailList::GetUsername(const LibMCEnv_uint32 nUserIndex)
{
    if (nUserIndex >= m_UserDetailList.size())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERINDEX);

    auto pUser = m_UserDetailList.at(nUserIndex);
    return pUser->getUsername();
}

std::string CUserDetailList::GetUUID(const LibMCEnv_uint32 nUserIndex)
{
    if (nUserIndex >= m_UserDetailList.size())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERINDEX);

    auto pUser = m_UserDetailList.at(nUserIndex);
    return pUser->getUUID();
}

std::string CUserDetailList::GetDescription(const LibMCEnv_uint32 nUserIndex)
{
    if (nUserIndex >= m_UserDetailList.size())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERINDEX);

    auto pUser = m_UserDetailList.at(nUserIndex);
    return pUser->getDescription();
}

std::string CUserDetailList::GetRole(const LibMCEnv_uint32 nUserIndex)
{
    if (nUserIndex >= m_UserDetailList.size())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERINDEX);

    auto pUser = m_UserDetailList.at(nUserIndex);
    return pUser->getRole();
}

std::string CUserDetailList::GetLanguage(const LibMCEnv_uint32 nUserIndex)
{
    if (nUserIndex >= m_UserDetailList.size())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDUSERINDEX);

    auto pUser = m_UserDetailList.at(nUserIndex);
    return pUser->getLanguageIdentifier();
}

