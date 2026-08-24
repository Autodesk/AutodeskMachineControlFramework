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
#include "amc_api_jsonrequest.hpp"

#include "amc_ui_module_layerview.hpp"

#include "amc_api_constants.hpp"
#include "amc_api_auth.hpp"
#include "amc_resourcepackage.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_toolpathhandler.hpp"

#include "common_utils.hpp"

#include "libmc_exceptiontypes.hpp"

using namespace AMC;



CUIModule_LayerViewPlatformItem::CUIModule_LayerViewPlatformItem (const std::string& sItemPath, CUIExpression sizeX, CUIExpression sizeY, CUIExpression originX, CUIExpression originY, CUIExpression layerIndex, CUIExpression baseImage, PUIModuleEnvironment pUIModuleEnvironment)
	: CUIModuleItem(sItemPath), m_SizeX(sizeX), m_SizeY(sizeY), m_OriginX(originX), m_OriginY(originY), m_BaseImage(baseImage), m_pUIModuleEnvironment(pUIModuleEnvironment),
	m_sUUID(AMCCommon::CUtils::createUUID()), m_LayerIndex(layerIndex)
{
	LibMCAssertNotNull(m_pUIModuleEnvironment);


}

std::string CUIModule_LayerViewPlatformItem::getUUID()
{
	return m_sUUID;
}

std::string CUIModule_LayerViewPlatformItem::findElementPathByUUID(const std::string& sUUID)
{
	if (sUUID == getUUID())
		return getItemPath();

	return "";
}



