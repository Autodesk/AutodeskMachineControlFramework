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


#ifndef __AMC_UI_HANDLER
#define __AMC_UI_HANDLER

#include "header_protection.hpp"
#include "header_pugixml.hpp"

#include <memory>
#include <vector>
#include <string>

namespace AMC {

	amcDeclareDependingClass(CUIMenuItem, PUIMenuItem);
	amcDeclareDependingClass(CUIToolbarItem, PUIToolbarItem);
	amcDeclareDependingClass(CJSONWriter, PJSONWriter);
	amcDeclareDependingClass(CUIPage, PUIPage);

	class CUIHandler {
	protected:
		std::string m_sAppName;
		std::string m_sCopyrightString;

		std::vector <PUIMenuItem> m_MenuItems;
		std::vector <PUIToolbarItem> m_ToolbarItems;

		std::map <std::pair<std::string, std::string>, PUIPage> m_Pages;

	public:

		CUIHandler();
		
		virtual ~CUIHandler();
		
		std::string getAppName();
		std::string getCopyrightString();

		void addMenuItem (const std::string & sID, const std::string & sIcon, const std::string & sCaption, const std::string & sTargetPage);
		void addToolbarItem(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage);

		PUIPage addPage(const std::string& sInstanceName, const std::string& sName);

		void writeToJSON (CJSONWriter & writer);

		void loadFromXML (pugi::xml_node & xmlNode);
	};
	
	typedef std::shared_ptr<CUIHandler> PUIHandler;
	
}


#endif //__AMC_UI_HANDLER

