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

#include "libmcdriver_bur_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

using namespace LibMCPlugin::Impl;

#include <iostream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(BuR, BuR)


/*************************************************************************************************************************
 Class definition of CPLCData
**************************************************************************************************************************/
class CPLCData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_BuR m_DriverCast_BuR;

public:

	PDriver_BuR acquireBuRDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_BuR.acquireDriver(pStateEnvironment, "bur");
	}

};

/*************************************************************************************************************************
 Class definition of CPLCData
**************************************************************************************************************************/
typedef CState<CPLCData> CPLCState;


/*************************************************************************************************************************
 Class definition of CPLCState_Init
**************************************************************************************************************************/
class CPLCState_Init : public virtual CPLCState {
public:

	CPLCState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CPLCState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		std::string sIPAddress = pStateEnvironment->GetStringParameter("plcconfig", "ipaddress");
		auto nPort = pStateEnvironment->GetIntegerParameter("plcconfig", "port");
		auto nTimeout = pStateEnvironment->GetIntegerParameter("plcconfig", "timeout");

		auto pDriver = m_pPluginData->acquireBuRDriver(pStateEnvironment); 
		if (pStateEnvironment->GetBoolParameter("plcconfig", "simulateplc")) {
			pStateEnvironment->LogMessage("PLC Simulation enabled!...");
			pDriver->SetToSimulationMode();
		}

		pStateEnvironment->LogMessage("Connecting to PLC...");
		pDriver->Connect(sIPAddress, (uint32_t) nPort, (uint32_t) nTimeout);
		pStateEnvironment->LogMessage("successful...");

		pStateEnvironment->LogMessage("Create Command List");
		auto pPLCCommandList = pDriver->CreateCommandList();

		pStateEnvironment->LogMessage("Turning on printer..");

		pStateEnvironment->LogMessage("Adding TurnOn");
		auto pPLCCommand_TurnOn = pDriver->CreateCommand("turnon");
		pPLCCommandList->AddCommand(pPLCCommand_TurnOn);

		pStateEnvironment->LogMessage("Adding Home");
		auto pPLCCommand_Home = pDriver->CreateCommand("home");
		pPLCCommandList->AddCommand(pPLCCommand_Home);


		pStateEnvironment->LogMessage("Finishing List");
		pPLCCommandList->FinishList();
		pStateEnvironment->LogMessage("Executing List");
		pPLCCommandList->ExecuteList();

		bool bReady = false;
		for (uint32_t nIndex = 0; nIndex < 1000; nIndex++) {
			pDriver->QueryParameters();

			if (pPLCCommandList->WaitForList(300, 100)) {
				std::cout << "waitforlist is true!" << std::endl;
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


		pStateEnvironment->LogMessage("Create Move Command List");
		auto pPLCMoveCommandList = pDriver->CreateCommandList();

		for (int moveIndex = 0; moveIndex < 20; moveIndex++) {
			pStateEnvironment->LogMessage("Adding Move #" + std::to_string (moveIndex));
			auto pPLCCommand_Move1 = pDriver->CreateCommand("move");
			pPLCCommand_Move1->SetIntegerParameter("targetx", 10000);
			pPLCCommand_Move1->SetIntegerParameter("targety", 0);
			pPLCCommand_Move1->SetIntegerParameter("targetz", 0);
			pPLCCommand_Move1->SetIntegerParameter("targete", 0);
			pPLCCommand_Move1->SetIntegerParameter("velocity", 5000);
			pPLCMoveCommandList->AddCommand(pPLCCommand_Move1);

			auto pPLCCommand_Move2 = pDriver->CreateCommand("move");
			pPLCCommand_Move2->SetIntegerParameter("targetx", 0);
			pPLCCommand_Move2->SetIntegerParameter("targety", 0);
			pPLCCommand_Move2->SetIntegerParameter("targetz", 0);
			pPLCCommand_Move2->SetIntegerParameter("targete", 0);
			pPLCCommand_Move2->SetIntegerParameter("velocity", 5000);
			pPLCMoveCommandList->AddCommand(pPLCCommand_Move2);

		}

		pStateEnvironment->LogMessage("Finishing List");
		pPLCMoveCommandList->FinishList();
		pStateEnvironment->LogMessage("Executing List");
		pPLCMoveCommandList->ExecuteList();

		bReady = false;
		for (uint32_t nIndex = 0; nIndex < 1000; nIndex++) {
			pDriver->QueryParameters();

			if (pPLCMoveCommandList->WaitForList(300, 1000)) {
				std::cout << "waitforlist is true!" << std::endl;
				bReady = true;
				break;
			}
		}

		if (bReady) {
			pStateEnvironment->LogMessage("Movements successful");
		}
		else {
			pStateEnvironment->LogMessage("Movement timeout!");
		}

		pStateEnvironment->SetNextState("success");
	}

};



/*************************************************************************************************************************
 Class definition of CPLCState_Success
**************************************************************************************************************************/
class CPLCState_Success : public virtual CPLCState {
public:

	CPLCState_Success(const std::string& sStateName, PPluginData pPluginData)
		: CPLCState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "success";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		pStateEnvironment->SetNextState("success");

	}

};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CPLCState_FatalError : public virtual CPLCState {
public:

	CPLCState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CPLCState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "fatalerror";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->SetNextState("fatalerror");
	}

};



/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName)
{
	m_pPluginData = std::make_shared<CPLCData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CPLCState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;


	if (createStateInstanceByName<CPLCState_Success>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPLCState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
