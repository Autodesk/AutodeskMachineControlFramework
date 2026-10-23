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

#include "libmcdriver_scanlab_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

#include <cmath>

using namespace LibMCPlugin::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(ScanLab, ScanLab_RTC6)


/*************************************************************************************************************************
 Class definition of CLaserData
**************************************************************************************************************************/
class CLaserData : public virtual CPluginData {
protected:
	// Globally store driver wrappers in the plugin
	PDriverCast_ScanLab_RTC6 m_DriverCast_ScanLab;

public:
	PDriver_ScanLab_RTC6 acquireScanLabDriver(LibMCEnv::PStateEnvironment pStateEnvironment) {
		return m_DriverCast_ScanLab.acquireDriver(pStateEnvironment, "scanlab");
	}
};


/*************************************************************************************************************************
 State type definition
**************************************************************************************************************************/
typedef CState<CLaserData> CLaserState;


/*************************************************************************************************************************
 Custom functionality implementation
**************************************************************************************************************************/
void InitialiseScanlabDriver(LibMCEnv::PStateEnvironment pStateEnvironment, PDriver_ScanLab_RTC6 pDriver)
{
	pStateEnvironment->LogMessage("Initialising Scanlab Driver");

	if (pStateEnvironment->GetBoolParameter("cardconfig", "simulatelaser")) {
		pStateEnvironment->LogMessage("Laser Simulation enabled!...");
		pDriver->SetToSimulationMode();
	}

	if (!pDriver->IsSimulationMode()) {
		pDriver->LoadSDK("rtc6dllx64");

		std::string sCardConfigGroup = "cardconfig";
		uint32_t nRTCIndex = 1;

		auto sIP = pStateEnvironment->GetStringParameter(sCardConfigGroup, "ipaddress");
		auto sNetmask = pStateEnvironment->GetStringParameter(sCardConfigGroup, "netmask");
		auto nTimeout = pStateEnvironment->GetIntegerParameter(sCardConfigGroup, "timeout");
		auto nSerial = pStateEnvironment->GetIntegerParameter(sCardConfigGroup, "serial");
		auto dLaserOnDelay = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laserondelay");
		auto dLaserOffDelay = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laseroffdelay");
		auto dMarkDelay = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "markdelay");
		auto dJumpDelay = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "jumpdelay");
		auto dPolygonDelay = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "polygondelay");

		auto dLaserOriginX = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laseroriginx");
		auto dLaserOriginY = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laseroriginy");
		auto dLaserFieldMinX = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laserfield_minx");
		auto dLaserFieldMinY = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laserfield_miny");
		auto dLaserFieldMaxX = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laserfield_maxx");
		auto dLaserFieldMaxY = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "laserfield_maxy");

		auto dTransformationAngle = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "transformation_angle");
		auto dTransformationOffsetX = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "transformation_offsetx");
		auto dTransformationOffsetY = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "transformation_offsety");
		auto dTransformationScaleX = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "transformation_scalex");
		auto dTransformationScaleY = pStateEnvironment->GetDoubleParameter(sCardConfigGroup, "transformation_scaley");


		auto sCorrectionResourceName = pStateEnvironment->GetStringParameter("correction", "resourcename");
		auto nTableIndex = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tableindex");
		auto nDimension = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "dimension");
		auto nTableNumberHeadA = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tablenumbera");
		auto nTableNumberHeadB = (uint32_t)pStateEnvironment->GetIntegerParameter("correction", "tablenumberb");
		auto dCorrectionFactor = pStateEnvironment->GetDoubleParameter("correction", "correctionfactor");


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

		std::vector<uint8_t> CorrectionFileBuffer;
		pStateEnvironment->LogMessage("Loading correction file...");
		pStateEnvironment->LoadResourceData(sCorrectionResourceName, CorrectionFileBuffer);

		pDriver->SetCorrectionFile(CorrectionFileBuffer, nTableIndex, nDimension, nTableNumberHeadA, nTableNumberHeadB);

		auto pContext = pDriver->GetContext();
		if (dCorrectionFactor > 0.0) {
			pContext->SetCorrectionFactors(dCorrectionFactor, dCorrectionFactor);
		}

		auto dLaserPowerAt0Percent = pStateEnvironment->GetDoubleParameter("powercalibration", "calibrationsetpoint0");
		auto dLaserPowerAt100Percent = pStateEnvironment->GetDoubleParameter("powercalibration", "calibrationsetpoint100");

		double dPulseHalfPeriodInMicroseconds = pStateEnvironment->GetDoubleParameter("lasersignaltiming", "pulsehalfperiod");
		double dPulseLengthInMicroseconds = pStateEnvironment->GetDoubleParameter("lasersignaltiming", "pulselength");
		double dStandbyHalfPeriodInMicroseconds = pStateEnvironment->GetDoubleParameter("lasersignaltiming", "standbyhalfperiod");
		double dStandbyLengthInMicroseconds = pStateEnvironment->GetDoubleParameter("lasersignaltiming", "standbylength");

		pStateEnvironment->LogMessage("Setting up laser fields and transformation.");

		double dBitsPerMM = pContext->GetCorrectionFactor();
		pStateEnvironment->LogMessage("Setting laser field for laser " + std::to_string(nRTCIndex) + ": " + std::to_string(dLaserFieldMinX) + "/" + std::to_string(dLaserFieldMinY) + " - " + std::to_string(dLaserFieldMaxX) + "/" + std::to_string(dLaserFieldMaxY));

		pStateEnvironment->LogMessage("  (in laser coordinates: " + std::to_string(dLaserFieldMinX - dLaserOriginX) + "/" + std::to_string(dLaserFieldMinY - dLaserOriginY) + " - " + std::to_string(dLaserFieldMaxX - dLaserOriginX) + "/" + std::to_string(dLaserFieldMaxY - dLaserOriginY));
		pStateEnvironment->LogMessage("Setting laser origin");
		pContext->SetLaserOrigin(dLaserOriginX, dLaserOriginY);

		pStateEnvironment->LogMessage("Setting laser field");
		// Don't set laser field for now because we have to take the rotation into account. The laser field will be set in the transformation matrix without range checking...
		//pContext->SetLaserField(dLaserFieldMinX - dLaserOriginX, dLaserFieldMinY - dLaserOriginY, dLaserFieldMaxX - dLaserOriginX, dLaserFieldMaxY - dLaserOriginY);

		//pStateEnvironment->LogMessage("Enable Range Checking");
		//pContext->EnableRangeChecking();

		double dAngleInRadian = dTransformationAngle / 180.0 * 3.14159265359;
		pStateEnvironment->LogMessage("Set Transformation Offset: " + std::to_string((int32_t)round(dTransformationOffsetX * dBitsPerMM)) + " / " + std::to_string((int32_t)round(dTransformationOffsetY * dBitsPerMM)));
		pContext->SetTransformationOffset((int32_t)round(dTransformationOffsetX * dBitsPerMM), (int32_t)round(dTransformationOffsetY * dBitsPerMM));


		double dM11 = dTransformationScaleX * cos(dAngleInRadian);
		double dM12 = -dTransformationScaleY * sin(dAngleInRadian);
		double dM21 = dTransformationScaleX * sin(dAngleInRadian);
		double dM22 = dTransformationScaleY * cos(dAngleInRadian);

		pStateEnvironment->LogMessage("Set Transformation Matrix: " + std::to_string(dM11) + " / " + std::to_string(dM12) + " / " + std::to_string(dM21) + " / " + std::to_string(dM22));
		pContext->SetTransformationMatrix(dM11, dM12, dM21, dM22);


		pStateEnvironment->LogMessage("Configuring laser...");
		pStateEnvironment->LogMessage("Setting Signal half period to " + std::to_string(dPulseHalfPeriodInMicroseconds) + "microseconds and pulse length " + std::to_string(dPulseLengthInMicroseconds));
		pStateEnvironment->LogMessage("Setting Standby half period to " + std::to_string(dStandbyHalfPeriodInMicroseconds) + "microseconds and pulse length " + std::to_string(dStandbyLengthInMicroseconds));

		pDriver->SetLaserSignalTimingDefaults(dPulseHalfPeriodInMicroseconds, dPulseLengthInMicroseconds, dStandbyHalfPeriodInMicroseconds, dStandbyLengthInMicroseconds);

		pDriver->ConfigureLaserMode(LibMCDriver_ScanLab::eLaserMode::CO2, LibMCDriver_ScanLab::eLaserPort::LaserPulseModulation, dLaserPowerAt100Percent, false, false, false, false, false, false);

		pStateEnvironment->LogMessage("Configuring delays...");
		pDriver->ConfigureDelays(dLaserOnDelay, dLaserOffDelay, dMarkDelay, dJumpDelay, dPolygonDelay);

		
		pStateEnvironment->LogMessage("Updating Laser Power Calibration table...");
		std::vector<LibMCDriver_ScanLab::sLaserCalibrationPoint> calibrationPoints;
		for (uint32_t nPercentage = 1; nPercentage <= 99; nPercentage++) {
			std::string sCalibrationParameterName = "calibrationsetpoint" + std::to_string(nPercentage);
			if (pStateEnvironment->HasParameter("powercalibration", sCalibrationParameterName)) {
				double dLaserPowerInWatts = pStateEnvironment->GetDoubleParameter("powercalibration", sCalibrationParameterName);
				pStateEnvironment->LogMessage("Calibration Power: " + std::to_string(dLaserPowerInWatts) + " Watts = " + std::to_string(nPercentage) + "%");

				calibrationPoints.push_back({ (double)nPercentage, (double)dLaserPowerInWatts });
			}
		}

		pContext->SetPiecewiseLinearLaserPowerCalibration(dLaserPowerAt0Percent, dLaserPowerAt100Percent, calibrationPoints);
		

		for (double dWatts = 0; dWatts <= 18.0; dWatts += 0.5) {
			double dPercent = pContext->MapPowerWattsToPercent(dWatts);
			pStateEnvironment->LogMessage("Power: " + std::to_string(dWatts) + " Watts = " + std::to_string(dPercent) + "%");
		}
	}
	else {
		pStateEnvironment->LogMessage("Laser in simulation mode - skipping hardware initialisation");
	}
}


