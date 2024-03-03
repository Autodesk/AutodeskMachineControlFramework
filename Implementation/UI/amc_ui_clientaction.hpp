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


#ifndef __AMC_UI_CLIENTACTION
#define __AMC_UI_CLIENTACTION

#include "header_protection.hpp"
#include "amc_jsonwriter.hpp"

#include <memory>
#include <string>

namespace AMC {


	class CUIClientAction {
	private:		
	public:
		CUIClientAction();
		virtual ~CUIClientAction();

		virtual void writeToJSON(CJSONWriter & writer, CJSONWriterObject & object) = 0;
	};

	typedef std::shared_ptr<CUIClientAction> PUIClientAction;

	class CUIClientAction_ActivateModalDialog : public CUIClientAction {
	private:
		std::string m_sDialogName;
	public:
		CUIClientAction_ActivateModalDialog(const std::string &sDialogName);
		virtual ~CUIClientAction_ActivateModalDialog();

		std::string getDialogName();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};
			
	class CUIClientAction_StreamDownload : public CUIClientAction {
	private:
		std::string m_sDownloadFileName;
		std::string m_sDownloadTicketUUID;
	public:
		CUIClientAction_StreamDownload(const std::string& sDownloadTicketUUID, const std::string& sDownloadFileName);
		virtual ~CUIClientAction_StreamDownload();

		std::string getDownloadTicketUUID();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};

	class CUIClientAction_ActivatePage : public CUIClientAction {
	private:
		std::string m_sPage;
	public:
		CUIClientAction_ActivatePage(const std::string& sPage);
		virtual ~CUIClientAction_ActivatePage();

		std::string getPage();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};

	class CUIClientAction_CloseModalDialog : public CUIClientAction {
	private:
	public:
		CUIClientAction_CloseModalDialog();
		virtual ~CUIClientAction_CloseModalDialog();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};

	class CUIClientAction_Logout : public CUIClientAction {
	private:
	public:
		CUIClientAction_Logout();
		virtual ~CUIClientAction_Logout();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};

	class CUIClientAction_ShowHint : public CUIClientAction {
	private:
		std::string m_sHint;
		uint32_t m_nTimeoutInMS; // 0 means no timeout
		int32_t m_nColorValue; // -1 means standard color
		int32_t m_nFontColorValue; // -1 means standard color
	public:
		CUIClientAction_ShowHint(const std::string & sHint, const uint32_t nTimeoutInMS, const int32_t nColorValue, const int32_t nFontColorValue);
		virtual ~CUIClientAction_ShowHint();

		std::string getHint ();
		uint32_t getTimeoutInMS (); // 0 means no timeout
		int32_t getColorValue (); // -1 means standard color
		int32_t getFontColorValue (); // -1 means standard color

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};

	class CUIClientAction_HideHint : public CUIClientAction {
	private:
	public:
		CUIClientAction_HideHint();
		virtual ~CUIClientAction_HideHint();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};


	class CUIClientAction_ShowMessageDlg : public CUIClientAction {
	private:
		std::string m_sDialogUUID;
		std::string m_sDialogType;
		std::string m_sCaption;
		std::string m_sTitle;
		std::string m_sYesEvent;
		std::string m_sNoEvent;
		std::string m_sCancelEvent;
	public:
		CUIClientAction_ShowMessageDlg(const std::string & sDialogUUID, const std::string& sDialogType, const std::string& sCaption, const std::string& sTitle, const std::string& sYesEvent, const std::string& sNoEvent, const std::string& sCancelEvent);
		virtual ~CUIClientAction_ShowMessageDlg();

		std::string getDialogUUID();
		std::string getDialogType();
		std::string getCaption();
		std::string getTitle();
		std::string getYesEvent();
		std::string getNoEvent();
		std::string getCancelEvent();

		virtual void writeToJSON(CJSONWriter& writer, CJSONWriterObject& object) override;
	};
	
}


#endif //__AMC_UI_CLIENTACTION

