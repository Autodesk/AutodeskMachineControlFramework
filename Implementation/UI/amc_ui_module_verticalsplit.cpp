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

#include "amc_ui_module_verticalsplit.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;


CUIModule_VerticalSection::CUIModule_VerticalSection(PUIModule pModule, double fixedHeightInPercentiles)
	: m_pModule (pModule), m_fixedHeightInPercentiles (fixedHeightInPercentiles)
{
	LibMCAssertNotNull(pModule.get ());
}

CUIModule* CUIModule_VerticalSection::getModule()
{
	return m_pModule.get();
}

double CUIModule_VerticalSection::getFixedHeightInPercentiles()
{
	return m_fixedHeightInPercentiles;
}

CUIModule_VerticalSplit::CUIModule_VerticalSplit(pugi::xml_node& xmlNode, PParameterInstances pParameterInstances, PResourcePackage pResourcePackage, LibMCData::PBuildJobHandler pBuildJobHandler)
: CUIModule (getNameFromXML(xmlNode))
{

	LibMCAssertNotNull(pParameterInstances.get());
	LibMCAssertNotNull(pResourcePackage.get());
	LibMCAssertNotNull(pBuildJobHandler.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	auto children = xmlNode.children();
	for (auto childNode : children) {

		auto nameAttrib = childNode.attribute("name");

		auto fixedHeightAttrib = childNode.attribute("height");
		if (fixedHeightAttrib.empty())
			throw ELibMCCustomException (LIBMC_ERROR_MISSINGSECTIONHEIGHT, nameAttrib.as_string ());

		double fixedHeightInPercentiles = fixedHeightAttrib.as_double(0.0);
		if (fixedHeightInPercentiles <= 0.0)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSECTIONHEIGHT, nameAttrib.as_string());

		auto pSection = CUIModuleFactory::createModule(childNode, pParameterInstances, pResourcePackage, pBuildJobHandler);
		addSection (pSection, fixedHeightInPercentiles);
	}

}


CUIModule_VerticalSplit::~CUIModule_VerticalSplit()
{
}



std::string CUIModule_VerticalSplit::getStaticType()
{
	return "verticalsplit";
}

std::string CUIModule_VerticalSplit::getType()
{
	return getStaticType();
}

std::string CUIModule_VerticalSplit::getCaption()
{
	return m_sCaption;
}


void CUIModule_VerticalSplit::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	CJSONWriterArray splitsNode(writer);
	for (auto section : m_SectionList) {
		CJSONWriterObject splitObject(writer);
		section->getModule()->writeDefinitionToJSON (writer, splitObject);
		splitObject.addDouble(AMC_API_KEY_UI_SECTIONHEIGHT, section->getFixedHeightInPercentiles());
		splitsNode.addObject(splitObject);
	}
	moduleObject.addArray(AMC_API_KEY_UI_SECTIONS, splitsNode);

}

PUIModuleItem CUIModule_VerticalSplit::findItem(const std::string& sUUID)
{
	auto iIter = m_ItemMap.find(sUUID);
	if (iIter != m_ItemMap.end())
		return iIter->second;

	return nullptr;
}

PUIModule_VerticalSection CUIModule_VerticalSplit::findSection(const std::string& sUUID)
{
	auto iIter = m_SectionMap.find(sUUID);
	if (iIter != m_SectionMap.end())
		return iIter->second;

	return nullptr;
}

void CUIModule_VerticalSplit::addSection(PUIModule pModule, double fixedHeightInPercentiles)
{
	LibMCAssertNotNull(pModule.get());
	auto pSection = std::make_shared<CUIModule_VerticalSection>(pModule, fixedHeightInPercentiles);

	m_SectionList.push_back(pSection);
	m_SectionMap.insert(std::make_pair (pSection->getModule ()->getUUID (), pSection));

	pSection->getModule()->populateItemMap(m_ItemMap);

}

void CUIModule_VerticalSplit::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	for (auto pSection : m_SectionList)
		pSection->getModule()->populateItemMap(m_ItemMap);
}
