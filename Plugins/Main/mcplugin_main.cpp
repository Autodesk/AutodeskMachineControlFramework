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

#include "libmcplugin_impl.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

/*************************************************************************************************************************
  Driver import definition
**************************************************************************************************************************/
__NODRIVERIMPORT


/*************************************************************************************************************************
  State definitions
**************************************************************************************************************************/
__BEGINSTATEDEFINITIONS

__DECLARESTATE(init)
{
	pStateEnvironment->LogMessage("Initializing...");

	pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", 0);
	pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
	pStateEnvironment->SetDoubleParameter("jobinfo", "totalheight", 0.0);
	pStateEnvironment->SetDoubleParameter("jobinfo", "currentheight", 0.0);
	pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

	pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "---");
	pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "---");

	pStateEnvironment->SetNextState("idle");
}


__DECLARESTATE(idle) 
{
	PSignalHandler pSignalHandler;
	pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", true);

	auto dCounterTest = pStateEnvironment->GetDoubleParameter("jobinfo", "countertest");
	auto nTimer = pStateEnvironment->GetGlobalTimerInMilliseconds();

	pStateEnvironment->SetDoubleParameter ("jobinfo", "countertest", dCounterTest + abs (sin (nTimer * 0.001)));

	//pStateEnvironment->LogMessage ("Waiting for user input...");
	if (pStateEnvironment->WaitForSignal("signal_initjob", 0, pSignalHandler)) {
		auto sJobUUID = pSignalHandler->GetUUID("jobuuid");
		pStateEnvironment->SetStringParameter("jobinfo", "jobuuid", sJobUUID);
		pSignalHandler->SignalHandled();

		pStateEnvironment->LogMessage("Preparing job " + sJobUUID);

		pStateEnvironment->SetNextState("initbuild");

	}
	else if (pStateEnvironment->WaitForSignal("signal_changesimulationparameters", 0, pSignalHandler)) {
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






__DECLARESTATE(initbuild) 
{
	pStateEnvironment->LogMessage("Initializing build...");
	auto sJobUUID = pStateEnvironment->GetUUIDParameter("jobinfo", "jobuuid");

	pStateEnvironment->LogMessage("Loading Toolpath...");
	auto pBuildJob = pStateEnvironment->GetBuildJob (sJobUUID);
	pBuildJob->LoadToolpath();

	auto sJobName = pBuildJob->GetName();
	double dTotalHeight = 0.0;
	auto nLayerCount = pBuildJob->GetLayerCount();
	{
		dTotalHeight = pBuildJob->GetBuildHeightInMM();
	}

	pStateEnvironment->LogMessage("Job Name: " + sJobName);
	pStateEnvironment->LogMessage("Layer Count: " + std::to_string (nLayerCount));

	pStateEnvironment->SetStringParameter("jobinfo", "jobname", sJobName);
	pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 1);
	pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);
	pStateEnvironment->SetDoubleParameter("jobinfo", "currentheight", 0.0);
	pStateEnvironment->SetDoubleParameter("jobinfo", "totalheight", dTotalHeight);

	//pStateEnvironment->SetBoolParameter("ui", "build_canbepaused", true);
	pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", true);
	pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", false);

	pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "initializing");
	pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "initializing");

	pStateEnvironment->Sleep(500);
	pStateEnvironment->LogMessage("Waiting for atmosphere...");
	pStateEnvironment->Sleep(1000);

	pStateEnvironment->SetNextState("beginlayer");

};


__DECLARESTATE(beginlayer) 
{
	auto nLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
	pStateEnvironment->LogMessage("Starting layer " + std::to_string (nLayer));

	auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
	auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");
	auto dTotalHeight = pStateEnvironment->GetDoubleParameter("jobinfo", "totalheight");

	double dCurrentHeight = 0.0;

	{
		auto pBuildJob = pStateEnvironment->GetBuildJob(sJobUUID);
		auto pAccessor = pBuildJob->CreateToolpathAccessor();
		auto pLayer = pAccessor->LoadLayer((uint32_t) nLayer);
		dCurrentHeight = pLayer->GetZValue() * pAccessor->GetUnits();
	}

	std::stringstream sLayerDisplay, sHeightDisplay;
	sLayerDisplay << nLayer << " / " << nLayerCount;
	sHeightDisplay << std::setprecision(4) << dCurrentHeight << " mm" << " / " << std::setprecision(4) << dTotalHeight << " mm";

	pStateEnvironment->SetDoubleParameter("jobinfo", "currentheight", dCurrentHeight);
	pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", sLayerDisplay.str ());
	pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", sHeightDisplay.str ());


	pStateEnvironment->SetNextState("recoatlayer");
}



__DECLARESTATE(recoatlayer) 
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




