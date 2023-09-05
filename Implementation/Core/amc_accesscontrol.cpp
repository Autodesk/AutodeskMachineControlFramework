/*++

Copyright (C) 2023 Autodesk Inc.

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


#include "amc_accesscontrol.hpp"
#include "amc_accessrole.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"



namespace AMC {


	CAccessControl::CAccessControl()
	{

	}

	CAccessControl::~CAccessControl()
	{

	}

	PAccessPermission CAccessControl::addPermission(const std::string& sIdentifier, const CStringResource& rDisplayName, const CStringResource& rDescription)
	{

		if (hasPermission(sIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEPERMISSIONIDENTIFIER, sIdentifier);

		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			auto pResult = std::make_shared<CAccessPermission>(sIdentifier, rDisplayName, rDescription);
			m_Permissions.insert(std::make_pair(pResult->getIdentifier(), pResult));

			return pResult;
		}

	}

	PAccessRole CAccessControl::addRole(const std::string& sIdentifier, const CStringResource& rDisplayName, const CStringResource& rDescription)
	{
		if (hasRole(sIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEROLEIDENTIFIER, sIdentifier);

		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			auto pResult = std::make_shared<CAccessRole>(sIdentifier, rDisplayName, rDescription);
			m_Roles.insert(std::make_pair(pResult->getIdentifier(), pResult));

			return pResult;
		}


	}

	PAccessPermission CAccessControl::findPermission(const std::string& sIdentifier, bool bMustExist)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIter = m_Permissions.find(sIdentifier);
		if (iIter == m_Permissions.end()) {
			if (bMustExist)
				throw ELibMCCustomException(LIBMC_ERROR_PERMISSIONNOTFOUND, sIdentifier);
			return nullptr;
		}

		return iIter->second;

	}

	PAccessRole CAccessControl::findRole(const std::string& sIdentifier, bool bMustExist)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIter = m_Roles.find(sIdentifier);
		if (iIter == m_Roles.end()) {
			if (bMustExist)
				throw ELibMCCustomException(LIBMC_ERROR_ROLENOTFOUND, sIdentifier);
			return nullptr;
		}

		return iIter->second;

	}

	bool CAccessControl::hasPermission(const std::string& sIdentifier)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIter = m_Permissions.find(sIdentifier);
		return iIter != m_Permissions.end();
	}

	bool CAccessControl::hasRole(const std::string& sIdentifier)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto iIter = m_Roles.find(sIdentifier);
		return iIter != m_Roles.end();

	}

	void CAccessControl::setDefaultRole(const std::string& sIdentifier)
	{
		PAccessRole pRole = findRole(sIdentifier, true);
		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);
			m_pDefaultRole = pRole;
		}
	}

	void CAccessControl::setToNoAccessControl()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		m_Permissions.clear();
		m_Roles.clear();

		CStringResource nameResource("Default Role");
		CStringResource descriptionResource("");

		auto pDefaultRole = std::make_shared<CAccessRole>("default", nameResource, descriptionResource);
		m_Roles.insert(std::make_pair(pDefaultRole->getIdentifier(), pDefaultRole));

		m_pDefaultRole = pDefaultRole;
	}


}


