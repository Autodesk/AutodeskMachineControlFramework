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

#include "libmcui_interfaceexception.hpp"
#include "libmcui_eventhandler.hpp"
#include "libmcui_event.hpp"

#include <cmath>
#include <sstream>
#include <iomanip>

using namespace LibMCUI::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

/*************************************************************************************************************************
 CEvent_Logout
**************************************************************************************************************************/

class CEvent_Logout : public virtual CEvent {
public:
	static std::string getEventName() { return "logout"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: User logged out");
		pUIEnvironment->LogOut();
	}
};


/*************************************************************************************************************************
 CEvent_StartProcess - triggers signal_start on main state machine
**************************************************************************************************************************/

class CEvent_StartProcess : public virtual CEvent {
public:
	static std::string getEventName() { return "startprocess"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Start process requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_start");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_StopProcess - triggers signal_stop on main state machine
**************************************************************************************************************************/

class CEvent_StopProcess : public virtual CEvent {
public:
	static std::string getEventName() { return "stopprocess"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Stop process requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_stop");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_PauseProcess
**************************************************************************************************************************/

class CEvent_PauseProcess : public virtual CEvent {
public:
	static std::string getEventName() { return "pauseprocess"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Pause process requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_pause");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_ResumeProcess
**************************************************************************************************************************/

class CEvent_ResumeProcess : public virtual CEvent {
public:
	static std::string getEventName() { return "resumeprocess"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Resume process requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_resume");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_CreateTestAlert
**************************************************************************************************************************/

class CEvent_CreateTestAlert : public virtual CEvent {
public:
	static std::string getEventName() { return "createtestalert"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Creating test alert");

		auto nTimer = pUIEnvironment->GetGlobalTimerInMilliseconds();
		uint32_t nVariant = (uint32_t)(nTimer % 4);

		std::string sContext = "User-triggered test alert at " + std::to_string(nTimer) + " ms";

		if (nVariant == 0)
			pUIEnvironment->CreateAlert("alert_info", sContext, true);
		else if (nVariant == 1)
			pUIEnvironment->CreateAlert("alert_warning", sContext, true);
		else if (nVariant == 2)
			pUIEnvironment->CreateAlert("alert_error", sContext, true);
		else
			pUIEnvironment->CreateAlert("alert_critical", sContext, true);
	}
};


/*************************************************************************************************************************
 CEvent_GenerateLogMessages
**************************************************************************************************************************/

class CEvent_GenerateLogMessages : public virtual CEvent {
public:
	static std::string getEventName() { return "generatelogmessages"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Generating test log messages...");

		for (int i = 1; i <= 10; i++) {
			std::stringstream ss;
			ss << "Test log message #" << i << " at timer "
			   << pUIEnvironment->GetGlobalTimerInMilliseconds() << " ms";
			pUIEnvironment->LogMessage(ss.str());
		}

		pUIEnvironment->LogMessage("WidgetTest: Log message generation complete.");
	}
};


/*************************************************************************************************************************
 CEvent_UpdateChartData - creates/updates a data series for chart testing
**************************************************************************************************************************/

class CEvent_UpdateChartData : public virtual CEvent {
public:
	static std::string getEventName() { return "updatechartdata"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Updating chart data");

		uint64_t nTimer = pUIEnvironment->GetGlobalTimerInMilliseconds();
		uint32_t nCount = 200;

		std::vector<LibMCEnv::sTimeStreamEntry> entries;
		entries.reserve(nCount);

		for (uint32_t i = 0; i < nCount; i++) {
			double dValue = sin(i * 0.05) * 50.0 + 50.0 + 10.0 * cos(i * 0.1 + nTimer * 0.001);
			entries.push_back({i, dValue});
		}

		std::string sSeriesUUID = pUIEnvironment->GetUIPropertyAsUUID("visualization.chartpanel.testchart", "dataseries");
		if (pUIEnvironment->HasDataSeries(sSeriesUUID)) {
			auto pSeries = pUIEnvironment->FindDataSeries(sSeriesUUID);
			pSeries->SetAllEntries(entries);
		}
		else {
			auto pSeries = pUIEnvironment->CreateDataSeries("widgettest_series", true);
			pSeries->SetAllEntries(entries);
			pUIEnvironment->SetUIPropertyAsUUID("visualization.chartpanel.testchart", "dataseries", pSeries->GetUUID());
		}
	}
};


/*************************************************************************************************************************
 CEvent_UploadFinished
**************************************************************************************************************************/

class CEvent_UploadFinished : public virtual CEvent {
public:
	static std::string getEventName() { return "onuploadfinished"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("WidgetTest: Upload completed from " + sSender);

		auto sUploadUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "uploaduuid");
		pUIEnvironment->LogMessage("WidgetTest: Uploaded file UUID = " + sUploadUUID);
	}
};


/*************************************************************************************************************************
 CEvent_SelectBuild
**************************************************************************************************************************/

class CEvent_SelectBuild : public virtual CEvent {
public:
	static std::string getEventName() { return "onselectbuild"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("WidgetTest: Build selected from " + sSender);

		auto sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "buttonuuid");
		if (sBuildUUID == "00000000-0000-0000-0000-000000000000")
			sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "selecteduuid");
		pUIEnvironment->LogMessage("WidgetTest: Selected build UUID = " + sBuildUUID);

		pUIEnvironment->SetUIProperty("buildworkflow.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("buildworkflow.preview", "currentlayer", 1);
		pUIEnvironment->LogMessage("WidgetTest: Set builduuid on LayerView for toolpath preview");
	}
};


/*************************************************************************************************************************
 CEvent_StartExecution - creates a build execution from an inline button click
**************************************************************************************************************************/

class CEvent_StartExecution : public virtual CEvent {
public:
	static std::string getEventName() { return "onstartexecution"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("WidgetTest: Start execution from " + sSender);

		auto sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "buttonuuid");
		pUIEnvironment->LogMessage("WidgetTest: Build UUID = " + sBuildUUID);

		auto pBuildJob = pUIEnvironment->GetBuildJob(sBuildUUID);
		auto pExecution = pBuildJob->StartExecution(
			"Execution of " + pBuildJob->GetName(),
			pUIEnvironment->GetCurrentUserUUID());

		pUIEnvironment->LogMessage("WidgetTest: Created build execution " + pExecution->GetUUID());

		pExecution->SetStatusToFinished();
		pUIEnvironment->LogMessage("WidgetTest: Execution finished");
	}
};


/*************************************************************************************************************************
 CEvent_ShowDialog
**************************************************************************************************************************/

class CEvent_ShowDialog : public virtual CEvent {
public:
	static std::string getEventName() { return "showdialog"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Showing test dialog");
		pUIEnvironment->ActivateModalDialog("testdialog");
	}
};


/*************************************************************************************************************************
 CEvent_DismissDialog
**************************************************************************************************************************/

class CEvent_DismissDialog : public virtual CEvent {
public:
	static std::string getEventName() { return "dismissdialog"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Dialog dismissed");
	}
};


/*************************************************************************************************************************
 CEvent_FormChanged
**************************************************************************************************************************/

class CEvent_FormChanged : public virtual CEvent {
public:
	static std::string getEventName() { return "formchanged"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("WidgetTest: Form value changed from " + sSender);
	}
};


/*************************************************************************************************************************
 CEvent_ToggleHeater - sends signal_toggleheater to main state machine
**************************************************************************************************************************/

class CEvent_ToggleHeater : public virtual CEvent {
public:
	static std::string getEventName() { return "toggleheater"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Toggle heater requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_toggleheater");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_ToggleGasFlow - sends signal_togglegasflow to main state machine
**************************************************************************************************************************/

class CEvent_ToggleGasFlow : public virtual CEvent {
public:
	static std::string getEventName() { return "togglegasflow"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Toggle gas flow requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_togglegasflow");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_ResetCounters - sends signal_resetcounters to main state machine
**************************************************************************************************************************/

class CEvent_ResetCounters : public virtual CEvent {
public:
	static std::string getEventName() { return "resetcounters"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("WidgetTest: Reset counters requested");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_resetcounters");
		pSignal->Trigger();
	}
};


/*************************************************************************************************************************
 CEvent_GotoBuilds
**************************************************************************************************************************/

class CEvent_GotoBuilds : public virtual CEvent {
public:
	static std::string getEventName() { return "gotobuilds"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->ActivatePage("buildworkflow");
	}
};


/*************************************************************************************************************************
 Event handler factory
**************************************************************************************************************************/

IEvent* CEventHandler::CreateEvent(const std::string& sEventName, LibMCEnv::PUIEnvironment pUIEnvironment)
{
	IEvent* pEventInstance = nullptr;

	if (createEventInstanceByName<CEvent_Logout>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_StartProcess>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_StopProcess>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_PauseProcess>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ResumeProcess>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_CreateTestAlert>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_GenerateLogMessages>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_UpdateChartData>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_UploadFinished>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_SelectBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_StartExecution>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ShowDialog>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_DismissDialog>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_FormChanged>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ToggleHeater>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ToggleGasFlow>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ResetCounters>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_GotoBuilds>(sEventName, pEventInstance))
		return pEventInstance;

	throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDEVENTNAME, "invalid event name: " + sEventName);
}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
