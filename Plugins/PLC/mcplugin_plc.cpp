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
#include "libmcdriver_bur_dynamic.hpp"

#include <cmath>

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(BuR, BuR)

__BEGINDRIVERIMPORT
__IMPORTDRIVER(BuR, "bur");
__ENDDRIVERIMPORT

/*************************************************************************************************************************
  State definitions
**************************************************************************************************************************/

__BEGINSTATEDEFINITIONS

__DECLARESTATE(init) 
{

	std::string sIPAddress = pStateEnvironment->GetStringParameter("plcconfig", "ipaddress");
	auto nPort = pStateEnvironment->GetIntegerParameter("plcconfig", "port");
	auto nTimeout = pStateEnvironment->GetIntegerParameter("plcconfig", "timeout");

	auto pDriver = __acquireDriver(BuR);

	if (pStateEnvironment->GetBoolParameter("plcconfig", "simulateplc")) {
		pStateEnvironment->LogMessage("PLC Simulation enabled!...");
		pDriver->SetToSimulationMode();
	}

	pStateEnvironment->LogMessage("Connecting to PLC...");
	pDriver->Connect(sIPAddress, (uint32_t) nPort, (uint32_t) nTimeout);
	pStateEnvironment->LogMessage("successful...");

	auto pPLCCommandList = pDriver->CreateCommandList();

	pStateEnvironment->LogMessage("Turning on printer..");

	auto pPLCCommand_TurnOn = pDriver->CreateCommand("turnon");
	pPLCCommandList->AddCommand(pPLCCommand_TurnOn);

	auto pPLCCommand_Home = pDriver->CreateCommand("home");
	pPLCCommandList->AddCommand(pPLCCommand_Home);


	pPLCCommandList->FinishList();
	pPLCCommandList->ExecuteList();

	bool bReady = false;
	for (uint32_t nIndex = 0; nIndex < 1000; nIndex++) {
		pDriver->QueryParameters();

		if (pPLCCommandList->WaitForList(300, 100)) {
			bReady = true;
			break;
		}
	}

	if (bReady) {
		pStateEnvironment->LogMessage("Printer turned on");
	}
	else {
		pStateEnvironment->LogMessage("Timeout while turning on printer!");
	} 

	pStateEnvironment->SetNextState("idle");
}



__DECLARESTATE(idle)
{

	auto pDriver = __acquireDriver(BuR);
	pDriver->QueryParameters();

	double dAxisValue;
	uint64_t nTimer = pStateEnvironment->GetGlobalTimerInMilliseconds();
	int nTimeInterval = (nTimer % 20000);
	dAxisValue = sin((nTimeInterval / 10000.0) * 3.1415) * 50.0 + 50.0;
	/*if (nTimeInterval < 10000)
		dAxisValue = nTimeInterval * 0.01;
	else
		dAxisValue = 100.0 - (nTimeInterval - 10000) * 0.01; */


	pStateEnvironment->SetDoubleParameter ("axisvalues", "recoateraxis", dAxisValue);



	LibMCEnv::PSignalHandler pHandlerInstance;
	if (pStateEnvironment->WaitForSignal("signal_recoatlayer", 0, pHandlerInstance)) {			

		pStateEnvironment->StoreSignal ("recoatsignal", pHandlerInstance);

		pStateEnvironment->SetNextState("recoating");

	}
	else {
		pStateEnvironment->SetNextState("idle");
	}

}

__DECLARESTATE(recoating)
{

	pStateEnvironment->LogMessage("Recoating...");
		
	auto pDriver = __acquireDriver(BuR);
		
	auto pPLCCommandList = pDriver->CreateCommandList();


	for (uint32_t nIndex = 0; nIndex < 1; nIndex++) {
		auto pPLCCommand1 = pDriver->CreateCommand("move");
		pPLCCommand1->SetIntegerParameter("targetx", 100000);
		pPLCCommand1->SetIntegerParameter("targety", 100000);
		pPLCCommand1->SetIntegerParameter("targetz", 0);
		pPLCCommand1->SetIntegerParameter("velocity", 10000);
		pPLCCommandList->AddCommand(pPLCCommand1);

		auto pPLCCommand2 = pDriver->CreateCommand("move");
		pPLCCommand2->SetIntegerParameter("targetx", 200000);
		pPLCCommand2->SetIntegerParameter("targety", 100000);
		pPLCCommand2->SetIntegerParameter("targetz", 0);
		pPLCCommand2->SetIntegerParameter("velocity", 10000);
		pPLCCommandList->AddCommand(pPLCCommand2);

		auto pPLCCommand3 = pDriver->CreateCommand("move");
		pPLCCommand3->SetIntegerParameter("targetx", 200000);
		pPLCCommand3->SetIntegerParameter("targety", 200000);
		pPLCCommand3->SetIntegerParameter("targetz", 0);
		pPLCCommand3->SetIntegerParameter("velocity", 10000);
		pPLCCommandList->AddCommand(pPLCCommand3);

		auto pPLCCommand4 = pDriver->CreateCommand("move");
		pPLCCommand4->SetIntegerParameter("targetx", 100000);
		pPLCCommand4->SetIntegerParameter("targety", 200000);
		pPLCCommand4->SetIntegerParameter("targetz", 0);
		pPLCCommand4->SetIntegerParameter("velocity", 10000);
		pPLCCommandList->AddCommand(pPLCCommand4);

	}


	pPLCCommandList->FinishList();
	pPLCCommandList->ExecuteList();


	if (pPLCCommandList->WaitForList(300, 100000)) {
		pStateEnvironment->LogMessage("Printer turned on");
	}
	else {
		pStateEnvironment->LogMessage("Timeout while turning on printer!");
	}
	auto pSignalHandler = pStateEnvironment->RetrieveSignal("recoatsignal");
	pSignalHandler->SetBoolResult("success", true);
	pSignalHandler->SignalHandled(); 

	pStateEnvironment->SetNextState("idle");
}



__DECLARESTATE(fatalerror)
{
	
	pStateEnvironment->SetNextState("fatalerror");
	

};

__ENDSTATEDEFINITIONS