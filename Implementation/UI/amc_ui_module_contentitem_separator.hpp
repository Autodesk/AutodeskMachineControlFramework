/*++

Copyright (C) 2026 Autodesk Inc.

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


#ifndef __AMC_UI_MODULE_CONTENTITEM_SEPARATOR
#define __AMC_UI_MODULE_CONTENTITEM_SEPARATOR

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_contentitem.hpp"

#include "pugixml.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule_ContentSeparator, PUIModule_ContentSeparator);


	class CUIModule_ContentSeparator : public CUIModule_ContentItem {
	protected:

		std::string m_sOrientation;
		std::string m_sVariant;
		CUIExpression m_OrientationExpression;
		CUIExpression m_VariantExpression;

	public:

		static PUIModule_ContentSeparator makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath);

		CUIModule_ContentSeparator(const std::string& sOrientation, const CUIExpression& orientationExpression, const std::string& sVariant, const CUIExpression& variantExpression, const std::string& sItemName, const std::string& sModulePath);

		virtual ~CUIModule_ContentSeparator();

		virtual void addLegacyContentToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler, uint32_t nStateID) override;

		virtual std::string getItemType() override;
		virtual void registerFrontendAttributes() override;

	};

}


#endif //__AMC_UI_MODULE_CONTENTITEM_SEPARATOR
