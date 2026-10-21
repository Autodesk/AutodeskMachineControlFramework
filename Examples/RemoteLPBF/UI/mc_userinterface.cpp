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

#include "libmcui_interfaceexception.hpp"
#include "libmcui_eventhandler.hpp"
#include "libmcui_event.hpp"

#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>
#include <string>
#include <stdexcept>

#include <thread>

using namespace LibMCUI::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Returns the first layer that contains toolpath segments, or 0 if all layers are empty.
static LibMCEnv_uint32 findFirstNonEmptyLayer(LibMCEnv::PBuild pBuild)
{
	if (!pBuild->ToolpathIsLoaded())
		pBuild->LoadToolpath();

	auto pToolpathAccessor = pBuild->CreateToolpathAccessor();
	try {
		return pToolpathAccessor->FindNonEmptyLayer(0, pToolpathAccessor->GetLayerCount(), true);
	}
	catch (LibMCEnv::ELibMCEnvException& E) {
		if (E.getErrorCode() == LIBMCENV_ERROR_NONONEMPTYLAYERFOUND)
			return 0;
		throw;
	}
}

/*************************************************************************************************************************
 Class declaration of CEvent_Logout
**************************************************************************************************************************/

class CEvent_Logout : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "logout";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogOut();
	}

};

/*************************************************************************************************************************
 Class declaration of CEvent_StartBuild
**************************************************************************************************************************/
class CEvent_StartBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "startexecution";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on Start Execution Button");

		if (pUIEnvironment->GetMachineParameterAsBool("main", "ui", "build_canbestarted")) {

			auto sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID("importbuildjob.preview", "builduuid");

			if (sBuildUUID.empty() || sBuildUUID == "00000000-0000-0000-0000-000000000000") {
				pUIEnvironment->LogMessage("No build job selected! Please upload or select a build file first.");
				return;
			}

			pUIEnvironment->SetUIPropertyAsUUID("buildstatus.preview", "builduuid", sBuildUUID);
			pUIEnvironment->SetUIPropertyAsInteger("buildstatus.preview", "currentlayer", 0);
			pUIEnvironment->SetUIPropertyAsUUID("buildstatus.preview", "highlightpartuuid", "00000000-0000-0000-0000-000000000000");

			auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_startexecution");
			pSignal->SetString("jobuuid", sBuildUUID);
			pSignal->Trigger();

			pUIEnvironment->ActivatePage("buildstatus");
		}
		else {

			pUIEnvironment->LogMessage("Execution cannot be started!");
		}

	}

};


/*************************************************************************************************************************
 Class declaration of CEvent_CloseBuild
**************************************************************************************************************************/

class CEvent_CloseBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "closebuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("Clicked on CloseBuild Button");

		// Unload the currently loaded build from the preview so the 3D view clears.
		pUIEnvironment->SetUIPropertyAsUUID("importbuildjob.preview", "builduuid", "00000000-0000-0000-0000-000000000000");
		pUIEnvironment->SetUIPropertyAsInteger("importbuildjob.preview", "currentlayer", 0);
		pUIEnvironment->SetUIPropertyAsUUID("importbuildjob.preview", "highlightpartuuid", "00000000-0000-0000-0000-000000000000");

		// Clear the selection of this client session, which hides the "Build Details"
		// tab and restores the "Select Build" tab.
		pUIEnvironment->SetSessionVariableAsBool("build_isselected", false);
		pUIEnvironment->SetSessionVariableAsUUID("selectedbuilduuid", "00000000-0000-0000-0000-000000000000");

		pUIEnvironment->ActivatePage("importbuildjob");
	}

};


/*************************************************************************************************************************
 Class declaration of CEvent_OnUploadFinished
**************************************************************************************************************************/
class CEvent_OnUploadFinished : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onuploadfinished";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("Uploaded success from " + sSender);

		auto sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "uploaduuid");
		pUIEnvironment->LogMessage("Build job ID " + sBuildUUID);

		auto pBuild = pUIEnvironment->GetBuildJob(sBuildUUID);
		pBuild->LoadToolpath();

		pUIEnvironment->SetUIProperty("importbuildjob.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("importbuildjob.preview", "currentlayer", findFirstNonEmptyLayer(pBuild));
		pUIEnvironment->SetUIPropertyAsUUID("importbuildjob.preview", "highlightpartuuid", "00000000-0000-0000-0000-000000000000");

		// A freshly uploaded build behaves like a selected build: switch to the
		// "Build Details" tab and remember the UUID for the part list.
		pUIEnvironment->SetSessionVariableAsUUID("selectedbuilduuid", sBuildUUID);
		pUIEnvironment->SetSessionVariableAsBool("build_isselected", true);

		pUIEnvironment->ActivatePage("importbuildjob");
	}

};


