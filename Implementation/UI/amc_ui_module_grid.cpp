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

#define __AMCIMPL_UI_MODULE
#define __AMCIMPL_API_CONSTANTS

#include "amc_ui_module.hpp"
#include "amc_ui_modulefactory.hpp"

#include "amc_ui_module_grid.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"

#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

using namespace AMC;


CUIModule_GridColumn::CUIModule_GridColumn(float fWidth, const std::string& sUnit)
	: m_Width (fWidth), m_sUnit (sUnit)
{

}


float CUIModule_GridColumn::getWidth()
{
	return m_Width;
}

std::string CUIModule_GridColumn::getWidthUnitString()
{
	return m_sUnit;
}


std::string CUIModule_GridColumn::gridPositionToString(eUIModule_GridColumnPosition ePosition)
{
	switch (ePosition) {
	case eUIModule_GridColumnPosition::gcpCentered: return "centered";
	case eUIModule_GridColumnPosition::gcpFull: return "full";
	case eUIModule_GridColumnPosition::gcpLeft: return "left";
	case eUIModule_GridColumnPosition::gcpRight: return "right";
	default:
		return "";
	}
}

eUIModule_GridColumnPosition CUIModule_GridColumn::stringToGridPosition(const std::string& sString)
{
	std::string sTrimmedString = AMCCommon::CUtils::toLowerString (AMCCommon::CUtils::trimString(sString));
	if (sTrimmedString == "centered") 
		return eUIModule_GridColumnPosition::gcpCentered;
	if (sTrimmedString == "full")
		return eUIModule_GridColumnPosition::gcpFull;
	if (sTrimmedString == "left")
		return eUIModule_GridColumnPosition::gcpLeft;
	if (sTrimmedString == "right")
		return eUIModule_GridColumnPosition::gcpRight;

	throw ELibMCCustomException(LIBMC_ERROR_INVALIDGRIDPOSITION, sString);
}


CUIModule_GridRow::CUIModule_GridRow(float fHeight, const std::string& sUnit)
	: m_Height (fHeight), m_sUnit (sUnit)
{
	
}

float CUIModule_GridRow::getHeight()
{
	return m_Height;
}

std::string CUIModule_GridRow::getHeightUnitString()
{
	return m_sUnit;
}

std::string CUIModule_GridRow::gridPositionToString(eUIModule_GridRowPosition ePosition)
{
	switch (ePosition) {
	case eUIModule_GridRowPosition::grpCentered: return "centered";
	case eUIModule_GridRowPosition::grpFull: return "full";
	case eUIModule_GridRowPosition::grpTop: return "top";
	case eUIModule_GridRowPosition::grpBottom: return "bottom";
	default:
		return "";
	}

}

eUIModule_GridRowPosition CUIModule_GridRow::stringToGridPosition(const std::string& sString)
{
	std::string sTrimmedString = AMCCommon::CUtils::toLowerString(AMCCommon::CUtils::trimString(sString));
	if (sTrimmedString == "centered")
		return eUIModule_GridRowPosition::grpCentered;
	if (sTrimmedString == "full")
		return eUIModule_GridRowPosition::grpFull;
	if (sTrimmedString == "top")
		return eUIModule_GridRowPosition::grpTop;
	if (sTrimmedString == "bottom")
		return eUIModule_GridRowPosition::grpBottom;

	throw ELibMCCustomException(LIBMC_ERROR_INVALIDGRIDPOSITION, sString);

}


CUIModule_GridSection::CUIModule_GridSection(PUIModule pModule, int nColumnStart, int nColumnEnd, int nRowStart, int nRowEnd, eUIModule_GridColumnPosition columnPosition, eUIModule_GridRowPosition rowPosition, bool bScrollbars)
	: m_pModule (pModule),
	m_nColumnStart (nColumnStart),
	m_nColumnEnd (nColumnEnd),
	m_nRowStart (nRowStart),
	m_nRowEnd (nRowEnd),
	m_ColumnPosition (columnPosition),
	m_RowPosition (rowPosition),
	m_bScrollbars (bScrollbars)
{
	LibMCAssertNotNull(pModule.get ());
}

CUIModule* CUIModule_GridSection::getModule()
{
	return m_pModule.get();
}

int CUIModule_GridSection::getColumnStart()
{
	return m_nColumnStart;
}

int CUIModule_GridSection::getColumnEnd()
{
	return m_nColumnEnd;
}

