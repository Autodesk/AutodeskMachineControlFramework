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

#include "amc_ui_module_infoboxitem.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"

using namespace AMC;


CUIModule_InfoboxItem::CUIModule_InfoboxItem(const std::string& sUUID)
	: m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID))
{

}

CUIModule_InfoboxItem::~CUIModule_InfoboxItem()
{

}

std::string CUIModule_InfoboxItem::getUUID()
{
	return m_sUUID;
}



CUIModule_InfoboxParagraph::CUIModule_InfoboxParagraph(const std::string& sText)
	: CUIModule_InfoboxItem (AMCCommon::CUtils::createUUID ()), m_sText (sText)
{

}

CUIModule_InfoboxParagraph::~CUIModule_InfoboxParagraph()
{

}

std::string CUIModule_InfoboxParagraph::getText()
{
	return m_sText;
}

void CUIModule_InfoboxParagraph::addToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "paragraph");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addString(AMC_API_KEY_UI_ITEMTEXT, m_sText);
}



CUIModule_InfoboxImage::CUIModule_InfoboxImage(const std::string& sUUID)
	: CUIModule_InfoboxItem (sUUID)
{

}

CUIModule_InfoboxImage::~CUIModule_InfoboxImage()
{

}


void CUIModule_InfoboxImage::addToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "image");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
}