/*************************************************************************************************************************
 Class definition of CLaserState_Init
**************************************************************************************************************************/
// Standard resting state after power on. It does no work itself; initialisation is triggered
// by transitioning into "init" (on start-up, or after a fatal error has been cleared).
// The card settings are derived from main, which mirrors them from the machine configuration
// during its own init, so the laser must not initialise before main has applied it.
class CLaserState_WaitForInit : public virtual CLaserState {
public:
	CLaserState_WaitForInit(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "waitforinit";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		if (!pStateEnvironment->GetBoolParameter("cardconfig", "configurationapplied")) {
			pStateEnvironment->SetNextState("waitforinit");
			return;
		}

		pStateEnvironment->SetNextState("init");
	}
};


class CLaserState_Init : public virtual CLaserState {
public:
	CLaserState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "init";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Initializing laser subsystem...");

		// Discard any stale signals (e.g. a recovery request) left over from a previous run
		// or error before re-establishing a clean baseline.
		pStateEnvironment->ClearAllUnhandledSignals();

		std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
		if (sCardType == "scanlab") {
			InitialiseScanlabDriver(pStateEnvironment, m_pPluginData->acquireScanLabDriver(pStateEnvironment));
		}
		else {
			throw std::runtime_error("invalid scanner card type: " + sCardType);
		}

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CLaserState_Idle
**************************************************************************************************************************/
class CLaserState_Idle : public virtual CLaserState {
public:
	CLaserState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "idle";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->SetNextState("idle");