class CEvent_OnSelectBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onselectbuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("Build item selected from " + sSender);

		auto sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "selecteduuid");
		pUIEnvironment->LogMessage("Build job ID " + sBuildUUID);

/*		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_initjob");
		pSignal->SetString("jobuuid", sBuildUUID);
		pSignal->Trigger();

		pUIEnvironment->SetUIPropertyAsUUID("buildstatus.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("buildstatus.preview", "currentlayer", 2);*/

		auto pBuild = pUIEnvironment->GetBuildJob(sBuildUUID);
		pUIEnvironment->SetUIProperty("importbuildjob.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("importbuildjob.preview", "currentlayer", findFirstNonEmptyLayer(pBuild));
		pUIEnvironment->SetUIPropertyAsUUID("importbuildjob.preview", "highlightpartuuid", "00000000-0000-0000-0000-000000000000");

		// Mark the build as selected in this client session, which shows the "Build Details"
		// tab and hands the selected build UUID to the part list.
		pUIEnvironment->SetSessionVariableAsUUID("selectedbuilduuid", sBuildUUID);
		pUIEnvironment->SetSessionVariableAsBool("build_isselected", true);

		pUIEnvironment->ActivatePage("importbuildjob");

	}

};


/*************************************************************************************************************************
 Class declaration of CEvent_OnSelectPart
**************************************************************************************************************************/

class CEvent_OnSelectPart : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onselectpart";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();

		// The part list sends the null UUID when the selected row is clicked again.
		auto sPartUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "selecteduuid");
		pUIEnvironment->SetUIPropertyAsUUID("importbuildjob.preview", "highlightpartuuid", sPartUUID);
	}

};


/*************************************************************************************************************************
 Part list of the process page ("Parts" tab): selection, part information and disabling parts
**************************************************************************************************************************/

#define REMOTELPBF_NULLUUID "00000000-0000-0000-0000-000000000000"
#define REMOTELPBF_DISABLEPART_DIALOGFORM "disablepartdialog.dpdlgcontent.dpdlgform."

// Clears the processpart_* session variables of the calling session.
static void clearProcessPartSelection(LibMCEnv::PUIEnvironment pUIEnvironment)
{
	pUIEnvironment->SetSessionVariableAsBool("processpart_isselected", false);
	pUIEnvironment->SetSessionVariableAsBool("processpart_candisable", false);
	pUIEnvironment->SetSessionVariableAsBool("processpart_isdisabled", false);
	pUIEnvironment->SetSessionVariableAsUUID("processpart_uuid", REMOTELPBF_NULLUUID);
	pUIEnvironment->SetSessionVariable("processpart_name", "");
	pUIEnvironment->SetSessionVariable("processpart_number", "");
	pUIEnvironment->SetSessionVariable("processpart_meshuuid", "");
	pUIEnvironment->SetSessionVariable("processpart_buildname", "");
	pUIEnvironment->SetSessionVariable("processpart_status", "");
}