void CUIModule_LayerViewPlatformItem::addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
	auto pGroup = pClientVariableHandler->findGroup(getItemPath (), true);

	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData ();	

	if (m_SizeX.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_SIZEX, m_SizeX.evaluateStringValue(pStateMachineData));
	if (m_SizeY.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_SIZEY, m_SizeY.evaluateStringValue(pStateMachineData));
	if (m_OriginX.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_ORIGINX, m_OriginX.evaluateStringValue(pStateMachineData));
	if (m_OriginY.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_ORIGINY, m_OriginY.evaluateStringValue(pStateMachineData));
	if (m_LayerIndex.needsSync())
		pGroup->setIntParameterValueByName(AMC_API_KEY_UI_CURRENTLAYER, m_LayerIndex.evaluateIntegerValue(pStateMachineData));
	if (m_LabelVisible.needsSync())
		pGroup->setIntParameterValueByName(AMC_API_KEY_UI_LABELVISIBLE, m_LabelVisible.evaluateIntegerValue(pStateMachineData));
	if (m_LabelCaption.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_LABELCAPTION, m_LabelCaption.evaluateStringValue(pStateMachineData));
	if (m_LabelIcon.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_LABELICON, m_LabelIcon.evaluateStringValue(pStateMachineData));
	if (m_SliderChangeEvent.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_SLIDERCHANGEEVENT, m_SliderChangeEvent.evaluateStringValue(pStateMachineData));
	if (m_SliderFixed.needsSync())
		pGroup->setIntParameterValueByName(AMC_API_KEY_UI_SLIDERFIXED, m_SliderFixed.evaluateIntegerValue(pStateMachineData));

	if (m_BuildUUID.needsSync ())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_BUILDUUID, m_BuildUUID.evaluateStringValue(pStateMachineData));
	if (m_ExecutionUUID.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_EXECUTIONUUID, m_ExecutionUUID.evaluateStringValue(pStateMachineData));
	if (m_ScatterplotUUID.needsSync())
		pGroup->setParameterValueByName(AMC_API_KEY_UI_SCATTERPLOTUUID, m_ScatterplotUUID.evaluateStringValue(pStateMachineData));


	object.addDouble(AMC_API_KEY_UI_SIZEX, pGroup->getDoubleParameterValueByName(AMC_API_KEY_UI_SIZEX));
	object.addDouble(AMC_API_KEY_UI_SIZEY, pGroup->getDoubleParameterValueByName(AMC_API_KEY_UI_SIZEY));
	object.addDouble(AMC_API_KEY_UI_ORIGINX, pGroup->getDoubleParameterValueByName(AMC_API_KEY_UI_ORIGINX));
	object.addDouble(AMC_API_KEY_UI_ORIGINY, pGroup->getDoubleParameterValueByName(AMC_API_KEY_UI_ORIGINY));

	std::string sBaseImageResource = m_BaseImage.evaluateStringValue(pStateMachineData);
	if (!sBaseImageResource.empty()) {
		auto pResourceEntry = m_pUIModuleEnvironment->resourcePackage()->findEntryByName(sBaseImageResource, true);
		object.addString(AMC_API_KEY_UI_BASEIMAGERESOURCE, pResourceEntry->getUUID());
	}

	std::string sBuildUUID = pGroup->getUUIDParameterValueByName(AMC_API_KEY_UI_BUILDUUID);
	std::string sExecutionUUID = pGroup->getUUIDParameterValueByName(AMC_API_KEY_UI_EXECUTIONUUID);
	std::string sScatterplotUUID = pGroup->getUUIDParameterValueByName(AMC_API_KEY_UI_SCATTERPLOTUUID);

	object.addString(AMC_API_KEY_UI_BUILDUUID, sBuildUUID);
	object.addString(AMC_API_KEY_UI_EXECUTIONUUID, sExecutionUUID);
	object.addString(AMC_API_KEY_UI_SCATTERPLOTUUID, sScatterplotUUID);
	object.addInteger(AMC_API_KEY_UI_CURRENTLAYER, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_CURRENTLAYER));
	object.addInteger(AMC_API_KEY_UI_CURRENTLAYERCOUNTER, pGroup->getChangeCounterOf(AMC_API_KEY_UI_CURRENTLAYER));

	uint32_t nLayerCount = 0;
	if (AMCCommon::CUtils::stringIsNonEmptyUUIDString (sBuildUUID)) {
		auto pToolpathHandler = m_pUIModuleEnvironment->toolpathHandler();
		auto pDataModel = m_pUIModuleEnvironment->dataModel();
		auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();

		if (pBuildJobHandler->JobExists(sBuildUUID)) {
			auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);
			auto pToolpathEntity = pToolpathHandler->findToolpathEntity(pBuildJob->GetStorageStreamUUID(), false);
			if (pToolpathEntity != nullptr) {
				nLayerCount = pToolpathEntity->getLayerCount();
			}
		}
	}

	object.addInteger(AMC_API_KEY_UI_LAYERCOUNT, nLayerCount);

	object.addInteger(AMC_API_KEY_UI_LABELVISIBLE, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_LABELVISIBLE));
	object.addString(AMC_API_KEY_UI_LABELCAPTION, pGroup->getParameterValueByName(AMC_API_KEY_UI_LABELCAPTION));
	object.addString(AMC_API_KEY_UI_LABELICON, pGroup->getParameterValueByName(AMC_API_KEY_UI_LABELICON));

	object.addString(AMC_API_KEY_UI_SLIDERCHANGEEVENT, pGroup->getParameterValueByName(AMC_API_KEY_UI_SLIDERCHANGEEVENT));
	object.addInteger(AMC_API_KEY_UI_SLIDERFIXED, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_SLIDERFIXED));



}

