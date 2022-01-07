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

#include "libmcplugin_statefactory.hpp"
#include "libmcplugin_interfaceexception.hpp"
#include "libmcplugin_state.hpp"

using namespace LibMCPlugin::Impl;

#include "libmcenv_drivercast.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


/*************************************************************************************************************************
 Class definition of CMainData
**************************************************************************************************************************/
class CMainData : public virtual CPluginData {
protected:

public:

};

/*************************************************************************************************************************
 Class definition of CMainState
**************************************************************************************************************************/
typedef CState<CMainData> CMainState;




/*************************************************************************************************************************
 Class definition of CMainState_Init
**************************************************************************************************************************/
class CMainState_Init : public virtual CMainState {
public:

	CMainState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CMainState (getStateName (), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		pStateEnvironment->LogMessage("Initializing...");

		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		pStateEnvironment->SetNextState("idle");

	}

};


/*************************************************************************************************************************
 Class definition of CMainState_Idle
**************************************************************************************************************************/
class CMainState_Idle : public virtual CMainState {
public:

	CMainState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "idle";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		LibMCEnv::PSignalHandler pSignalHandler;

		double targetO2 = pStateEnvironment->GetDoubleParameter("processsettings", "targeto2");
		pStateEnvironment->SetDoubleParameter("processsettings", "targeto2", targetO2 + 1.0);

		pStateEnvironment->SetBoolParameter("ui", "preparebuilddisabled", !pStateEnvironment->GetBoolParameter("ui", "preparebuilddisabled"));

		pStateEnvironment->LogMessage ("Waiting for user input...");
		if (pStateEnvironment->WaitForSignal("signal_preparebuildjob", 100, pSignalHandler)) {
			auto sJobUUID = pSignalHandler->GetUUID("jobuuid");
			pStateEnvironment->SetStringParameter("jobinfo", "jobuuid", sJobUUID);
			pSignalHandler->SignalHandled();

			pStateEnvironment->LogMessage("Preparing job " + sJobUUID);

			pStateEnvironment->SetNextState("preparebuild");

		}
		else if (pStateEnvironment->WaitForSignal("signal_changeprocesssettings", 100, pSignalHandler)) {
			double dTargetO2 = pSignalHandler->GetDouble("targeto2");
			double dRecoaterSpeed = pSignalHandler->GetDouble("recoaterspeed");
			double dGasFlowSpeed = pSignalHandler->GetDouble("gasflowspeed");
			pSignalHandler->SignalHandled();

			pStateEnvironment->SetDoubleParameter("processsettings", "targeto2", dTargetO2);
			pStateEnvironment->SetDoubleParameter("processsettings", "recoaterspeed", dRecoaterSpeed);
			pStateEnvironment->SetDoubleParameter("processsettings", "gasflowspeed", dGasFlowSpeed);

			pStateEnvironment->LogMessage("Updated process Parameters!");

			pStateEnvironment->SetNextState("idle");

		}
		else if (pStateEnvironment->WaitForSignal("signal_changesimulationparameters", 100, pSignalHandler)) {
			bool bSimulateLaser = pSignalHandler->GetBool("simulatelaser");
			bool bSimulatePLC = pSignalHandler->GetBool("simulateplc");
			pSignalHandler->SignalHandled();

			pStateEnvironment->SetBoolParameter("configuration", "simulatelaser", bSimulateLaser);
			pStateEnvironment->SetBoolParameter("configuration", "simulateplc", bSimulatePLC);

			pStateEnvironment->LogMessage("Updated Simulation Parameters!");

			pStateEnvironment->SetNextState("idle");

		}
		else {

			pStateEnvironment->SetNextState("idle");

		}

	}

};




/*************************************************************************************************************************
 Class definition of CMainState_PrepareBuild
**************************************************************************************************************************/
class CMainState_PrepareBuild : public virtual CMainState {
public:

	CMainState_PrepareBuild(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "preparebuild";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Waiting for build preparation...");
		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_cancelbuildpreparation", 100, pSignalHandler)) {
			pStateEnvironment->SetStringParameter("jobinfo", "jobuuid", "00000000-0000-0000-0000-000000000000");
			pSignalHandler->SignalHandled();
			pStateEnvironment->SetNextState("idle");

		}
		else if (pStateEnvironment->WaitForSignal("signal_startbuild", 100, pSignalHandler)) {
			pSignalHandler->SignalHandled();
			pStateEnvironment->SetNextState("initbuild");

		}
		else {
			pStateEnvironment->SetNextState("preparebuild");
		}
	}

};



/*************************************************************************************************************************
 Class definition of CMainState_InitBuild
**************************************************************************************************************************/
class CMainState_InitBuild : public virtual CMainState {
public:

	CMainState_InitBuild(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "initbuild";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Initializing build...");
		auto sJobUUID = pStateEnvironment->GetUUIDParameter("jobinfo", "jobuuid");

		pStateEnvironment->LogMessage("Loading Toolpath...");
		auto pBuildJob = pStateEnvironment->GetBuildJob (sJobUUID);
		pBuildJob->LoadToolpath();

		auto sJobName = pBuildJob->GetName();
		auto nLayerCount = pBuildJob->GetLayerCount();

		pStateEnvironment->LogMessage("Job Name: " + sJobName);
		pStateEnvironment->LogMessage("Layer Count: " + std::to_string (nLayerCount));

		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);

