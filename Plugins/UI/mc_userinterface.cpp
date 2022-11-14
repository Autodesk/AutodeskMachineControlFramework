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

using namespace LibMCUI::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

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

		pUIEnvironment->LogMessage("Clicked on Logout");
		pUIEnvironment->LogOut();

	}

};

/*************************************************************************************************************************
 Class declaration of CEvent_StartBuildPreparation
**************************************************************************************************************************/

class CEvent_StartBuildPreparation : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "startbuildpreparation";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on StartBuildPreparation Button");
		std::string sBuildUUID = pUIEnvironment->GetUIPropertyAsUUID("previewbuild.preview", "builduuid");
		pUIEnvironment->LogMessage("Preparing build " + sBuildUUID);

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_preparebuildjob");
		pSignal->SetString("jobuuid", sBuildUUID);
		pSignal->SetString("jobname", "Job");
		pSignal->Trigger(); 

	}

};



/*************************************************************************************************************************
 Class declaration of CEvent_CancelBuildPreparation
**************************************************************************************************************************/

class CEvent_CancelBuildPreparation : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "cancelbuildpreparation";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on CancelBuildPreparation Button");

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_cancelbuildpreparation");
		pSignal->Trigger();
		

	}

};



/*************************************************************************************************************************
 Class declaration of CEvent_StartBuild
**************************************************************************************************************************/

class CEvent_StartBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "startbuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on StartBuild Button");

		auto sBuildUUID = pUIEnvironment->GetMachineParameterAsUUID("main", "jobinfo", "jobuuid");
		pUIEnvironment->SetUIPropertyAsUUID("buildstatus.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("buildstatus.preview", "currentlayer", 2);

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_startbuild");
		pSignal->Trigger();

		pUIEnvironment->ActivatePage("buildstatus");

	}

};


/*************************************************************************************************************************
 Class declaration of CEvent_StartBuild
**************************************************************************************************************************/

class CEvent_NewLayerStarted : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "newlayerstarted";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		auto nCurrentLayer = pUIEnvironment->GetMachineParameterAsInteger("main", "jobinfo", "currentlayer");

		pUIEnvironment->LogMessage("New layer started: " + std::to_string (nCurrentLayer));
		pUIEnvironment->SetUIPropertyAsInteger("buildstatus.preview", "currentlayer", nCurrentLayer);

	}

};


class CEvent_ChangeManualValues : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "changemanualvalues";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		double dO2Value = pUIEnvironment->GetMachineParameterAsDouble("main", "processsettings", "targeto2");
		double dGasFlowSpeed = pUIEnvironment->GetMachineParameterAsDouble("main", "processsettings", "gasflowspeed");
		double dRecoaterSpeed = pUIEnvironment->GetMachineParameterAsDouble("main", "processsettings", "recoaterspeed");
		pUIEnvironment->SetUIPropertyAsDouble("testdialog.infobox.processparameters.o2_value", "value", dO2Value);
		pUIEnvironment->SetUIPropertyAsDouble("testdialog.infobox.processparameters.gasflowspeed", "value", dGasFlowSpeed);
		pUIEnvironment->SetUIPropertyAsDouble("testdialog.infobox.processparameters.recoaterspeed", "value", dRecoaterSpeed);

		pUIEnvironment->ActivateModalDialog("testdialog");

	}

};


class CEvent_OnProcessParameterSave : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onprocessparametersave";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		auto dO2Value = pUIEnvironment->GetUIPropertyAsDouble("testdialog.infobox.processparameters.o2_value", "value");
		auto dGasFlowSpeed = pUIEnvironment->GetUIPropertyAsDouble("testdialog.infobox.processparameters.gasflowspeed", "value");
		auto dRecoaterSpeed = pUIEnvironment->GetUIPropertyAsDouble("testdialog.infobox.processparameters.recoaterspeed", "value");

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_changeprocesssettings");
		pSignal->SetDouble("targeto2", dO2Value);
		pSignal->SetDouble("gasflowspeed", dGasFlowSpeed);
		pSignal->SetDouble("recoaterspeed", dRecoaterSpeed);
		pSignal->Trigger();


		pUIEnvironment->CloseModalDialog();
	}

};

class CEvent_OnProcessParameterCancel : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "onprocessparametercancel";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->CloseModalDialog();
	}

};

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

		pUIEnvironment->SetUIPropertyAsUUID("previewbuild.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("previewbuild.preview", "currentlayer", 2);

		pUIEnvironment->ActivatePage("previewbuild");
	}

};


class CEvent_OnUnloadBuildPreview : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "unloadbuildpreview";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		pUIEnvironment->LogMessage("Unloading build preview...");

		pUIEnvironment->SetUIPropertyAsUUID("previewbuild.preview", "builduuid", "");
		pUIEnvironment->SetUIPropertyAsInteger("previewbuild.preview", "currentlayer", 1);
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

		pUIEnvironment->SetUIPropertyAsUUID("previewbuild.preview", "builduuid", sBuildUUID);
		pUIEnvironment->SetUIPropertyAsInteger("previewbuild.preview", "currentlayer", 2);

		pUIEnvironment->ActivatePage("previewbuild");
	}

};


class CEvent_OnChangeSimulationParameterEvent : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "changesimulationparameter";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		auto sSender = pUIEnvironment->RetrieveEventSender();
		pUIEnvironment->LogMessage("Change simulation parameter from " + sSender);

		bool bValue = pUIEnvironment->GetUIPropertyAsBool(sSender, "value");
		pUIEnvironment->LogMessage("Change " + sSender + " to " + std::to_string (bValue));

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_changesimulationparameters");
		pSignal->SetBool("simulatelaser", pUIEnvironment->GetUIPropertyAsBool("main.infobox.simulationparameters.simulatelaser", "value"));
		pSignal->SetBool("simulateplc", pUIEnvironment->GetUIPropertyAsBool("main.infobox.simulationparameters.simulateplc", "value"));
		pSignal->Trigger();
	}

};


class CEvent_TestMovement : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "testmovement";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on TestMovement Button");

		auto pSignal = pUIEnvironment->PrepareSignal("plc", "signal_recoatlayer");
		pSignal->Trigger();


	}

};


class CEvent_PauseBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "pausebuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on Pause Build");

	}

};


class CEvent_CancelBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "cancelbuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on Cancel Build");

	}

};


class CEvent_ResumeBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "resumebuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{

		pUIEnvironment->LogMessage("Clicked on Resume Build");

	}

};


IEvent* CEventHandler::CreateEvent(const std::string& sEventName, LibMCEnv::PUIEnvironment pUIEnvironment)
{
	IEvent* pEventInstance = nullptr;
	if (createEventInstanceByName<CEvent_StartBuildPreparation>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_CancelBuildPreparation>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_StartBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ChangeManualValues>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnProcessParameterSave>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnProcessParameterCancel>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnUploadFinished>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnSelectBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_OnUnloadBuildPreview>(sEventName, pEventInstance))
		return pEventInstance;	
	if (createEventInstanceByName<CEvent_OnChangeSimulationParameterEvent>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_TestMovement>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_CancelBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_PauseBuild>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_ResumeBuild>(sEventName, pEventInstance))
		return pEventInstance;	
	if (createEventInstanceByName<CEvent_NewLayerStarted>(sEventName, pEventInstance))
		return pEventInstance;
	if (createEventInstanceByName<CEvent_Logout>(sEventName, pEventInstance))
		return pEventInstance;

	
	

	throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDEVENTNAME, "invalid event name: " + sEventName);
}




#ifdef _MSC_VER
#pragma warning(pop)
#endif