void CUIModule_LayerViewPlatformItem::handleCustomRequest(PAPIAuth pAuth, const std::string& requestType, const CAPIJSONRequest& requestData, CJSONWriter& response, CUIModule_UIEventHandler* pEventHandler)
{
	if (pAuth == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pEventHandler == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	

	auto pClientVariableHandler = pAuth->getLegacyParameterHandler(true);

	if ((requestType == "changelayer") && (pClientVariableHandler != nullptr)) {
		uint64_t nLayer = requestData.getUint64(AMC_API_KEY_UI_TARGETLAYER, 0, UINT32_MAX, LIBMC_ERROR_MISSINGCUSTOMREQUESTLAYER);
		auto pGroup = pClientVariableHandler->findGroup(getItemPath(), true);
		pGroup->setIntParameterValueByName(AMC_API_KEY_UI_CURRENTLAYER, nLayer);		

		std::string sChangeEvent = pGroup->getParameterValueByName(AMC_API_KEY_UI_SLIDERCHANGEEVENT);
		if (!sChangeEvent.empty()) {
			pEventHandler->handleEvent(sChangeEvent, m_sUUID, "", "", pAuth);
		}
	}

}


void CUIModule_LayerViewPlatformItem::setLabelExpressions(CUIExpression labelVisible, CUIExpression labelCaption, CUIExpression labelIcon)
{
	m_LabelVisible = labelVisible;
	m_LabelCaption = labelCaption;
	m_LabelIcon = labelIcon;
}

void CUIModule_LayerViewPlatformItem::setSliderExpressions(CUIExpression sliderChangeEvent, CUIExpression sliderFixed)
{
	m_SliderChangeEvent = sliderChangeEvent;
	m_SliderFixed = sliderFixed;
}

void CUIModule_LayerViewPlatformItem::setBuildReference(CUIExpression buildUUID, CUIExpression executionUUID, CUIExpression scatterplotUUID)
{
	m_BuildUUID = buildUUID;
	m_ExecutionUUID = executionUUID;
	m_ScatterplotUUID = scatterplotUUID;
}


void CUIModule_LayerViewPlatformItem::populateClientVariables(CParameterHandler* pClientVariableHandler)
{
	auto pStateMachineData = m_pUIModuleEnvironment->stateMachineData();
	auto pGroup = pClientVariableHandler->addGroup(getItemPath(), "layer view");
	pGroup->addNewUUIDParameter(AMC_API_KEY_UI_BUILDUUID, "Build UUID", m_BuildUUID.evaluateUUIDValue(pStateMachineData));
	pGroup->addNewUUIDParameter(AMC_API_KEY_UI_EXECUTIONUUID, "Execution UUID", m_ExecutionUUID.evaluateUUIDValue(pStateMachineData));
	pGroup->addNewUUIDParameter(AMC_API_KEY_UI_SCATTERPLOTUUID, "Scatterplot UUID", m_ScatterplotUUID.evaluateUUIDValue(pStateMachineData));
	pGroup->addNewIntParameter(AMC_API_KEY_UI_CURRENTLAYER, "Current layer index", 0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_SIZEX, "Platform size x", m_SizeX.evaluateNumberValue (pStateMachineData), 1.0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_SIZEY, "Platform size y", m_SizeY.evaluateNumberValue(pStateMachineData), 1.0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_ORIGINX, "Platform origin x", m_OriginX.evaluateNumberValue(pStateMachineData), 1.0);
	pGroup->addNewDoubleParameter(AMC_API_KEY_UI_ORIGINY, "Platform origin y", m_OriginY.evaluateNumberValue(pStateMachineData), 1.0);
	pGroup->addNewStringParameter(AMC_API_KEY_UI_BASEIMAGERESOURCE, "Platform base image", m_BaseImage.evaluateStringValue(pStateMachineData));
	pGroup->addNewIntParameter(AMC_API_KEY_UI_LABELVISIBLE, "Label is visible", m_LabelVisible.evaluateIntegerValue (pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_LABELCAPTION, "Label caption", m_LabelCaption.evaluateStringValue(pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_LABELICON, "Label icon", m_LabelIcon.evaluateStringValue(pStateMachineData));
	pGroup->addNewStringParameter(AMC_API_KEY_UI_SLIDERCHANGEEVENT, "Slider change event", m_SliderChangeEvent.evaluateStringValue(pStateMachineData));
	pGroup->addNewIntParameter(AMC_API_KEY_UI_SLIDERFIXED, "Slider is fixed", m_SliderFixed.evaluateIntegerValue(pStateMachineData));


}



CUIModule_LayerView::CUIModule_LayerView(pugi::xml_node& xmlNode, const std::string& sPath, PUIModuleEnvironment pUIModuleEnvironment)
: CUIModule (getNameFromXML(xmlNode), sPath, pUIModuleEnvironment->getFrontendDefinition ()), m_pUIModuleEnvironment(pUIModuleEnvironment)
{

	LibMCAssertNotNull(pUIModuleEnvironment.get());
	if (getTypeFromXML(xmlNode) != getStaticType())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMODULETYPE, "should be " + getStaticType ());

	if (sPath.empty())
		throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODULEPATH, m_sName);

	m_sModulePath = sPath + "." + m_sName;

	auto platformNode = xmlNode.child("platform");
	if (platformNode.empty ())
		throw ELibMCCustomException(LIBMC_ERROR_PLATFORMINFORMATIONMISSING, m_sName);

	CUIExpression sizeX (platformNode, "sizex");
	CUIExpression sizeY(platformNode, "sizey");
	CUIExpression originX(platformNode, "originx");
	CUIExpression originY(platformNode, "originy");
	CUIExpression baseImage(platformNode, "baseimage");
	CUIExpression darkBaseImage(platformNode, "dark_baseimage", false);
	CUIExpression layerIndex(platformNode, "layerindex", false);

	CUIExpression buildUUID;
	CUIExpression executionUUID;
	CUIExpression scatterplotUUID;
	CUIExpression currentLayer;

	CUIExpression labelVisible;
	CUIExpression labelCaption;
	CUIExpression labelIcon;

	CUIExpression sliderChangeEvent;
	CUIExpression sliderFixed;

	m_PlatformItem = std::make_shared<CUIModule_LayerViewPlatformItem>(m_sModulePath, sizeX, sizeY, originX, originY, layerIndex, baseImage, pUIModuleEnvironment);

	auto labelNode = xmlNode.child("label");
	if (!labelNode.empty()) {
		labelVisible = CUIExpression(labelNode, "visible");
		labelCaption = CUIExpression(labelNode, "caption");
		labelIcon = CUIExpression(labelNode, "icon");

		m_PlatformItem->setLabelExpressions(labelVisible, labelCaption, labelIcon);

	}
	

	auto referencesNode = xmlNode.child("references");
	if (!referencesNode.empty()) {
		buildUUID = CUIExpression (platformNode, "builduuid", false);
		executionUUID = CUIExpression (platformNode, "executionuuid", false);
		scatterplotUUID = CUIExpression (platformNode, "scatterplotuuid", false);

		m_PlatformItem->setBuildReference(buildUUID, executionUUID, scatterplotUUID);



	}

	auto sliderNode = xmlNode.child("slider");
	if (!sliderNode.empty()) {
		sliderChangeEvent = CUIExpression(sliderNode, "changeevent");
		sliderFixed = CUIExpression(sliderNode, "fixed");

		m_PlatformItem->setSliderExpressions(sliderChangeEvent, sliderFixed);

	}

	/////////////////////////////////////////////////////////////////////////////////////
	// Color configuration: <colors> child element with light/dark mode attributes.
	// Fallback to sensible defaults when not specified.
	/////////////////////////////////////////////////////////////////////////////////////
	auto colorsNode = xmlNode.child("colors");

	auto readColorAttr = [&](pugi::xml_node& node, const char* attrName, const char* fallback) -> CUIExpression {
		CUIExpression expr;
		auto attr = node.attribute(attrName);
		if (!attr.empty())
			expr.setFixedValue(attr.as_string());
		else
			expr.setFixedValue(fallback);
		return expr;
	};

	CUIExpression colorBackground, colorGrid, colorContour, colorHatch, colorTravel;
	CUIExpression darkColorBackground, darkColorGrid, darkColorContour, darkColorHatch, darkColorTravel;

	if (!colorsNode.empty()) {
		colorBackground = readColorAttr(colorsNode, "background", "#ffffff");
		colorGrid = readColorAttr(colorsNode, "grid", "#e0e0e0");
		colorContour = readColorAttr(colorsNode, "contour", "#00aa88");
		colorHatch = readColorAttr(colorsNode, "hatch", "#cc88cc");
		colorTravel = readColorAttr(colorsNode, "travel", "#aaaaaa");

		darkColorBackground = readColorAttr(colorsNode, "dark_background", "#1a1a2e");
		darkColorGrid = readColorAttr(colorsNode, "dark_grid", "#333344");
		darkColorContour = readColorAttr(colorsNode, "dark_contour", "#33ddaa");
		darkColorHatch = readColorAttr(colorsNode, "dark_hatch", "#dd99dd");
		darkColorTravel = readColorAttr(colorsNode, "dark_travel", "#555566");
	}
	else {
		colorBackground.setFixedValue("#ffffff");
		colorGrid.setFixedValue("#e0e0e0");
		colorContour.setFixedValue("#00aa88");
		colorHatch.setFixedValue("#cc88cc");
		colorTravel.setFixedValue("#aaaaaa");

		darkColorBackground.setFixedValue("#1a1a2e");
		darkColorGrid.setFixedValue("#333344");
		darkColorContour.setFixedValue("#33ddaa");
		darkColorHatch.setFixedValue("#dd99dd");
		darkColorTravel.setFixedValue("#555566");
	}

	registerStringAttribute("color_background", colorBackground);
	registerStringAttribute("color_grid", colorGrid);
	registerStringAttribute("color_contour", colorContour);
	registerStringAttribute("color_hatch", colorHatch);
	registerStringAttribute("color_travel", colorTravel);
	registerStringAttribute("darkcolor_background", darkColorBackground);
	registerStringAttribute("darkcolor_grid", darkColorGrid);
	registerStringAttribute("darkcolor_contour", darkColorContour);
	registerStringAttribute("darkcolor_hatch", darkColorHatch);
	registerStringAttribute("darkcolor_travel", darkColorTravel);

	/////////////////////////////////////////////////////////////////////////////////////
	// New UI Frontend System
	/////////////////////////////////////////////////////////////////////////////////////
	registerUUIDAttribute(AMC_API_KEY_UI_BUILDUUID, buildUUID);
	registerUUIDAttribute(AMC_API_KEY_UI_EXECUTIONUUID, executionUUID);
	registerUUIDAttribute(AMC_API_KEY_UI_SCATTERPLOTUUID, scatterplotUUID);
	registerIntegerAttribute(AMC_API_KEY_UI_CURRENTLAYER, currentLayer);
	registerNumberAttribute(AMC_API_KEY_UI_SIZEX, sizeX);
	registerNumberAttribute(AMC_API_KEY_UI_SIZEY, sizeY);
	registerNumberAttribute(AMC_API_KEY_UI_ORIGINX, originX);
	registerNumberAttribute(AMC_API_KEY_UI_ORIGINY, originY);
	// baseimageresource must be the UUID so the frontend's /image/{uuid} endpoint works.
	// Resolve the resource name -> UUID once at startup and register as a fixed value.
	{
		auto pStateMachineData = pUIModuleEnvironment->stateMachineData();
		std::string sBaseImageName = baseImage.evaluateStringValue(pStateMachineData.get());
		CUIExpression baseImageUUIDExpr;
		if (!sBaseImageName.empty()) {
			auto pResourceEntry = pUIModuleEnvironment->resourcePackage()->findEntryByName(sBaseImageName, true);
			baseImageUUIDExpr.setFixedValue(pResourceEntry->getUUID());
		} else {
			baseImageUUIDExpr.setFixedValue(AMCCommon::CUtils::createEmptyUUID());
		}
		registerStringAttribute(AMC_API_KEY_UI_BASEIMAGERESOURCE, baseImageUUIDExpr);
	}
	{
		auto pStateMachineData = pUIModuleEnvironment->stateMachineData();
		std::string sDarkBaseImageName = darkBaseImage.evaluateStringValue(pStateMachineData.get());
		CUIExpression darkBaseImageUUIDExpr;
		if (!sDarkBaseImageName.empty()) {
			auto pResourceEntry = pUIModuleEnvironment->resourcePackage()->findEntryByName(sDarkBaseImageName, true);
			darkBaseImageUUIDExpr.setFixedValue(pResourceEntry->getUUID());
		} else {
			darkBaseImageUUIDExpr.setFixedValue("");
		}
		registerStringAttribute("dark_baseimageresource", darkBaseImageUUIDExpr);
	}
	registerBoolAttribute(AMC_API_KEY_UI_LABELVISIBLE, labelVisible);
	registerStringAttribute(AMC_API_KEY_UI_LABELCAPTION, labelCaption);
	registerStringAttribute(AMC_API_KEY_UI_LABELICON, labelIcon);
	registerStringAttribute(AMC_API_KEY_UI_SLIDERCHANGEEVENT, sliderChangeEvent);
	registerBoolAttribute(AMC_API_KEY_UI_SLIDERFIXED, sliderFixed);

	CUIExpression platformUUIDExpr;
	platformUUIDExpr.setFixedValue(m_PlatformItem->getUUID());
	registerStringAttribute(AMC_API_KEY_UI_PLATFORMUUID, platformUUIDExpr);

	auto captionAttrib = xmlNode.attribute("caption");
	m_sCaption = captionAttrib.as_string();

	CUIExpression captionExpr;
	captionExpr.setFixedValue(m_sCaption);
	registerStringAttribute("caption", captionExpr);

	CUIExpression visibleExpr;
	visibleExpr.setFixedValue("1");
	registerBoolAttribute("visible", visibleExpr);
}


CUIModule_LayerView::~CUIModule_LayerView()
{
}



std::string CUIModule_LayerView::getStaticType()
{
	return "layerview";
}

std::string CUIModule_LayerView::getType()
{
	return getStaticType();
}

std::string CUIModule_LayerView::getCaption()
{
	return m_sCaption;
}


void CUIModule_LayerView::writeLegacyDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pLegacyClientVariableHandler)
{
	moduleObject.addString(AMC_API_KEY_UI_MODULENAME, getName());
	moduleObject.addString(AMC_API_KEY_UI_MODULEUUID, getUUID());
	moduleObject.addString(AMC_API_KEY_UI_MODULETYPE, getType());
	moduleObject.addString(AMC_API_KEY_UI_CAPTION, m_sCaption);

	CJSONWriterArray itemsNode(writer);
	CJSONWriterObject itemObject(writer);
	itemObject.addString(AMC_API_KEY_UI_ITEMTYPE, "platform");
	itemObject.addString(AMC_API_KEY_UI_ITEMUUID, m_PlatformItem->getUUID ());
	m_PlatformItem->addLegacyContentToJSON(writer, itemObject, pLegacyClientVariableHandler, 0);
	itemsNode.addObject(itemObject);

	moduleObject.addArray(AMC_API_KEY_UI_ITEMS, itemsNode);

}

