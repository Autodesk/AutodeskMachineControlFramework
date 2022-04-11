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
#include "amc_ui_module_item.hpp"
#include "amc_ui_modulefactory.hpp"

#include "amc_ui_module_logs.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"

#include "common_utils.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;

CUIModule_LogsItem::CUIModule_LogsItem (const std::string& sItemPath, PUIModuleEnvironment pUIModuleEnvironment)
  : CUIModuleItem(sItemPath), m_pUIModuleEnvironment(pUIModuleEnvironment), m_sUUID(AMCCommon::CUtils::createUUID())
{
	LibMCAssertNotNull(m_pUIModuleEnvironment);
}


std::string CUIModule_LogsItem::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_LogsItem::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return getItemPath();

	return "";
}

void CUIModule_LogsItem::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler)
{
	auto pGroup = pClientVariableHandler->findGroup(getItemPath (), true);

	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData ();

	

}

void CUIModule_LogsItem::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{

}

void CUIModule_LogsItem::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();
/*	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "layer view");
	pGroup->addNewUUIDParameter(AMC_API_KEY_UI_BUILDUUID, "Build UUID", AMCCommon::CUtils::createEmptyUUID());
	pGroup->addNewIntParameter(AMC_API_KEY_UI_CURRENTLAYER, "Current layer index", 0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_SIZEX, "Platform size x", m_SizeX.evaluateNumberValue (pStateMachineData), 1.0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_SIZEY, "Platform size y", m_SizeY.evaluateNumberValue(pStateMachineData), 1.0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_ORIGINX, "Platform origin x", m_OriginX.evaluateNumberValue(pStateMachineData), 1.0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_ORIGINY, "Platform origin y", m_OriginY.evaluateNumberValue(pStateMachineData), 1.0);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_BASEIMAGERESOURCE, "Platform base image", m_BaseImage.evaluateStringValue(pStateMachineData)); */

}



CUIModule_Logs::CUIModule_Logs(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode))
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	m_sModulePath = sPath + "." + m_sName;

	auto captionAttrib = xmlNode.attribute("caption");
	m_sCaption = captionAttrib.as_string();
	
	m_LogsItem = std::make_shared<CUIModule_LogsItem>(m_sModulePath, pUIModuleEnvironment);

}


CUIModule_Logs::~CUIModule_Logs()
{
}



std::string CUIModule_Logs::getStaticType()
{
	return "logs";
}

std::string CUIModule_Logs::getType()
{
	return getStaticType();
}

std::string CUIModule_Logs::getCaption()
{
	return m_sCaption;
}


void CUIModule_Logs::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

/*	CJSONWriterArray itemsNode(writer);
	CJSONWriterObject itemObject(writer);
	itemObject.addString(AMC_API_KEY_UI_ITEMTYPE, "platform");
	itemObject.addString(AMC_API_KEY_UI_ITEMUUID, m_PlatformItem->getUUID ());
	m_PlatformItem->addContentToJSON(writer, itemObject, pClientVariableHandler);
	itemsNode.addObject(itemObject);

	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode); */

}

PUIModuleItem CUIModule_Logs::findItem(const std::string& sUUID)
{
	if (m_LogsItem->getUUID() == sUUID)
		return m_LogsItem;

	return nullptr;
}

void CUIModule_Logs::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	itemMap.insert (std::make_pair (m_LogsItem->getUUID (), m_LogsItem));
}

void CUIModule_Logs::configurePostLoading()
{
}


void CUIModule_Logs::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	m_LogsItem->populateClientVariables(pParameterHandler);


}

