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


#ifndef __AMC_UI_MENUITEM
#define __AMC_UI_MENUITEM

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MENUITEM
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

namespace AMC {

	amcDeclareDependingClass(CUIPage, PUIPage);

	class CUIMenuItem {
	protected:
		std::string m_sID;
		std::string m_sIcon;
		std::string m_sCaption;

		std::string m_sPageName;
		std::string m_sEventName;
		
		std::string m_sDescription;

	public:

		CUIMenuItem(const std::string & sID, const std::string & sIcon, const std::string & sCaption, const std::string & sDescription, const std::string& sPageName, const std::string & sEventName);
		
		virtual ~CUIMenuItem();
						
		std::string getID ();
		std::string getIcon ();
		std::string getCaption ();
		std::string getDescription();
		std::string getEventName();
		std::string getPageName();
	};
	
	typedef std::shared_ptr<CUIMenuItem> PUIMenuItem;
	
}


#endif //__AMC_UI_MENUITEM

