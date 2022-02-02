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


#ifndef __AMC_UI_MODULE_GRAPHICITEM_SVGIMAGE
#define __AMC_UI_MODULE_GRAPHICITEM_SVGIMAGE

#include "header_protection.hpp"

#ifndef __AMCIMPL_UI_MODULE
#error this header is protected and should only be included in the corresponding implementation CPP files.
#endif

#include "amc_ui_module_graphicitem.hpp"
#include "pugixml.hpp"

#include "amc_ui_expression.hpp"

namespace AMC {

	amcDeclareDependingClass(CUIModule_GraphicSVGImage, PUIModule_GraphicSVGImage);
	amcDeclareDependingClass(CUIModuleEnvironment, PUIModuleEnvironment);

	class CUIModule_GraphicSVGImage : public CUIModuleGraphicItem {
	protected:		
		CUIExpression m_ResourceName;
		CUIExpression m_X;
		CUIExpression m_Y;
		CUIExpression m_Z;
		CUIExpression m_ScaleX;
		CUIExpression m_ScaleY;
		CUIExpression m_Angle;

		PUIModuleEnvironment m_pUIModuleEnvironment;

	public:

		static PUIModule_GraphicSVGImage makeFromXML(const pugi::xml_node& xmlNode, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);

		CUIModule_GraphicSVGImage(CUIExpression resourceName, CUIExpression x, CUIExpression y, CUIExpression z, CUIExpression scaleX, CUIExpression scaleY, CUIExpression angle, const std::string& sItemName, const std::string& sModulePath, PUIModuleEnvironment pUIModuleEnvironment);
		
		virtual ~CUIModule_GraphicSVGImage();

		void addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object, CParameterHandler* pClientVariableHandler) override;

	};


}


#endif //__AMC_UI_MODULE_GRAPHICITEM_SVGIMAGE