int CUIModule_GridSection::getRowStart()
{
	return m_nRowStart;
}

int CUIModule_GridSection::CUIModule_GridSection::getRowEnd()
{
	return m_nRowEnd;
}

bool CUIModule_GridSection::getScrollbars()
{
	return m_bScrollbars;
}


eUIModule_GridColumnPosition CUIModule_GridSection::getColumnPosition()
{
	return m_ColumnPosition;
}

std::string CUIModule_GridSection::getColumnPositionString()
{
	return CUIModule_GridColumn::gridPositionToString(m_ColumnPosition);
}

eUIModule_GridRowPosition CUIModule_GridSection::getRowPosition()
{
	return m_RowPosition;
}

std::string CUIModule_GridSection::getRowPositionString()
{
	return CUIModule_GridRow::gridPositionToString(m_RowPosition);
}

CUIModule_Grid::CUIModule_Grid(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode))
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	auto children = xmlNode.children();
	for (auto childNode : children) {

		std::string sChildName = childNode.name();
		if (sChildName == "column") {
			auto columnWidthAttrib = childNode.attribute("width");
			if (columnWidthAttrib.empty())
				throw ELibMCCustomException (LIBMC_ERROR_MISSINGCOLUMNWIDTH, m_sName);

			double columnWidth = columnWidthAttrib.as_double(0.0);
			if (columnWidth <= 0.0)
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDCOLUMNWIDTH, m_sName);

			auto unitAttrib = childNode.attribute("unit");
			if (unitAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGCOLUMNUNIT, m_sName);
			std::string sUnit = unitAttrib.as_string();


			m_Columns.push_back(std::make_shared<CUIModule_GridColumn> ((float) columnWidth, sUnit));
		}

		if (sChildName == "row") {
			auto rowHeightAttrib = childNode.attribute("height");
			if (rowHeightAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGROWHEIGHT, m_sName);

			double rowHeight = rowHeightAttrib.as_double(0.0);
			if (rowHeight <= 0.0)
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDROWHEIGHT, m_sName);

			auto unitAttrib = childNode.attribute("unit");
			if (unitAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGROWHEIGHT, m_sName);
			std::string sUnit = unitAttrib.as_string();

			m_Rows.push_back(std::make_shared<CUIModule_GridRow>((float) rowHeight, sUnit));
		}

	}

	for (auto childNode : children) {

		std::string sChildName = childNode.name();
		if (CUIModuleFactory::moduleTypeIsRegistered (sChildName)) {

			auto columnStartAttrib = childNode.attribute("columnstart");
			if (columnStartAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGCOLUMNSTARTATTRIB, m_sName);

			auto columnEndAttrib = childNode.attribute("columnend");
			if (columnEndAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGCOLUMNENDATTRIB, m_sName);

			auto rowStartAttrib = childNode.attribute("rowstart");
			if (rowStartAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGROWSTARTATTRIB, m_sName);

			auto rowEndAttrib = childNode.attribute("rowend");
			if (rowEndAttrib.empty())
				throw ELibMCCustomException(LIBMC_ERROR_MISSINGROWENDATTRIB, m_sName);

			int nColumnStart = columnStartAttrib.as_int(-1);
			int nColumnEnd = columnEndAttrib.as_int(-1);
			int nRowStart = rowStartAttrib.as_int(-1);
			int nRowEnd = rowEndAttrib.as_int(-1);

			if (nColumnStart < 0)
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDCOLUMNSTARTATTRIB, m_sName);
			if (nColumnEnd < nColumnStart)
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDCOLUMNENDATTRIB, m_sName); 
			if (nRowStart < 0)
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDROWSTARTATTRIB, m_sName);
			if (nRowEnd < nRowStart)
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDROWENDATTRIB, m_sName); 

			eUIModule_GridColumnPosition eColumnPosition = eUIModule_GridColumnPosition::gcpFull;
			std::string sColumnPosition = childNode.attribute("columnposition").as_string();
			if (!sColumnPosition.empty())
				eColumnPosition = CUIModule_GridColumn::stringToGridPosition(sColumnPosition);

			eUIModule_GridRowPosition eRowPosition = eUIModule_GridRowPosition::grpFull;
			std::string sRowPosition = childNode.attribute("rowposition").as_string();
			if (!sRowPosition.empty())
				eRowPosition = CUIModule_GridRow::stringToGridPosition(sRowPosition);

			bool bScrollbars = false;
			auto scrollbarsAttrib = childNode.attribute("scrollbars");
			if (!scrollbarsAttrib.empty())
				bScrollbars = scrollbarsAttrib.as_bool();


			auto pSection = CUIModuleFactory::createModule(childNode, sPath, pUIModuleEnvironment);
			addSection (pSection, nColumnStart, nColumnEnd, nRowStart, nRowEnd, eColumnPosition, eRowPosition, bScrollbars);

		}

	}	

}


