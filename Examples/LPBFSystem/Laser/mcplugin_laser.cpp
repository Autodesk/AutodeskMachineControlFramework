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
#include "libmcdriver_scanlab_dynamic.hpp"
#include "libmcdriver_raylase_dynamic.hpp"
#include "libmcdriver_scanlabsmc_dynamic.hpp"

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(ScanLab, ScanLab_RTC6)
LIBMC_IMPORTDRIVERCLASSES(Raylase, Raylase)
LIBMC_IMPORTDRIVERCLASSES(ScanLabSMC, ScanLabSMC)

__BEGINDRIVERIMPORT
__IMPORTDRIVER(ScanLab_RTC6, "scanlab");
__IMPORTDRIVER(Raylase, "raylase");
__IMPORTDRIVER(ScanLabSMC, "scanlabsmc_0_9");
__ENDDRIVERIMPORT


/*************************************************************************************************************************
 Custom functionality implementation
**************************************************************************************************************************/

void InitialiseRaylaseDriver(LibMCEnv::PStateEnvironment pStateEnvironment, PDriver_Raylase pDriver)
{
	pStateEnvironment->LogMessage("Initialising Raylase Driver");

	if (pStateEnvironment->GetBoolParameter("cardconfig", "simulatelaser")) {
		pStateEnvironment->LogMessage("Laser Simulation enabled!...");
		pDriver->SetToSimulationMode();
	}

	if (!pDriver->IsSimulationMode()) {
		pDriver->LoadSDK();
	}

	auto sIP = pStateEnvironment->GetStringParameter("cardconfig", "ipaddress");
	auto nPort = pStateEnvironment->GetIntegerParameter("cardconfig", "port");
	auto dMaxLaserPower = pStateEnvironment->GetDoubleParameter("cardconfig", "maxlaserpower");

	pStateEnvironment->LogMessage("Connecting to Raylase card " + sIP + ":" + std::to_string(nPort));
	auto pCard = pDriver->ConnectByIP("card1", sIP, (uint32_t)nPort, dMaxLaserPower);
	pCard->ResetToSystemDefaults();
	pCard->EnablePilot(true);

}

void InitialiseScanlabDriver(LibMCEnv::PStateEnvironment pStateEnvironment, PDriver_ScanLab_RTC6 pDriver)
{
	pStateEnvironment->LogMessage("Initialising Scanlab Driver");

	if (pStateEnvironment->GetBoolParameter("cardconfig", "simulatelaser")) {
		pStateEnvironment->LogMessage("Laser Simulation enabled!...");
		pDriver->SetToSimulationMode();
	}

	if (!pDriver->IsSimulationMode()) {
		pDriver->LoadSDK("rtc6dllx64");
		//pDriver->LoadSDK("rtcsimdll");
	}

	auto sIP = pStateEnvironment->GetStringParameter("cardconfig", "ipaddress");
	auto sNetmask = pStateEnvironment->GetStringParameter("cardconfig", "netmask");
	auto nTimeout = pStateEnvironment->GetIntegerParameter("cardconfig", "timeout");
	auto nSerial = pStateEnvironment->GetIntegerParameter("cardconfig", "serial");
	auto dMaxLaserPower = pStateEnvironment->GetDoubleParameter("cardconfig", "maxlaserpower");
	auto dLaserOnDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "laserondelay");
	auto dLaserOffDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "laseroffdelay");
	auto dMarkDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "markdelay");
	auto dJumpDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "jumpdelay");
	auto dPolygonDelay = pStateEnvironment->GetDoubleParameter("cardconfig", "polygondelay");

	auto sCorrectionResourceName = pStateEnvironment->GetStringParameter("correction", "resourcename");
	auto nTableIndex = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tableindex");
	auto nDimension = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "dimension");
	auto nTableNumberHeadA = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tablenumbera");
	auto nTableNumberHeadB = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tablenumberb");

	pStateEnvironment->LogMessage("Acquiring ScanLab card #" + std::to_string(nSerial));
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
	pDriver->ConfigureLaserMode(LibMCDriver_ScanLab::eLaserMode::YAG1, LibMCDriver_ScanLab::eLaserPort::Port12BitAnalog1, dMaxLaserPower, false, false, false, false, false, false);

	pStateEnvironment->LogMessage("Configuring delays...");
	pDriver->ConfigureDelays(dLaserOnDelay, dLaserOffDelay, dMarkDelay, dJumpDelay, dPolygonDelay);
	pStateEnvironment->LogMessage("Initialising done..");


}

