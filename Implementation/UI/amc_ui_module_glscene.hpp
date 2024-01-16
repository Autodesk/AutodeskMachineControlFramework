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


#ifndef __AMC_UI_MODULE_GLSCENE
#define __AMC_UI_MODULE_GLSCENE

#include "header_protection.hpp"
#include "amc_ui_module_item.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_expression.hpp"

#include "Libraries/PugiXML/pugixml.hpp"
#include <array>

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModule_GLScene, PUIModule_GLScene);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CResourcePackage, PResourcePackage);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CUIModule_GLSceneModel, PUIModule_GLSceneModel);
	amcDeclareDependingClass(CUIModule_GLSceneInstance, PUIModule_GLSceneInstance);
	amcDeclareDependingClass(CUIModule_GLSceneItem, PUIModule_GLSceneItem);

	class CUIModule_GLSceneModel {
	private:
		std::string m_sUUID;
		std::string m_sName;
		std::string m_sResourceName;
		std::string m_sMeshUUID;

	public:

		CUIModule_GLSceneModel(const std::string & sUUID, const std::string & sName, const std::string & sResourceName, const std::string & sMeshUUID);

		virtual ~CUIModule_GLSceneModel();

		std::string getUUID ();

		std::string getName ();

		std::string getResourceName ();

		std::string getMeshUUID();

	};


	class CUIModule_GLSceneInstance {
	private:
		std::string m_sUUID;
		std::string m_sName;
		PUIModule_GLSceneModel m_pModel;
		
		std::array<CUIExpression, 3> m_Position;
		std::array<std::array <CUIExpression, 3>, 3> m_Transform;


	public:
		CUIModule_GLSceneInstance(const std::string& sUUID, const std::string& sName, PUIModule_GLSceneModel pModel);

		virtual ~CUIModule_GLSceneInstance();

		std::string getUUID();

		std::string getName();

		PUIModule_GLSceneModel getModel();

		void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object);

		void setPosition(CUIExpression positionX, CUIExpression positionY, CUIExpression positionZ);

		void getPosition(CUIExpression & positionX, CUIExpression & positionY, CUIExpression & positionZ);

		void setMatrix (uint32_t nRow, uint32_t nColumn, CUIExpression value);

		CUIExpression getMatrix(uint32_t nRow, uint32_t nColumn);

	};

	class CUIModule_GLSceneItem : public CUIModuleItem {
	private:
		std::string m_sUUID;
		PUIModuleEnvironment m_pUIModuleEnvironment;
		CUIModule_GLScene * m_pScene;

	public:

		CUIModule_GLSceneItem(const std::string& sItemPath, const std::string & sUUID, PUIModuleEnvironment pUIModuleEnvironment, CUIModule_GLScene * pScene);

		virtual std::string getUUID() override;

		virtual std::string findElementPathByUUID(const std::string& sUUID) override;

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		virtual void setEventPayloadValue(const std::string& sEventName, const std::string& sPayloadUUID, const std::string& sPayloadValue, CParameterHandler* pClientVariableHandler) override;

		virtual void populateClientVariables(CParameterHandler* pClientVariableHandler);

	};



	class CUIModule_GLScene : public CUIModule {
	protected:		

		std::string m_sCaption;
		std::string m_sModulePath;

		std::map<std::string, PUIModule_GLSceneModel> m_ModelNameMap;
		std::map<std::string, PUIModule_GLSceneModel> m_ModelUUIDMap;

		std::map<std::string, PUIModule_GLSceneInstance> m_InstanceNameMap;
		std::map<std::string, PUIModule_GLSceneInstance> m_InstanceUUIDMap;

		PUIModule_GLSceneItem m_pSceneItem;

	public:

		CUIModule_GLScene(pugi::xml_node & xmlNode, const std::string & sPath, PUIModuleEnvironment pUIModuleEnvironment);
		
		virtual ~CUIModule_GLScene();

		virtual std::string getType() override;

		static std::string getStaticType();

		std::string getCaption () override;

		virtual void writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler) override;

		void writeSceneToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler);

		virtual void populateItemMap(std::map<std::string, PUIModuleItem>& itemMap) override;

		virtual PUIModuleItem findItem(const std::string& sUUID) override;

		void configurePostLoading() override;

		virtual void populateClientVariables(CParameterHandler* pParameterHandler) override;


	};

	
}


#endif //__AMC_UI_MODULE_GLSCENE