void CUIModule_LayerView::addContentToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler, uint32_t nStateID)
{
}

PUIModuleItem CUIModule_LayerView::findLegacyItem(const std::string& sUUID)
{
	if (m_PlatformItem->getUUID() == sUUID)
		return m_PlatformItem;

	return nullptr;
}

void CUIModule_LayerView::populateModuleMap(std::map<std::string, PUIModule>& moduleMap)
{
	moduleMap.insert(std::make_pair(m_sUUID, std::make_shared<CUIModule_LayerView>(*this)));
}

void CUIModule_LayerView::populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap)
{
	itemMap.insert (std::make_pair (m_PlatformItem->getUUID (), m_PlatformItem));
}


void CUIModule_LayerView::populateLegacyClientVariables(CParameterHandler* pParameterHandler)
{
	LibMCAssertNotNull(pParameterHandler);

	m_PlatformItem->populateClientVariables(pParameterHandler);


}

bool CUIModule_LayerView::isVersion2FrontendModule()
{
	return true;
}

void CUIModule_LayerView::frontendWriteModuleStatusToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData)
{
	CUIModule::frontendWriteModuleStatusToJSON(writer, moduleObject, pFrontendState, pStateMachineData);

	auto pParamHandler = pFrontendState->getLegacyParameterHandler();
	auto pGroup = pParamHandler->findGroup(m_PlatformItem->getItemPath(), false);
	if (pGroup == nullptr)
		return;

	if (m_PlatformItem != nullptr) {
		CJSONWriterObject cvObject(writer);

		std::string sBuildUUID = pGroup->getUUIDParameterValueByName(AMC_API_KEY_UI_BUILDUUID);
		cvObject.addString(AMC_API_KEY_UI_BUILDUUID, sBuildUUID);
		cvObject.addInteger(AMC_API_KEY_UI_CURRENTLAYER, pGroup->getIntParameterValueByName(AMC_API_KEY_UI_CURRENTLAYER));

		uint32_t nLayerCount = 0;
		if (AMCCommon::CUtils::stringIsNonEmptyUUIDString(sBuildUUID)) {
			auto pToolpathHandler = m_pUIModuleEnvironment->toolpathHandler();
			auto pDataModel = m_pUIModuleEnvironment->dataModel();
			auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
			if (pBuildJobHandler->JobExists(sBuildUUID)) {
				auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);
				auto pToolpathEntity = pToolpathHandler->findToolpathEntity(pBuildJob->GetStorageStreamUUID(), false);
				if (pToolpathEntity != nullptr)
					nLayerCount = pToolpathEntity->getLayerCount();
			}
		}
		cvObject.addInteger(AMC_API_KEY_UI_LAYERCOUNT, nLayerCount);

		moduleObject.addObject("clientvariables", cvObject);
	}
}