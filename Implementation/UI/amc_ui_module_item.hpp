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


#ifndef __AMC_UI_MODULE_ITEM
#define __AMC_UI_MODULE_ITEM

#include "header_protection.hpp"
#include "amc_jsonwriter.hpp"


namespace AMC {

	amcDeclareDependingClass(CUIModule_Item, PUIModule_Item);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);

	class CUIModuleItem {
	protected:		

		std::string m_sItemPath;
	
	public:

		CUIModuleItem(const std::string& sItemPath);		

		virtual ~CUIModuleItem();

		virtual void configurePostLoading();
	
		virtual std::string getItemPath();

		virtual std::string getUUID () = 0;

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler * pClientVariableHandler) = 0;

		virtual void setEventPayloadValue (const std::string & sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler);

	};

}


#endif //__AMC_UI_MODULE_ITEM

