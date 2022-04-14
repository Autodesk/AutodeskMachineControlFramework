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

#define __AMCIMPL_UI_TOOLBARITEM
#define __AMCIMPL_UI_PAGE

#include "amc_ui_toolbaritem.hpp"
#include "amc_ui_page.hpp"
#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"


using namespace AMC;

CUIToolbarItem::CUIToolbarItem(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sPageName, const std::string& sEventName)
	: m_sID(sID), m_sIcon(sIcon), m_sCaption(sCaption), m_sPageName(sPageName), m_sEventName(sEventName),
	m_sUUID(AMCCommon::CUtils::createUUID ())
{

}

CUIToolbarItem::~CUIToolbarItem()
{

}

std::string CUIToolbarItem::getID()
{
	return m_sID;
}

std::string CUIToolbarItem::getIcon()
{
	return m_sIcon;
}

std::string CUIToolbarItem::getUUID()
{
	return m_sUUID;
}

std::string CUIToolbarItem::getCaption()
{
	return m_sCaption;
}


std::string CUIToolbarItem::getEventName()
{
	return m_sEventName;
}

std::string CUIToolbarItem::getPageName()
{
	return m_sPageName;
}
