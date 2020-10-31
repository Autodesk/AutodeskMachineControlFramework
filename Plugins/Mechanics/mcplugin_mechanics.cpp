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

#include "libmcdriver_marlin_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

using namespace LibMCPlugin::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
typedef LibMCDriver_Marlin::PDriver_Marlin PDriver_Marlin;
typedef LibMCEnv::CDriverCast <LibMCDriver_Marlin::CDriver_Marlin, LibMCDriver_Marlin::CWrapper> PDriverCast_Marlin;


/*************************************************************************************************************************
 Class definition of CMechanicsData
**************************************************************************************************************************/
class CMechanicsData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_Marlin m_DriverCast_Marlin;

public:

	PDriver_Marlin acquireMarlinDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_Marlin.acquireDriver(pStateEnvironment, "marlin");
	}

};

/*************************************************************************************************************************
 Class definition of CMechanicsData
**************************************************************************************************************************/
typedef CState<CMechanicsData> CMechanicsState;


/*************************************************************************************************************************
 Class definition of CMechanicsState_Init
**************************************************************************************************************************/
class CMechanicsState_Init : public virtual CMechanicsState {
public:

	CMechanicsState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CMechanicsState(getStateName(), sStateName, pPluginData)
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

		std::string sCOMPort = pStateEnvironment->GetStringParameter("connectionsettings", "comport");
		auto nBaudRate = pStateEnvironment->GetIntegerParameter("connectionsettings", "baudrate");
		auto nStatusUpdateInterval = pStateEnvironment->GetIntegerParameter("connectionsettings", "statusupdateinterval");
		auto nConnectTimeout = pStateEnvironment->GetIntegerParameter("connectionsettings", "connecttimeout");

		auto pDriver = m_pPluginData->acquireMarlinDriver(pStateEnvironment);
		pDriver->Connect(sCOMPort, nBaudRate, nStatusUpdateInterval, nConnectTimeout);

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CMechanicsState_Idle
**************************************************************************************************************************/
class CMechanicsState_Idle : public virtual CMechanicsState {
public:

	CMechanicsState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CMechanicsState(getStateName(), sStateName, pPluginData)
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
 Class definition of CMechanicsState_Recoating
**************************************************************************************************************************/
class CMechanicsState_Recoating : public virtual CMechanicsState {
public:

	CMechanicsState_Recoating(const std::string& sStateName, PPluginData pPluginData)
		: CMechanicsState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "recoating";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Recoating...");
		auto pDriver = m_pPluginData->acquireMarlinDriver(pStateEnvironment);

		if (!pDriver->CanExecuteMovement())
			throw std::runtime_error("could not execute movement");

		pDriver->MoveToXY(100.0, 0.0, 0, 100.0);

		if (!pDriver->CanExecuteMovement())
			throw std::runtime_error("could not execute movement");

		pDriver->MoveToXY(0.0, 0.0, 0, 100.0);

		auto pSignalHandler = pStateEnvironment->RetrieveSignal("recoatsignal");
		pSignalHandler->SetBoolResult("success", true);
		pSignalHandler->SignalHandled();

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CMechanicsState_FatalError : public virtual CMechanicsState {
public:

	CMechanicsState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CMechanicsState(getStateName(), sStateName, pPluginData)
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
	m_pPluginData = std::make_shared<CMechanicsData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CMechanicsState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMechanicsState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMechanicsState_Recoating>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMechanicsState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
