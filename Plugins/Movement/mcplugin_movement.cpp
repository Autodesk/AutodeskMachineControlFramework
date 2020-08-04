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
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
typedef LibMCDriver_Marlin::PDriver_Marlin PDriver_Marlin;
typedef LibMCEnv::CDriverCast <LibMCDriver_Marlin::CDriver_Marlin, LibMCDriver_Marlin::CWrapper> PDriverCast_Marlin;


/*************************************************************************************************************************
 Class definition of CMovementData
**************************************************************************************************************************/
class CMovementData : public virtual CPluginData {	
private:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_Marlin m_DriverCast_Marlin;

public:

	PDriver_Marlin acquireDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_Marlin.acquireDriver(pStateEnvironment, "marlin");
	}


	void updateStateFromDriver(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pDriver.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		double dTargetX;
		double dTargetY;
		double dTargetZ;
		double dCurrentX;
		double dCurrentY;
		double dCurrentZ;
		double dCurrentTemperature;
		double dTargetTemperature;

		pDriver->UpdateState();
		pDriver->GetCurrentPosition(dCurrentX, dCurrentY, dCurrentZ);
		pDriver->GetTargetPosition(dTargetX, dTargetY, dTargetZ);
		pDriver->GetExtruderTemperature(0, dCurrentTemperature, dTargetTemperature);

		pStateEnvironment->SetDoubleParameter("movementstate", "targetx", dTargetX);
		pStateEnvironment->SetDoubleParameter("movementstate", "targety", dTargetY);
		pStateEnvironment->SetDoubleParameter("movementstate", "targetz", dTargetZ);
		pStateEnvironment->SetDoubleParameter("movementstate", "currentx", dCurrentX);
		pStateEnvironment->SetDoubleParameter("movementstate", "currenty", dCurrentY);
		pStateEnvironment->SetDoubleParameter("movementstate", "currentz", dCurrentZ);
		pStateEnvironment->SetDoubleParameter("movementstate", "currenttemperature", dCurrentTemperature);
		pStateEnvironment->SetDoubleParameter("movementstate", "targettemperature", dTargetTemperature);
	}


};


/*************************************************************************************************************************
 Class definition of CMovementState
**************************************************************************************************************************/
typedef CState<CMovementData> CMovementState;

/*************************************************************************************************************************
 Class definition of CMovementState_Init
**************************************************************************************************************************/
class CMovementState_Init : public virtual CMovementState {
public:

	CMovementState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
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

		auto sCOMPort = pStateEnvironment->GetStringParameter ("comdata", "port");
		auto nBaudRate = pStateEnvironment->GetIntegerParameter("comdata", "baudrate");

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		pDriver->Connect(sCOMPort, (uint32_t) nBaudRate);

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CMovementState_Idle
**************************************************************************************************************************/
class CMovementState_Idle : public virtual CMovementState {
public:

	CMovementState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
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

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		
		m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment);
		if (pDriver->CanExecuteMovement()) {

			std::string sParameterData;

			LibMCEnv::PSignalHandler pSignalHandler;
			if (pStateEnvironment->WaitForSignal("signal_recoatlayer", 100, pSignalHandler)) {
				pStateEnvironment->StoreSignal("signal_recoat", pSignalHandler.get());

				pStateEnvironment->SetNextState("moveleft");

			}
			else {
				pStateEnvironment->SetNextState("idle");
			}
		}
		else {
			pStateEnvironment->SetNextState("idle");
		}
	}

};



/*************************************************************************************************************************
 Class definition of CMovementState_FatalError
**************************************************************************************************************************/
class CMovementState_FatalError : public virtual CMovementState {
public:

	CMovementState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
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
 Class definition of CMovementState_MoveLeft
**************************************************************************************************************************/
class CMovementState_MoveLeft : public virtual CMovementState {
public:

	CMovementState_MoveLeft(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
	{

	}

	static const std::string getStateName()
	{
		return "moveleft";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		uint32_t nCounter = 0;

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);

		while ((nCounter < 20) || (pDriver->IsMoving())) {

			if (pDriver->CanExecuteMovement() && (nCounter < 20)) {

				nCounter++;

				pDriver->MoveTo (nCounter * 5.0, 0.0, 0.0, 100.0);
			}

			m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment);


			
			pStateEnvironment->Sleep(10);


		} 

		pStateEnvironment->SetNextState("moveright");

	}

};


/*************************************************************************************************************************
 Class definition of CMovementState_MoveRight
**************************************************************************************************************************/
class CMovementState_MoveRight : public virtual CMovementState {
public:

	CMovementState_MoveRight(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
	{

	}

	static const std::string getStateName()
	{
		return "moveright";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->Sleep(1500);

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		pDriver->MoveTo(0.0, 0.0, 0.0, 100.0);
		
		while (pDriver->IsMoving()) {
			pDriver->UpdateState ();
			pStateEnvironment->Sleep(10);
		} 

		auto pSignal = pStateEnvironment->RetrieveSignal("signal_recoat");
		pSignal->SetBoolResult("success", true);
		pSignal->SignalHandled();

		pStateEnvironment->SetNextState("idle");

	}

};



/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName)
{
	m_pPluginData = std::make_shared<CMovementData>();
}


IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CMovementState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMovementState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMovementState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMovementState_MoveLeft>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMovementState_MoveRight>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}

