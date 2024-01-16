/*++

Copyright (C) 2023 Autodesk Inc.

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
#include "libmcdriver_scanlabsmc_dynamic.hpp"

using namespace LibMCPlugin::Impl;

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cmath>


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(ScanLabSMC, ScanLabSMC)


/*************************************************************************************************************************
 Class definition of CTestData
**************************************************************************************************************************/
class CTestData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_ScanLabSMC m_DriverCast_ScanLab_SMC;

public:

	PDriver_ScanLabSMC acquireSMC (LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_ScanLab_SMC.acquireDriver(pStateEnvironment, "scanlab_smc");
	}

};

/*************************************************************************************************************************
 Class definition of CRasterizerState
**************************************************************************************************************************/
typedef CState<CTestData> CTestState;


/*************************************************************************************************************************
 Class definition of CTestState_Init
**************************************************************************************************************************/
class CTestState_Init : public virtual CTestState {
public:

	CTestState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CTestState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		auto pDriver = m_pPluginData->acquireSMC(pStateEnvironment);

		auto pConfiguration = pDriver->CreateEmptyConfiguration();

		pConfiguration->SetDynamicViolationReaction(LibMCDriver_ScanLabSMC::eDynamicViolationReaction::StopAndReport);

		pConfiguration->SetSerialNumber(1234);
		pConfiguration->SetIPAddress("192.168.0.1");
		pConfiguration->SetCorrectionFileResource("d2_1238");
		//pConfiguration->SetFirmwareResources("rtc6eth", "rtc6rbf", "rtc6dat");

		auto pContext = pDriver->CreateContext("smccontext", pConfiguration);

		std::vector<LibMCDriver_ScanLabSMC::sHatch2D> Hatches;

		for (uint32_t nIndex = 0; nIndex < 10; nIndex++) {
			Hatches.push_back({ 10.0, nIndex + 10.0f, 50.0f, nIndex + 10.0f });
		}

		for (uint32_t nIndex = 0; nIndex < 10; nIndex++) {
			Hatches.push_back({ nIndex + 10.0f, 10.0f, nIndex + 10.0f, 110.0f });
		}

		auto pJob = pContext->BeginJob (0.0, 0.0, LibMCDriver_ScanLabSMC::eBlendMode::MaxAccuracy);
		pJob->DrawHatches(Hatches, 2000.0, 2000.0, 10.0, 0.0);

		std::vector <LibMCDriver_ScanLabSMC::sPoint2D> polyLine;
		polyLine.push_back(LibMCDriver_ScanLabSMC::sPoint2D{ 10.0, 10.0 });
		polyLine.push_back(LibMCDriver_ScanLabSMC::sPoint2D{ 100.0, 10.0 });
		polyLine.push_back(LibMCDriver_ScanLabSMC::sPoint2D{ 100.0, 100.0 });
		polyLine.push_back(LibMCDriver_ScanLabSMC::sPoint2D{ 10.0, 100.0 });
		polyLine.push_back(LibMCDriver_ScanLabSMC::sPoint2D{ 10.0, 10.0 });

		pStateEnvironment->LogMessage("DrawLoop");

		pJob->DrawLoop(polyLine, 2000.0, 20.0, 10000.0, 10.0, 0.1, 0.0);

		pStateEnvironment->LogMessage("Finalize");

		pJob->Finalize();

		pStateEnvironment->LogMessage("Execute");
		pJob->Execute (true);

		pStateEnvironment->LogMessage("Execute done");



		pStateEnvironment->SetNextState("success");
	}

};



/*************************************************************************************************************************
 Class definition of CTestState_Success
**************************************************************************************************************************/
class CTestState_Success : public virtual CTestState {
public:

	CTestState_Success(const std::string& sStateName, PPluginData pPluginData)
		: CTestState(getStateName(), sStateName, pPluginData)
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
 Class definition of CTestState_FatalError
**************************************************************************************************************************/
class CTestState_FatalError : public virtual CTestState {
public:

	CTestState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CTestState(getStateName(), sStateName, pPluginData)
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
	m_pPluginData = std::make_shared<CTestData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CTestState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;


	if (createStateInstanceByName<CTestState_Success>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CTestState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