// Fills the processpart_* session variables of the calling session with the details of a part of
// the current build job. Clears the selection and returns false if the part is not part of it.
static bool loadProcessPartSelection(LibMCEnv::PUIEnvironment pUIEnvironment, const std::string& sPartUUID)
{
	auto sBuildUUID = pUIEnvironment->GetMachineParameterAsUUID("main", "jobinfo", "jobuuid");
	if ((sBuildUUID == REMOTELPBF_NULLUUID) || (sPartUUID == REMOTELPBF_NULLUUID)) {
		clearProcessPartSelection(pUIEnvironment);
		return false;
	}

	auto pBuild = pUIEnvironment->GetBuildJob(sBuildUUID);
	if (!pBuild->ToolpathIsLoaded())
		pBuild->LoadToolpath();

	auto pAccessor = pBuild->CreateToolpathAccessor();
	auto pPart = pAccessor->FindPartByUUID(sPartUUID);
	if (pPart.get() == nullptr) {
		clearProcessPartSelection(pUIEnvironment);
		return false;
	}

	bool bIsDisabled = pPart->IsDisabled();

	pUIEnvironment->SetSessionVariableAsBool("processpart_isselected", true);
	pUIEnvironment->SetSessionVariableAsBool("processpart_candisable", !bIsDisabled);
	pUIEnvironment->SetSessionVariableAsBool("processpart_isdisabled", bIsDisabled);
	pUIEnvironment->SetSessionVariableAsUUID("processpart_uuid", pPart->GetUUID());
	pUIEnvironment->SetSessionVariable("processpart_name", pPart->GetName());
	pUIEnvironment->SetSessionVariable("processpart_number", pPart->GetPartNumber());
	pUIEnvironment->SetSessionVariable("processpart_meshuuid", pPart->GetMeshUUID());
	pUIEnvironment->SetSessionVariable("processpart_buildname", pBuild->GetName());
	pUIEnvironment->SetSessionVariable("processpart_status", bIsDisabled ? "Disabled (not exposed in subsequent layers)" : "Active");

	return true;
}

static std::string trimWhitespace(const std::string& sValue)
{
	const char* pWhitespace = " \t\r\n";
	auto nStart = sValue.find_first_not_of(pWhitespace);
	if (nStart == std::string::npos)
		return "";
	auto nEnd = sValue.find_last_not_of(pWhitespace);
	return sValue.substr(nStart, nEnd - nStart + 1);
}


// A part has been clicked in the process page part list: highlight it in the toolpath viewer and
// load its details into the session. Clicking the selected row again sends the null UUID.
class CEvent_OnSelectProcessPart : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onselectprocesspart";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();

		auto sPartUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "selecteduuid");
		pUIEnvironment->SetUIPropertyAsUUID("buildstatus.preview", "highlightpartuuid", sPartUUID);

		loadProcessPartSelection(pUIEnvironment, sPartUUID);
	}

};


class CEvent_OnProcessPartInfo : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onprocesspartinfo";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		// Reload, so the dialog shows the current disabled state.
		if (!loadProcessPartSelection(pUIEnvironment, pUIEnvironment->GetSessionVariableAsUUID("processpart_uuid"))) {
			pUIEnvironment->ShowHint("Please select a part of the current build first.", 3000);
			return;
		}

		pUIEnvironment->ActivateModalDialog("partinfodialog");
	}

};


class CEvent_OnProcessPartDisable : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onprocesspartdisable";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!loadProcessPartSelection(pUIEnvironment, pUIEnvironment->GetSessionVariableAsUUID("processpart_uuid"))) {
			pUIEnvironment->ShowHint("Please select a part of the current build first.", 3000);
			return;
		}

		auto sPartName = pUIEnvironment->GetSessionVariable("processpart_name");
		if (pUIEnvironment->GetSessionVariableAsBool("processpart_isdisabled")) {
			pUIEnvironment->ShowHint("Part \"" + sPartName + "\" is already disabled.", 3000);
			return;
		}

		pUIEnvironment->SetSessionVariable("processpart_confirmname", "");
		pUIEnvironment->ActivateModalDialog("disablepartdialog");
	}

};


// OK in the disable-part dialog: only disables the part if the typed name matches the part name.
class CEvent_OnConfirmDisablePart : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onconfirmdisablepart";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sTypedName = pUIEnvironment->GetUIProperty(REMOTELPBF_DISABLEPART_DIALOGFORM "confirmname", "value");
		pUIEnvironment->SetSessionVariable("processpart_confirmname", sTypedName);

		auto sPartUUID = pUIEnvironment->GetSessionVariableAsUUID("processpart_uuid");
		if (!loadProcessPartSelection(pUIEnvironment, sPartUUID)) {
			pUIEnvironment->CloseModalDialog();
			pUIEnvironment->ShowHint("The selected part is not part of the current build anymore.", 3000);
			return;
		}

		auto sPartName = pUIEnvironment->GetSessionVariable("processpart_name");
		if (trimWhitespace(sTypedName) != trimWhitespace(sPartName)) {
			pUIEnvironment->ShowHint("The entered name does not match \"" + sPartName + "\". The part has not been disabled.", 4000);
			return;
		}

		auto pBuild = pUIEnvironment->GetBuildJob(pUIEnvironment->GetMachineParameterAsUUID("main", "jobinfo", "jobuuid"));
		pBuild->DisablePart(sPartUUID);
		pUIEnvironment->LogMessage("Disabled part \"" + sPartName + "\" (" + sPartUUID + ")");

		loadProcessPartSelection(pUIEnvironment, sPartUUID);

		pUIEnvironment->CloseModalDialog();
		pUIEnvironment->ShowHint("Part \"" + sPartName + "\" has been disabled.", 3000);
	}

};


