/*++

Copyright (C) 2026 Autodesk Inc.

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


#ifndef __AMC_UI_MODULE_CONTENTITEM_PARTLIST
#define __AMC_UI_MODULE_CONTENTITEM_PARTLIST

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"

#include "pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentPartList, PUIModule_ContentPartList);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);


	// A part-list content item shows the build details (name, layer count, layer
	// thickness, file size, thumbnail) and the part table of a build job. The
	// heavy lifting happens on the client, which fetches "api/build/<builduuid>"
	// whenever the "builduuid" attribute changes; this backend item only exposes
	// the build UUID (typically driven by a sync: expression) plus a few captions.
	// showdetails="false" hides the build header and only shows the part table.
	// Clicking a part row triggers the optional "selectevent"; the event handler reads
	// the clicked part's build item UUID from the "selecteduuid" UI property.
	// The "partstateversion" attribute changes whenever parts of the build are disabled
	// or re-enabled, so the client knows when to re-fetch the part table.
	class CUIModule_ContentPartList : public CUIModule_ContentItem {
	protected:

		std::string m_sBuildUUID;
		CUIExpression m_BuildUUIDExpression;
		CUIExpression m_LoadingText;
		CUIExpression m_ShowDetails;
		std::string m_sSelectEvent;
		std::string m_sSelectedPartFieldUUID;
		PUIModuleEnvironment m_pUIModuleEnvironment;

	public:

		static PUIModule_ContentPartList makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentPartList(
			const std::string& sBuildUUID, const CUIExpression& buildUUIDExpression,
			const CUIExpression& loadingText, const CUIExpression& showDetails, const std::string& sSelectEvent,
			const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		virtual ~CUIModule_ContentPartList();

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler) override;

		virtual void setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler) override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		virtual std::list <std::string> getReferenceUUIDs() override;

		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;

	};

}


#endif //__AMC_UI_MODULE_CONTENTITEM_PARTLIST
