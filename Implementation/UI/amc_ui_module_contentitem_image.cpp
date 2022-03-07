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

#include "amc_ui_module_contentitem_image.hpp"
#include "libmc_interfaceexception.hpp"

#include "amc_api_constants.hpp"
#include "Common/common_utils.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_parameterinstances.hpp"

#include "libmcdata_dynamic.hpp"

using namespace AMC;



CUIModule_ContentImage::CUIModule_ContentImage(const std::string& sUUID, double dAspectRatio)
	: CUIModule_ContentItem(sUUID), 
	m_dAspectRatio(dAspectRatio),
	m_dMaxWidth (0.0),
	m_dMaxHeight (0.0),
	m_bHasMaxWidth (false),
	m_bHasMaxHeight (false)
{
	if ((dAspectRatio < AMC_UI_IMAGE_MINASPECTRATIO) || (dAspectRatio > AMC_UI_IMAGE_MAXASPECTRATIO))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDASPECTRATIO, std::to_string (dAspectRatio));

}

CUIModule_ContentImage::~CUIModule_ContentImage()
{

}


void CUIModule_ContentImage::addDefinitionToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString(AMC_API_KEY_UI_ITEMTYPE, "image");
	object.addString(AMC_API_KEY_UI_ITEMUUID, m_sUUID);
	object.addDouble(AMC_API_KEY_UI_ITEMASPECTRATIO, m_dAspectRatio);

	if (m_bHasMaxWidth)
		object.addDouble(AMC_API_KEY_UI_ITEMMAXWIDTH, m_dMaxWidth);
	if (m_bHasMaxHeight)
		object.addDouble(AMC_API_KEY_UI_ITEMMAXHEIGHT, m_dMaxHeight);
}

void CUIModule_ContentImage::setMaxWidth(double dMaxWidth)
{
	m_bHasMaxWidth = (dMaxWidth > 0.0);
	if (m_bHasMaxWidth)
		m_dMaxWidth = dMaxWidth;
	else
		m_dMaxWidth = 0.0;
}

void CUIModule_ContentImage::clearMaxWidth()
{
	m_bHasMaxWidth = false;
	m_dMaxWidth = 0.0;
}

void CUIModule_ContentImage::setMaxHeight(double dMaxHeight)
{
	m_bHasMaxHeight = (dMaxHeight > 0.0);
	if (m_bHasMaxHeight)
		m_dMaxHeight = dMaxHeight;
	else
		m_dMaxHeight = 0.0;

}

void CUIModule_ContentImage::clearMaxHeight()
{
	m_bHasMaxHeight = false;
	m_dMaxHeight = 0.0;
}