class CEvent_OnCancelDisablePart : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "oncanceldisablepart";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		// Stored so that resetting it to "" on the next opening clears the field on the client.
		pUIEnvironment->SetSessionVariable("processpart_confirmname", pUIEnvironment->GetUIProperty(REMOTELPBF_DISABLEPART_DIALOGFORM "confirmname", "value"));
		pUIEnvironment->CloseModalDialog();
	}

};


// Recovery actions shown on the build-status "Overview" tab when a subsystem faults. Each one
// raises the "signal_initialize_again" recovery signal on the target state machine, which its
// fatal-error state claims to transition back through waitforinit -> init -> idle.
class CEvent_OnResetMain : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onresetmain";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("Reset Main System requested.");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_initialize_again");
		pSignal->Trigger();
	}

};


class CEvent_OnResetLaser : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onresetlaser";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("Reset Laser System requested.");
		auto pSignal = pUIEnvironment->PrepareSignal("laser", "signal_initialize_again");
		pSignal->Trigger();
	}

};

// Schema identifier of the persistent Remote LPBF machine settings configuration type.
#define REMOTELPBF_CONFIG_SCHEMA "com.autodesk.remotelpbf"

// Element path (dialog.content.form) of the machine settings modal dialog form.
#define REMOTELPBF_MACHINESETTINGS_DIALOGFORM "machinesettingsdialog.msdlgcontent.msdlgform."

// Maximum length of a configuration version name, as accepted by MachineConfiguration.Commit.
#define REMOTELPBF_CONFIGVERSION_MAXNAMELENGTH 256

// Reads the configuration version name typed into the "versionname" field of the given dialog
// form. The typed text is stored back into the session variable, so resetting it to "" on the
// next opening clears the field on the client. Returns false and shows a hint if the name is
// empty or too long.
static bool readConfigurationVersionName(LibMCEnv::PUIEnvironment pUIEnvironment, const std::string& sFormPath, std::string& sName)
{
	auto sTypedName = pUIEnvironment->GetUIProperty(sFormPath + "versionname", "value");
	pUIEnvironment->SetSessionVariable("configversion_name", sTypedName);

	sName = trimWhitespace(sTypedName);
	if (sName.empty()) {
		pUIEnvironment->ShowHint("Please enter a name for the new configuration version.", 3000);
		return false;
	}

	if (sName.length() > REMOTELPBF_CONFIGVERSION_MAXNAMELENGTH) {
		pUIEnvironment->ShowHint("The configuration name must not be longer than " + std::to_string(REMOTELPBF_CONFIGVERSION_MAXNAMELENGTH) + " characters.", 3000);
		return false;
	}

	return true;
}

// Helper: activates the configuration version selected in a configuration list of the given
// schema and asks the main state machine to re-apply the configuration to live parameters.
static void activateSelectedConfiguration(LibMCEnv::PUIEnvironment pUIEnvironment, const std::string& sSchema, const std::string& sLabel)
{
	auto sSender = pUIEnvironment->RetrieveEventSender();
	auto sVersionUUID = pUIEnvironment->GetUIPropertyAsUUID(sSender, "selecteduuid");

	if (sVersionUUID.empty() || (sVersionUUID == "00000000-0000-0000-0000-000000000000")) {
		pUIEnvironment->LogWarning("Activate " + sLabel + " ignored: no configuration selected.");
		return;
	}

	auto pHandler = pUIEnvironment->CreateMachineConfigurationHandler();
	auto pType = pHandler->FindConfigurationTypeBySchema(sSchema);
	if (pType.get() == nullptr) {
		pUIEnvironment->LogWarning("Activate " + sLabel + " failed: configuration type not registered.");
		return;
	}

	pType->SetActiveConfigurationVersion(sVersionUUID);
	pUIEnvironment->LogMessage("Activated " + sLabel + " " + sVersionUUID);

	auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_reloadconfiguration");
	pSignal->Trigger();
}


