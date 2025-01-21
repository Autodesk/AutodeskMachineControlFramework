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

#ifndef __AMC_UI_INTERFACES
#define __AMC_UI_INTERFACES

#include "header_protection.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CUIClientAction, PUIClientAction);
	amcDeclareDependingClass(CAPIAuth, PAPIAuth);

	class CUIHandleEventResponse {
	private:
		uint32_t m_nErrorCode;
		std::string m_sErrorMessage;

		std::vector<PUIClientAction> m_clientActions;
		std::map<std::string, std::string> m_returnValues;

	public:
		CUIHandleEventResponse(uint32_t nErrorCode, const std::string& sErrorMessage, const std::vector<PUIClientAction>& clientActions, const std::map<std::string, std::string>& returnValues);

		uint32_t getErrorCode();

		std::string getErrorMessage();

		std::vector<PUIClientAction>& getClientActions();

		std::map<std::string, std::string>& getReturnValues();

		static bool externalValueNameIsReserved(const std::string& sName);
	};
		
	class CUIModule_ContentRegistry {
	public:

		virtual void registerFormName(const std::string& sFormUUID, const std::string & sFormName) = 0;
		virtual std::string findFormUUIDByName(const std::string & sFormName) = 0;

		virtual PUIModuleItem findModuleItemByUUID(const std::string& sItemUUID) = 0;

		virtual void ensureUIEventExists(const std::string& sEventName) = 0;
	};
	
	class CUIModule_UIEventHandler {
	public:

		virtual void ensureUIEventExists(const std::string& sEventName) = 0;

		virtual CUIHandleEventResponse handleEvent(const std::string& sEventName, const std::string& sSenderUUID, const std::string& sEventFormPayloadJSON, const std::string& sEventParameterJSON, PAPIAuth pAPIAuth) = 0;
	};

	
}


#endif // __AMC_UI_INTERFACES