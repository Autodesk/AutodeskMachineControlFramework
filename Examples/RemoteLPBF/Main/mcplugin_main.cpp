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

/*

 Autodesk Remote LPBF - "main" state machine.

 This is the execution orchestrator of the Remote LPBF variant. Unlike the PC-master
 SLS specialization it was derived from, this machine is oblivious to the machine
 periphery (axes, recoating, temperature, gas). It only manages build-data and drives
 the Scanlab RTC6 scanner through the "laser" state machine.

 Execution is driven by an external master (a PLC/MES/cell controller) over the
 framework's REST external-event interface. Once an operator selects a build and starts
 execution, the external master triggers one layer exposure at a time, in any order.
 The execution ends when the master finishes it. Execution is cancelable but NOT pausable.

 State flow:
   waitforinit -> init -> idle
   idle --(signal_startexecution)--> initexecution -> execution_waiting
   execution_waiting --(signal_exposelayer, 0 <= index < layercount)--> execution_exposing
   execution_exposing --(layer done)--> execution_waiting
   execution_waiting --(signal_finishexecution)--> finishexecution -> idle
   execution_waiting --(cancel / watchdog)--> cancelexecution -> idle
   execution_waiting --(signal_keepalive)--> re-arms the watchdog, stays waiting
   execution_exposing --(cancel latched)--> cancelexecution -> idle
   any --(laser error / exception)--> fatalerror

*/

#include "libmcplugin_statefactory.hpp" 
#include "libmcplugin_interfaceexception.hpp"
#include "libmcplugin_state.hpp" 

#include "libmcenv_drivercast.hpp"

#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>
#include <stdexcept>

using namespace LibMCPlugin::Impl;

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
 State type definition
**************************************************************************************************************************/
typedef CState<CMainData> CMainState;


/*************************************************************************************************************************
 Persistent machine configuration
**************************************************************************************************************************/

// Schema identifier of the persistent Remote LPBF machine settings configuration.
#define REMOTELPBF_CONFIG_SCHEMA "com.autodesk.remotelpbf"

// Registers the machine-settings configuration type (idempotent), bootstraps schema v1 from
// the factory defaults declared in config.xml and synchronizes the active version to the mapped
// live state-machine parameters. Publishes a status summary for the configuration page.
static void applyMachineSettingsToRuntime(LibMCEnv::PStateEnvironment pStateEnvironment) {
	auto pHandler = pStateEnvironment->CreateMachineConfigurationHandler();
	auto pType = pHandler->RegisterMachineConfigurationType(REMOTELPBF_CONFIG_SCHEMA, "Autodesk Remote LPBF Machine Settings");

	std::string sXSD = pStateEnvironment->LoadResourceString("remotelpbf_cfg_schema_v1");
	pType->EnsureXSDVersionFromParameterDefaults(sXSD, 1);
	auto nAppliedParameterCount = pType->SynchronizeParameters();
	auto pConfig = pType->EnsureActiveConfiguration();

	pStateEnvironment->SetStringParameter("configstatus", "activeversiondisplay", "Version " + std::to_string(pConfig->GetNumericVersion()));

	auto pActiveVersion = pType->GetActiveConfigurationVersion();
	if (pActiveVersion.get() != nullptr) {
		pStateEnvironment->SetStringParameter("configstatus", "activetimestamp", pActiveVersion->GetTimestamp());
		pStateEnvironment->SetStringParameter("configstatus", "activeuser", pActiveVersion->GetUserUUID());
	}

	pStateEnvironment->LogMessage("Machine settings configuration v" + std::to_string(pConfig->GetNumericVersion()) + " applied to " + std::to_string(nAppliedParameterCount) + " parameters.");
}

static void applyMachineConfigurationToRuntime(LibMCEnv::PStateEnvironment pStateEnvironment) {
	try {
		applyMachineSettingsToRuntime(pStateEnvironment);
	}
	catch (std::exception& e) {
		pStateEnvironment->LogWarning(std::string("Failed to apply machine settings configuration: ") + e.what());
	}

	// Releases the laser initialisation, which waits for this flag. Also set on failure so the
	// laser does not hang forever; it then runs with the config.xml defaults.
	pStateEnvironment->SetBoolParameter("configstatus", "applied", true);
}