// Activates the machine-settings configuration version selected in its list.
class CEvent_OnActivateMachineSettings : public virtual CEvent {
public:
	static std::string getEventName() { return "onactivatemachinesettings"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		activateSelectedConfiguration(pUIEnvironment, REMOTELPBF_CONFIG_SCHEMA, "machine settings configuration");
	}
};


// Opens the machine settings modal dialog. The dialog form fields are pre-populated from the
// live parameters via sync:value, so the operator edits a copy and confirms with OK.
class CEvent_OnOpenMachineSettings : public virtual CEvent {
public:
	static std::string getEventName() { return "onopenmachinesettings"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->SetSessionVariable("configversion_name", "");
		pUIEnvironment->ActivateModalDialog("machinesettingsdialog");
	}
};


// Confirms the machine settings modal dialog: reads the edited values, stores them into a new
// machine-settings configuration version with the entered name, activates it, asks the main
// state machine to re-apply it and closes the dialog. Keeps the dialog open if no name is given.
class CEvent_OnMachineSettingsOK : public virtual CEvent {
public:
	static std::string getEventName() { return "onmachinesettingsok"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		const std::string sForm = REMOTELPBF_MACHINESETTINGS_DIALOGFORM;

		std::string sVersionName;
		if (!readConfigurationVersionName(pUIEnvironment, sForm, sVersionName))
			return;

		bool bSimulateLaser = pUIEnvironment->GetUIPropertyAsBool(sForm + "simulatelaser", "value");
		int64_t nExposureTimeout = pUIEnvironment->GetUIPropertyAsInteger(sForm + "exposuretimeout", "value");
		int64_t nWatchdogTimeout = pUIEnvironment->GetUIPropertyAsInteger(sForm + "watchdogtimeout", "value");

		auto pHandler = pUIEnvironment->CreateMachineConfigurationHandler();
		auto pType = pHandler->FindConfigurationTypeBySchema(REMOTELPBF_CONFIG_SCHEMA);
		if (pType.get() == nullptr) {
			pUIEnvironment->LogWarning("Change machine settings failed: configuration type not registered.");
			pUIEnvironment->CloseModalDialog();
			return;
		}

		auto pConfig = pType->EnsureActiveConfiguration();
		pConfig->SetBoolParameter("Simulation/SimulateLaser", bSimulateLaser);
		pConfig->SetIntegerParameter("Process/ExposureTimeout", nExposureTimeout);
		pConfig->SetIntegerParameter("Process/WatchdogTimeout", nWatchdogTimeout);

		pConfig->SetIntegerParameter("ScannerCard/Serial", pUIEnvironment->GetUIPropertyAsInteger(sForm + "card_serial", "value"));
		pConfig->SetStringParameter("ScannerCard/IPAddress", trimWhitespace(pUIEnvironment->GetUIProperty(sForm + "card_ipaddress", "value")));
		pConfig->SetStringParameter("ScannerCard/Netmask", trimWhitespace(pUIEnvironment->GetUIProperty(sForm + "card_netmask", "value")));
		pConfig->SetIntegerParameter("ScannerCard/Port", pUIEnvironment->GetUIPropertyAsInteger(sForm + "card_port", "value"));
		pConfig->SetIntegerParameter("ScannerCard/Timeout", pUIEnvironment->GetUIPropertyAsInteger(sForm + "card_timeout", "value"));

		const std::pair<const char*, const char*> cardDoubleFields[] = {
			{ "card_laserondelay", "ScannerCard/LaserOnDelay" },
			{ "card_laseroffdelay", "ScannerCard/LaserOffDelay" },
			{ "card_markdelay", "ScannerCard/MarkDelay" },
			{ "card_jumpdelay", "ScannerCard/JumpDelay" },
			{ "card_polygondelay", "ScannerCard/PolygonDelay" },
			{ "card_laseroriginx", "ScannerCard/LaserOriginX" },
			{ "card_laseroriginy", "ScannerCard/LaserOriginY" },
			{ "card_laserfield_minx", "ScannerCard/LaserFieldMinX" },
			{ "card_laserfield_miny", "ScannerCard/LaserFieldMinY" },
			{ "card_laserfield_maxx", "ScannerCard/LaserFieldMaxX" },
			{ "card_laserfield_maxy", "ScannerCard/LaserFieldMaxY" },
			{ "card_transformation_angle", "ScannerCard/TransformationAngle" },
			{ "card_transformation_offsetx", "ScannerCard/TransformationOffsetX" },
			{ "card_transformation_offsety", "ScannerCard/TransformationOffsetY" },
			{ "card_transformation_scalex", "ScannerCard/TransformationScaleX" },
			{ "card_transformation_scaley", "ScannerCard/TransformationScaleY" },
		};
		for (auto& field : cardDoubleFields)
			pConfig->SetDoubleParameter(field.second, pUIEnvironment->GetUIPropertyAsDouble(sForm + field.first, "value"));

		auto pNewVersion = pConfig->CommitAndActivate(pUIEnvironment->GetCurrentUserUUID(), sVersionName);
		pUIEnvironment->LogMessage("Saved machine settings configuration v" + std::to_string(pNewVersion->GetNumericVersion()) + " \"" + sVersionName + "\"");

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_reloadconfiguration");
		pSignal->Trigger();

		pUIEnvironment->CloseModalDialog();
	}
};