		std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
		if (sCardType == "scanlab") {
			auto pDriver = m_pPluginData->acquireScanLabDriver(pStateEnvironment);
			pDriver->QueryParameters();

			// Periodically read laser feedback pins
			bool bLaserOK = true;
			bool bTemperatureOK = true;
			uint32_t nLaserDigitalInput = 0;

			if (pDriver->IsSimulationMode()) {
				// In simulation mode, default to true
				bLaserOK = true;
				bTemperatureOK = true;
			}
			else {
				// Read actual pin values from RTC Context
				// DI0 is LaserOK, DI1 is TemperatureOK
				auto pContext = pDriver->GetContext();
				bLaserOK = pContext->ReadDigitalInputBit(0);
				bTemperatureOK = pContext->ReadDigitalInputBit(1);
				nLaserDigitalInput = pContext->ReadDigitalInputs();
			}

			// Store the values in the laserfeedback parameter group
			pStateEnvironment->SetBoolParameter("laserfeedback", "laserok", bLaserOK);
			pStateEnvironment->SetBoolParameter("laserfeedback", "temperatureok", bTemperatureOK);
			pStateEnvironment->SetIntegerParameter("laserfeedback", "laserdigitalinput", nLaserDigitalInput);
		}

		auto pExposureSignal = pStateEnvironment->GetUnhandledSignal("signal_exposure");
		if (pExposureSignal != nullptr) {
			pStateEnvironment->StoreSignal("exposuresignal", pExposureSignal);
			pStateEnvironment->SetNextState("exposure");
		}

		auto pLaserOnOffSignal = pStateEnvironment->GetUnhandledSignal("signal_changelaseronoff");
		if (pLaserOnOffSignal != nullptr) {
			pStateEnvironment->SetNextState("laserpowerchange");
		}
	}
};


