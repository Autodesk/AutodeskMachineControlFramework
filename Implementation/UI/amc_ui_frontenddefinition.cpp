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

#include "amc_ui_frontenddefinition.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

using namespace AMC;



CUIFrontendDefinitionAttribute::CUIFrontendDefinitionAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType)
	: m_sName(sName), m_AttributeType(attributeType)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDATTRIBUTENAME, sName);
}

CUIFrontendDefinitionAttribute::~CUIFrontendDefinitionAttribute()
{

}


std::string CUIFrontendDefinitionAttribute::getName()
{
	return m_sName;
}

eUIFrontendDefinitionAttributeType CUIFrontendDefinitionAttribute::getAttributeType()
{
	return m_AttributeType;
}

CUIFrontendDefinitionExpressionAttribute::CUIFrontendDefinitionExpressionAttribute(const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, const CUIExpression& valueExpression)
	: CUIFrontendDefinitionAttribute(sName, attributeType), m_ValueExpression(valueExpression)
{
}

CUIFrontendDefinitionExpressionAttribute::~CUIFrontendDefinitionExpressionAttribute()
{

}

void CUIFrontendDefinitionExpressionAttribute::writeToFrontendJSON(CJSONWriter& writer, CJSONWriterObject& attributesObject, CStateMachineData* pStateMachineData)
{

	switch (getAttributeType()) {
		case eUIFrontendDefinitionAttributeType::atBoolean: {
			bool bValue = m_ValueExpression.evaluateBoolValue (pStateMachineData);
			attributesObject.addBool(getName(), bValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atString: {
			std::string sValue = m_ValueExpression.evaluateStringValue(pStateMachineData);
			attributesObject.addString(getName(), sValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atNumber: {
			double dValue = m_ValueExpression.evaluateNumberValue(pStateMachineData);
			attributesObject.addDouble(getName(), dValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atInteger: {
			int64_t nValue = m_ValueExpression.evaluateIntegerValue(pStateMachineData);
			attributesObject.addInteger(getName(), nValue);
			break;
		}

		case eUIFrontendDefinitionAttributeType::atUUID: {
			std::string sValue = m_ValueExpression.evaluateUUIDValue(pStateMachineData);
			attributesObject.addString(getName(), sValue);
			break;
		}

	}
}


CUIFrontendDefinitionModuleStore::CUIFrontendDefinitionModuleStore(const std::string& sModuleUUID, const std::string& sModulePath, const std::string& sModuleType)
	: m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sModuleUUID)), m_sPath(sModulePath), m_sModuleType(sModuleType)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString (sModulePath))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDMODULEPATH, sModulePath);

}

CUIFrontendDefinitionModuleStore::~CUIFrontendDefinitionModuleStore()
{

}

PUIFrontendDefinitionAttribute CUIFrontendDefinitionModuleStore::registerValue (const std::string& sName, eUIFrontendDefinitionAttributeType attributeType, const CUIExpression& valueExpression)
{
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDFRONTENDATTRIBUTENAME, sName);

	if (m_Attributes.find(sName) != m_Attributes.end())
		throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEFRONTENDATTRIBUTENAME, sName);

	auto pAttribute = std::make_shared<CUIFrontendDefinitionExpressionAttribute>(sName, attributeType, valueExpression);
	m_Attributes.insert(std::make_pair(sName, pAttribute));

	return pAttribute;
}


std::vector<PUIFrontendDefinitionAttribute> CUIFrontendDefinitionModuleStore::getAttributes()
{
	std::vector<PUIFrontendDefinitionAttribute> attributes;
	for (auto attributePair : m_Attributes) {
		attributes.push_back(attributePair.second);
	}
	return attributes;
}


PUIFrontendDefinitionModuleStore CUIFrontendDefinitionModuleStore::addChildStore(const std::string& sChildUUID, const std::string& sChildPath, const std::string& sChildModuleType)
{
	auto pChildStore = std::make_shared<CUIFrontendDefinitionModuleStore>(sChildUUID, sChildPath, sChildModuleType);
	m_ChildStores.push_back(pChildStore);
	return pChildStore;
}

std::vector<PUIFrontendDefinitionModuleStore> CUIFrontendDefinitionModuleStore::getChildStores()
{
	return m_ChildStores;
}

bool CUIFrontendDefinitionModuleStore::hasChildren()
{
	return !m_ChildStores.empty();
}

std::string CUIFrontendDefinitionModuleStore::getModuleType()
{
	return m_sModuleType;
}

std::string CUIFrontendDefinitionModuleStore::getUUID()
{
	return m_sUUID;
}


CUIFrontendDefinition::CUIFrontendDefinition(AMCCommon::PChrono pGlobalChrono)
	: m_pGlobalChrono (pGlobalChrono)
{
	if (pGlobalChrono.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CUIFrontendDefinition::~CUIFrontendDefinition()
{

}

PUIFrontendDefinitionModuleStore CUIFrontendDefinition::registerModuleStore(const std::string& sModuleUUID, const std::string& sPath, const std::string& sModuleType)
{
	return std::make_shared<CUIFrontendDefinitionModuleStore>(sModuleUUID, sPath, sModuleType);

}


AMCCommon::PChrono CUIFrontendDefinition::getGlobalChrono()
{
	return m_pGlobalChrono;
}