		pStateEnvironment->Sleep(1000);
		pStateEnvironment->LogMessage("Waiting for atmosphere...");
		pStateEnvironment->Sleep(3000);

		pStateEnvironment->SetNextState("beginlayer");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_BeginLayer
**************************************************************************************************************************/
class CMainState_BeginLayer : public virtual CMainState {
public:

	CMainState_BeginLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "beginlayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		auto nLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
		pStateEnvironment->LogMessage("Starting layer " + std::to_string (nLayer));


		pStateEnvironment->SetNextState("recoatlayer");
	}

};



/*************************************************************************************************************************
 Class definition of CMainState_BeginLayer
**************************************************************************************************************************/
class CMainState_RecoatLayer : public virtual CMainState {
public:

	CMainState_RecoatLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "recoatlayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Recoating layer...");
		
		auto nRecoatingTimeOut = pStateEnvironment->GetIntegerParameter("jobinfo", "recoatingtimeout");

		auto pSignal = pStateEnvironment->PrepareSignal("plc", "signal_recoatlayer");
		pSignal->Trigger();

		if (pSignal->WaitForHandling((uint32_t)nRecoatingTimeOut)) {

			if (pSignal->GetBoolResult("success")) {

				pStateEnvironment->LogMessage("Recoating successful...");
				pStateEnvironment->SetNextState("exposelayer");
			}
			else {
				pStateEnvironment->LogMessage("Recoating failed...");
				pStateEnvironment->SetNextState("fatalerror");

			}

		}
		else {
			pStateEnvironment->LogMessage("Recoating timeout...");
			pStateEnvironment->SetNextState("fatalerror");
		}


		
	}

};



/*************************************************************************************************************************
 Class definition of CMainState_ExposeLayer
**************************************************************************************************************************/
class CMainState_ExposeLayer : public virtual CMainState {
public:

	CMainState_ExposeLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "exposelayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Exposing layer...");

		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		auto nCurrentLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
		auto nExposureTimeOut = pStateEnvironment->GetIntegerParameter("jobinfo", "exposuretimeout");

		auto pSignal = pStateEnvironment->PrepareSignal("laser", "signal_exposure");
		pSignal->SetString("jobuuid", sJobUUID);
		pSignal->SetInteger("layerindex", nCurrentLayer);
		pSignal->SetInteger("timeout", nExposureTimeOut);
		pSignal->Trigger();

		if (pSignal->WaitForHandling((uint32_t)nExposureTimeOut)) {
			pStateEnvironment->LogMessage("Layer successfully exposed...");
			pStateEnvironment->SetNextState("finishlayer");

		}
		else {
			pStateEnvironment->LogMessage("Layer exposure failed...");
			pStateEnvironment->SetNextState("fatalerror");
		}

	}

};



/*************************************************************************************************************************
 Class definition of CMainState_FinishLayer
**************************************************************************************************************************/
class CMainState_FinishLayer : public virtual CMainState {
public:

	CMainState_FinishLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "finishlayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		auto nLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
		auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");

		pStateEnvironment->LogMessage("Finished layer " + std::to_string (nLayer));

		nLayer++;
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", nLayer);

		if (nLayer < nLayerCount) {
			pStateEnvironment->SetNextState("beginlayer");
		}
		else {
			pStateEnvironment->SetNextState("finishbuild");
		}

	}

};


/*************************************************************************************************************************
 Class definition of CMainState_FinishBuild
**************************************************************************************************************************/
class CMainState_FinishBuild : public virtual CMainState {
public:

	CMainState_FinishBuild(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "finishbuild";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Finishing Build...");
		pStateEnvironment->LogMessage("Turning laser off...");
		pStateEnvironment->Sleep(1000);

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_PauseBuild
**************************************************************************************************************************/
class CMainState_PauseBuild : public virtual CMainState {
public:

	CMainState_PauseBuild(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "pausebuild";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Build paused");
		pStateEnvironment->Sleep(3000);

		pStateEnvironment->SetNextState("pausebuild");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CMainState_FatalError : public virtual CMainState {
public:

	CMainState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "fatalerror";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		// Unload all toolpathes that might be in memory
		pStateEnvironment->UnloadAllToolpathes();

		pStateEnvironment->SetNextState("fatalerror");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_CancelBuild
**************************************************************************************************************************/
class CMainState_CancelBuild : public virtual CMainState {
public:

	CMainState_CancelBuild(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "cancelbuild";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Canceling Build...");
		pStateEnvironment->LogMessage("Turning laser off...");
		pStateEnvironment->Sleep(1000);

		pStateEnvironment->SetNextState("idle");
	}

};



/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName)
{
	m_pPluginData = std::make_shared<CMainData>();
}

IState * CStateFactory::CreateState(const std::string & sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CMainState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_PrepareBuild>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_InitBuild>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_BeginLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_RecoatLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_ExposeLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_FinishLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_FinishBuild>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_PauseBuild>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_CancelBuild>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
