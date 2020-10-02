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


#ifndef __AMC_UI_MODULE_CONTENTITEM
#define __AMC_UI_MODULE_CONTENTITEM

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"

#include "amc_parameterhandler.hpp"
#include "amc_jsonwriter.hpp"
#include "amc_ui_module_item.hpp"
#include "amc_systemstate.hpp"

#define AMC_CONTENT_MAXENTRYCOUNT (1024 * 1024)

namespace LibMCData {
	amcDeclareDependingClass(CBuildJobHandler, PBuildJobHandler);
}

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModule_Item, PUIModule_Item);
	amcDeclareDependingClass(CUIModule_ContentItem, PUIModule_ContentItem);
	amcDeclareDependingClass(CUIModule_ContentParagraph, PUIModule_ContentParagraph);
	amcDeclareDependingClass(CUIModule_ContentImage, PUIModule_ContentImage);
	amcDeclareDependingClass(CUIModule_ContentUpload, PUIModule_ContentUpload);
	amcDeclareDependingClass(CParameterInstances, PParameterInstances);

	class CUIModule_ContentItem : public CUIModuleItem {
	protected:		

		std::string m_sUUID;

	public:

		CUIModule_ContentItem(const std::string & sUUID);
		
		virtual ~CUIModule_ContentItem();

		virtual std::string getUUID () override;

		virtual void addDefinitionToJSON (CJSONWriter & writer, CJSONWriterObject & object);

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};

	class CUIModule_ContentParagraph : public CUIModule_ContentItem {
	protected:		

		std::string m_sText;

	public:

		CUIModule_ContentParagraph(const std::string & sText);
		
		virtual ~CUIModule_ContentParagraph();

		std::string getText ();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};


	class CUIModule_ContentImage : public CUIModule_ContentItem {
	protected:		
		double m_dAspectRatio;

	public:

		CUIModule_ContentImage(const std::string & sUUID, double dAspectRatio);
		
		virtual ~CUIModule_ContentImage();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};

	class CUIModule_ContentUpload : public CUIModule_ContentItem {
	protected:
		std::string m_sUploadClass;
		std::string m_sUploadCaption;

	public:

		CUIModule_ContentUpload(const std::string& sUploadClass, const std::string& sUploadCaption);

		virtual ~CUIModule_ContentUpload();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};
	
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

	typedef std::shared_ptr<CUIModule_ContentParameterListEntry> PUIModule_ContentParameterListEntry;


	class CUIModule_ContentParameterList : public CUIModule_ContentItem {
	protected:

		std::vector<PUIModule_ContentParameterListEntry> m_List;

		std::string m_sLoadingText;
		std::string m_sParameterDescCaption;
		std::string m_sParameterValueCaption;
		std::string m_sParameterGroupCaption;
		std::string m_sParameterSystemCaption;

		uint32_t m_nEntriesPerPage;

		PParameterInstances m_pParameterInstances;

	public:

		CUIModule_ContentParameterList(const std::string & sLoadingText, const uint32_t nEntriesPerPage, PParameterInstances pParameterInstances);

		virtual ~CUIModule_ContentParameterList();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

		void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

		void addEntry(const std::string& sStateMachine, const std::string& sParameterGroup, const std::string& sParameter);

		uint32_t getEntryCount();

		CUIModule_ContentParameterListEntry* getEntry(const uint32_t nIndex);

		void loadFromXML(pugi::xml_node& xmlNode);

	};


	class CUIModule_ContentBuildList : public CUIModule_ContentItem {
	protected:

		std::string m_sLoadingText;
		std::string m_sBuildNameCaption;
		std::string m_sBuildLayersCaption;
		std::string m_sBuildUUIDCaption;

		uint32_t m_nEntriesPerPage;

		PParameterInstances m_pParameterInstances;

		LibMCData::PBuildJobHandler m_pBuildJobHandler;

	public:

		CUIModule_ContentBuildList(const std::string& sLoadingText, const uint32_t nEntriesPerPage, LibMCData::PBuildJobHandler pBuildJobHandler);

		virtual ~CUIModule_ContentBuildList();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

		void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;

	};


}


#endif //__AMC_UI_MODULE_CONTENTITEM