/*************************************************************************************************************************
 Class definition of CLaserState_Exposure
**************************************************************************************************************************/
class CLaserState_Exposure : public virtual CLaserState {
public:
	CLaserState_Exposure(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "exposure";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto pSignalHandler = pStateEnvironment->RetrieveSignal("exposuresignal");

		pStateEnvironment->LogMessage("Exposure...");
		auto pBuildJob = pStateEnvironment->GetBuildJob(pSignalHandler->GetString("jobuuid"));
		auto nLayerIndex = (uint32_t)pSignalHandler->GetInteger("layerindex");
		auto nExposureTimeout = (uint32_t)pSignalHandler->GetInteger("timeout");

		pStateEnvironment->LogMessage("Exposing layer " + std::to_string(nLayerIndex) + " (timeout " + std::to_string(nExposureTimeout) + " ms)");

		std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
		if (sCardType == "scanlab") {
			auto pDriver = m_pPluginData->acquireScanLabDriver(pStateEnvironment);

			if (pDriver->IsSimulationMode()) {
				pStateEnvironment->LogMessage("Simulation mode delay of 2 seconds..");
				pStateEnvironment->Sleep(2000);
				pDriver->DrawLayer(pBuildJob->GetStorageUUID(), nLayerIndex);
			}
			else {
				// The laser enable pin is only high while the layer is drawn: with the pin
				// high the idle standby (tickle) pulses already make the laser emit.
				// DrawLayer blocks until the card has finished the list.
				auto pContext = pDriver->GetContext();
				auto nLeadTime = pStateEnvironment->GetIntegerParameter("lasersignaltiming", "enableleadtime");

				pContext->SetLaserPinOut(true, false);
				pStateEnvironment->SetBoolParameter("lasersignaltiming", "laseronsignal", true);
				try {
					if (nLeadTime > 0)
						pStateEnvironment->Sleep((uint32_t)nLeadTime);

					pDriver->DrawLayer(pBuildJob->GetStorageUUID(), nLayerIndex);
				}
				catch (...) {
					pContext->SetLaserPinOut(false, false);
					pStateEnvironment->SetBoolParameter("lasersignaltiming", "laseronsignal", false);
					throw;
				}
				pContext->SetLaserPinOut(false, false);
				pStateEnvironment->SetBoolParameter("lasersignaltiming", "laseronsignal", false);
			}
		}
		pSignalHandler->SetBoolResult("success", true);
		pSignalHandler->SignalHandled();

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CLaserState_LaserPowerChange
**************************************************************************************************************************/
class CLaserState_LaserPowerChange : public virtual CLaserState {
public:
	CLaserState_LaserPowerChange(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "laserpowerchange";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto pLaserOnOffSignal = pStateEnvironment->GetUnhandledSignal("signal_changelaseronoff");
		if (pLaserOnOffSignal == nullptr)
			throw std::runtime_error("could not find laser on/off signal");

		bool bLaserOn = pLaserOnOffSignal->GetBool("laseron");
		pStateEnvironment->LogMessage("Changing laser on/off state to " + std::string((bLaserOn) ? "ON" : "OFF"));

		pStateEnvironment->SetBoolParameter("lasersignaltiming", "laseronsignal", bLaserOn);

		std::string sCardType = pStateEnvironment->GetStringParameter("cardconfig", "cardtype");
		if (sCardType == "scanlab") {
			auto pRTCDriver = m_pPluginData->acquireScanLabDriver(pStateEnvironment);

			if (pRTCDriver->IsSimulationMode()) {
				pStateEnvironment->LogMessage("Simulation mode delay of 1 seconds..");
				pStateEnvironment->Sleep(1000);
			}
			else {
				auto pContext = pRTCDriver->GetContext();
				pContext->SetLaserPinOut(bLaserOn, false);
			}
		}

		pLaserOnOffSignal->SetBoolResult("success", true);
		pLaserOnOffSignal->SignalHandled();

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CLaserState_FatalError
**************************************************************************************************************************/
class CLaserState_FatalError : public virtual CLaserState {
public:
	CLaserState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData) {
	}

	static const std::string getStateName() {
		return "fatalerror";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override {
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		// Stay in the fatal-error state until an external recovery request is received, then
		// go back to "waitforinit" so the machine re-runs its full initialisation sequence.
		auto pSignal = pStateEnvironment->ClaimSignalFromQueue("signal_initialize_again");
		if (pSignal) {
			pSignal->SetBoolResult("success", true);
			pSignal->SignalHandled();

			pStateEnvironment->LogMessage("Laser recovery requested, re-initializing...");
			pStateEnvironment->SetNextState("waitforinit");
		}
		else {
			pStateEnvironment->SetNextState("fatalerror");
		}
	}
};


/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName) {
	m_pPluginData = std::make_shared<CLaserData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName) {
	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CLaserState_WaitForInit>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CLaserState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CLaserState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CLaserState_Exposure>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CLaserState_LaserPowerChange>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;
	if (createStateInstanceByName<CLaserState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME, "invalid state name: " + sStateName);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