CUIModule_Grid::~CUIModule_Grid()
{
}



std::string CUIModule_Grid::getStaticType()
{
	return "grid";
}

std::string CUIModule_Grid::getType()
{
	return getStaticType();
}

std::string CUIModule_Grid::getCaption()
{
	return m_sCaption;
}


void CUIModule_Grid::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	CJSONWriterArray columnsNode(writer);
	for (auto column : m_Columns) {
		CJSONWriterObject columnObject(writer);
		columnObject.addDouble(AMC_API_KEY_UI_COLUMNWIDTH, column->getWidth ());
		columnObject.addString(AMC_API_KEY_UI_COLUMNWIDTHUNIT, column->getWidthUnitString());
		columnsNode.addObject(columnObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_COLUMNS, columnsNode);


	CJSONWriterArray rowsNode(writer);
	for (auto row : m_Rows) {
		CJSONWriterObject rowObject(writer);
		rowObject.addDouble(AMC_API_KEY_UI_ROWHEIGHT, row->getHeight());
		rowObject.addString(AMC_API_KEY_UI_ROWHEIGHTUNIT, row->getHeightUnitString());
		rowsNode.addObject(rowObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_ROWS, rowsNode);


	CJSONWriterArray sectionsNode(writer);
	for (auto section : m_SectionList) {
		CJSONWriterObject sectionObject(writer);
		section->getModule()->writeDefinitionToJSON (writer, sectionObject, pClientVariableHandler);
		sectionObject.addInteger(AMC_API_KEY_UI_COLUMNSTART, section->getColumnStart ());
		sectionObject.addInteger(AMC_API_KEY_UI_COLUMNEND, section->getColumnEnd ());
		sectionObject.addInteger(AMC_API_KEY_UI_ROWSTART, section->getRowStart());
		sectionObject.addInteger(AMC_API_KEY_UI_ROWEND, section->getRowEnd());
		sectionObject.addBool(AMC_API_KEY_UI_SCROLLBARS, section->getScrollbars());
		sectionObject.addString(AMC_API_KEY_UI_COLUMNPOSITION, CUIModule_GridColumn::gridPositionToString(section->getColumnPosition()));
		sectionObject.addString(AMC_API_KEY_UI_ROWPOSITION, CUIModule_GridRow::gridPositionToString(section->getRowPosition()));
		sectionsNode.addObject(sectionObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_SECTIONS, sectionsNode);

}

PUIModuleItem CUIModule_Grid::findItem(const std::string& sUUID)
{
	auto iIter = m_ItemMap.find(sUUID);
	if (iIter != m_ItemMap.end())
		return iIter->second;

	return nullptr;
}

PUIModule_GridSection CUIModule_Grid::findSection(const std::string& sUUID)
{
	auto iIter = m_SectionMap.find(sUUID);
	if (iIter != m_SectionMap.end())
		return iIter->second;

	return nullptr;
}

void CUIModule_Grid::addSection(PUIModule pModule, int nColumnStart, int nColumnEnd, int nRowStart, int nRowEnd, eUIModule_GridColumnPosition columnPosition, eUIModule_GridRowPosition rowPosition, bool bScrollbars)
{
	LibMCAssertNotNull(pModule.get());
	auto pSection = std::make_shared<CUIModule_GridSection>(pModule, nColumnStart, nColumnEnd, nRowStart, nRowEnd, columnPosition, rowPosition, bScrollbars);

	m_SectionList.push_back(pSection);
	m_SectionMap.insert(std::make_pair (pSection->getModule ()->getUUID (), pSection));

	pSection->getModule()->populateItemMap(m_ItemMap);
}

void CUIModule_Grid::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto pSection : m_SectionList)
		pSection->getModule()->populateItemMap(itemMap);
}

void CUIModule_Grid::configurePostLoading()
{
	for (auto pSection : m_SectionList)
		pSection->getModule()->configurePostLoading();
}


void CUIModule_Grid::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);
	for (auto pSection : m_SectionList)
		pSection->getModule()->populateClientVariables(pParameterHandler);

}
