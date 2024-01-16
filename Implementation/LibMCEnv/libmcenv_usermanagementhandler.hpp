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


Abstract: This is the class declaration of CUserManagementHandler

*/


#ifndef __LIBMCENV_USERMANAGEMENTHANDLER
#define __LIBMCENV_USERMANAGEMENTHANDLER

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"
#include "amc_languagehandler.hpp"
#include "amc_accesscontrol.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CUserManagementHandler 
**************************************************************************************************************************/

class CUserManagementHandler : public virtual IUserManagementHandler, public virtual CBase {
private:
	LibMCData::PLoginHandler m_pLoginHandler;
	AMC::PAccessControl m_pAccessControl;
	AMC::PLanguageHandler m_pLanguageHandler;

public:

	CUserManagementHandler(LibMCData::PLoginHandler pLoginHandler, AMC::PAccessControl pAccessControl, AMC::PLanguageHandler pLanguageHandler);

	virtual ~CUserManagementHandler();

	bool UserExists(const std::string & sUsername) override;

	void GetUserProperties(const std::string & sUsername, std::string & sUUID, std::string & sDescription, std::string & sRole, std::string & sLanguageIdentifier) override;

	void GetUserPropertiesByUUID(const std::string & sUUID, std::string & sUsername, std::string & sDescription, std::string & sRole, std::string & sLanguageIdentifier) override;

	std::string GetUsernameByUUID(const std::string & sUUID) override;

	std::string GetUserUUID(const std::string & sUsername) override;

	std::string GetUserDescription(const std::string & sUsername) override;

	std::string GetUserDescriptionByUUID(const std::string & sUUID) override;

	std::string GetUserRole(const std::string & sUsername) override;

	std::string GetUserRoleByUUID(const std::string & sUUID) override;

	std::string GetUserLanguage(const std::string & sUsername) override;

	std::string GetUserLanguageByUUID(const std::string & sUUID) override;

	std::string CreateUser(const std::string & sUsername, const std::string & sRole, const std::string & sSalt, const std::string & sHashedPassword, const std::string & sDescription) override;

	void SetUserLanguage(const std::string & sUsername, const std::string & sLanguageIdentifier) override;

	void SetUserRole(const std::string & sUsername, const std::string & sLanguageIdentifier) override;

	void SetUserDescription(const std::string & sUsername, const std::string & sDescription) override;

	void SetUserPassword(const std::string & sUsername, const std::string & sSalt, const std::string & sHashedPassword) override;

	void SetUserLanguageByUUID(const std::string & sUUID, const std::string & sLanguageIdentifier) override;

	void SetUserRoleByUUID(const std::string & sUUID, const std::string & sLanguageIdentifier) override;

	void SetUserDescriptionByUUID(const std::string & sUUID, const std::string & sDescription) override;

	void SetUserPasswordByUUID(const std::string & sUUID, const std::string & sSalt, const std::string & sHashedPassword) override;

	IUserDetailList* GetActiveUsers() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_USERMANAGEMENTHANDLER
