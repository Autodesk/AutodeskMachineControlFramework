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

#include "amc_ui_module_graphic.hpp"
#include "amc_ui_module_graphicitem.hpp"
#include "amc_ui_module_graphicitem_image.hpp"
#include "amc_ui_module_graphicitem_svgimage.hpp"

#include "common_utils.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

#define GRAPHIC_INVALIDVIEWCOORD -2.0E6
#define GRAPHIC_MINVIEWCOORD -1.0E6
#define GRAPHIC_MAXVIEWCOORD +1.0E6

CUIModule_Graphic::CUIModule_Graphic(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode)), m_nNamingIDCounter (1), m_dMinX (0.0), m_dMinY (0.0), m_dMaxX (100.0), m_dMaxY (100.0)
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())		
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	m_sModulePath = sPath + "." + m_sName;

	auto children = xmlNode.children();
	for (auto childNode : children) {
		std::string sChildName = childNode.name();
		auto sItemName = readItemNameFromXML(childNode, sChildName);

		if (sChildName == "image")
			addItem(CUIModule_GraphicImage::makeFromXML(childNode, sItemName, m_sModulePath, pUIModuleEnvironment));
		if (sChildName == "svgimage")
			addItem(CUIModule_GraphicSVGImage::makeFromXML(childNode, sItemName, m_sModulePath, pUIModuleEnvironment));
		if (sChildName == "view") {
			readViewPort(childNode);

		}
	}

}


CUIModule_Graphic::~CUIModule_Graphic()
{
}



std::string CUIModule_Graphic::getStaticType()
{
	return "graphic";
}

std::string CUIModule_Graphic::getType()
{
	return getStaticType();
}

std::string CUIModule_Graphic::getCaption()
{
	return m_sCaption;
}


void CUIModule_Graphic::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	moduleObject.addDouble(AMC_API_KEY_UI_VIEWMINX, m_dMinX);
	moduleObject.addDouble(AMC_API_KEY_UI_VIEWMINY, m_dMinY);
	moduleObject.addDouble(AMC_API_KEY_UI_VIEWMAXX, m_dMaxX);
	moduleObject.addDouble(AMC_API_KEY_UI_VIEWMAXY, m_dMaxY);

	CJSONWriterArray itemsNode(writer);
	for (auto item : m_Items) {
		CJSONWriterObject itemObject(writer);
		item->addDefinitionToJSON(writer, itemObject, pClientVariableHandler);
		itemsNode.addObject(itemObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode);

}

PUIModuleItem CUIModule_Graphic::findItem(const std::string& sUUID)
{
	auto iIter = m_ItemMap.find(sUUID);
	if (iIter != m_ItemMap.end())
		return iIter->second;

	return nullptr;
}

void CUIModule_Graphic::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto item : m_Items) {
		auto referenceList = item->getReferenceUUIDs();
		for (auto sUUID : referenceList)
			itemMap.insert(std::make_pair(sUUID, item));

	}
}

void CUIModule_Graphic::configurePostLoading()
{
}


void CUIModule_Graphic::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	for (auto pItem : m_Items)
		pItem->populateClientVariables(pParameterHandler);

}

std::string CUIModule_Graphic::getDefaultContentName(const std::string& sPrefix)
{
	if (sPrefix.empty())
		throw ELibMCCustomException(LIBMC_ERROR_EMPTYITEMPREFIX, m_sModulePath);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sPrefix))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDITEMPREFIX, m_sModulePath + "." + sPrefix);


	std::string sName = sPrefix + std::to_string(m_nNamingIDCounter);
	m_nNamingIDCounter++;

	return sName;
}


std::string CUIModule_Graphic::readItemNameFromXML(const pugi::xml_node& itemNode, const std::string& sPrefix)
{
	auto nameAttrib = itemNode.attribute("name");
	std::string sItemName = nameAttrib.as_string();
	if (sItemName.empty())
		sItemName = getDefaultContentName(sPrefix);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sItemName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDITEMPATH, m_sModulePath + "." + sItemName);

	return sItemName;
}


void CUIModule_Graphic::addItem(PUIModuleGraphicItem pItem)
{
	LibMCAssertNotNull(pItem.get());

	m_Items.push_back(pItem);

	auto referenceList = pItem->getReferenceUUIDs();
	for (auto sUUID : referenceList)
		m_ItemMap.insert(std::make_pair(sUUID, pItem));

}


void CUIModule_Graphic::readViewPort(const pugi::xml_node& viewportNode)
{
	auto minXattrib = viewportNode.attribute("minx");
	auto minYattrib = viewportNode.attribute("miny");
	auto maxXattrib = viewportNode.attribute("maxx");
	auto maxYattrib = viewportNode.attribute("maxy");

	if (minXattrib.empty())
		throw ELibMCCustomException(LIBMC_ERROR_VIEWPORTCOORDMISSING, m_sModulePath);
	if (minYattrib.empty())
		throw ELibMCCustomException(LIBMC_ERROR_VIEWPORTCOORDMISSING, m_sModulePath);
	if (maxXattrib.empty())
		throw ELibMCCustomException(LIBMC_ERROR_VIEWPORTCOORDMISSING, m_sModulePath);
	if (maxYattrib.empty())
		throw ELibMCCustomException(LIBMC_ERROR_VIEWPORTCOORDMISSING, m_sModulePath);

	m_dMinX = minXattrib.as_double(-GRAPHIC_INVALIDVIEWCOORD);
	if ((m_dMinX < GRAPHIC_MINVIEWCOORD) || (m_dMinX > GRAPHIC_MAXVIEWCOORD))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDVIEWPORTCOORD, m_sModulePath + " " + minXattrib.as_string());

	m_dMinY = minYattrib.as_double(-GRAPHIC_INVALIDVIEWCOORD);
	if ((m_dMinY < GRAPHIC_MINVIEWCOORD) || (m_dMinY > GRAPHIC_MAXVIEWCOORD))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDVIEWPORTCOORD, m_sModulePath + " " + minYattrib.as_string());

	m_dMaxX = maxXattrib.as_double(-GRAPHIC_INVALIDVIEWCOORD);
	if ((m_dMaxX < GRAPHIC_MINVIEWCOORD) || (m_dMaxX > GRAPHIC_MAXVIEWCOORD))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDVIEWPORTCOORD, m_sModulePath + " " + maxXattrib.as_string());

	m_dMaxY = maxYattrib.as_double(-GRAPHIC_INVALIDVIEWCOORD);
	if ((m_dMaxY < GRAPHIC_MINVIEWCOORD) || (m_dMaxY > GRAPHIC_MAXVIEWCOORD))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDVIEWPORTCOORD, m_sModulePath + " " + maxYattrib.as_string());

	if (m_dMaxX < m_dMinX)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDVIEWPORTCOORD, m_sModulePath);
	if (m_dMaxY < m_dMinY)
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDVIEWPORTCOORD, m_sModulePath);

	

}

