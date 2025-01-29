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
	pStateEnvironment->LogMessage("Initializing machine..");
	pDriver->ReinitializeMachine();
	pStateEnvironment->LogMessage("Successfully Initialized machine..");


	/*auto pPLCCommandList = pDriver->CreateCommandList();

	//auto pPLCCommand_TurnOn = pDriver->CreateCommand("turnon");
	//pPLCCommandList->AddCommand(pPLCCommand_TurnOn);

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
	} */

	pStateEnvironment->SetNextState("idle");
}



__DECLARESTATE(idle)
{

	auto pDriver = __acquireDriver(BuR);
	pDriver->QueryParameters();



	LibMCEnv::PSignalHandler pHandlerInstance;
	if (pStateEnvironment->WaitForSignal("signal_recoatlayer", 0, pHandlerInstance)) {			

		pStateEnvironment->StoreSignal ("recoatsignal", pHandlerInstance);

		pStateEnvironment->SetNextState("recoating");

	} else if (pStateEnvironment->WaitForSignal("signal_laserpointer", 0, pHandlerInstance)) {

		bool bLaserOn = pHandlerInstance->GetBool("laseron");

		if (bLaserOn) {
			pStateEnvironment->LogMessage("Turning laser pointer on");
		}
		else {
			pStateEnvironment->LogMessage("Turning laser pointer off");
		}

		auto pPLCCommandList = pDriver->CreateCommandList();

		/*auto pPLCCommand_LaserPointerOn = pDriver->CreateCommand("setlaserpointer");
		pPLCCommand_LaserPointerOn->SetIntegerParameter("laseron", (int)bLaserOn);
		pPLCCommandList->AddCommand(pPLCCommand_LaserPointerOn);
		pPLCCommandList->FinishList();
		pPLCCommandList->ExecuteList();

		if (pPLCCommandList->WaitForList(300, 10000)) {
			pStateEnvironment->LogMessage("Laser pointer state changed"); */
			pHandlerInstance->SetBoolResult("success", true);
			pStateEnvironment->SetNextState("idle");
/*
		}
		else {
			pStateEnvironment->LogMessage("Timeout while laser pointer state change!");
			pHandlerInstance->SetBoolResult("success", false);
			pStateEnvironment->SetNextState("fatalerror");

		}*/

		pHandlerInstance->SignalHandled();

	}
	else {
		pStateEnvironment->SetNextState("idle");
	}

}

__DECLARESTATE(recoating)
{

	pStateEnvironment->LogMessage("Recoating...");
		
	auto pDriver = __acquireDriver(BuR);

	if (pDriver->IsSimulationMode()) {
		pStateEnvironment->LogMessage ("Simulation mode delay of 0.4 seconds..");
		pStateEnvironment->Sleep (400);
	}


	/*auto pPLCCommandList = pDriver->CreateCommandList();

	auto pPLCCommand_MoveRecoater = pDriver->CreateCommand("moverecoater");
	pPLCCommand_MoveRecoater->SetIntegerParameter("targetposition", 50000);
	pPLCCommand_MoveRecoater->SetIntegerParameter("targetspeed", 10000);
	pPLCCommand_MoveRecoater->SetIntegerParameter("acceleration", 60000);
	pPLCCommandList->AddCommand(pPLCCommand_MoveRecoater);

	auto pPLCCommand_MoveRecoater2 = pDriver->CreateCommand("moverecoater");
	pPLCCommand_MoveRecoater2->SetIntegerParameter("targetposition", 00000);
	pPLCCommand_MoveRecoater2->SetIntegerParameter("targetspeed", 10000);
	pPLCCommand_MoveRecoater2->SetIntegerParameter("acceleration", 60000);
	pPLCCommandList->AddCommand(pPLCCommand_MoveRecoater2);

	pPLCCommandList->FinishList();
	pPLCCommandList->ExecuteList(); 

	bool bReady = false;
	for (uint32_t nIndex = 0; nIndex < 1000; nIndex++) {
		pDriver->QueryParameters();

		if (pPLCCommandList->WaitForList(300, 100)) {
			bReady = true;
			break;
		}
	} */

	bool bReady = true;

	if (bReady) {
		pStateEnvironment->LogMessage("Recoating finished");

		auto pSignalHandler = pStateEnvironment->RetrieveSignal("recoatsignal");
		pSignalHandler->SetBoolResult("success", true);
		pSignalHandler->SignalHandled();
	}
	else {
		pStateEnvironment->LogMessage("Timeout while recoating!");

		auto pSignalHandler = pStateEnvironment->RetrieveSignal("recoatsignal");
		pSignalHandler->SetBoolResult("success", false);
		pSignalHandler->SignalHandled();

	} 


	pStateEnvironment->SetNextState("idle");
}



__DECLARESTATE(fatalerror)
{
	
	pStateEnvironment->SetNextState("fatalerror");
	

};

__ENDSTATEDEFINITIONS