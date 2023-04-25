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


#ifndef __AMC_UI_MODULE_CUSTOMITEM
#define __AMC_UI_MODULE_CUSTOMITEM

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "Libraries/PugiXML/pugixml.hpp"
#include "amc_jsonwriter.hpp"
#include "amc_ui_module_item.hpp"
#include "amc_ui_expression.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule, PUIModule);
	amcDeclareDependingClass(CUIModuleItem, PUIModuleItem);
	amcDeclareDependingClass(CUIModuleCustomItem_Properties, PUIModuleCustomItem_Properties);
	amcDeclareDependingClass(CStateMachineData, PStateMachineData);
	amcDeclareDependingClass(CResourcePackage, PResourcePackage);
	amcDeclareDependingClass(CParameterHandler, PParameterHandler);
	amcDeclareDependingClass(CUIExpression, PUIExpression);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);

	

	class CUIModuleCustomItem_Properties : public CUIModuleItem {
	protected:		
	
		std::string m_sUUID;

		std::map<std::string, CUIExpression> m_Properties;
		PUIModuleEnvironment m_pUIModuleEnvironment;
		
	public:

		CUIModuleCustomItem_Properties(const std::string & sUUID, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);
		
		virtual ~CUIModuleCustomItem_Properties();

		virtual std::string getUUID () override;

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		// Returns all UUIDs that could be contained in this Item
		virtual std::list <std::string> getReferenceUUIDs();

		virtual void populateClientVariables(CParameterHandler* pParameterHandler);

		void registerProperty(const std::string & sName, const std::string & sTypeString, CUIExpression valueExpression);

	};


	class CUIModuleCustomItem_EventParameter
	{
	private:

		std::string m_sParameterName;
		CUIExpression m_Expression;

	public:
	
		CUIModuleCustomItem_EventParameter(const std::string & sParameterName, CUIExpression expression);

		virtual ~CUIModuleCustomItem_EventParameter();

		std::string getParameterName ();
		CUIExpression& getExpression();

	};


	class CUIModuleCustomItem_Event : public CUIModuleItem {
	protected:

		std::string m_sUUID;

		std::string m_sEventName;

		std::map<std::string, CUIModuleCustomItem_EventParameter> m_EventParameters;

		PUIModuleEnvironment m_pUIModuleEnvironment;

	public:

		CUIModuleCustomItem_Event(const std::string& sUUID, const std::string & sEventName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		virtual ~CUIModuleCustomItem_Event();

		virtual std::string getUUID() override;

		std::string getEventName ();

		virtual void addContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		// Returns all UUIDs that could be contained in this Item
		virtual std::list <std::string> getReferenceUUIDs();

		virtual void populateClientVariables(CParameterHandler* pParameterHandler);

	};

}


#endif //__AMC_UI_MODULE_CUSTOMITEM

