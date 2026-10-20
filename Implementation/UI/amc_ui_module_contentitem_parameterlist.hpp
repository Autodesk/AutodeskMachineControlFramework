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

#include <cstdint>
#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"

#include "amc_ui_module_contentitem.hpp"

#include "pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CUIModule_ContentParameterList, PUIModule_ContentParameterList);
	amcDeclareDependingClass(CUIModule_ContentParameterListEntry, PUIModule_ContentParameterListEntry);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);
	amcDeclareDependingClass(CParameterGroup, PParameterGroup);

	class CUIModule_ContentParameterListEntry {
	private:
		std::string m_sInstance;
		std::string m_sParameterGroup;
		std::string m_sParameter;

		// Optional inline-editing configuration. When m_bEditable is set, the
		// frontend renders the value cell as an editable field and triggers the
		// list's edit event. min/max/step are advisory bounds (empty = unbounded).
		bool m_bEditable;
		std::string m_sMin;
		std::string m_sMax;
		std::string m_sStep;
	public:

		CUIModule_ContentParameterListEntry(const std::string & sInstance, const std::string & sParameterGroup, const std::string & sParameter, bool bEditable = false, const std::string & sMin = "", const std::string & sMax = "", const std::string & sStep = "");
		~CUIModule_ContentParameterListEntry();

		std::string getInstance ();
		std::string getParameterGroup ();
		std::string getParameter ();

		bool isEditable();
		std::string getMin();
		std::string getMax();
		std::string getStep();

		bool isFullGroup();
		bool isFullInstance();

	};


	class CUIModule_ContentParameterList : public CUIModule_ContentItem {
	protected:

		std::vector<PUIModule_ContentParameterListEntry> m_List;

		std::string m_sLoadingText;
		std::string m_sParameterDescCaption;
		std::string m_sParameterValueCaption;
		std::string m_sParameterGroupCaption;
		std::string m_sParameterSystemCaption;

		// UI event triggered by the frontend when a user commits an inline edit.
		// Empty disables editing for the whole list.
		std::string m_sEditEvent;

		// Stable, restart-invariant identifier used by the frontend to scope
		// per-user view preferences (favorites, sorting, filters, presets) to
		// this specific list. Defaults to the config-derived item path; can be
		// overridden via the optional "preferencekey" (or "name") attribute.
		std::string m_sPreferenceKey;

		// Per-column configuration. Columns are addressed by the canonical
		// identifiers "parameter", "value", "group" and "system" via <column>
		// subnodes. width is a raw CSS length (empty or "auto" = flexible),
		// sizeable enables drag-resizing on the frontend (Svelte only).
		struct sColumnConfig {
			bool visible = true;
			std::string width;
			bool sizeable = false;
		};
		sColumnConfig m_ColumnDescription;
		sColumnConfig m_ColumnValue;
		sColumnConfig m_ColumnGroup;
		sColumnConfig m_ColumnSystem;

		uint32_t m_nEntriesPerPage;

		PStateMachineData m_pStateMachineData;

		// One displayed row. Rows of full groups are addressed by index, single parameter rows by name.
		struct sParameterListRow {
			PParameterGroup m_pGroup;
			uint32_t m_nParameterIndex;
			std::string m_sParameterName;
			std::string m_sInstanceName;
			std::string m_sSystemDescription;
			CUIModule_ContentParameterListEntry* m_pEntry;
		};

		// Resolves the configured entries into rows. The signature changes whenever the row layout changes.
		void collectRows(std::vector<sParameterListRow>& rows, std::string& sDefinitionSignature);

		std::string readRowValue(const sParameterListRow& row);

		void writeRowDefinitionToJSON(CJSONWriter& writer, CJSONWriterArray& entryArray, const sParameterListRow& row);

		// Parses a comma separated list of row indices and index ranges (e.g. "0-24,30").
		static std::vector<uint32_t> parseRowIndices(const std::string& sRowIndices, size_t nRowCount);

		// Emits the ordered per-column configuration array (identifier, frontend
		// value key, caption, visible, width, sizeable) consumed by both clients.
		void writeColumnsToJSON(CJSONWriter& writer, CJSONWriterObject& object);

	public:

		static PUIModule_ContentParameterList makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_ContentParameterList(const std::string & sLoadingText, const uint32_t nEntriesPerPage, PStateMachineData pStateMachineData, const std::string& sItemName, const std::string& sModulePath);

		virtual ~CUIModule_ContentParameterList();

		void addEntry(const std::string& sInstance, const std::string& sParameterGroup, const std::string& sParameter, bool bEditable = false, const std::string& sMin = "", const std::string& sMax = "", const std::string& sStep = "");

		uint32_t getEntryCount();

		CUIModule_ContentParameterListEntry* getEntry(const uint32_t nIndex);

		void loadFromXML(const pugi::xml_node& xmlNode);

		// New UI Frontend System
		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;

		// Writes the static row definitions and the current values of all rows.
		void writeDefinitionToJSON(CJSONWriter& writer);

		// Writes the current values of the requested rows (all rows if sRowIndices is empty), in request order.
		// Only signals a changed definition if sKnownDefinitionHash is outdated, and omits the values if sKnownValuesHash is still current.
		void writeValuesToJSON(CJSONWriter& writer, const std::string& sKnownDefinitionHash, const std::string& sKnownValuesHash, const std::string& sRowIndices);
	};



}


#endif //__AMC_UI_MODULE_CONTENTITEM_PARAMETERLIST

