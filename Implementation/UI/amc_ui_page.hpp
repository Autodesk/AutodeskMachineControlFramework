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


#ifndef __AMC_UI_PAGE
#define __AMC_UI_PAGE

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_PAGE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Core/amc_jsonwriter.hpp"

#include <vector>
#include <map>

#include "Libraries/PugiXML/pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIPage, PUIPage);	
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);

	class CUIPage {
	protected:
		std::string m_sName;

		std::vector<PUIModule> m_Modules;
		std::map <std::string, PUIModule> m_ModuleMap;

	public:

		CUIPage(const std::string & sName);
		
		virtual ~CUIPage();

		std::string getName();

		void addModule (PUIModule pModule);

		PUIModule findModule (const std::string & sName);
		uint32_t getModuleCount();
		PUIModule getModule (const uint32_t nIndex);

		void writeModulesToJSON(CJSONWriter & writer, CJSONWriterArray & moduleArray);

		PUIModuleItem findModuleItem(const std::string& sUUID);
										
	};
		
}


#endif //__AMC_UI_MENUITEM