// Cancels the machine settings modal dialog without committing any change.
class CEvent_OnMachineSettingsCancel : public virtual CEvent {
public:
	static std::string getEventName() { return "onmachinesettingscancel"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		// Stored so that resetting it to "" on the next opening clears the field on the client.
		pUIEnvironment->SetSessionVariable("configversion_name", pUIEnvironment->GetUIProperty(REMOTELPBF_MACHINESETTINGS_DIALOGFORM "versionname", "value"));
		pUIEnvironment->CloseModalDialog();
	}
};


// Opens the machine settings version history modal dialog.
class CEvent_OnOpenMachineHistory : public virtual CEvent {
public:
	static std::string getEventName() { return "onopenmachinehistory"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->ActivateModalDialog("machinehistorydialog");
	}
};


// Generic "close the current modal dialog" handler, used e.g. by the history dialog's Back
// button.
class CEvent_OnCloseDialog : public virtual CEvent {
public:
	static std::string getEventName() { return "onclosedialog"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->CloseModalDialog();
	}
};


// "Expose Layer" button in the build library: exposes the layer currently shown in the preview of
// the selected build. The preview slider reports every layer change back to the server, so the
// "currentlayer" property always matches what the operator sees.
class CEvent_OnExposePreviewLayer : public virtual CEvent {
public:
	static std::string getEventName() { return "onexposepreviewlayer"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!pUIEnvironment->CheckPermission("permission_start_build")) {
			pUIEnvironment->ShowHint("You are not allowed to expose layers.", 3000);
			return;
		}

		auto sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID("importbuildjob.preview", "builduuid");
		if (sBuildUUID.empty() || (sBuildUUID == REMOTELPBF_NULLUUID)) {
			pUIEnvironment->ShowHint("Please select a build first.", 3000);
			return;
		}

		auto nLayerIndex = pUIEnvironment->GetUIPropertyAsInteger("importbuildjob.preview", "currentlayer");

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_exposesinglelayer");
		pSignal->SetString("jobuuid", sBuildUUID);
		pSignal->SetInteger("layerindex", nLayerIndex);
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(5000)) {
			pUIEnvironment->ShowHint("The machine is busy - layer was not exposed.", 3000);
			return;
		}

		if (pSignal->GetBoolResult("accepted")) {
			pUIEnvironment->LogMessage("Exposing preview layer " + std::to_string(nLayerIndex));
			pUIEnvironment->ShowHint("Exposing layer " + std::to_string(nLayerIndex) + "...", 3000);
		}
		else {
			pUIEnvironment->ShowHint("Layer " + std::to_string(nLayerIndex) + " not exposed: " + pSignal->GetStringResult("errormessage"), 5000);
		}
	}
};


/*************************************************************************************************************************
 Remote execution events

 These handlers implement the machine's remote control contract. They are meant to be driven by
 an external master (PLC/MES/cell controller) over the framework's REST external-event interface
 (POST /api/ext/<eventname>), but "cancelexecution" is also wired to the UI Cancel button.

 All of them require the "permission_remote_execution" permission of the authenticated caller.
**************************************************************************************************************************/

