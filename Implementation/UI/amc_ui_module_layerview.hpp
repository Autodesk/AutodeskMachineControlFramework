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


#ifndef __AMC_UI_MODULE_LAYERVIEW
#define __AMC_UI_MODULE_LAYERVIEW

#include <cstdint>
#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"
#include <string>
#include "amc_ui_expression.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModule_LayerView, PUIModule_LayerView);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CResourcePackage, PResourcePackage);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CParameterGroup, PParameterGroup);
	amcDeclareDependingClass(CUIModule_LayerViewPlatformItem, PUIModule_LayerViewPlatformItem);
		

	class CUIModule_LayerViewPlatformItem : public CUIModuleItem {
	private:
		CUIExpression m_SizeX;
		CUIExpression m_SizeY;
		CUIExpression m_OriginX;
		CUIExpression m_OriginY;
		CUIExpression m_PaddingX;
		CUIExpression m_PaddingY;
		CUIExpression m_TransformAngle;
		CUIExpression m_RotationCenterX;
		CUIExpression m_RotationCenterY;
		CUIExpression m_TranslationX;
		CUIExpression m_TranslationY;
		CUIExpression m_ShowCoordinateSystem;
		CUIExpression m_BaseImage;
		CUIExpression m_LayerIndex;
		CUIExpression m_LabelVisible;
		CUIExpression m_LabelCaption;
		CUIExpression m_LabelIcon;
		CUIExpression m_SliderFixed;
		CUIExpression m_SliderChangeEvent;
		CUIExpression m_BuildUUID;
		CUIExpression m_ExecutionUUID;
		CUIExpression m_ScatterplotUUID;

		PUIModuleEnvironment m_pUIModuleEnvironment;
		std::string m_sUUID;

	public:

		CUIModule_LayerViewPlatformItem (const std::string& sItemPath, CUIExpression sizeX, CUIExpression sizeY, CUIExpression originX, CUIExpression originY, CUIExpression paddingX, CUIExpression paddingY, CUIExpression transformAngle, CUIExpression rotationCenterX, CUIExpression rotationCenterY, CUIExpression translationX, CUIExpression translationY, CUIExpression showCoordinateSystem, CUIExpression layerIndex, CUIExpression baseImage, PUIModuleEnvironment pUIModuleEnvironment);

		virtual std::string getUUID() override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler);

		// v2 frontend: applies the synced build references to the per-session client variables.
		// The synced layer index is only applied when its value changes, so the slider can still
		// be used to browse other layers in between.
		void syncFrontendClientVariables(CParameterGroup* pGroup, CStateMachineData* pStateMachineData);

		void handleCustomRequest(PAPIAuth pAuth, const std::string& requestType, const CAPIJSONRequest& requestData, CJSONWriter& response, CUIModule_UIEventHandler* pEventHandler) override;

		void setBuildReference(CUIExpression buildUUID, CUIExpression executionUUID, CUIExpression scatterplotUUID);

		void setLabelExpressions (CUIExpression labelVisible, CUIExpression labelCaption, CUIExpression labelIcon);

		void setSliderExpressions(CUIExpression sliderChangeEvent, CUIExpression sliderFixed);
	};


	class CUIModule_LayerView : public CUIModule {
	protected:		

		std::string m_sCaption;

		PUIModule_LayerViewPlatformItem m_PlatformItem;

		PUIModuleEnvironment m_pUIModuleEnvironment;

		// Returns the session's platform client variables after applying the synced references, or nullptr.
		PParameterGroup findSyncedPlatformGroup(CUIFrontendState* pFrontendState, CStateMachineData* pStateMachineData);

		void retrieveBuildInformation(const std::string& sBuildUUID, uint32_t& nLayerCount, uint64_t& nPartStateVersion);

	public:

		/////////////////////////////////////////////////////////////////////////////////////
		// General module functionality
		/////////////////////////////////////////////////////////////////////////////////////

		CUIModule_LayerView(pugi::xml_node & xmlNode, const std::string & sPath, PUIModuleEnvironment pUIModuleEnvironment);
		
		virtual ~CUIModule_LayerView();

		virtual std::string getType() override;

		static std::string getStaticType();

		std::string getCaption () override;

		/////////////////////////////////////////////////////////////////////////////////////
		// Legacy UI System
		/////////////////////////////////////////////////////////////////////////////////////
		
		virtual void populateModuleMap(std::map<std::string, PUIModule>& moduleMap) override;

		virtual void populateLegacyItemMap(std::map<std::string, PUIModuleItem>& itemMap) override;

		virtual PUIModuleItem findLegacyItem(const std::string& sUUID) override;

		virtual void populateLegacyClientVariables(CParameterHandler* pParameterHandler) override;

		/////////////////////////////////////////////////////////////////////////////////////
		// New UI Frontend System
		/////////////////////////////////////////////////////////////////////////////////////

		bool isVersion2FrontendModule() override;

	};

	
}


#endif //__AMC_UI_MODULE_LAYERVIEW

