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


#ifndef __AMC_UI_MODULE_CONTENTITEM_PARAMETERLIST
#define __AMC_UI_MODULE_CONTENTITEM_PARAMETERLIST

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"

#include "amc_ui_module_contentitem.hpp"

namespace AMC {

	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CUIModule_ContentParameterListEntry, PUIModule_ContentParameterListEntry);
	
	class CUIModule_ContentParameterListEntry {
	private:
		std::string m_sStateMachine;
		std::string m_sParameterGroup;
		std::string m_sParameter;
	public:

		CUIModule_ContentParameterListEntry(const std::string & sStateMachine, const std::string & sParameterGroup, const std::string & sParameter);
		~CUIModule_ContentParameterListEntry();

		std::string getStateMachine ();
		std::string getParameterGroup ();
		std::string getParameter ();

		bool isFullGroup();

	};


	class CUIModule_ContentParameterList : public CUIModule_ContentItem {
	protected:

		std::vector<PUIModule_ContentParameterListEntry> m_List;

		std::string m_sLoadingText;
		std::string m_sParameterDescCaption;
		std::string m_sParameterValueCaption;
		std::string m_sParameterGroupCaption;
		std::string m_sParameterSystemCaption;

		uint32_t m_nEntriesPerPage;

		PStateMachineData m_pStateMachineData;

	public:

		CUIModule_ContentParameterList(const std::string & sLoadingText, const uint32_t nEntriesPerPage, PStateMachineData pStateMachineData);

		virtual ~CUIModule_ContentParameterList();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

		void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

		void addEntry(const std::string& sStateMachine, const std::string& sParameterGroup, const std::string& sParameter);

		uint32_t getEntryCount();

		CUIModule_ContentParameterListEntry* getEntry(const uint32_t nIndex);

		void loadFromXML(pugi::xml_node& xmlNode);

	};



}


#endif //__AMC_UI_MODULE_CONTENTITEM_PARAMETERLIST