// Permission gate shared by the remote execution events. Returns true if the caller is allowed;
// otherwise logs, sets an "accepted"/"errormessage" result and returns false.
static bool checkRemoteExecutionPermission(LibMCEnv::PUIEnvironment pUIEnvironment)
{
	if (pUIEnvironment->CheckPermission("permission_remote_execution"))
		return true;

	pUIEnvironment->LogWarning("Remote execution request denied: missing permission_remote_execution.");
	pUIEnvironment->SetBoolResult("accepted", false);
	pUIEnvironment->SetStringResult("errormessage", "missing permission_remote_execution");
	return false;
}

// Reports the current execution status to the external caller. No side effects.
class CEvent_ExecutionStatus : public virtual CEvent {
public:
	static std::string getEventName() { return "executionstatus"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!checkRemoteExecutionPermission(pUIEnvironment))
			return;

		bool bExecutionActive = pUIEnvironment->GetMachineParameterAsBool("main", "execution", "executionactive");
		bool bExposing = pUIEnvironment->GetMachineParameterAsBool("main", "execution", "exposing");
		int64_t nCurrentLayer = pUIEnvironment->GetMachineParameterAsInteger("main", "execution", "currentlayer");
		int64_t nLastExposedLayer = pUIEnvironment->GetMachineParameterAsInteger("main", "execution", "lastexposedlayer");
		int64_t nExposureCount = pUIEnvironment->GetMachineParameterAsInteger("main", "execution", "exposurecount");
		int64_t nWatchdogRemaining = pUIEnvironment->GetMachineParameterAsInteger("main", "execution", "watchdogremainingms");
		bool bLaserError = pUIEnvironment->GetMachineParameterAsBool("main", "execution", "lasterror");
		int64_t nLayerCount = pUIEnvironment->GetMachineParameterAsInteger("main", "jobinfo", "layercount");
		std::string sJobName = pUIEnvironment->GetMachineParameter("main", "jobinfo", "jobname");
		std::string sJobUUID = pUIEnvironment->GetMachineParameter("main", "jobinfo", "jobuuid");
		std::string sStatus = pUIEnvironment->GetMachineParameter("main", "ui", "executionstatusdisplay");

		bool bLaserOK = pUIEnvironment->GetMachineParameterAsBool("laser", "laserfeedback", "laserok");

		pUIEnvironment->SetBoolResult("accepted", true);
		pUIEnvironment->SetBoolResult("executionactive", bExecutionActive);
		pUIEnvironment->SetBoolResult("exposing", bExposing);
		pUIEnvironment->SetIntegerResult("currentlayer", nCurrentLayer);
		pUIEnvironment->SetIntegerResult("lastexposedlayer", nLastExposedLayer);
		pUIEnvironment->SetIntegerResult("exposurecount", nExposureCount);
		pUIEnvironment->SetIntegerResult("layercount", nLayerCount);
		pUIEnvironment->SetIntegerResult("watchdogremainingms", nWatchdogRemaining);
		pUIEnvironment->SetBoolResult("laserok", bLaserOK && !bLaserError);
		pUIEnvironment->SetStringResult("jobname", sJobName);
		pUIEnvironment->SetStringResult("jobuuid", sJobUUID);
		pUIEnvironment->SetStringResult("status", sStatus);
	}
};

// Requests exposure of a specific layer. Accepts a JSON body {"layerindex": <n>}. The request is
// forwarded to the main state machine, which validates the layer range and laser readiness and
// returns synchronously whether the request was accepted; the exposure itself then runs
// asynchronously (poll "executionstatus" until "exposurecount" has increased).
class CEvent_ExposeLayer : public virtual CEvent {
public:
	static std::string getEventName() { return "exposelayer"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!checkRemoteExecutionPermission(pUIEnvironment))
			return;

