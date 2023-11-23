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


#ifndef __AMC_ACCESSROLE
#define __AMC_ACCESSROLE

#include "amc_accesspermission.hpp"

#include <mutex>
#include <map>
#include <vector>

namespace AMC {

	class CAccessRole;
	typedef std::shared_ptr<CAccessRole> PAccessRole;


	class CAccessRole {
	private:

		std::string m_sIdentifier;
		CStringResource m_DisplayName;
		CStringResource m_Description;

		std::mutex m_Mutex;

		std::map<std::string, PAccessPermission> m_Permissions;

	public:

		CAccessRole(const std::string& sIdentifier, const CStringResource& rDisplayName, const CStringResource& rDescription);
		
		virtual ~CAccessRole();

		std::string getIdentifier ();

		CStringResource getDisplayName();

		std::string getDisplayNameString(StringLanguageID languageID);

		CStringResource getDescription ();

		std::string getDescriptionString(StringLanguageID languageID);

		bool hasPermission (const std::string & sPermissionIdentifier);

		void addPermission (PAccessPermission pPermission);

		void removePermission (const std::string& sPermissionIdentifier);

		std::vector<CAccessPermission*> getPermissions();

	};

	
}


#endif //__AMC_ACCESSROLE

