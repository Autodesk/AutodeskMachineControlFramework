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

#include "amc_ui_module_custom.hpp"
#include "amc_ui_module_customitem.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"

#include "amc_ui_expression.hpp"

#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

using namespace AMC;


CUIModule_Custom::CUIModule_Custom(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode))
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	m_sModulePath = sPath + "." + m_sName;

	m_pCustomItem = std::make_shared<CUIModuleCustomItem>(m_sUUID, m_sModulePath, pUIModuleEnvironment);

	pugi::xml_node propertiesNode = xmlNode.child("properties");
	if (!propertiesNode.empty()) {
		auto propertyNodes = propertiesNode.children("property");
		for (auto propertyNode : propertyNodes) {
			auto nameAttribute = propertyNode.attribute("name");
			if (nameAttribute.empty ())
				throw ELibMCCustomException(LIBMC_ERROR_CUSTOMPAGEPROPERTYNAMEMISSING, m_sName);
			std::string sName = nameAttribute.as_string();

			auto typeAttribute = propertyNode.attribute("type");
			if (typeAttribute.empty())
				throw ELibMCCustomException(LIBMC_ERROR_CUSTOMPAGEPROPERTYTYPEMISSING, m_sName);
			std::string sType = typeAttribute.as_string();

			CUIExpression valueExpression (propertyNode, "value");
			m_pCustomItem->registerProperty (sName, sType, valueExpression);
		}
	}
}


CUIModule_Custom::~CUIModule_Custom()
{
}



std::string CUIModule_Custom::getStaticType()
{
	return "custom";
}

std::string CUIModule_Custom::getType()
{
	return getStaticType();
}

std::string CUIModule_Custom::getCaption()
{
	return "custom";
}


void CUIModule_Custom::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());


}

PUIModuleItem CUIModule_Custom::findItem(const std::string& sUUID)
{
	if (m_pCustomItem->getUUID() == sUUID)
		return m_pCustomItem;

	return nullptr;

}


void CUIModule_Custom::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	itemMap.insert(std::make_pair(m_pCustomItem->getUUID(), m_pCustomItem));
}

void CUIModule_Custom::configurePostLoading()
{
}


void CUIModule_Custom::populateClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	m_pCustomItem->populateClientVariables(pParameterHandler);

}
