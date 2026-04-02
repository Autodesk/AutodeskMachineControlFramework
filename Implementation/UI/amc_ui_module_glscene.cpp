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
#include "amc_toolpathhandler.hpp"
#include "amc_meshhandler.hpp"
#include "amc_meshentity.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_toolpathpart.hpp"
#include "amc_toolpathentity.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;


CUIModule_GLSceneModel::CUIModule_GLSceneModel(const std::string& sUUID, const std::string& sName, const std::string& sResourceName, const std::string& sMeshUUID)
	: m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)), m_sName(sName), m_sResourceName(sResourceName),
	  m_sMeshUUID(AMCCommon::CUtils::normalizeUUIDString (sMeshUUID))
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

std::string CUIModule_GLSceneModel::getMeshUUID()
{
	return m_sMeshUUID;
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
	
	for (uint32_t nIndex = 0; nIndex < 3; nIndex++)
		m_Position.at(nIndex).setFixedValue ("0.0");

	for (uint32_t i = 0; i < 3; i++) {
		for (uint32_t j = 0; j < 3; j++) {
			m_Transform[i][j].setFixedValue ((i == j) ? "1.0" : "0.0");
		}
	}

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

void CUIModule_GLSceneInstance::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{

}

void CUIModule_GLSceneInstance::setPosition(CUIExpression positionX, CUIExpression positionY, CUIExpression positionZ)
{
	m_Position.at(0) = positionX;
	m_Position.at(1) = positionY;
	m_Position.at(2) = positionZ;
}

void CUIModule_GLSceneInstance::getPosition(CUIExpression& positionX, CUIExpression& positionY, CUIExpression& positionZ)
{
	positionX = m_Position.at(0);
	positionY = m_Position.at(1);
	positionZ = m_Position.at(2);
}


void CUIModule_GLSceneInstance::setMatrix(uint32_t nRow, uint32_t nColumn, CUIExpression value)
{
	if ((nRow >= 3) || (nColumn >= 3))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_Transform.at(nRow).at(nColumn) = value;
}

CUIExpression CUIModule_GLSceneInstance::getMatrix(uint32_t nRow, uint32_t nColumn)
{
	if ((nRow >= 3) || (nColumn >= 3))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	return m_Transform.at(nRow).at(nColumn);
}


CUIModule_GLSceneItem::CUIModule_GLSceneItem(const std::string& sItemPath, const std::string& sUUID, PUIModuleEnvironment pUIModuleEnvironment, CUIModule_GLScene* pScene)
	: CUIModuleItem(sItemPath), m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)),
	   m_pUIModuleEnvironment (pUIModuleEnvironment), m_pScene (pScene)
{

}

std::string CUIModule_GLSceneItem::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_GLSceneItem::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return getItemPath();

	return "";
}

void CUIModule_GLSceneItem::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);

	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();

	m_pScene->writeLegacySceneToJSON(writer, object, pClientVariableHandler);

}

void CUIModule_GLSceneItem::setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler)
{

}

void CUIModule_GLSceneItem::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "gl scene");
	pGroup->addNewStringParameter("builduuid", "selected build UUID", AMCCommon::CUtils::createEmptyUUID());
}


CUIModule_GLScene::CUIModule_GLScene(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode), sPath, pUIModuleEnvironment->getFrontendDefinition ())
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);


	auto pResourcePackage = pUIModuleEnvironment->resourcePackage();
	auto pMeshHandler = pUIModuleEnvironment->meshHandler();
	auto pLib3MFWrapper = pUIModuleEnvironment->toolpathHandler()->getLib3MFWrapper();
	auto pLogger = pUIModuleEnvironment->getLogger();


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

			pLogger->logMessage("registering 3D resource " + sModelResourceName + " as " + sModelName, LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Message);

			/*auto pMeshEntity = pMeshHandler->register3MFResource(pLib3MFWrapper.get(), pResourcePackage.get(), sModelResourceName);

			auto pModel = std::make_shared<CUIModule_GLSceneModel>(sModelUUID, sModelName, sModelResourceName, pMeshEntity->getUUID ());
			m_ModelNameMap.insert(std::make_pair (sModelName, pModel));
			m_ModelUUIDMap.insert(std::make_pair (sModelUUID, pModel));
			 */
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


	m_pSceneItem = std::make_shared<CUIModule_GLSceneItem>(getModulePath (), m_sUUID, pUIModuleEnvironment, this);
	m_pUIModuleEnvironment = pUIModuleEnvironment;

	auto captionAttrib = xmlNode.attribute("caption");
	m_sCaption = captionAttrib.as_string();

	CUIExpression captionExpr;
	captionExpr.setFixedValue(m_sCaption);
	registerStringAttribute("caption", captionExpr);

	CUIExpression visibleExpr;
	visibleExpr.setFixedValue("1");
	registerBoolAttribute("visible", visibleExpr);

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

void CUIModule_GLScene::writeLegacySceneToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler)
{
	CJSONWriterObject sceneNode(writer);

	sceneNode.addString(AMC_API_KEY_UI_ITEMTYPE, "scene");
	sceneNode.addString(AMC_API_KEY_UI_ITEMUUID, getUUID());

	CJSONWriterArray instancesNode(writer);
	for (auto instance : m_InstanceNameMap) {
		CJSONWriterObject instanceObject(writer);

		auto pInstance = instance.second;
		auto pModel = pInstance->getModel();

		pInstance->addContentToJSON(writer, instanceObject);

		instanceObject.addString(AMC_API_KEY_UI_INSTANCENAME, pInstance->getName());
		instanceObject.addString(AMC_API_KEY_UI_UUID, pInstance->getUUID());
		instanceObject.addString(AMC_API_KEY_UI_MESHUUID, pModel->getMeshUUID());

		instancesNode.addObject(instanceObject);
	}
	sceneNode.addArray(AMC_API_KEY_UI_INSTANCES, instancesNode);
	moduleObject.addObject(AMC_API_KEY_UI_SCENE, sceneNode);

}


