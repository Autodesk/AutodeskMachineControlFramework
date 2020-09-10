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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

using namespace LibMCPlugin::Impl;


/*************************************************************************************************************************
 Class definition of CPIDControlData
**************************************************************************************************************************/
class CPIDControlData : public virtual CPluginData {

public:

	bool setPIDValues (LibMCEnv::PStateEnvironment pStateEnvironment, double dP, double dI, double dD)
	{
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_setpidvalues");
		pSignal->SetDouble("dp", dP);
		pSignal->SetDouble("di", dI);
		pSignal->SetDouble("dd", dD);
		pSignal->Trigger();

		pStateEnvironment->LogMessage("PID-Control: setting P-Values");
		if (pSignal->WaitForHandling(20000)) {
			// Store the p-Values in the state
			pStateEnvironment->SetDoubleParameter("status", "dp", dP);
			pStateEnvironment->SetDoubleParameter("status", "di", dI);
			pStateEnvironment->SetDoubleParameter("status", "dd", dD);

			return true;

		}
		else {
			pStateEnvironment->LogMessage("PID-Control: Set P-Values timeout");
		}

		return false;

	}

};

/*************************************************************************************************************************
 Class definition of CPIDControlState
**************************************************************************************************************************/
typedef CState<CPIDControlData> CPIDControlState;

/*************************************************************************************************************************
 Class definition of CPIDControlState_Init
**************************************************************************************************************************/
class CPIDControlState_Init : public virtual CPIDControlState {
public:

	CPIDControlState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CPIDControlState(getStateName(), sStateName, pPluginData)
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

		pStateEnvironment->LogMessage("PIDControl Initialisation...");

		pStateEnvironment->SetNextState("idle");


	}

};


/*************************************************************************************************************************
 Class definition of CPIDControlState_Idle
**************************************************************************************************************************/
class CPIDControlState_Idle : public virtual CPIDControlState {
public:

	CPIDControlState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CPIDControlState(getStateName(), sStateName, pPluginData)
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

		pStateEnvironment->SetBoolParameter("status", "isactive", false);

		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_startcontrolling", 100, pSignalHandler)) {

			//pSignalHandler->SignalHandled();

			pStateEnvironment->LogMessage("PIDControl Active...");
			// TODO blocked while testing extrusion => remove sleep/idle and uncomment later pStateEnvironment->SetNextState("controlling");
			pStateEnvironment->Sleep(2000);
			pStateEnvironment->SetNextState("idle");

		}
		else {
			pStateEnvironment->SetNextState("idle");
		}


	}

};


/*************************************************************************************************************************
 Class definition of CPIDControlState_Controlling
**************************************************************************************************************************/
class CPIDControlState_Controlling : public virtual CPIDControlState {
public:

	CPIDControlState_Controlling(const std::string& sStateName, PPluginData pPluginData)
		: CPIDControlState(getStateName(), sStateName, pPluginData)
	{

	}

	static const std::string getStateName()
	{
		return "controlling";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		// Check if I should stop
		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_stopcontrolling", 100, pSignalHandler)) {

			pSignalHandler->SignalHandled();

			pStateEnvironment->LogMessage("PIDControl Active...");
			pStateEnvironment->SetNextState("idle");
			return;
		} 


		// Not stopping

		pStateEnvironment->SetBoolParameter("status", "isactive", true);
		double dP = pStateEnvironment->GetDoubleParameter("status", "dp");
		double dI = pStateEnvironment->GetDoubleParameter("status", "di");
		double dD = pStateEnvironment->GetDoubleParameter("status", "dd");

		// Adjust the parameters somehow
		dP += 0.1;
		dI -= 0.05;
		dD = (dP + dI) * 0.5;

		if (m_pPluginData->setPIDValues(pStateEnvironment, dP, dI, dD)) {
			pStateEnvironment->SetNextState("controlling");

		}
		else {
			// Could not change signal...
			pStateEnvironment->LogMessage("Could not set P-Values!");
			pStateEnvironment->SetNextState("fatalerror");
		} 


	}

};



/*************************************************************************************************************************
 Class definition of CPIDControlState_FatalError
**************************************************************************************************************************/
class CPIDControlState_FatalError : public virtual CPIDControlState {
public:

	CPIDControlState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CPIDControlState(getStateName(), sStateName, pPluginData)
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
	m_pPluginData = std::make_shared<CPIDControlData>();
}


IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CPIDControlState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPIDControlState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPIDControlState_Controlling>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPIDControlState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}

