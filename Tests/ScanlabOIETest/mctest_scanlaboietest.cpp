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
#include <chrono>
#include <thread>
#include <cmath>


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

#if 0

		std::string sOIEIniName = "oie_test2";
		std::string sIP = "169.254.1.4";
		std::string sNetmask = "255.255.0.0";
		std::string sOIEIPAddress = "169.254.1.6";
		uint32_t nTimeout = 1000;
		uint32_t nSerial = 360208;
		double dMaxLaserPower = 400;
		double dLaserOnDelay = 330;
		double dLaserOffDelay = 660;
		double dMarkDelay = 550;
		double dJumpDelay = 10;
		double dPolygonDelay = 10;

		std::string  sCorrectionResourceName = "d3_2995";
		uint32_t nTableIndex = 1;
		uint32_t nDimension = 3;
		uint32_t nTableNumberHeadA = 1;
		uint32_t nTableNumberHeadB = 0;

#else

		std::string sOIEIniName = "oie_test1";
		std::string sIP = "192.168.5.2";
		std::string sNetmask = "255.255.255.0";
		std::string sOIEIPAddress = "192.168.5.8";
		uint32_t nTimeout = 4000;
		uint32_t nSerial = 356266;
		double dMaxLaserPower = 300;
		double dLaserOnDelay = 100;
		double dLaserOffDelay = 100;
		double dMarkDelay = 10;
		double dJumpDelay = 10;
		double dPolygonDelay = 10;

		std::string  sCorrectionResourceName = "d3_2889";
		uint32_t nTableIndex = 1;
		uint32_t nDimension = 3;
		uint32_t nTableNumberHeadA = 1;
		uint32_t nTableNumberHeadB = 0;