/*************************************************************************************************************************
 Shared helper functions
**************************************************************************************************************************/

// Mirrors the laser subsystem health into the UI/execution flags. Called from healthy main
// states (idle / execution loop). main's own fatal-error flag is owned by CMainState_FatalError.
static void updateSubsystemErrorFlags(LibMCEnv::PStateEnvironment pStateEnvironment) {
	bool bLaserError = (pStateEnvironment->GetMachineState("laser") == "fatalerror");

	pStateEnvironment->SetBoolParameter("ui", "main_haserror", false);
	pStateEnvironment->SetBoolParameter("ui", "laser_haserror", bLaserError);
	pStateEnvironment->SetBoolParameter("ui", "system_haserror", bLaserError);
	pStateEnvironment->SetBoolParameter("execution", "lasterror", bLaserError);
}

// Switches the laser enable pin via the "laser" state machine. Best-effort: returns whether the
// signal was handled successfully within the timeout. Used to assert laser-off on finish/cancel/
// error and at execution start (the laser state machine enables it per exposure).
static bool setLaserPin(LibMCEnv::PStateEnvironment pStateEnvironment, bool bOn, uint32_t nTimeoutMS) {
	auto pSignal = pStateEnvironment->PrepareSignal("laser", "signal_changelaseronoff");
	pSignal->SetBool("laseron", bOn);
	pSignal->Trigger();

	if (pSignal->WaitForHandling(nTimeoutMS)) {
		return pSignal->GetBoolResult("success");
	}

	pStateEnvironment->LogWarning(std::string("Timeout switching laser pin ") + (bOn ? "on" : "off"));
	return false;
}

// Refreshes the layer/height display parameters (and the preview layer index) after the layer
// with the given (0-based) index has been exposed.
static void updateLayerDisplays(LibMCEnv::PStateEnvironment pStateEnvironment, int64_t nLayerIndex, int64_t nLayerCount, double dCurrentHeight, double dTotalHeight) {
	pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", nLayerIndex);
	pStateEnvironment->SetDoubleParameter("jobinfo", "currentheight", dCurrentHeight);

	std::stringstream sLayerDisplay, sHeightDisplay, sLayerHeightDisplay;
	sLayerDisplay << nLayerIndex << " (0.." << (nLayerCount - 1) << ")";
	sHeightDisplay << std::setprecision(4) << dCurrentHeight << " mm / " << std::setprecision(4) << dTotalHeight << " mm";
	sLayerHeightDisplay << sLayerDisplay.str() << "  \xc2\xb7  " << std::setprecision(4) << dCurrentHeight << " mm";

	pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", sLayerDisplay.str());
	pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", sHeightDisplay.str());
	pStateEnvironment->SetStringParameter("ui", "currentlayerheightdisplay", sLayerHeightDisplay.str());
}


/*************************************************************************************************************************
 Class definition of CMainState_WaitForInit
**************************************************************************************************************************/
class CMainState_WaitForInit : public virtual CMainState {
public:
	CMainState_WaitForInit(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "waitforinit";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->SetNextState("init");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_Init
**************************************************************************************************************************/
class CMainState_Init : public virtual CMainState {
public:
	CMainState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "init";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Initializing...");

		// Discard any stale signals left over from a previous run, and clear the error flags.
		pStateEnvironment->ClearAllUnhandledSignals();
		pStateEnvironment->SetBoolParameter("ui", "main_haserror", false);
		pStateEnvironment->SetBoolParameter("ui", "system_haserror", false);

		// Bootstrap and apply the persistent machine configuration.
		applyMachineConfigurationToRuntime(pStateEnvironment);

		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetDoubleParameter("jobinfo", "totalheight", 0.0);
		pStateEnvironment->SetDoubleParameter("jobinfo", "currentheight", 0.0);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		// Reset the execution state.
		pStateEnvironment->SetBoolParameter("execution", "executionactive", false);
		pStateEnvironment->SetBoolParameter("execution", "exposing", false);
		pStateEnvironment->SetBoolParameter("execution", "cancelrequested", false);
		pStateEnvironment->SetIntegerParameter("execution", "currentlayer", -1);
		pStateEnvironment->SetIntegerParameter("execution", "lastexposedlayer", -1);
		pStateEnvironment->SetIntegerParameter("execution", "exposurecount", 0);
		pStateEnvironment->SetIntegerParameter("execution", "waitingsince", 0);
		pStateEnvironment->SetIntegerParameter("execution", "watchdogremainingms", 0);

		pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "---");
		pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "---");
		pStateEnvironment->SetStringParameter("ui", "currentlayerheightdisplay", "---");
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Idle");

		pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);
		pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", true);

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_Idle
**************************************************************************************************************************/
class CMainState_Idle : public virtual CMainState {
public:
	CMainState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "idle";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		updateSubsystemErrorFlags(pStateEnvironment);

		pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", true);
		pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Idle");

		LibMCEnv::PSignalHandler pSignalHandler;

		if (pStateEnvironment->WaitForSignal("signal_startexecution", 0, pSignalHandler)) {
			auto sJobUUID = pSignalHandler->GetUUID("jobuuid");
			pStateEnvironment->SetStringParameter("jobinfo", "jobuuid", sJobUUID);
			pSignalHandler->SignalHandled();

			pStateEnvironment->LogMessage("Preparing execution of job " + sJobUUID);
			pStateEnvironment->SetNextState("initexecution");
		}
		else if (pStateEnvironment->WaitForSignal("signal_exposesinglelayer", 0, pSignalHandler)) {
			auto sJobUUID = pSignalHandler->GetUUID("jobuuid");
			auto nLayerIndex = pSignalHandler->GetInteger("layerindex");

			std::string sError;
			try {
				auto nLayerCount = (int64_t)pStateEnvironment->GetBuildJob(sJobUUID)->GetLayerCount();
				if (pStateEnvironment->GetBoolParameter("execution", "lasterror"))
					sError = "laser subsystem is not OK";
				else if ((nLayerIndex < 0) || (nLayerIndex >= nLayerCount))
					sError = "layer index " + std::to_string(nLayerIndex) + " is out of range (0.." + std::to_string(nLayerCount - 1) + ")";
			}
			catch (std::exception& e) {
				sError = std::string("build job not found: ") + e.what();
			}

			pSignalHandler->SetBoolResult("accepted", sError.empty());
			pSignalHandler->SetStringResult("errormessage", sError);
			pSignalHandler->SignalHandled();

			if (sError.empty()) {
				pStateEnvironment->SetStringParameter("singleexposure", "jobuuid", sJobUUID);
				pStateEnvironment->SetIntegerParameter("singleexposure", "layerindex", nLayerIndex);
				pStateEnvironment->SetNextState("singleexposure");
			}
			else {
				pStateEnvironment->LogWarning("Rejected single layer exposure: " + sError);
				pStateEnvironment->SetNextState("idle");
			}
		}
		else if (pStateEnvironment->WaitForSignal("signal_changesimulationparameters", 0, pSignalHandler)) {
			bool bSimulateLaser = pSignalHandler->GetBool("simulatelaser");
			pSignalHandler->SignalHandled();

			pStateEnvironment->SetBoolParameter("configuration", "simulatelaser", bSimulateLaser);
			pStateEnvironment->LogMessage("Updated Simulation Parameters!");
			pStateEnvironment->SetNextState("idle");
		}
		else if (pStateEnvironment->WaitForSignal("signal_reloadconfiguration", 0, pSignalHandler)) {
			pSignalHandler->SignalHandled();

			applyMachineConfigurationToRuntime(pStateEnvironment);
			pStateEnvironment->LogMessage("Reloaded active machine configuration.");
			pStateEnvironment->SetNextState("idle");
		}
		else {
			// Drain any stray execution signals so they do not accumulate while idle.
			if (pStateEnvironment->WaitForSignal("signal_cancelbuild", 0, pSignalHandler))
				pSignalHandler->SignalHandled();
			if (pStateEnvironment->WaitForSignal("signal_finishexecution", 0, pSignalHandler))
				pSignalHandler->SignalHandled();
			if (pStateEnvironment->WaitForSignal("signal_keepalive", 0, pSignalHandler))
				pSignalHandler->SignalHandled();

			pStateEnvironment->SetNextState("idle");
		}
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_SingleExposure
**************************************************************************************************************************/
// Exposes one operator-selected layer outside of an execution: laser on, expose, laser off.
// Keeps the toolpath loaded afterwards, because the build library preview still shows it.
class CMainState_SingleExposure : public virtual CMainState {
public:
	CMainState_SingleExposure(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "singleexposure";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto sJobUUID = pStateEnvironment->GetStringParameter("singleexposure", "jobuuid");
		auto nLayerIndex = pStateEnvironment->GetIntegerParameter("singleexposure", "layerindex");
		auto nExposureTimeOut = pStateEnvironment->GetIntegerParameter("jobinfo", "exposuretimeout");

		pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", false);
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Exposing single layer " + std::to_string(nLayerIndex));
		pStateEnvironment->LogMessage("Single layer exposure of layer " + std::to_string(nLayerIndex) + " of job " + sJobUUID);

		auto pBuildJob = pStateEnvironment->GetBuildJob(sJobUUID);
		if (!pBuildJob->ToolpathIsLoaded())
			pBuildJob->LoadToolpath();

		if (!setLaserPin(pStateEnvironment, true, 30000)) {
			pStateEnvironment->LogWarning("Could not turn laser on - single layer exposure aborted.");
			setLaserPin(pStateEnvironment, false, 30000);
			pStateEnvironment->SetNextState("idle");
			return;
		}

		auto pExposureSignal = pStateEnvironment->PrepareSignal("laser", "signal_exposure");
		pExposureSignal->SetString("jobuuid", sJobUUID);
		pExposureSignal->SetInteger("layerindex", nLayerIndex);
		pExposureSignal->SetInteger("timeout", nExposureTimeOut);
		pExposureSignal->Trigger();

		bool bSuccess = pExposureSignal->WaitForHandling((uint32_t)nExposureTimeOut) && pExposureSignal->GetBoolResult("success");

		setLaserPin(pStateEnvironment, false, 30000);

		if (!bSuccess) {
			pStateEnvironment->LogMessage("Single layer exposure failed or timed out.");
			pStateEnvironment->SetNextState("fatalerror");
			return;
		}

		pStateEnvironment->LogMessage("Single layer exposure of layer " + std::to_string(nLayerIndex) + " finished.");
		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_InitExecution
**************************************************************************************************************************/
class CMainState_InitExecution : public virtual CMainState {
public:
	CMainState_InitExecution(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "initexecution";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Initializing execution...");
		auto sJobUUID = pStateEnvironment->GetUUIDParameter("jobinfo", "jobuuid");

		pStateEnvironment->LogMessage("Loading Toolpath...");
		auto pBuildJob = pStateEnvironment->GetBuildJob(sJobUUID);
		pBuildJob->LoadToolpath();

		// A fresh execution exposes all parts again, even if some were disabled in a previous run.
		pBuildJob->EnableAllParts();
		pStateEnvironment->SetSessionVariableForAllSessionsAsBool("processpart_isselected", false);
		pStateEnvironment->SetSessionVariableForAllSessionsAsBool("processpart_candisable", false);
		pStateEnvironment->SetSessionVariableForAllSessionsAsBool("processpart_isdisabled", false);
		pStateEnvironment->SetSessionVariableForAllSessionsAsUUID("processpart_uuid", "00000000-0000-0000-0000-000000000000");
		pStateEnvironment->SetSessionVariableForAllSessions("processpart_name", "");
		pStateEnvironment->SetSessionVariableForAllSessions("processpart_number", "");
		pStateEnvironment->SetSessionVariableForAllSessions("processpart_meshuuid", "");
		pStateEnvironment->SetSessionVariableForAllSessions("processpart_buildname", "");
		pStateEnvironment->SetSessionVariableForAllSessions("processpart_status", "");

		auto sJobName = pBuildJob->GetName();
		auto nLayerCount = pBuildJob->GetLayerCount();
		double dTotalHeight = pBuildJob->GetBuildHeightInMM();

		pStateEnvironment->LogMessage("Job Name: " + sJobName);
		pStateEnvironment->LogMessage("Layer Count: " + std::to_string(nLayerCount));

		pStateEnvironment->SetStringParameter("jobinfo", "jobname", sJobName);
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);
		pStateEnvironment->SetDoubleParameter("jobinfo", "currentheight", 0.0);
		pStateEnvironment->SetDoubleParameter("jobinfo", "totalheight", dTotalHeight);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", true);

		// Reset the execution progress.
		pStateEnvironment->SetBoolParameter("execution", "executionactive", true);
		pStateEnvironment->SetBoolParameter("execution", "exposing", false);
		pStateEnvironment->SetBoolParameter("execution", "cancelrequested", false);
		pStateEnvironment->SetIntegerParameter("execution", "currentlayer", -1);
		pStateEnvironment->SetIntegerParameter("execution", "lastexposedlayer", -1);
		pStateEnvironment->SetIntegerParameter("execution", "exposurecount", 0);

		pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", true);
		pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", false);

		pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "---");
		pStateEnvironment->SetStringParameter("ui", "currentlayerheightdisplay", "---");
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Waiting for remote");

		// Discard any stale exposure/cancel signals before starting the loop.
		pStateEnvironment->ClearAllUnhandledSignals();

		// The laser state machine switches the enable pin on only while a layer is drawn;
		// make sure it is not left on (e.g. from the manual UI toggle) while waiting.
		pStateEnvironment->LogMessage("Turning laser off until the first exposure...");
		if (!setLaserPin(pStateEnvironment, false, 30000)) {
			pStateEnvironment->LogWarning("Could not turn laser off - aborting execution.");
			pStateEnvironment->SetNextState("cancelexecution");
			return;
		}

		// Arm the watchdog.
		pStateEnvironment->SetIntegerParameter("execution", "waitingsince", (int64_t)pStateEnvironment->GetGlobalTimerInMilliseconds());

		pStateEnvironment->LogMessage("Execution armed, waiting for remote layer commands...");
		pStateEnvironment->SetNextState("execution_waiting");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_ExecutionWaiting
**************************************************************************************************************************/
class CMainState_ExecutionWaiting : public virtual CMainState {
public:
	CMainState_ExecutionWaiting(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "execution_waiting";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		updateSubsystemErrorFlags(pStateEnvironment);
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Waiting for remote");

		LibMCEnv::PSignalHandler pSignalHandler;

		// Cancel takes priority (from UI or REST).
		if (pStateEnvironment->WaitForSignal("signal_cancelbuild", 0, pSignalHandler)) {
			pSignalHandler->SignalHandled();
			pStateEnvironment->LogMessage("Cancel requested.");
			pStateEnvironment->SetNextState("cancelexecution");
			return;
		}

		// Explicit early finish from the external master.
		if (pStateEnvironment->WaitForSignal("signal_finishexecution", 0, pSignalHandler)) {
			pSignalHandler->SignalHandled();
			pStateEnvironment->LogMessage("Finish requested by remote master.");
			pStateEnvironment->SetNextState("finishexecution");
			return;
		}

		// Layer exposure request.
		if (pStateEnvironment->WaitForSignal("signal_exposelayer", 0, pSignalHandler)) {
			auto nRequestedLayer = pSignalHandler->GetInteger("layerindex");
			auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");
			bool bLaserOK = !pStateEnvironment->GetBoolParameter("execution", "lasterror");

			std::string sError;
			bool bAccepted = false;

			if (!pStateEnvironment->GetBoolParameter("execution", "executionactive")) {
				sError = "no execution is active";
			}
			else if (!bLaserOK) {
				sError = "laser subsystem is not OK";
			}
			else if ((nRequestedLayer < 0) || (nRequestedLayer >= nLayerCount)) {
				sError = "layer index " + std::to_string(nRequestedLayer) + " is out of range (0.." + std::to_string(nLayerCount - 1) + ")";
			}
			else {
				bAccepted = true;
			}

			// Mark the exposure as started before answering, so a caller that polls the status
			// right after the acceptance never sees the previous, idle state.
			if (bAccepted) {
				pStateEnvironment->SetIntegerParameter("execution", "currentlayer", nRequestedLayer);
				pStateEnvironment->SetBoolParameter("execution", "exposing", true);
			}

			pSignalHandler->SetBoolResult("accepted", bAccepted);
			pSignalHandler->SetStringResult("errormessage", sError);
			pSignalHandler->SignalHandled();

			if (bAccepted) {
				pStateEnvironment->LogMessage("Accepted exposure request for layer " + std::to_string(nRequestedLayer));
				pStateEnvironment->SetNextState("execution_exposing");
			}
			else {
				pStateEnvironment->LogWarning("Rejected exposure request: " + sError);
				pStateEnvironment->SetNextState("execution_waiting");
			}
			return;
		}

		// Keep-alive from the external master between layers re-arms the watchdog.
		if (pStateEnvironment->WaitForSignal("signal_keepalive", 0, pSignalHandler)) {
			pSignalHandler->SignalHandled();
			pStateEnvironment->SetIntegerParameter("execution", "waitingsince", (int64_t)pStateEnvironment->GetGlobalTimerInMilliseconds());
		}

		// Watchdog: cancel the execution if the external master goes silent for too long.
		auto nWatchdogTimeout = pStateEnvironment->GetIntegerParameter("execution", "watchdogtimeout");
		if (nWatchdogTimeout > 0) {
			auto nWaitingSince = pStateEnvironment->GetIntegerParameter("execution", "waitingsince");
			int64_t nNow = (int64_t)pStateEnvironment->GetGlobalTimerInMilliseconds();
			int64_t nElapsed = nNow - nWaitingSince;
			int64_t nRemaining = nWatchdogTimeout - nElapsed;
			if (nRemaining < 0)
				nRemaining = 0;
			pStateEnvironment->SetIntegerParameter("execution", "watchdogremainingms", nRemaining);

			if (nElapsed > nWatchdogTimeout) {
				pStateEnvironment->LogWarning("Watchdog expired after " + std::to_string(nElapsed) + " ms without a layer command - canceling execution.");
				pStateEnvironment->SetNextState("cancelexecution");
				return;
			}
		}
		else {
			pStateEnvironment->SetIntegerParameter("execution", "watchdogremainingms", 0);
		}

		pStateEnvironment->SetNextState("execution_waiting");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_ExecutionExposing
**************************************************************************************************************************/
class CMainState_ExecutionExposing : public virtual CMainState {
public:
	CMainState_ExecutionExposing(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "execution_exposing";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		auto nCurrentLayer = pStateEnvironment->GetIntegerParameter("execution", "currentlayer");
		auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");
		auto nExposureTimeOut = pStateEnvironment->GetIntegerParameter("jobinfo", "exposuretimeout");
		double dTotalHeight = pStateEnvironment->GetDoubleParameter("jobinfo", "totalheight");

		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Exposing layer " + std::to_string(nCurrentLayer));
		pStateEnvironment->LogMessage("Exposing layer " + std::to_string(nCurrentLayer) + " / " + std::to_string(nLayerCount));

		auto pExposureSignal = pStateEnvironment->PrepareSignal("laser", "signal_exposure");
		pExposureSignal->SetString("jobuuid", sJobUUID);
		pExposureSignal->SetInteger("layerindex", nCurrentLayer);
		pExposureSignal->SetInteger("timeout", nExposureTimeOut);
		pExposureSignal->Trigger();

		// Wait for the exposure in short slices so a cancel request can be latched while the
		// layer is being exposed (the layer itself is never interrupted mid-exposure).
		bool bHandled = false;
		int64_t nElapsed = 0;
		const uint32_t nSliceMS = 250;
		while (!bHandled && nElapsed <= nExposureTimeOut) {
			bHandled = pExposureSignal->WaitForHandling(nSliceMS);
			nElapsed += nSliceMS;

			LibMCEnv::PSignalHandler pCancelHandler;
			if (pStateEnvironment->WaitForSignal("signal_cancelbuild", 0, pCancelHandler)) {
				pCancelHandler->SignalHandled();
				pStateEnvironment->SetBoolParameter("execution", "cancelrequested", true);
				pStateEnvironment->LogMessage("Cancel latched during exposure - will take effect after this layer.");
			}
		}

		if (!bHandled) {
			pStateEnvironment->LogMessage("Layer exposure failed or timed out.");
			pStateEnvironment->SetNextState("fatalerror");
			return;
		}

		if (!pExposureSignal->GetBoolResult("success")) {
			pStateEnvironment->LogMessage("Laser reported exposure failure.");
			pStateEnvironment->SetNextState("fatalerror");
			return;
		}

		// Layer exposed: update the counters.
		pStateEnvironment->SetIntegerParameter("execution", "lastexposedlayer", nCurrentLayer);
		pStateEnvironment->SetIntegerParameter("execution", "exposurecount", pStateEnvironment->GetIntegerParameter("execution", "exposurecount") + 1);
		pStateEnvironment->SetBoolParameter("execution", "exposing", false);

		auto pBuildJob = pStateEnvironment->GetBuildJob(sJobUUID);
		auto pAccessor = pBuildJob->CreateToolpathAccessor();
		auto pLayerData = pAccessor->LoadLayer((uint32_t)nCurrentLayer);
		double dCurrentHeight = pLayerData->GetZValue() * pAccessor->GetUnits();
		updateLayerDisplays(pStateEnvironment, nCurrentLayer, nLayerCount, dCurrentHeight, dTotalHeight);

		pStateEnvironment->LogMessage("Layer " + std::to_string(nCurrentLayer) + " exposed successfully.");

		// A cancel latched during the exposure takes effect now.
		if (pStateEnvironment->GetBoolParameter("execution", "cancelrequested")) {
			pStateEnvironment->LogMessage("Applying latched cancel request.");
			pStateEnvironment->SetNextState("cancelexecution");
			return;
		}

		// Re-arm the watchdog and wait for the next command.
		pStateEnvironment->SetIntegerParameter("execution", "waitingsince", (int64_t)pStateEnvironment->GetGlobalTimerInMilliseconds());
		pStateEnvironment->SetNextState("execution_waiting");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_FinishExecution
**************************************************************************************************************************/
class CMainState_FinishExecution : public virtual CMainState {
public:
	CMainState_FinishExecution(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "finishexecution";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Finishing execution...");
		pStateEnvironment->LogMessage("Turning laser off...");
		setLaserPin(pStateEnvironment, false, 30000);

		pStateEnvironment->SetBoolParameter("execution", "executionactive", false);
		pStateEnvironment->SetBoolParameter("execution", "exposing", false);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "finished");
		pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "finished");
		pStateEnvironment->SetStringParameter("ui", "currentlayerheightdisplay", "finished");
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Finished");
		pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);
		pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", true);

		pStateEnvironment->UnloadAllToolpathes();

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_CancelExecution
**************************************************************************************************************************/
class CMainState_CancelExecution : public virtual CMainState {
public:
	CMainState_CancelExecution(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "cancelexecution";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Canceling execution...");
		pStateEnvironment->LogMessage("Turning laser off...");
		setLaserPin(pStateEnvironment, false, 30000);

		pStateEnvironment->SetBoolParameter("execution", "executionactive", false);
		pStateEnvironment->SetBoolParameter("execution", "exposing", false);
		pStateEnvironment->SetBoolParameter("execution", "cancelrequested", false);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		pStateEnvironment->UnloadAllToolpathes();

		pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "canceled");
		pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "canceled");
		pStateEnvironment->SetStringParameter("ui", "currentlayerheightdisplay", "canceled");
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Cancelled");
		pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);
		pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", true);

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CMainState_FatalError : public virtual CMainState {
public:
	CMainState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "fatalerror";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		// Best-effort: assert laser-off and unload any active toolpath.
		setLaserPin(pStateEnvironment, false, 5000);
		pStateEnvironment->UnloadAllToolpathes();

		pStateEnvironment->SetBoolParameter("execution", "executionactive", false);
		pStateEnvironment->SetBoolParameter("execution", "exposing", false);
		pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);
		pStateEnvironment->SetStringParameter("ui", "executionstatusdisplay", "Error");

		// Surface the main fault so the recovery button appears.
		pStateEnvironment->SetBoolParameter("ui", "main_haserror", true);
		pStateEnvironment->SetBoolParameter("ui", "system_haserror", true);

		// Stay faulted until a recovery request arrives, then re-run full initialisation.
		auto pSignal = pStateEnvironment->ClaimSignalFromQueue("signal_initialize_again");
		if (pSignal) {
			pSignal->SetBoolResult("success", true);
			pSignal->SignalHandled();

			pStateEnvironment->LogMessage("Main recovery requested, re-initializing...");
			pStateEnvironment->SetNextState("waitforinit");
		}
		else {
			pStateEnvironment->SetNextState("fatalerror");
		}
	}
};


/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName) {
	m_pPluginData = std::make_shared<CMainData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName) {
	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CMainState_WaitForInit>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_SingleExposure>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_InitExecution>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_ExecutionWaiting>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_ExecutionExposing>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_FinishExecution>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_CancelExecution>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CMainState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME, "invalid state name: " + sStateName);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