void CUIModule_GLScene::writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pLegacyClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	m_pSceneItem->addLegacyContentToJSON(writer, moduleObject, pLegacyClientVariableHandler, 0);
}

void CUIModule_GLScene::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
}

PUIModuleItem CUIModule_GLScene::findLegacyItem(const std::string& sUUID)
{
	if (m_pSceneItem->getUUID() == sUUID)
		return m_pSceneItem;

	return nullptr;
}

void CUIModule_GLScene::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_GLScene>(*this)));
}

void CUIModule_GLScene::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	itemMap.insert(std::make_pair(m_pSceneItem->getUUID(), m_pSceneItem));
}


void CUIModule_GLScene::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	m_pSceneItem->populateClientVariables(pParameterHandler);

}

/////////////////////////////////////////////////////////////////////////////////////
// New UI Frontend System
/////////////////////////////////////////////////////////////////////////////////////

bool CUIModule_GLScene::isVersion2FrontendModule()
{
	return true;
}

void CUIModule_GLScene::ensureBuildMeshesRegistered(const std::string& sBuildUUID, std::vector<DynamicMeshInstance>& instances)
{
	if (sBuildUUID.empty() || sBuildUUID == AMCCommon::CUtils::createEmptyUUID())
		return;

	auto pDataModel = m_pUIModuleEnvironment->dataModel();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);
	auto sStreamUUID = pBuildJob->GetStorageStreamUUID();

	auto pToolpathHandler = m_pUIModuleEnvironment->toolpathHandler();
	auto pMeshHandler = m_pUIModuleEnvironment->meshHandler();

	auto pToolpath = pToolpathHandler->findToolpathEntity(sStreamUUID, false);
	if (pToolpath == nullptr)
		pToolpath = pToolpathHandler->loadToolpathEntity(sStreamUUID);

	uint32_t nPartCount = pToolpath->getPartCount();
	for (uint32_t nPartIndex = 0; nPartIndex < nPartCount; nPartIndex++) {
		auto pPart = pToolpath->getPart(nPartIndex);
		std::string sMeshObjectUUID = pPart->getMeshUUID();

		if (!pMeshHandler->hasMeshEntity(sMeshObjectUUID)) {
			auto pModel = pPart->getModel();
			auto pBuildItem = pPart->getBuildItem();
			auto nResourceID = pBuildItem->GetObjectResourceID();
			auto pObjectResource = pBuildItem->GetObjectResource();
			if (pObjectResource->IsMeshObject()) {
				Lib3MF::PMeshObject pMeshObject = pModel->GetMeshObjectByID(nResourceID);
				auto pMeshEntity = std::make_shared<CMeshEntity>(sMeshObjectUUID, pPart->getName());
				pMeshEntity->loadFrom3MF(pMeshObject.get());
				pMeshHandler->registerEntity(pMeshEntity);
			}
		}

		DynamicMeshInstance inst;
		inst.uuid = sMeshObjectUUID;
		inst.name = pPart->getName();
		inst.meshUUID = sMeshObjectUUID;
		instances.push_back(inst);
	}
}


void CUIModule_GLScene::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	CUIModule::frontendWriteModuleStatusToJSON(writer, moduleObject, pFrontendState, pStateMachineData);

	std::vector<DynamicMeshInstance> dynamicInstances;
	if (pFrontendState != nullptr) {
		auto pParamHandler = pFrontendState->getLegacyParameterHandler();
		auto pGroup = pParamHandler->findGroup(getModulePath(), false);
		if (pGroup != nullptr) {
			std::string sBuildUUID = pGroup->getParameterValueByName("builduuid");
			try {
				ensureBuildMeshesRegistered(sBuildUUID, dynamicInstances);
			}
			catch (std::exception& e) {
				auto pLogger = m_pUIModuleEnvironment->getLogger();
				pLogger->logMessage(std::string("GLScene: failed to load build meshes: ") + e.what(), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::Warning);
			}
		}
	}

	CJSONWriterArray submodulesArray(writer);

	for (auto& instancePair : m_InstanceNameMap) {
		auto pInstance = instancePair.second;
		auto pModel = pInstance->getModel();

		CJSONWriterObject subModuleObject(writer);
		subModuleObject.addString("moduletype", "glsceneinstance");
		subModuleObject.addString("uuid", pInstance->getUUID());

		CJSONWriterObject attributesObject(writer);
		attributesObject.addString("instancename", pInstance->getName());
		attributesObject.addString("meshuuid", pModel->getMeshUUID());
		subModuleObject.addObject("attributes", attributesObject);

		submodulesArray.addObject(subModuleObject);
	}

	for (auto& dynInst : dynamicInstances) {
		CJSONWriterObject subModuleObject(writer);
		subModuleObject.addString("moduletype", "glsceneinstance");
		subModuleObject.addString("uuid", dynInst.uuid);

		CJSONWriterObject attributesObject(writer);
		attributesObject.addString("instancename", dynInst.name);
		attributesObject.addString("meshuuid", dynInst.meshUUID);
		subModuleObject.addObject("attributes", attributesObject);

		submodulesArray.addObject(subModuleObject);
	}

	moduleObject.addArray("submodules", submodulesArray);
}