#endif

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);
		
		pStateEnvironment->LogMessage("acquiring OIE Driver...");
		auto pOIEDriver = m_pPluginData->acquireOIE(pStateEnvironment);

		pStateEnvironment->LogMessage("Loading SDK...");
		pOIEDriver->SetDependencyResourceNames("libssl-1_1-x64", "libcrypto-1_1-x64", "qt5core-x64", "qt5network-x64");
		pOIEDriver->SetOIE3ResourceNames("oiecalibrationlibrary64", "rtcstreamparser64");
		pOIEDriver->InitializeSDK("liboie-x64");

		pStateEnvironment->LogMessage("Loading OIE Device Config...");
		std::string sDeviceConfig = pStateEnvironment->LoadResourceString(sOIEIniName);

		auto pDeviceConfigurationInstance = pOIEDriver->ParseDeviceConfiguration(sDeviceConfig);

		switch (pDeviceConfigurationInstance->GetDeviceType()) {
		case LibMCDriver_ScanLabOIE::eRTCDeviceType::RTC5:
			pStateEnvironment->LogMessage("Configured for RTC5");
			break;
		case LibMCDriver_ScanLabOIE::eRTCDeviceType::RTC6:
			pStateEnvironment->LogMessage("Configured for RTC6");
			break;
		default:
			throw std::runtime_error("Unknown RTC Card");
		}

		std::cout << "RTC Signal Count: " << pDeviceConfigurationInstance->GetRTCSignalCount() << std::endl;
		std::cout << "Sensor Signal Count: " << pDeviceConfigurationInstance->GetSensorSignalCount() << std::endl;

		std::vector<uint32_t> signals;
		pDeviceConfigurationInstance->GetRTCSignalIDs(signals);

		for (auto signal : signals)
			pStateEnvironment->LogMessage("  --Found signal #" + std::to_string(signal));

		pStateEnvironment->LogMessage("Initialising Scanlab Driver");
		auto pRTC6Driver = m_pPluginData->acquireRTC6(pStateEnvironment);
		pRTC6Driver->LoadSDK("rtc6dllx64");

		pStateEnvironment->LogMessage("Acquiring ScanLab card #" + std::to_string(nSerial));
		pRTC6Driver->Initialise(sIP, sNetmask, (uint32_t)nTimeout, (uint32_t)nSerial);

		pRTC6Driver->SetCommunicationTimeouts(500, 3000, 1.2);

		std::string sFirmwareResource;
		if (sIP.empty()) {
			pStateEnvironment->LogMessage("Loading RTC PCI firmware...");
			sFirmwareResource = "rtc6out";
		}
		else {
			pStateEnvironment->LogMessage("Loading RTC Ethernet firmware...");
			sFirmwareResource = "rtc6eth";
		}

		pRTC6Driver->LoadFirmware(sFirmwareResource, "rtc6rbf", "rtc6dat");

		std::vector<uint8_t> CorrectionFileBuffer;
		pStateEnvironment->LogMessage("Loading correction file...");
		pStateEnvironment->LoadResourceData(sCorrectionResourceName, CorrectionFileBuffer);
		pStateEnvironment->LogMessage("correction file size: " + std::to_string (CorrectionFileBuffer.size()));

		pRTC6Driver->SetCorrectionFile(CorrectionFileBuffer, nTableIndex, nDimension, nTableNumberHeadA, nTableNumberHeadB);

		pStateEnvironment->LogMessage("Configuring laser...");
		pRTC6Driver->ConfigureLaserMode(LibMCDriver_ScanLab::eLaserMode::YAG1, LibMCDriver_ScanLab::eLaserPort::Port12BitAnalog1, dMaxLaserPower, false, false, true, true, false, false);

		pStateEnvironment->LogMessage("Configuring delays...");
		pRTC6Driver->ConfigureDelays(dLaserOnDelay, dLaserOffDelay, dMarkDelay, dJumpDelay, dPolygonDelay);
		pStateEnvironment->LogMessage("Initialising done..");

		pStateEnvironment->LogMessage("Initializing for OIE..");
		auto pRTCContext = pRTC6Driver->GetContext();

		switch (pOIEDriver->GetDriverType()) {
		case LibMCDriver_ScanLabOIE::eOIEDeviceDriverType::OIEVersion2:

			pStateEnvironment->LogMessage("Initializing RTC Context for SDK Version 2...");
			pRTCContext->InitializeForOIE(signals, LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion2);
			break;

		case LibMCDriver_ScanLabOIE::eOIEDeviceDriverType::OIEVersion3Compatibility:
			pStateEnvironment->LogMessage("Initializing RTC Context for SDK Version 3 (compatibilty mode)...");
			pRTCContext->InitializeForOIE(signals, LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3Compatibility);
			break;

		case LibMCDriver_ScanLabOIE::eOIEDeviceDriverType::OIEVersion3:
			pStateEnvironment->LogMessage("Initializing RTC Context for SDK Version 3...");
			pRTCContext->InitializeForOIE(signals, LibMCDriver_ScanLab::eOIEOperationMode::OIEVersion3);
			break;

		default:
			throw std::runtime_error("unsupported OIE SDK Version");

		}
		

		pStateEnvironment->LogMessage("Adding Device...");
		auto pDevice = pOIEDriver->AddDevice("oie1", sOIEIPAddress, 21072, pDeviceConfigurationInstance, CorrectionFileBuffer, 200000);

		pStateEnvironment->LogMessage("  Device Name: " + pDevice->GetDeviceName ());
		pStateEnvironment->LogMessage("  Device ID: " + std::to_string(pDevice->GetDeviceID()));

		pStateEnvironment->LogMessage("Connecting..");
		pDevice->Connect("sluser", "sluser");

		//std::vector<uint8_t> AppPackageBuffer;
		//pStateEnvironment->LoadResourceData("aib-3.0.0", AppPackageBuffer);

		//pStateEnvironment->LogMessage("Installing App");
		//pDevice->InstallApp(AppPackageBuffer);

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
		pDevice->StartAppByMinorVersion("AIB", 2, 0);

		pStateEnvironment->LogMessage("Waiting..");

		pStateEnvironment->LogMessage("App is running:  " + std::to_string ((uint32_t)pDevice->AppIsRunning()));
		std::string sName;
		uint32_t nMajor = 0;
		uint32_t nMinor = 0;
		uint32_t nPatch = 0;
		pDevice->GetRunningApp(sName, nMajor, nMinor, nPatch);
		pStateEnvironment->LogMessage("  Running app: " + sName + " (" + std::to_string(nMajor) + "." + std::to_string(nMinor) + "." + std::to_string(nPatch) + ")");



		pStateEnvironment->LogMessage("Running OIE Test..");


		pRTCContext->SetStartList(1, 0);
		pRTCContext->EnableOIE();
		pRTCContext->StartOIEMeasurement();

		std::vector<LibMCDriver_ScanLab::sPoint2D> ContourPoints;
		//ContourPoints.resize(1000);
		for (int32_t nIndex = 0; nIndex < 1000; nIndex++) {
			float T = (float)(nIndex - 500) / 500.0f * 3.14159f;
			float dX = nIndex * 0.05f;
			float dY = sin(T) * 20.0f;
			ContourPoints.push_back(LibMCDriver_ScanLab::sPoint2D{ dX, dY });

			//std::cout << "Point X: " << dX << " " << dY << std::endl;
		}

		std::cout << "How many points? " << ContourPoints.size() << std::endl;


		pRTCContext->DrawPolyline(ContourPoints, 10.0, 100.0, 1.0, 0.0);

		pRTCContext->StopOIEMeasurement();
		pRTCContext->DisableOIE();

		pRTCContext->SetEndOfList();
		pRTCContext->ExecuteList(1, 0);

		//pRTC6Driver->OIETest();

		bool bBusy = true;
		while (bBusy) {
			uint32_t nPosition = 0;
			pRTCContext->GetStatus (bBusy, nPosition);

			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			std::cout << "List Position: " << nPosition << std::endl;

		}

		pDevice->StopApp();

		auto pRecording = pDevice->RetrieveCurrentRecording();
		pStateEnvironment->LogMessage("Storing as debug CSV..");
		std::vector<double> XArrayBuffer;
		std::vector<double> YArrayBuffer;
		std::vector<int32_t> SignalsBuffer;
		std::vector<uint32_t> PacketNumbersBuffer;
		pRecording->GetAllCoordinates(XArrayBuffer, YArrayBuffer);
		pRecording->GetAllSensorSignals(0, SignalsBuffer);
		pRecording->GetAllPacketNumbers(PacketNumbersBuffer);

		size_t nRecordCount = pRecording->GetRecordCount();

		if (XArrayBuffer.size () != nRecordCount)
			throw std::runtime_error("retrieved invalid recording x coord data");
		if (YArrayBuffer.size() != nRecordCount)
			throw std::runtime_error("retrieved invalid recording y coord data");
		if (SignalsBuffer.size() != nRecordCount)
			throw std::runtime_error("retrieved invalid recording signal data");
		if (PacketNumbersBuffer.size() != nRecordCount)
			throw std::runtime_error("retrieved invalid packet numbers data");

		std::ofstream fStream;
		fStream.open("debug.csv");
		if (!fStream.is_open())
			throw std::runtime_error("could not write file");

		fStream << "packet number, X, Y, Sensor Value" << std::endl;

		for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
			fStream << PacketNumbersBuffer[nRecordIndex] << ", " << XArrayBuffer[nRecordIndex] << ", " << YArrayBuffer[nRecordIndex] << ", " << SignalsBuffer[nRecordIndex] << std::endl;
		}

		pStateEnvironment->LogMessage("Waiting..");

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


