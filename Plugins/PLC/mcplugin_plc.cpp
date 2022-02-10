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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
typedef LibMCDriver_BuR::PDriver_BuR PDriver_BuR;
typedef LibMCEnv::CDriverCast <LibMCDriver_BuR::CDriver_BuR, LibMCDriver_BuR::CWrapper> PDriverCast_BuR;


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

};


/*************************************************************************************************************************
 Class definition of CPLCState_Idle
**************************************************************************************************************************/
class CPLCState_Idle : public virtual CPLCState {
public:

	CPLCState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CPLCState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "idle";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto pDriver = m_pPluginData->acquireBuRDriver(pStateEnvironment);
		pDriver->QueryParameters();


		LibMCEnv::PSignalHandler pHandlerInstance;
		if (pStateEnvironment->WaitForSignal("signal_recoatlayer", 0, pHandlerInstance)) {			

			pStateEnvironment->StoreSignal ("recoatsignal", pHandlerInstance);

			pStateEnvironment->SetNextState("recoating");

		}
		else {
			pStateEnvironment->SetNextState("idle");
		}

	}

};

/*************************************************************************************************************************
 Class definition of CPLCState_Recoating
**************************************************************************************************************************/
class CPLCState_Recoating : public virtual CPLCState {
public:

	CPLCState_Recoating(const std::string& sStateName, PPluginData pPluginData)
		: CPLCState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "recoating";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		pStateEnvironment->LogMessage("Recoating...");
		
		auto pDriver = m_pPluginData->acquireBuRDriver(pStateEnvironment);
		
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

	if (createStateInstanceByName<CPLCState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPLCState_Recoating>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPLCState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
