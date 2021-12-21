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

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_preparebuildjob");
		//pSignal->SetString("jobuuid", sJobUUID);
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

		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_startbuild");
		pSignal->Trigger();

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


	throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDEVENTNAME);
}




#ifdef _MSC_VER
#pragma warning(pop)
#endif
