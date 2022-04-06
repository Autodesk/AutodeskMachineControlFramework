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

using namespace LibMCPlugin::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


#include "libmcdriver_scanlab_dynamic.hpp"
#include "libmcdriver_raylase_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(ScanLab, ScanLab_RTC6)
LIBMC_IMPORTDRIVERCLASSES(Raylase, Raylase)


/*************************************************************************************************************************
 Class definition of CLaserData
**************************************************************************************************************************/
class CLaserData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_ScanLab_RTC6 m_DriverCast_ScanLab;
	PDriverCast_Raylase m_DriverCast_Raylase;

public:

	PDriver_ScanLab_RTC6 acquireScanLabDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_ScanLab.acquireDriver(pStateEnvironment, "scanlab");
	}

	PDriver_Raylase acquireRaylaseDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_Raylase.acquireDriver(pStateEnvironment, "raylase");
	}

};

/*************************************************************************************************************************
 Class definition of CLaserState
**************************************************************************************************************************/
typedef CState<CLaserData> CLaserState;


/*************************************************************************************************************************
 Class definition of CLaserState_Init
**************************************************************************************************************************/
class CLaserState_Init : public virtual CLaserState {
public:

	CLaserState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName (), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		pStateEnvironment->LogMessage("Initialising Raylase Driver");

		auto pDriver = m_pPluginData->acquireRaylaseDriver(pStateEnvironment);
		if (pStateEnvironment->GetBoolParameter("cardconfig", "simulatelaser")) {
			pStateEnvironment->LogMessage("Laser Simulation enabled!...");
			pDriver->SetToSimulationMode();
		}

		if (!pDriver->IsSimulationMode()) {
			pDriver->LoadSDK();
		}

		auto sIP = pStateEnvironment->GetStringParameter("cardconfig", "ipaddress");
		auto nPort = pStateEnvironment->GetIntegerParameter("cardconfig", "port");
		auto sNetmask = pStateEnvironment->GetStringParameter("cardconfig", "netmask");
		auto nTimeout = pStateEnvironment->GetIntegerParameter("cardconfig", "timeout");
		auto nSerial = pStateEnvironment->GetIntegerParameter("cardconfig", "serial");
		auto dMaxLaserPower = pStateEnvironment->GetDoubleParameter("cardconfig", "maxlaserpower");
		auto dLaserOnDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "laserondelay");
		auto dLaserOffDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "laseroffdelay");
		auto dMarkDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "markdelay");
		auto dJumpDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "jumpdelay");
		auto dPolygonDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "polygondelay");

		pStateEnvironment->LogMessage("Connecting to Raylase card " + sIP + ":" +  std::to_string(nPort));
		auto pCard = pDriver->ConnectByIP("card1", sIP, (uint32_t)nPort, dMaxLaserPower);

/*		auto sCorrectionResourceName = pStateEnvironment->GetStringParameter("correction", "resourcename");
		auto nTableIndex = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tableindex");
		auto nDimension = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "dimension");
		auto nTableNumberHeadA = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tablenumbera");
		auto nTableNumberHeadB = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tablenumberb");

		pDriver->Initialise(sIP, sNetmask, (uint32_t)nTimeout, (uint32_t)nSerial);

		std::string sFirmwareResource;
		if (sIP.empty()) {
			pStateEnvironment->LogMessage("Loading RTC PCI firmware...");
			sFirmwareResource = "rtc6out";
		}
		else {
			pStateEnvironment->LogMessage("Loading RTC Ethernet firmware...");
			sFirmwareResource = "rtc6eth";
		}

		pDriver->LoadFirmware(sFirmwareResource, "rtc6rbf", "rtc6dat");

		if (!pDriver->IsSimulationMode()) {
			std::vector<uint8_t> CorrectionFileBuffer;
			pStateEnvironment->LogMessage("Loading correction file...");
			pStateEnvironment->LoadResourceData(sCorrectionResourceName, CorrectionFileBuffer);

			pDriver->SetCorrectionFile(CorrectionFileBuffer, nTableIndex, nDimension, nTableNumberHeadA, nTableNumberHeadB);
		}


		pStateEnvironment->LogMessage("Configuring laser...");

		pStateEnvironment->LogMessage("Configuring delays...");
		pDriver->ConfigureDelays(dLaserOnDelay, dLaserOffDelay, dMarkDelay, dJumpDelay, dPolygonDelay);
		pStateEnvironment->LogMessage("Initialising done.."); */

		pStateEnvironment->SetNextState("idle");

	
	}

};


/*************************************************************************************************************************
 Class definition of CLaserState_Idle
**************************************************************************************************************************/
class CLaserState_Idle : public virtual CLaserState {
public:

	CLaserState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "idle";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		auto pDriver = m_pPluginData->acquireRaylaseDriver(pStateEnvironment);
		pDriver->QueryParameters();
		
		LibMCEnv::PSignalHandler pHandlerInstance;
		if (pStateEnvironment->WaitForSignal("signal_exposure", 0, pHandlerInstance)) {
			pStateEnvironment->StoreSignal("exposuresignal", pHandlerInstance);

			pStateEnvironment->SetNextState("exposure");

		}
		else {
			pStateEnvironment->SetNextState("idle");
		}

	}

};


/*************************************************************************************************************************
 Class definition of CLaserState_Exposure
**************************************************************************************************************************/
class CLaserState_Exposure : public virtual CLaserState {
public:

	CLaserState_Exposure(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "exposure";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		auto pSignalHandler = pStateEnvironment->RetrieveSignal("exposuresignal");

		pStateEnvironment->LogMessage("Exposure...");
		auto pBuildJob = pStateEnvironment->GetBuildJob(pSignalHandler->GetString("jobuuid"));
		auto nLayerIndex = (uint32_t)pSignalHandler->GetInteger("layerindex");

		auto pDriver = m_pPluginData->acquireRaylaseDriver(pStateEnvironment);
		auto pCard = pDriver->GetConnectedCard("card1");

		pCard->DrawLayer(pBuildJob->GetStorageUUID(), nLayerIndex);

		pSignalHandler->SetBoolResult("success", true);
		pSignalHandler->SignalHandled();

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CLaserState_FatalError : public virtual CLaserState {
public:

	CLaserState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData)
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
	m_pPluginData = std::make_shared<CLaserData>();
}

IState * CStateFactory::CreateState(const std::string & sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CLaserState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CLaserState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CLaserState_Exposure>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CLaserState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
