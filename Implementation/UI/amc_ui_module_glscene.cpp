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

#include "amc_ui_module_glscene.hpp"

#include "amc_api_constants.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;


CUIModule_GLSceneModel::CUIModule_GLSceneModel(const std::string& sUUID, const std::string& sName, const std::string& sResourceName)
	: m_sUUID(AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sName (sName), m_sResourceName (sResourceName)
{
	if (sName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDGLSCENEMODELNAME);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDGLSCENEMODELNAME, sName);

	if (sResourceName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDGLSCENEINSTANCENAME);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sResourceName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDGLSCENEINSTANCENAME, sResourceName);

}

CUIModule_GLSceneModel::~CUIModule_GLSceneModel()
{

}

std::string CUIModule_GLSceneModel::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_GLSceneModel::getName()
{
	return m_sName;
}

std::string CUIModule_GLSceneModel::getResourceName()
{
	return m_sResourceName;
}



CUIModule_GLSceneInstance::CUIModule_GLSceneInstance(const std::string& sUUID, const std::string& sName, PUIModule_GLSceneModel pModel)
	: m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)), m_sName(sName), m_pModel(pModel)
{
	if (sName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDGLSCENEMODELNAME);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDGLSCENEMODELNAME, sName);

	if (pModel.get () == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CUIModule_GLSceneInstance::~CUIModule_GLSceneInstance()
{

}

std::string CUIModule_GLSceneInstance::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_GLSceneInstance::getName()
{
	return m_sName;
}

PUIModule_GLSceneModel CUIModule_GLSceneInstance::getModel()
{
	return m_pModel;
}


CUIModule_GLScene::CUIModule_GLScene(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode))
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	auto pResourcePackage = pUIModuleEnvironment->resourcePackage();

	auto modelsNode = xmlNode.child("models");
	if (!modelsNode.empty()) {
		auto modelNodes = modelsNode.children("model");
		for (auto modelNode : modelNodes) {
			auto modelNameAttrib = modelNode.attribute("name");
			auto modelResourceAttrib = modelNode.attribute("resource");
			std::string sModelName = modelNameAttrib.as_string();
			std::string sModelResourceName = modelResourceAttrib.as_string();

			if (sModelName.empty ())
				throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYGLMODELNAME);
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sModelName))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDGLMODELNAME, sModelName);

			auto iIter = m_ModelNameMap.find(sModelName);
			if (iIter != m_ModelNameMap.end())
				throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEGLMODELNAME, sModelName);

			if (sModelResourceName.empty())
				throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYGLMODELNAME);

			auto pModelResource = pResourcePackage->findEntryByName (sModelResourceName, false);
			if (pModelResource.get () == nullptr)
				throw ELibMCCustomException(LIBMC_ERROR_MODELGLRESOURCENOTFOUND, sModelResourceName);

			std::string sModelUUID = AMCCommon::CUtils::createUUID();

			auto pModel = std::make_shared<CUIModule_GLSceneModel>(sModelUUID, sModelName, sModelResourceName);
			m_ModelNameMap.insert(std::make_pair (sModelName, pModel));
			m_ModelUUIDMap.insert(std::make_pair (sModelUUID, pModel));
		}

	}

	auto sceneNode = xmlNode.child("scene");
	if (!sceneNode.empty()) {
		auto instancesNodes = sceneNode.children("instance");
		for (auto instanceNode : instancesNodes) {
			auto instanceNameAttrib = instanceNode.attribute("name");
			auto instanceModelAttrib = instanceNode.attribute("model");

			std::string sName = instanceNameAttrib.as_string();
			std::string sModelName = instanceModelAttrib.as_string();

			if (sName.empty ())
				throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYGLINSTANCENAME);
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sName))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDGLINSTANCENAME, sName);
			auto iNameIter = m_InstanceNameMap.find(sName);
			if (iNameIter != m_InstanceNameMap.end())
				throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEGLINSTANCENAME, sName);

			if (sModelName.empty())
				throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYGLINSTANCEMODEL);
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sModelName))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDGLINSTANCEMODEL, sModelName);

			auto iIter = m_ModelNameMap.find(sModelName);
			if (iIter == m_ModelNameMap.end())
				throw ELibMCCustomException(LIBMC_ERROR_GLINSTANCEMODELNOTFOUND, sModelName);

			std::string sInstanceUUID = AMCCommon::CUtils::createUUID();

			auto pInstance = std::make_shared<CUIModule_GLSceneInstance>(sInstanceUUID, sName, iIter->second);
			m_InstanceNameMap.insert(std::make_pair (sName, pInstance));
			m_InstanceUUIDMap.insert(std::make_pair(sInstanceUUID, pInstance));
		}
	}




}


CUIModule_GLScene::~CUIModule_GLScene()
{
}



std::string CUIModule_GLScene::getStaticType()
{
	return "glscene";
}

std::string CUIModule_GLScene::getType()
{
	return getStaticType();
}

std::string CUIModule_GLScene::getCaption()
{
	return m_sCaption;
}


void CUIModule_GLScene::writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

}

PUIModuleItem CUIModule_GLScene::findItem(const std::string& sUUID)
{
	return nullptr;
}

void CUIModule_GLScene::populateItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
}

void CUIModule_GLScene::configurePostLoading()
{
}


void CUIModule_GLScene::populateClientVariables(CParameterHandler* pParameterHandler)
{

}