void InitialiseScanlabSMCDriver(LibMCEnv::PStateEnvironment pStateEnvironment, PDriver_ScanLabSMC pDriver, const std::string & sJobUUID)
{
	pStateEnvironment->LogMessage("Initialising Scanlab SMC Driver");

	pStateEnvironment->LogMessage("Loading build job " + sJobUUID);
	auto pBuildJob = pStateEnvironment->GetBuildJob(sJobUUID);
	auto pToolpath = pBuildJob->CreateToolpathAccessor();
	auto pMetaData = pToolpath->FindUniqueMetaData("http://schemas.scanlab.com/smc/2024/10", "smc_control");

	auto pAttribute = pMetaData->FindAttribute("http://schemas.scanlab.com/smc/2024/10", "config", true);
	std::string sConfigName = pAttribute->GetValue();

	pStateEnvironment->LogMessage("Loading SMC Configuration Template from " + sConfigName);

	std::vector<uint8_t> buffer;
	pToolpath->GetBinaryMetaData(sConfigName, buffer);

	std::string sSMCConfigurationTemplate(buffer.begin(), buffer.end());


	auto sIPAddress = pStateEnvironment->GetStringParameter("cardconfig", "ipaddress");
	auto nSerial = pStateEnvironment->GetIntegerParameter("cardconfig", "serial");
	auto sCorrectionResourceName = pStateEnvironment->GetStringParameter("correction", "resourcename");

	auto pConfiguration = pDriver->CreateEmptyConfiguration();

	pConfiguration->SetDynamicViolationReaction(LibMCDriver_ScanLabSMC::eDynamicViolationReaction::StopAndReport);

	pConfiguration->SetSerialNumber((uint32_t) nSerial);
	pConfiguration->SetConfigurationTemplate (sSMCConfigurationTemplate);
	pConfiguration->SetIPAddress(sIPAddress);
	pConfiguration->SetCorrectionFileResource(sCorrectionResourceName);
	pConfiguration->SetSimulationSubDirectory("SMCSimulation");
	pConfiguration->SetFirmwareResources("rtc6eth", "rtc6rbf", "rtc6dat");

	auto pContext = pDriver->CreateContext("smccontext", pConfiguration);

}


/*************************************************************************************************************************
  State definitions
**************************************************************************************************************************/
__BEGINSTATEDEFINITIONS

__DECLARESTATE(init)
{
	std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
	if (sCardType == "scanlab")
	{
		InitialiseScanlabDriver(pStateEnvironment, __acquireDriver(ScanLab_RTC6));
	}
	else if (sCardType == "scanlabsmc")
	{
	}
	else if (sCardType == "raylase") {
		InitialiseRaylaseDriver(pStateEnvironment, __acquireDriver(Raylase));
	}
	else
		throw std::runtime_error("invalid scanner card type: " + sCardType);

	pStateEnvironment->SetNextState("idle");	
}

__DECLARESTATE(idle)
{
	std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
	if (sCardType == "scanlab")
	{
		auto pDriver = __acquireDriver(ScanLab_RTC6);
		pDriver->QueryParameters();
	}
	else if (sCardType == "raylase") {
		auto pDriver = __acquireDriver(Raylase);
		pDriver->QueryParameters();
	}
	else if (sCardType == "scanlabsmc") {
		auto pDriver = __acquireDriver(ScanLabSMC);
		pDriver->QueryParameters();
	}

		
	LibMCEnv::PSignalHandler pHandlerInstance;

	if (pStateEnvironment->WaitForSignal("signal_initlaserforjob", 0, pHandlerInstance)) {
		std::string sJobUUID = pHandlerInstance->GetUUID("jobuuid");

		if (sCardType == "scanlabsmc") {

			auto pDriver = __acquireDriver(ScanLabSMC);
			InitialiseScanlabSMCDriver(pStateEnvironment, pDriver, sJobUUID);

		}		

		pHandlerInstance->SetBoolResult("success", true);
		pHandlerInstance->SignalHandled();
	}


	if (pStateEnvironment->WaitForSignal("signal_exposure", 0, pHandlerInstance)) {
		pStateEnvironment->StoreSignal("exposuresignal", pHandlerInstance);

		pStateEnvironment->SetNextState("exposure");

	}
	else {
		pStateEnvironment->SetNextState("idle");
	}

}




