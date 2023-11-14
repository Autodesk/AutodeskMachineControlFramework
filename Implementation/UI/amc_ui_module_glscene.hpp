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

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"


namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModule_GLScene, PUIModule_GLScene);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CResourcePackage, PResourcePackage);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CUIModule_GLSceneModel, PUIModule_GLSceneModel);
	amcDeclareDependingClass(CUIModule_GLSceneInstance, PUIModule_GLSceneInstance);

	class CUIModule_GLSceneModel {
	private:
		std::string m_sUUID;
		std::string m_sName;
		std::string m_sResourceName;

	public:
		CUIModule_GLSceneModel(const std::string & sUUID, const std::string & sName, const std::string & sResourceName);

		virtual ~CUIModule_GLSceneModel();

		std::string getUUID ();

		std::string getName ();

		std::string getResourceName ();

	};


	class CUIModule_GLSceneInstance {
	private:
		std::string m_sUUID;
		std::string m_sName;
		PUIModule_GLSceneModel m_pModel;
		double dColor;

	public:
		CUIModule_GLSceneInstance(const std::string& sUUID, const std::string& sName, PUIModule_GLSceneModel pModel);

		virtual ~CUIModule_GLSceneInstance();

		std::string getUUID();

		std::string getName();

		PUIModule_GLSceneModel getModel();

		void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object);

	};


	class CUIModule_GLScene : public CUIModule {
	protected:		

		std::string m_sCaption;

		std::map<std::string, PUIModule_GLSceneModel> m_ModelNameMap;
		std::map<std::string, PUIModule_GLSceneModel> m_ModelUUIDMap;

		std::map<std::string, PUIModule_GLSceneInstance> m_InstanceNameMap;
		std::map<std::string, PUIModule_GLSceneInstance> m_InstanceUUIDMap;

	public:

		CUIModule_GLScene(pugi::xml_node & xmlNode, const std::string & sPath, PUIModuleEnvironment pUIModuleEnvironment);
		
		virtual ~CUIModule_GLScene();

		virtual std::string getType() override;

		static std::string getStaticType();

		std::string getCaption () override;

		virtual void writeDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& moduleObject, CParameterHandler* pClientVariableHandler) override;

		virtual void populateItemMap(std::map<std::string, PUIModuleItem>& itemMap) override;

		virtual PUIModuleItem findItem(const std::string& sUUID) override;

		void configurePostLoading() override;

		virtual void populateClientVariables(CParameterHandler* pParameterHandler) override;


	};

	
}


#endif //__AMC_UI_MODULE_GLSCENE