__DECLARESTATE(exposelayer) 
{
	pStateEnvironment->LogMessage("Exposing layer...");

	auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
	auto nCurrentLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
	auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");
	auto nExposureTimeOut = pStateEnvironment->GetIntegerParameter("jobinfo", "exposuretimeout");

	if (nCurrentLayer < nLayerCount) {

		pStateEnvironment->LogMessage("Setting laser pointer on...");
		auto pLaserOnSignal = pStateEnvironment->PrepareSignal("plc", "signal_laserpointer");
		pLaserOnSignal->SetBool("laseron", true);
		pLaserOnSignal->Trigger();

		if (!pLaserOnSignal->WaitForHandling((uint32_t)nExposureTimeOut)) {
			if (pLaserOnSignal->GetBoolResult("success") == false) {
				pStateEnvironment->LogMessage("Could not set laser pointer on...");
				pStateEnvironment->SetNextState("fatalerror");
				return;
			}
		}

		auto pExposureSignal = pStateEnvironment->PrepareSignal("laser", "signal_exposure");
		pExposureSignal->SetString("jobuuid", sJobUUID);
		pExposureSignal->SetInteger("layerindex", nCurrentLayer);
		pExposureSignal->SetInteger("timeout", nExposureTimeOut);
		pExposureSignal->Trigger();

		if (pExposureSignal->WaitForHandling((uint32_t)nExposureTimeOut)) {
			pStateEnvironment->LogMessage("Layer successfully exposed...");

		}
		else {
			pStateEnvironment->LogMessage("Layer exposure failed...");
			pStateEnvironment->SetNextState("fatalerror");
			return;
		}

		pStateEnvironment->LogMessage("Setting laser pointer off...");
		auto pLaserOffSignal = pStateEnvironment->PrepareSignal("plc", "signal_laserpointer");
		pLaserOffSignal->SetBool("laseron", false);
		pLaserOffSignal->Trigger();

		if (!pLaserOffSignal->WaitForHandling((uint32_t)nExposureTimeOut)) {
			if (pLaserOffSignal->GetBoolResult("success") == false) {
				pStateEnvironment->LogMessage("Could not set laser pointer off...");
				pStateEnvironment->SetNextState("fatalerror");
				return;
			}
		}

	}
	else {
		pStateEnvironment->LogMessage("Last layer is recoated only.");
	}

	pStateEnvironment->SetNextState("finishlayer");

}




__DECLARESTATE(finishlayer) 
{
	auto nLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
	auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");

	pStateEnvironment->LogMessage("Finished layer " + std::to_string (nLayer));

	nLayer++;
	pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", nLayer);

	if (nLayer < nLayerCount) {
		PSignalHandler pSignalHandler;
			pStateEnvironment->SetNextState("cancelbuild");
		if (pStateEnvironment->WaitForSignal("signal_cancelbuild", 100, pSignalHandler)) {

			pStateEnvironment->SetNextState("cancelbuild");

		} else if (pStateEnvironment->WaitForSignal("signal_pausebuild", 100, pSignalHandler)) {

			pStateEnvironment->SetNextState("buildpaused");

		}
		else {

			pStateEnvironment->SetNextState("beginlayer");

		}

	}
	else {
		pStateEnvironment->SetNextState("finishbuild");
	}

}



__DECLARESTATE(finishbuild) 
{
	pStateEnvironment->LogMessage("Finishing Build...");
	pStateEnvironment->LogMessage("Turning laser off...");
	pStateEnvironment->Sleep(1000);

	pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "finished");
	pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "finished");
	pStateEnvironment->SetBoolParameter("ui", "build_canbepaused", false);
	pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);
	pStateEnvironment->SetBoolParameter("ui", "build_canbestarted", true);

	pStateEnvironment->SetNextState("idle");
}


__DECLARESTATE(buildpaused) 
{
	pStateEnvironment->LogMessage("Build paused");
	pStateEnvironment->Sleep(3000);

	pStateEnvironment->SetNextState("pausebuild");
}


__DECLARESTATE(fatalerror) 
{
	// Unload all toolpathes that might be in memory
	pStateEnvironment->UnloadAllToolpathes();

	pStateEnvironment->SetBoolParameter("ui", "build_canbepaused", false);
	pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);

	pStateEnvironment->SetNextState("fatalerror");

}


__DECLARESTATE(cancelbuild) 
{
	pStateEnvironment->LogMessage("Canceling Build...");
	pStateEnvironment->LogMessage("Turning laser off...");
	pStateEnvironment->Sleep(1000);

	pStateEnvironment->SetStringParameter("ui", "currentlayerdisplay", "canceled");
	pStateEnvironment->SetStringParameter("ui", "currentheightdisplay", "canceled");
	pStateEnvironment->SetBoolParameter("ui", "build_canbepaused", false);
	pStateEnvironment->SetBoolParameter("ui", "build_canbecanceled", false);

	pStateEnvironment->SetNextState("idle");
}


__ENDSTATEDEFINITIONS