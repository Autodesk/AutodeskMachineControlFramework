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
	pStateEnvironment->LogMessage("WidgetTest: Initializing...");

	pStateEnvironment->SetStringParameter("display", "machine_state", "Initializing");
	pStateEnvironment->SetDoubleParameter("sensors", "temperature", 22.5);
	pStateEnvironment->SetDoubleParameter("sensors", "pressure", 1013.25);
	pStateEnvironment->SetDoubleParameter("sensors", "humidity", 45.0);
	pStateEnvironment->SetDoubleParameter("sensors", "laser_power", 0.0);
	pStateEnvironment->SetDoubleParameter("sensors", "gas_flow", 0.0);
	pStateEnvironment->SetDoubleParameter("sensors", "build_progress", 0.0);

	pStateEnvironment->SetIntegerParameter("counters", "cycle_count", 0);
	pStateEnvironment->SetIntegerParameter("counters", "layer_current", 0);
	pStateEnvironment->SetIntegerParameter("counters", "layer_total", 250);
	pStateEnvironment->SetIntegerParameter("counters", "error_count", 0);
	pStateEnvironment->SetIntegerParameter("counters", "warning_count", 0);

	pStateEnvironment->SetStringParameter("display", "status_message", "System ready");
	pStateEnvironment->SetStringParameter("display", "last_event", "none");
	pStateEnvironment->SetStringParameter("display", "formatted_time", "00:00:00");

	pStateEnvironment->SetBoolParameter("flags", "is_running", false);
	pStateEnvironment->SetBoolParameter("flags", "is_paused", false);
	pStateEnvironment->SetBoolParameter("flags", "heater_on", false);
	pStateEnvironment->SetBoolParameter("flags", "gas_flowing", false);
	pStateEnvironment->SetBoolParameter("flags", "door_locked", true);

	pStateEnvironment->SetNextState("idle");
}


