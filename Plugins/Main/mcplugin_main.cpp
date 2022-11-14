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
	pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

	pStateEnvironment->SetNextState("idle");
}


__DECLARESTATE(idle) 
{
	PSignalHandler pSignalHandler;

	double targetO2 = pStateEnvironment->GetDoubleParameter("processsettings", "targeto2");
	pStateEnvironment->SetDoubleParameter("processsettings", "targeto2", targetO2 + 1.0);

	pStateEnvironment->SetBoolParameter("ui", "preparebuilddisabled", true);

	pStateEnvironment->LogMessage ("Waiting for user input...");
	if (pStateEnvironment->WaitForSignal("signal_preparebuildjob", 100, pSignalHandler)) {
		auto sJobUUID = pSignalHandler->GetUUID("jobuuid");
		pStateEnvironment->SetStringParameter("jobinfo", "jobuuid", sJobUUID);
		pSignalHandler->SignalHandled();

		pStateEnvironment->LogMessage("Preparing job " + sJobUUID);

		pStateEnvironment->SetNextState("preparebuild");

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




__DECLARESTATE(preparebuild) 
{
	pStateEnvironment->LogMessage("Waiting for build preparation...");

	pStateEnvironment->SetBoolParameter("ui", "preparebuilddisabled", false);

	PSignalHandler pSignalHandler;
	if (pStateEnvironment->WaitForSignal("signal_cancelbuildpreparation", 100, pSignalHandler)) {
		pStateEnvironment->SetStringParameter("jobinfo", "jobuuid", "00000000-0000-0000-0000-000000000000");
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetNextState("idle");

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

		pStateEnvironment->SetNextState("preparebuild");

	}
	else if (pStateEnvironment->WaitForSignal("signal_startbuild", 100, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetNextState("initbuild");

	}
	else {
		pStateEnvironment->SetNextState("preparebuild");
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
	auto nLayerCount = pBuildJob->GetLayerCount();

	pStateEnvironment->LogMessage("Job Name: " + sJobName);
	pStateEnvironment->LogMessage("Layer Count: " + std::to_string (nLayerCount));

	pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
	pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);

	pStateEnvironment->Sleep(1000);
	pStateEnvironment->LogMessage("Waiting for atmosphere...");
	pStateEnvironment->Sleep(3000);

	pStateEnvironment->SetNextState("beginlayer");

};


__DECLARESTATE(beginlayer) 
{
	auto nLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
	pStateEnvironment->LogMessage("Starting layer " + std::to_string (nLayer));

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




__DECLARESTATE(finishlayer) 
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



__DECLARESTATE(finishbuild) 
{
	pStateEnvironment->LogMessage("Finishing Build...");
	pStateEnvironment->LogMessage("Turning laser off...");
	pStateEnvironment->Sleep(1000);

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

	pStateEnvironment->SetNextState("fatalerror");

}


__DECLARESTATE(cancelbuild) 
{
	pStateEnvironment->LogMessage("Canceling Build...");
	pStateEnvironment->LogMessage("Turning laser off...");
	pStateEnvironment->Sleep(1000);

	pStateEnvironment->SetNextState("idle");
}


__ENDSTATEDEFINITIONS