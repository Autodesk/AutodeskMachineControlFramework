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


#ifndef __AMC_UI_MODULE_GRID
#define __AMC_UI_MODULE_GRID

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"

namespace LibMCData {
	amcDeclareDependingClass(CBuildJobHandler, PBuildJobHandler);
}


namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModule_Grid, PUIModule_Grid);
	amcDeclareDependingClass(CUIModule_GridSection, PUIModule_GridSection);
	amcDeclareDependingClass(CUIModule_GridRow, PUIModule_GridRow);
	amcDeclareDependingClass(CUIModule_GridColumn, PUIModule_GridColumn);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CResourcePackage, PResourcePackage);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);


	enum class eUIModule_GridColumnPosition {
		gcpUnknown, gcpFull, gcpLeft, gcpCentered, gcpRight
	};

	enum class eUIModule_GridRowPosition {
		crpUnknown, grpFull, grpTop, grpCentered, grpBottom
	};

	class CUIModule_GridColumn {
	private:
		float m_Width;
		std::string m_sUnit;
	public:
		CUIModule_GridColumn(float fWidth, const std::string & sUnit);

		float getWidth();
		std::string getWidthUnitString();

		static std::string gridPositionToString(eUIModule_GridColumnPosition ePosition);
		static eUIModule_GridColumnPosition stringToGridPosition(const std::string& sString);
	};

	class CUIModule_GridRow {
	private:
		float m_Height;
		std::string m_sUnit;
	public:
		CUIModule_GridRow(float fHeight, const std::string& sUnit);

		float getHeight();
		std::string getHeightUnitString();

		static std::string gridPositionToString(eUIModule_GridRowPosition ePosition);
		static eUIModule_GridRowPosition stringToGridPosition(const std::string& sString);

	};

	class CUIModule_GridSection {
	private:
		PUIModule m_pModule;
		int m_nColumnStart, m_nColumnEnd, m_nRowStart, m_nRowEnd;
		eUIModule_GridColumnPosition m_ColumnPosition;
		eUIModule_GridRowPosition m_RowPosition;

	public:
		CUIModule_GridSection(PUIModule pModule, int nColumnStart, int nColumnEnd, int nRowStart, int nRowEnd, eUIModule_GridColumnPosition columnPosition, eUIModule_GridRowPosition rowPosition);

		CUIModule * getModule();

		int getColumnStart();
		int getColumnEnd();
		int getRowStart();
		int getRowEnd ();

		eUIModule_GridColumnPosition getColumnPosition ();
		std::string getColumnPositionString();
		eUIModule_GridRowPosition getRowPosition ();
		std::string getRowPositionString();

	};


	class CUIModule_Grid : public CUIModule {
	protected:		

		std::string m_sCaption;

		std::map<std::string, PUIModuleItem> m_ItemMap;
		std::map<std::string, PUIModule_GridSection> m_SectionMap;
		std::vector<PUIModule_GridSection> m_SectionList;

		std::vector<PUIModule_GridRow> m_Rows;
		std::vector<PUIModule_GridColumn> m_Columns;

		void addSection(PUIModule pModule, int nColumnStart, int nColumnEnd, int nRowStart, int nRowEnd, eUIModule_GridColumnPosition columnPosition, eUIModule_GridRowPosition rowPosition);

	public:

		CUIModule_Grid(pugi::xml_node & xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment);
		
		virtual ~CUIModule_Grid();

		virtual std::string getType() override;

		static std::string getStaticType();

		std::string getCaption () override;

		virtual void writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler) override;

		virtual void populateItemMap(std::map<std::string, PUIModuleItem>& itemMap) override;

		virtual PUIModuleItem findItem(const std::string& sUUID) override;
		PUIModule_GridSection findSection(const std::string& sUUID);

		void configurePostLoading() override;

		virtual void populateClientVariables(CParameterHandler* pParameterHandler) override;


	};

	
}


#endif //__AMC_UI_MODULE_GRID

