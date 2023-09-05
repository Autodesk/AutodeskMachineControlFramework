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


#include "amc_accessrole.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"



namespace AMC {

	CAccessRole::CAccessRole(const std::string& sIdentifier, const CStringResource& rDisplayName, const CStringResource& rDescription)
		: m_sIdentifier (sIdentifier), m_DisplayName (rDisplayName), m_Description (rDescription)
	{
		if (sIdentifier.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYACCESSROLEIDENTIFIER);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDACCESSROLEIDENTIFIER, sIdentifier);


	}

	CAccessRole::~CAccessRole()
	{

	}

	std::string CAccessRole::getIdentifier()
	{
		return m_sIdentifier;
	}

	CStringResource CAccessRole::getDisplayName()
	{
		return m_DisplayName;
	}

	std::string CAccessRole::getDisplayNameString(StringLanguageID languageID)
	{
		return m_DisplayName.get(languageID);
	}

	CStringResource CAccessRole::getDescription()
	{
		return m_Description;
	}

	std::string CAccessRole::getDescriptionString(StringLanguageID languageID)
	{
		return m_Description.get (languageID);
	}

	bool CAccessRole::hasPermission(const std::string& sPermissionIdentifier)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		auto iIter = m_Permissions.find(sPermissionIdentifier);
		return iIter != m_Permissions.end();
	}

	void CAccessRole::addPermission(PAccessPermission pPermission)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (pPermission.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "CAccessRole::addPermission");
		
		std::string sIdentifier = pPermission->getIdentifier ();
		if (hasPermission (sIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEROLEPERMISSION, sIdentifier);


		m_Permissions.insert(std::make_pair (sIdentifier, pPermission));
	}

	void CAccessRole::removePermission(const std::string& sPermissionIdentifier)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		m_Permissions.erase(sPermissionIdentifier);
	}

	std::vector<CAccessPermission*> CAccessRole::getPermissions()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::vector<CAccessPermission*> resultVector;
		for (auto iIter : m_Permissions)
			resultVector.push_back(iIter.second.get());

		return resultVector;
	}

}