__DECLARESTATE(exposure)
{

	auto pSignalHandler = pStateEnvironment->RetrieveSignal("exposuresignal");

	pStateEnvironment->LogMessage("Exposure...");
	auto pBuildJob = pStateEnvironment->GetBuildJob(pSignalHandler->GetString("jobuuid"));
	auto pExecution = pBuildJob->FindExecution(pSignalHandler->GetString("executionuuid"));
	auto nLayerIndex = (uint32_t)pSignalHandler->GetInteger("layerindex");


	std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
	if (sCardType == "scanlab")
	{
		auto pDriver = __acquireDriver(ScanLab_RTC6);

		if (pDriver->IsSimulationMode()) {
			pStateEnvironment->LogMessage("Simulation mode delay of 2 seconds..");
			pStateEnvironment->Sleep(2000);
		}

		auto pContext = pDriver->GetContext();
		auto pRecording = pContext->PrepareRecording(true);

		pRecording->AddChannel("x", LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetXBacktransformed);
		pRecording->AddChannel("y", LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetYBacktransformed);
		pRecording->AddChannel("power", LibMCDriver_ScanLab::eRTCChannelType::ChannelAnalogOut1);

		pContext->SetStartList(1, 0);
		pRecording->EnableRecording(LibMCDriver_ScanLab::eRTCRecordingFrequency::Record100kHz);

		auto pAccessor = pBuildJob->CreateToolpathAccessor();
		auto pLayer = pAccessor->LoadLayer(nLayerIndex);
		pContext->AddLayerToList(pLayer, false);

		pRecording->DisableRecording();

		pStateEnvironment->LogMessage("Starting Execution...");

		pRecording->ExecuteListWithRecording();

		pStateEnvironment->LogMessage("Execution finished");

		auto pDataTable = pStateEnvironment->CreateDataTable();
		pRecording->AddRecordsToDataTable("x", pDataTable, "x", "X");
		pRecording->AddRecordsToDataTable("y", pDataTable, "y", "Y");
		pRecording->AddRecordsToDataTable("power", pDataTable, "power", "Power");

		auto pTempStream = pStateEnvironment->CreateTemporaryStream("layer_" + std::to_string (nLayerIndex), "text/csv");
		pDataTable->WriteCSVToStream(pTempStream, nullptr);
		pTempStream->Finish();

		//pDriver->DrawLayer(pBuildJob->GetStorageUUID(), nLayerIndex);
	}
	else if (sCardType == "scanlabsmc")
	{
		auto pDriver = __acquireDriver(ScanLabSMC);



		auto pContext = pDriver->FindContext("smccontext");
		//pContext->DrawLayer(pBuildJob->GetStorageUUID(), nLayerIndex);
		auto pJob = pContext->BeginJob(0.0, 0.0, LibMCDriver_ScanLabSMC::eBlendMode::Deactivated);

		auto pDataTable = pStateEnvironment->CreateDataTable();

		auto pAccessor = pBuildJob->CreateToolpathAccessor();
		pStateEnvironment->LogMessage("Loading layer...");

		auto pLayer = pAccessor->LoadLayer(nLayerIndex);
		pStateEnvironment->LogMessage("Writing layer to SCANmotionControl...");

		pJob->AddLayerToList(pLayer);
		pJob->Finalize();
		pStateEnvironment->LogMessage("Executing layer...");
		pJob->Execute (true);

		pStateEnvironment->LogMessage("Parsing simulation data...");
		pJob->LoadSimulationData(pDataTable);

		pStateEnvironment->LogMessage("Parsing finished...");
		//pExecution->StoreDataTable("smcdatalayer" + std::to_string(nLayerIndex), "SMC Data Layer " + std::to_string(nLayerIndex), pDataTable, nullptr, "");

		auto pTempStream = pStateEnvironment->CreateTemporaryStream("smcdatalayer" + std::to_string(nLayerIndex), "text/csv");
		pDataTable->WriteCSVToStream(pTempStream, nullptr);	
		pTempStream->Finish();

		pExecution->AttachTempStream("smcdatalayer" + std::to_string(nLayerIndex), "SMC Data Layer " + std::to_string(nLayerIndex), "", pTempStream.get());
		
	}
	else if (sCardType == "raylase") {
		auto pDriver = __acquireDriver(Raylase);

		if (pDriver->IsSimulationMode()) {
			pStateEnvironment->LogMessage("Simulation mode delay of 2 seconds..");
			pStateEnvironment->Sleep(2000);
		}

		pStateEnvironment->LogMessage("Get Connected Card...");
		auto pCard = pDriver->GetConnectedCard("card1");
		pStateEnvironment->LogMessage("Drawing Layer...");
		pCard->DrawLayer(pBuildJob->GetStorageUUID(), nLayerIndex, 100000);
		pStateEnvironment->LogMessage("Drawing Layer successful...");
	}

	pSignalHandler->SetBoolResult("success", true);
	pSignalHandler->SignalHandled();

	pStateEnvironment->SetNextState("idle");
}




__DECLARESTATE(fatalerror)
{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->SetNextState("fatalerror");

}

__ENDSTATEDEFINITIONS