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
#include "libmcdriver_scanlaboie_dynamic.hpp"
#include "libmcdriver_scanlab_dynamic.hpp"

using namespace LibMCPlugin::Impl;

#include <iostream>
#include <fstream>


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(ScanLabOIE, ScanLab_OIE)
LIBMC_IMPORTDRIVERCLASSES(ScanLab, ScanLab_RTC6)


/*************************************************************************************************************************
 Class definition of CTestData
**************************************************************************************************************************/
class CTestData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_ScanLab_OIE m_DriverCast_ScanLab_OIE;
	PDriverCast_ScanLab_RTC6 m_DriverCast_ScanLab_RTC6;

public:

	PDriver_ScanLab_OIE acquireOIE (LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_ScanLab_OIE.acquireDriver(pStateEnvironment, "scanlab_oie");
	}

	PDriver_ScanLab_RTC6 acquireRTC6(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_ScanLab_RTC6.acquireDriver(pStateEnvironment, "scanlab_rtc6");
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

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);
		
		
		pStateEnvironment->LogMessage("Loading OIE Device Config...");
		std::vector<uint8_t> buffer;
		pStateEnvironment->LoadResourceData("oie_test1", buffer);
		buffer.push_back(0);
		std::string sDeviceConfig((char*)buffer.data());

		std::cout << "device config: " << sDeviceConfig << std::endl;

		pStateEnvironment->LogMessage("acquiring Driver...");
		auto pOIEDriver = m_pPluginData->acquireOIE(pStateEnvironment);

		pStateEnvironment->LogMessage("Loading SDK...");
		pOIEDriver->SetDependencyResourceNames("libssl-1_1-x64", "libcrypto-1_1-x64", "qt5core-x64", "qt5network-x64");
		pOIEDriver->InitializeSDK("liboie-x64");

		pStateEnvironment->LogMessage("Adding Device...");
		auto pDevice = pOIEDriver->AddDevice("oie1", "192.168.5.8", 21072, 200000);

		pStateEnvironment->LogMessage("  Device Name: " + pDevice->GetDeviceName ());
		pStateEnvironment->LogMessage("  Device ID: " + std::to_string(pDevice->GetDeviceID()));

		pStateEnvironment->LogMessage("Connecting..");
		pDevice->Connect("sluser", "sluser");

		uint32_t appCount = pDevice->GetAppCount();
		pStateEnvironment->LogMessage("Found " + std::to_string (appCount) + " apps...");

		for (uint32_t appIndex = 0; appIndex < appCount; appIndex++) {
			std::string sName;
			uint32_t nMajor = 0;
			uint32_t nMinor = 0;
			uint32_t nPatch = 0;
			pDevice->GetAppInfo(appIndex, sName, nMajor, nMinor, nPatch);
			pStateEnvironment->LogMessage("  - " + sName + " (" + std::to_string(nMajor) + "." + std::to_string(nMinor) + "." + std::to_string(nPatch) + ")");
		}



		pStateEnvironment->LogMessage("Starting App AIB...");
		pDevice->StartAppByName("AIB", sDeviceConfig);

		pStateEnvironment->LogMessage("Waiting..");
		pStateEnvironment->Sleep(3000);

		pStateEnvironment->LogMessage("App is running:  " + std::to_string ((uint32_t)pDevice->AppIsRunning()));
		std::string sName;
		uint32_t nMajor = 0;
		uint32_t nMinor = 0;
		uint32_t nPatch = 0;
		pDevice->GetRunningApp(sName, nMajor, nMinor, nPatch);
		pStateEnvironment->LogMessage("  Running app: " + sName + " (" + std::to_string(nMajor) + "." + std::to_string(nMinor) + "." + std::to_string(nPatch) + ")");

		pStateEnvironment->LogMessage("Waiting..");
		pStateEnvironment->Sleep(3000);

		pStateEnvironment->LogMessage("Disconnecting Device");
		pDevice->Disconnect();

		pStateEnvironment->LogMessage("Done..");



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