__DECLARESTATE(idle)
{
	auto nTimer = pStateEnvironment->GetGlobalTimerInMilliseconds();
	auto nCycle = pStateEnvironment->GetIntegerParameter("counters", "cycle_count");

	double dTemp = 22.5 + 2.0 * sin(nTimer * 0.0005);
	double dPressure = 1013.25 + 5.0 * cos(nTimer * 0.0003);
	double dHumidity = 45.0 + 10.0 * sin(nTimer * 0.0002);
	double dLaserPower = 50.0 + 50.0 * fabs(sin(nTimer * 0.001));
	double dGasFlow = 10.0 + 5.0 * sin(nTimer * 0.0004);

	pStateEnvironment->SetDoubleParameter("sensors", "temperature", dTemp);
	pStateEnvironment->SetDoubleParameter("sensors", "pressure", dPressure);
	pStateEnvironment->SetDoubleParameter("sensors", "humidity", dHumidity);
	pStateEnvironment->SetDoubleParameter("sensors", "laser_power", dLaserPower);
	pStateEnvironment->SetDoubleParameter("sensors", "gas_flow", dGasFlow);

	nCycle++;
	pStateEnvironment->SetIntegerParameter("counters", "cycle_count", nCycle);

	uint64_t nSeconds = nTimer / 1000;
	uint64_t nHours = nSeconds / 3600;
	uint64_t nMinutes = (nSeconds % 3600) / 60;
	uint64_t nSecs = nSeconds % 60;

	std::stringstream ssTime;
	ssTime << std::setfill('0') << std::setw(2) << nHours << ":"
	       << std::setfill('0') << std::setw(2) << nMinutes << ":"
	       << std::setfill('0') << std::setw(2) << nSecs;
	pStateEnvironment->SetStringParameter("display", "formatted_time", ssTime.str());
	pStateEnvironment->SetStringParameter("display", "machine_state", "Idle");

	PSignalHandler pSignalHandler;

	if (pStateEnvironment->WaitForSignal("signal_toggleheater", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		bool bHeater = pStateEnvironment->GetBoolParameter("flags", "heater_on");
		pStateEnvironment->SetBoolParameter("flags", "heater_on", !bHeater);
		pStateEnvironment->LogMessage(std::string("WidgetTest: Heater toggled ") + (!bHeater ? "ON" : "OFF"));
	}

	if (pStateEnvironment->WaitForSignal("signal_togglegasflow", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		bool bGas = pStateEnvironment->GetBoolParameter("flags", "gas_flowing");
		pStateEnvironment->SetBoolParameter("flags", "gas_flowing", !bGas);
		pStateEnvironment->LogMessage(std::string("WidgetTest: Gas flow toggled ") + (!bGas ? "ON" : "OFF"));
	}

	if (pStateEnvironment->WaitForSignal("signal_resetcounters", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetIntegerParameter("counters", "cycle_count", 0);
		pStateEnvironment->SetIntegerParameter("counters", "layer_current", 0);
		pStateEnvironment->SetIntegerParameter("counters", "error_count", 0);
		pStateEnvironment->SetIntegerParameter("counters", "warning_count", 0);
		pStateEnvironment->SetDoubleParameter("sensors", "build_progress", 0.0);
		pStateEnvironment->LogMessage("WidgetTest: Counters reset");
	}

	bool bIsRunning = pStateEnvironment->GetBoolParameter("flags", "is_running");

	if (bIsRunning) {
		pStateEnvironment->SetNextState("running");
	}
	else if (pStateEnvironment->WaitForSignal("signal_start", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetBoolParameter("flags", "is_running", true);
		pStateEnvironment->SetStringParameter("display", "status_message", "Process started");
		pStateEnvironment->LogMessage("WidgetTest: Process started by user");
		pStateEnvironment->SetNextState("running");
	}
	else {
		pStateEnvironment->SetNextState("idle");
	}
}


__DECLARESTATE(running)
{
	auto nTimer = pStateEnvironment->GetGlobalTimerInMilliseconds();
	auto nLayerCurrent = pStateEnvironment->GetIntegerParameter("counters", "layer_current");
	auto nLayerTotal = pStateEnvironment->GetIntegerParameter("counters", "layer_total");

	double dTemp = 35.0 + 5.0 * sin(nTimer * 0.0008);
	double dPressure = 1010.0 + 3.0 * cos(nTimer * 0.0004);
	double dLaserPower = 80.0 + 20.0 * sin(nTimer * 0.002);
	double dGasFlow = 15.0 + 3.0 * sin(nTimer * 0.0006);

	pStateEnvironment->SetDoubleParameter("sensors", "temperature", dTemp);
	pStateEnvironment->SetDoubleParameter("sensors", "pressure", dPressure);
	pStateEnvironment->SetDoubleParameter("sensors", "laser_power", dLaserPower);
	pStateEnvironment->SetDoubleParameter("sensors", "gas_flow", dGasFlow);

	nLayerCurrent++;
	if (nLayerCurrent > nLayerTotal)
		nLayerCurrent = nLayerTotal;

	pStateEnvironment->SetIntegerParameter("counters", "layer_current", nLayerCurrent);

	double dProgress = (nLayerTotal > 0) ? (100.0 * nLayerCurrent / nLayerTotal) : 0.0;
	pStateEnvironment->SetDoubleParameter("sensors", "build_progress", dProgress);

	std::stringstream ssStatus;
	ssStatus << "Processing layer " << nLayerCurrent << " / " << nLayerTotal;
	pStateEnvironment->SetStringParameter("display", "status_message", ssStatus.str());
	pStateEnvironment->SetStringParameter("display", "machine_state", "Running");

	auto nCycle = pStateEnvironment->GetIntegerParameter("counters", "cycle_count");
	pStateEnvironment->SetIntegerParameter("counters", "cycle_count", nCycle + 1);

	PSignalHandler pSignalHandler;
	if (pStateEnvironment->WaitForSignal("signal_stop", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetBoolParameter("flags", "is_running", false);
		pStateEnvironment->SetStringParameter("display", "status_message", "Process stopped by user");
		pStateEnvironment->LogMessage("WidgetTest: Process stopped by user");
		pStateEnvironment->SetNextState("idle");
	}
	else if (pStateEnvironment->WaitForSignal("signal_pause", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetBoolParameter("flags", "is_paused", true);
		pStateEnvironment->SetStringParameter("display", "status_message", "Process paused");
		pStateEnvironment->LogMessage("WidgetTest: Process paused by user");
		pStateEnvironment->SetNextState("paused");
	}
	else if (nLayerCurrent >= nLayerTotal) {
		pStateEnvironment->SetBoolParameter("flags", "is_running", false);
		pStateEnvironment->SetStringParameter("display", "status_message", "Process completed");
		pStateEnvironment->LogMessage("WidgetTest: Process completed");
		pStateEnvironment->SetNextState("idle");
	}
	else {
		pStateEnvironment->SetNextState("running");
	}
}


__DECLARESTATE(paused)
{
	pStateEnvironment->SetStringParameter("display", "machine_state", "Paused");

	PSignalHandler pSignalHandler;
	if (pStateEnvironment->WaitForSignal("signal_resume", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetBoolParameter("flags", "is_paused", false);
		pStateEnvironment->SetStringParameter("display", "status_message", "Process resumed");
		pStateEnvironment->LogMessage("WidgetTest: Process resumed");
		pStateEnvironment->SetNextState("running");
	}
	else if (pStateEnvironment->WaitForSignal("signal_stop", 0, pSignalHandler)) {
		pSignalHandler->SignalHandled();
		pStateEnvironment->SetBoolParameter("flags", "is_running", false);
		pStateEnvironment->SetBoolParameter("flags", "is_paused", false);
		pStateEnvironment->SetStringParameter("display", "status_message", "Process stopped");
		pStateEnvironment->LogMessage("WidgetTest: Process stopped from pause");
		pStateEnvironment->SetNextState("idle");
	}
	else {
		pStateEnvironment->SetNextState("paused");
	}
}


__DECLARESTATE(fatalerror)
{
	pStateEnvironment->SetStringParameter("display", "machine_state", "Fatal Error");
	pStateEnvironment->SetStringParameter("display", "status_message", "Fatal error occurred - restarting");
	pStateEnvironment->SetBoolParameter("flags", "is_running", false);
	pStateEnvironment->SetBoolParameter("flags", "is_paused", false);

	pStateEnvironment->Sleep(5000);
	pStateEnvironment->SetNextState("init");
}


__ENDSTATEDEFINITIONS
