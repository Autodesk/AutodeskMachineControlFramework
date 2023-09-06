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


#include "amc_userinformation.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"



namespace AMC {

	PUserInformation CUserInformation::makeEmpty()
	{
		return std::make_shared<CUserInformation>(AMCCommon::CUtils::createEmptyUUID (), "", "", "", "");
	}

	CUserInformation::CUserInformation(const std::string& sUUID, const std::string& sLogin, const std::string& sDescription, const std::string& sRoleIdentifier, const std::string& sLanguageIdentifier)
		: m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)),
		m_sLogin (sLogin),
		m_sDescription (sDescription),
		m_sRoleIdentifier (sRoleIdentifier),
		m_sLanguageIdentifier (sLanguageIdentifier)
	{

	}

	CUserInformation::~CUserInformation()
	{

	}

	std::string CUserInformation::getUUID()
	{
		return m_sUUID;
	}

	std::string CUserInformation::getLogin()
	{
		return m_sLogin;
	}

	std::string CUserInformation::getDescription()
	{
		return m_sDescription;
	}

	std::string CUserInformation::getRoleIdentifier()
	{
		return m_sRoleIdentifier;
	}

	std::string CUserInformation::getLanguageIdentifier()
	{
		return m_sLanguageIdentifier;
	}

}