		// Validate and parse the requested layer index from the external JSON parameters.
		int64_t nLayerIndex = 0;
		try {
			auto pParameters = pUIEnvironment->GetExternalEventParameters();
			if (!pParameters->HasMember("layerindex")) {
				pUIEnvironment->SetBoolResult("accepted", false);
				pUIEnvironment->SetStringResult("errormessage", "missing required parameter 'layerindex'");
				return;
			}
			nLayerIndex = pParameters->GetIntegerValue("layerindex");
		}
		catch (std::exception& e) {
			pUIEnvironment->SetBoolResult("accepted", false);
			pUIEnvironment->SetStringResult("errormessage", std::string("invalid 'layerindex' parameter: ") + e.what());
			return;
		}

		if (nLayerIndex < 0) {
			pUIEnvironment->SetBoolResult("accepted", false);
			pUIEnvironment->SetStringResult("errormessage", "'layerindex' must not be negative");
			return;
		}

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_exposelayer");
		pSignal->SetInteger("layerindex", nLayerIndex);
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(30000)) {
			pUIEnvironment->SetBoolResult("accepted", false);
			pUIEnvironment->SetStringResult("errormessage", "timeout waiting for the machine to accept the request");
			return;
		}

		bool bAccepted = pSignal->GetBoolResult("accepted");
		std::string sError = pSignal->GetStringResult("errormessage");

		pUIEnvironment->SetBoolResult("accepted", bAccepted);
		pUIEnvironment->SetStringResult("errormessage", sError);
		pUIEnvironment->SetIntegerResult("layerindex", nLayerIndex);
	}
};

// Ends the running execution. This is the regular end of an execution, as layers can be
// exposed in any order and the machine cannot know when the master is done.
class CEvent_FinishExecution : public virtual CEvent {
public:
	static std::string getEventName() { return "finishexecution"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!checkRemoteExecutionPermission(pUIEnvironment))
			return;

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_finishexecution");
		pSignal->Trigger();

		pUIEnvironment->SetBoolResult("accepted", true);
	}
};

// Keeps the execution alive between layers by re-arming the watchdog. Only signaled while the main
// state machine waits for a layer, so the signal cannot pile up during an exposure. Kept separate from
// "executionstatus" so a monitoring client cannot keep an abandoned execution alive by accident.
class CEvent_KeepAlive : public virtual CEvent {
public:
	static std::string getEventName() { return "keepalive"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!checkRemoteExecutionPermission(pUIEnvironment))
			return;

		bool bWaiting = (pUIEnvironment->GetMachineState("main") == "execution_waiting");
		if (bWaiting) {
			auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_keepalive");
			pSignal->Trigger();
		}

		pUIEnvironment->SetBoolResult("accepted", true);
		pUIEnvironment->SetBoolResult("rearmed", bWaiting);
		pUIEnvironment->SetBoolResult("executionactive", pUIEnvironment->GetMachineParameterAsBool("main", "execution", "executionactive"));
		pUIEnvironment->SetIntegerResult("watchdogremainingms", pUIEnvironment->GetMachineParameterAsInteger("main", "execution", "watchdogremainingms"));
	}
};

// Cancels the running execution. Wired both to the REST interface and to the UI Cancel button.
class CEvent_CancelExecution : public virtual CEvent {
public:
	static std::string getEventName() { return "cancelexecution"; }

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (!checkRemoteExecutionPermission(pUIEnvironment))
			return;

		pUIEnvironment->LogMessage("Cancel execution requested.");
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_cancelbuild");
		pSignal->Trigger();

		pUIEnvironment->SetBoolResult("accepted", true);
	}
};


IEvent* CEventHandler::CreateEvent(const std::string& sEventName, LibMCEnv::PUIEnvironment pUIEnvironment)
{
	IEvent* pEventInstance = nullptr;
	if (createEventInstanceByName<CEvent_StartBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_CloseBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnUploadFinished>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnSelectBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnSelectPart>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnSelectProcessPart>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnProcessPartInfo>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnProcessPartDisable>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnConfirmDisablePart>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnCancelDisablePart>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnResetMain>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnResetLaser>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_Logout>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnActivateMachineSettings>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnOpenMachineSettings>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnMachineSettingsOK>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnMachineSettingsCancel>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnOpenMachineHistory>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnCloseDialog>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnExposePreviewLayer>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ExposeLayer>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ExecutionStatus>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_FinishExecution>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_CancelExecution>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_KeepAlive>(sEventName, pEventInstance))
		return pEventInstance;

	throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDEVENTNAME, "invalid event name: " + sEventName);
}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
