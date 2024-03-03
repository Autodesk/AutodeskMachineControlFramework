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

#include "amc_ui_clientaction.hpp"
#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"


using namespace AMC;

CUIClientAction::CUIClientAction()
{

}

CUIClientAction::~CUIClientAction()
{

}

CUIClientAction_ActivateModalDialog::CUIClientAction_ActivateModalDialog(const std::string& sDialogName)
	: CUIClientAction (), m_sDialogName (sDialogName)
{

}

CUIClientAction_ActivateModalDialog::~CUIClientAction_ActivateModalDialog()
{

}

std::string CUIClientAction_ActivateModalDialog::getDialogName()
{
	return m_sDialogName;
}

void CUIClientAction_ActivateModalDialog::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "activatemodaldialog");
	object.addString("dialogname", m_sDialogName);
}

CUIClientAction_ActivatePage::CUIClientAction_ActivatePage(const std::string& sPage)
	: CUIClientAction(), m_sPage (sPage)
{

}

CUIClientAction_ActivatePage::~CUIClientAction_ActivatePage()
{

}

std::string CUIClientAction_ActivatePage::getPage()
{
	return m_sPage;
}


void CUIClientAction_ActivatePage::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "activatepage");
	object.addString("pagename", m_sPage);

}

CUIClientAction_StreamDownload::CUIClientAction_StreamDownload(const std::string& sDownloadTicketUUID)
	: m_sDownloadTicketUUID (AMCCommon::CUtils::normalizeUUIDString (sDownloadTicketUUID))
{

}

CUIClientAction_StreamDownload::~CUIClientAction_StreamDownload()
{
}

std::string CUIClientAction_StreamDownload::getDownloadTicketUUID()
{
	return m_sDownloadTicketUUID;
}

void CUIClientAction_StreamDownload::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("downloadticket", m_sDownloadTicketUUID);

}


CUIClientAction_CloseModalDialog::CUIClientAction_CloseModalDialog()
	: CUIClientAction ()
{

}

CUIClientAction_CloseModalDialog::~CUIClientAction_CloseModalDialog()
{

}

void CUIClientAction_CloseModalDialog::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "closemodaldialog");
}

CUIClientAction_Logout::CUIClientAction_Logout()
{

}

CUIClientAction_Logout::~CUIClientAction_Logout()
{

}

void CUIClientAction_Logout::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "logout");

}


CUIClientAction_ShowHint::CUIClientAction_ShowHint(const std::string& sHint, const uint32_t nTimeoutInMS, const int32_t nColorValue, const int32_t nFontColorValue)
	: CUIClientAction (), m_sHint (sHint), m_nTimeoutInMS (nTimeoutInMS), m_nColorValue (nColorValue), m_nFontColorValue (nFontColorValue)
{

}


CUIClientAction_ShowHint::~CUIClientAction_ShowHint()
{

}

void CUIClientAction_ShowHint::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "showhint");

}


std::string CUIClientAction_ShowHint::getHint()
{
	return m_sHint;
}


uint32_t CUIClientAction_ShowHint::getTimeoutInMS()
{
	return m_nTimeoutInMS;
}


int32_t CUIClientAction_ShowHint::getColorValue()
{
	return m_nColorValue;
}


int32_t CUIClientAction_ShowHint::getFontColorValue()
{
	return m_nFontColorValue;
}

CUIClientAction_HideHint::CUIClientAction_HideHint()
	: CUIClientAction ()
{

}

CUIClientAction_HideHint::~CUIClientAction_HideHint()
{

}

void CUIClientAction_HideHint::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "hidehint");

}

CUIClientAction_ShowMessageDlg::CUIClientAction_ShowMessageDlg(const std::string& sDialogUUID, const std::string& sDialogType, const std::string& sCaption, const std::string& sTitle, const std::string& sYesEvent, const std::string& sNoEvent, const std::string& sCancelEvent)
	: CUIClientAction(),
	m_sDialogUUID (AMCCommon::CUtils::normalizeUUIDString (sDialogUUID)),
	m_sDialogType (sDialogType),
	m_sCaption (sCaption),
	m_sTitle (sTitle),
	m_sYesEvent (sYesEvent),
	m_sNoEvent (sNoEvent),
	m_sCancelEvent (sCancelEvent)
{

}

CUIClientAction_ShowMessageDlg::~CUIClientAction_ShowMessageDlg()
{

}

void CUIClientAction_ShowMessageDlg::writeToJSON(CJSONWriter& writer, CJSONWriterObject& object)
{
	object.addString("action", "showmessagedlg");

}


std::string CUIClientAction_ShowMessageDlg::getDialogUUID()
{
	return m_sDialogUUID;
}

std::string CUIClientAction_ShowMessageDlg::getDialogType()
{
	return m_sDialogType;
}

std::string CUIClientAction_ShowMessageDlg::getCaption()
{
	return m_sCaption;
}

std::string CUIClientAction_ShowMessageDlg::getTitle()
{
	return m_sTitle;
}

std::string CUIClientAction_ShowMessageDlg::getYesEvent()
{
	return m_sYesEvent;
}

std::string CUIClientAction_ShowMessageDlg::getNoEvent()
{
	return m_sNoEvent;
}

std::string CUIClientAction_ShowMessageDlg::getCancelEvent()
{
	return m_sCancelEvent;
}

