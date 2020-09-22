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

#include <chrono>
#include <sstream>
#include <cmath>

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
 Class definition of CPrinterConnectionData
**************************************************************************************************************************/
class CPrinterConnectionData : public virtual CPluginData {	
private:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_Marlin m_DriverCast_Marlin;

public:

	PDriver_Marlin acquireDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_Marlin.acquireDriver(pStateEnvironment, "marlin");
	}



	void updatePositionStateFromDriver(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment)
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

		pDriver->UpdatePositionState();
		pDriver->GetCurrentPosition(dCurrentX, dCurrentY, dCurrentZ);
		pDriver->GetTargetPosition(dTargetX, dTargetY, dTargetZ);

		pStateEnvironment->SetDoubleParameter("printerstate", "targetx", dTargetX);
		pStateEnvironment->SetDoubleParameter("printerstate", "targety", dTargetY);
		pStateEnvironment->SetDoubleParameter("printerstate", "targetz", dTargetZ);
		pStateEnvironment->SetDoubleParameter("printerstate", "currentx", dCurrentX);
		pStateEnvironment->SetDoubleParameter("printerstate", "currenty", dCurrentY);
		pStateEnvironment->SetDoubleParameter("printerstate", "currentz", dCurrentZ);
		pStateEnvironment->SetBoolParameter("printerstate", "ishomed", pDriver->IsHomed());
		pStateEnvironment->SetBoolParameter("printerstate", "ismoving", pDriver->IsMoving());
		pStateEnvironment->SetBoolParameter("printerstate", "isconnected", pDriver->IsConnected());
		pStateEnvironment->SetBoolParameter("printerstate", "bufferavailable", pDriver->CanExecuteMovement());
	}

	void updatePIDValues(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_setpidvalues", 0, pSignalHandler)) {
			double dP = pSignalHandler->GetDouble("dp");
			double dI = pSignalHandler->GetDouble("di");
			double dD = pSignalHandler->GetDouble("dd");

			pStateEnvironment->LogMessage("Setting PID Value to P: " + std::to_string (dP) + " I: " + std::to_string(dI) + "D: " + std::to_string(dD));
			pDriver->SetPidParameters (dP, dI, dD);

			pSignalHandler->SetBoolResult("success", true);
			pSignalHandler->SignalHandled();

			pDriver->GetPidParameters(dP, dI, dD);
			pStateEnvironment->SetDoubleParameter("printerstate", "pidvaluep", dP);
			pStateEnvironment->SetDoubleParameter("printerstate", "pidvaluei", dI);
			pStateEnvironment->SetDoubleParameter("printerstate", "pidvalued", dD);
		}

	}

	void updateTemperatureValues(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_settemperature", 0, pSignalHandler)) {
			bool bBedSetValue = pSignalHandler->GetDouble("bedsetvalue");
			double dBedTemperature = pSignalHandler->GetDouble("bedtemperature");
			bool bBedDoWait = pSignalHandler->GetDouble("beddowait");

			bool bExtruderSetValue = pSignalHandler->GetDouble("extrudersetvalue");
			uint32_t nExtruderId = pSignalHandler->GetInteger("extruderid");
			double dExtruderTemperature = pSignalHandler->GetDouble("extrudertemperature");
			bool bExtruderDoWait = pSignalHandler->GetDouble("extruderdowait");

			// store read value => to be used in other state/function of main state machine (handleSignals)
			pStateEnvironment->StoreInteger("extruderid", nExtruderId);

			if (bBedSetValue) {
				pStateEnvironment->LogMessage("Setting bed temperature value to " + std::to_string(dBedTemperature));
				pDriver->SetHeatedBedTargetTemperature(dBedTemperature, bBedDoWait);
			}

			if (bExtruderSetValue) {
				pStateEnvironment->LogMessage("Setting temperature value of extruder: " + std::to_string(nExtruderId) + " to " + std::to_string(dExtruderTemperature));
				pDriver->SetExtruderTargetTemperature(nExtruderId, dExtruderTemperature, bExtruderDoWait);
			}

			pSignalHandler->SetBoolResult("success", true);
			pSignalHandler->SignalHandled();
		}

	}

	void updateFanSpeed(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_setfanspeed", 0, pSignalHandler)) {
			uint32_t nFanId = pSignalHandler->GetInteger("fanid");
			double dFanSpeed = pSignalHandler->GetDouble("fanspeed");
			pStateEnvironment->LogMessage("Setting speed value of fan: " + std::to_string(nFanId) + " to " + std::to_string(dFanSpeed));
			pDriver->SetFanSpeed(nFanId, dFanSpeed);
			pSignalHandler->SetBoolResult("success", true);
			pSignalHandler->SignalHandled();
		}

	}

	void handleSignals(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment, bool bDoUpdatePIDValues, bool bDoUpdateTemperatureValues, bool bDoReportTemperatureValues, bool bDoUpdateFanValue)
	{

		if (pDriver.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		if (bDoUpdatePIDValues) {
			// read signal "signal_setpidvalues" and update driver with new PID values
			updatePIDValues(pDriver, pStateEnvironment);
		}
		if (bDoUpdateTemperatureValues) {
			// read signal "signal_settemperature" and update driver with new temperature values
			updateTemperatureValues(pDriver, pStateEnvironment);
		}
		if (bDoUpdateFanValue) {
			// read signal "signal_setfanspeed" and update driver with new fan speed values
			updateFanSpeed(pDriver, pStateEnvironment);
		}
		
		auto nExtruderId = pStateEnvironment->RetrieveInteger("extruderid");
		if (bDoReportTemperatureValues && (nExtruderId > -1)) {
			auto nExtruderId = pStateEnvironment->RetrieveInteger("extruderid");
			pDriver->UpdateTemperatureState(nExtruderId);
			// read current temperature values from plugin member (previously set in pDriver->UpdateState in updatePositionStateFromDriver)
			double dCurrentExtruderTemperature;
			double dCurrentBedTemperature;
			pDriver->GetExtruderCurrentTemperature(nExtruderId, dCurrentExtruderTemperature);
			pDriver->GetHeatedBedCurrentTemperature( dCurrentBedTemperature);

			// send current temperature as signal => to be interpreted by "main" 
			auto pSignal = pStateEnvironment->PrepareSignal("main", "signal_gettemperature");
			if (pSignal->CanTrigger()) {
				pSignal->SetBool("bedgetvalue", true);
				pSignal->SetDouble("bedtemperature", dCurrentBedTemperature);
				pSignal->SetBool("extrudergetvalue", true);
				pSignal->SetInteger("extruderid", nExtruderId);
				pSignal->SetDouble("extrudertemperature", dCurrentExtruderTemperature);
				pSignal->Trigger();
			}
		}

		LibMCEnv::PSignalHandler pSignalHandler;
		if (pStateEnvironment->WaitForSignal("signal_disconnect", 0, pSignalHandler)) {
			// and then disconnect
			pDriver->Disconnect();
			pSignalHandler->SetBoolResult("success", true);
			pSignalHandler->SignalHandled();
			pStateEnvironment->LogMessage("Printer disconnected.");
		}
		
		if (pStateEnvironment->WaitForSignal("signal_emergencystop", 0, pSignalHandler)) {
			pDriver->EmergencyStop();
			pSignalHandler->SetBoolResult("success", true);
			pSignalHandler->SignalHandled();
			pStateEnvironment->LogWarning("EMERGENCY STOP!");
		}
		if (pStateEnvironment->WaitForSignal("signal_dohoming", 0, pSignalHandler)) {
			if (!pDriver->IsHomed()) {
				pStateEnvironment->LogMessage("Do Homing.");
				pDriver->StartHoming();
			}
			pSignalHandler->SetBoolResult("success", true);
			pSignalHandler->SignalHandled();
		}
		
		if (pStateEnvironment->WaitForSignal("signal_doextruderinit", 0, pSignalHandler)) {
			if (pDriver->IsHomed()) {
				pStateEnvironment->LogMessage("Initialize extruder.");
				// move to a save pos
				pDriver->MoveFastToZ(10.0, 50);
				pDriver->MoveFastToXY(0.0, 0.0, 50);

				// zero extruded length
				pDriver->SetAxisPosition("E", 0.0);
				// extrude 3mm of feed stock
				pDriver->ExtruderDoExtrude(3.0, 5);
				// zero extruded length again
				pDriver->SetAxisPosition("E", 0.0);
				// store reset value
				pStateEnvironment->StoreDouble("ExtrudeValue", 0.0);
				pSignalHandler->SetBoolResult("success", true);
			}
			else {
				pSignalHandler->SetBoolResult("success", false);
			}
			pSignalHandler->SignalHandled();
		}
		
		if (pStateEnvironment->WaitForSignal("signal_dofinalizeextrude", 0, pSignalHandler)) {
			if (pDriver->IsHomed()) {
				pStateEnvironment->LogMessage("Finalize extrude.");

				// retract filament (relative)
				pDriver->SetAbsoluteExtrusion(false);
				pDriver->ExtruderDoExtrude(-1.0, 5);
				pDriver->SetAbsoluteExtrusion(true);
				// move to a save pos (Z relative, x/y absolute)
				pDriver->SetAbsolutePositioning(false);
				pDriver->MoveFastToZ(5.0, 50);
				pDriver->SetAbsolutePositioning(true);
				pDriver->MoveFastToXY(0.0, 0.0, 50);

				// steppers off
				pDriver->StopIdleHold();
				// power off
				pDriver->PowerOff();
				pSignalHandler->SetBoolResult("success", true);
			}
			else {
				pSignalHandler->SetBoolResult("success", false);
			}
			pSignalHandler->SignalHandled();
		}

		if (pStateEnvironment->WaitForSignal("signal_retractfilament", 0, pSignalHandler)) {
			if (pDriver->IsHomed()) {
				pStateEnvironment->LogMessage("Retract filament.");

				// retract filament (relatively by -0.5)
				double dE = pStateEnvironment->RetrieveDouble("ExtrudeValue");
				pDriver->ExtruderDoExtrude(dE - 0.5, 30);
				pSignalHandler->SetBoolResult("success", true);
			}
			else {
				pSignalHandler->SetBoolResult("success", false);
			}
			pSignalHandler->SignalHandled();
		}

		if (pStateEnvironment->WaitForSignal("signal_restorefilament", 0, pSignalHandler)) {
			if (pDriver->IsHomed()) {
				pStateEnvironment->LogMessage("Restore filament.");

				// restore filament to internally stored absolute E axis value ("ExtrudeValue")
				double dE = pStateEnvironment->RetrieveDouble("ExtrudeValue");
				pDriver->ExtruderDoExtrude(dE, 30);
				pSignalHandler->SetBoolResult("success", true);
			}
			else {
				pSignalHandler->SetBoolResult("success", false);
			}
			pSignalHandler->SignalHandled();
		}
	}

};


/*************************************************************************************************************************
 Class definition of CPrinterConnectionState
**************************************************************************************************************************/
typedef CState<CPrinterConnectionData> CPrinterConnectionState;

/*************************************************************************************************************************
 Class definition of CPrinterConnectionState_Init
**************************************************************************************************************************/
class CPrinterConnectionState_Init : public virtual CPrinterConnectionState {
public:

	CPrinterConnectionState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CPrinterConnectionState(getStateName(), sStateName, pPluginData)
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
		// preset store value 
		pStateEnvironment->StoreInteger("extruderid", -1);

		auto sCOMPort = pStateEnvironment->GetStringParameter ("comdata", "port");
		auto nBaudRate = pStateEnvironment->GetIntegerParameter("comdata", "baudrate");
		auto nConnectTimeout = pStateEnvironment->GetIntegerParameter("comdata", "connecttimeout");
		auto dTimerInterval = pStateEnvironment->GetDoubleParameter("printerstate", "statusupdateinterval");

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		pDriver->Connect(sCOMPort, (uint32_t) nBaudRate, dTimerInterval, nConnectTimeout);

		LibMCEnv::PSignalHandler pSignalHandler;
		// wait until calling state machine asks for "isConnected" => report connect state, so calling state machine can proceed with next state
		if (pStateEnvironment->WaitForSignal("signal_isconnected", 0, pSignalHandler)) {

			if (pDriver->IsConnected()) {
				pSignalHandler->SetBoolResult("success", true);
				pSignalHandler->SignalHandled();

				pStateEnvironment->SetNextState("idle");
			}
			else {
				pSignalHandler->SetBoolResult("success", false);
				pSignalHandler->SignalHandled();

				pStateEnvironment->LogWarning("Printer not connected.");
				pStateEnvironment->SetNextState("fatalerror");
			}
		}
		else {
			pStateEnvironment->LogWarning("Printer connect timeout.");
			pStateEnvironment->SetNextState("fatalerror");
		}
	}

};



/*************************************************************************************************************************
 Class definition of CPrinterConnectionState_Idle
**************************************************************************************************************************/
class CPrinterConnectionState_Idle : public virtual CPrinterConnectionState {
public:

	CPrinterConnectionState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CPrinterConnectionState(getStateName(), sStateName, pPluginData)
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

		if (pDriver->IsConnected()) {
			m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
			m_pPluginData->handleSignals(pDriver, pStateEnvironment, false, true, true, true);
			if (pDriver->IsHomed() && pDriver->CanExecuteMovement()) {

				std::string sParameterData;

				LibMCEnv::PSignalHandler pSignalHandler;
				if (pStateEnvironment->WaitForSignal("signal_doextrudelayer", 100, pSignalHandler)) {

					pStateEnvironment->StoreSignal("globalsignal_doextrudelayer", pSignalHandler.get());
					pStateEnvironment->SetNextState("doextrudelayer");

				}
				else {
					pStateEnvironment->SetNextState("idle");
				}
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
 Class definition of CPrinterConnectionState_FatalError
**************************************************************************************************************************/
class CPrinterConnectionState_FatalError : public virtual CPrinterConnectionState {
public:

	CPrinterConnectionState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CPrinterConnectionState(getStateName(), sStateName, pPluginData)
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
 Class definition of CPrinterConnectionState_DoExtrudeLayer
**************************************************************************************************************************/
class CPrinterConnectionState_DoExtrudeLayer : public virtual CPrinterConnectionState {
private:

	bool timeElapsed(int64_t nLayerTimeout, std::chrono::high_resolution_clock::time_point tStart) {
		auto tEnd = std::chrono::high_resolution_clock::now();
		if ((std::chrono::duration_cast<std::chrono::milliseconds> (tEnd -tStart).count() > nLayerTimeout)) {
			return true;
		}
		else {
			return false;
		}
	}

	bool canExecuteMovement(LibMCEnv::PStateEnvironment pStateEnvironment, double dStatusUpdateInterval, PDriver_Marlin pDriver, int64_t nLayerTimeout, std::chrono::high_resolution_clock::time_point tStart)
	{
		bool bSucces = true;
		while (!pDriver->CanExecuteMovement()) {
			if (dStatusUpdateInterval < 0.0)
				dStatusUpdateInterval = 0.0;

			pStateEnvironment->Sleep((uint32_t) dStatusUpdateInterval);
			// call updatePositionStateFromDriver to update current planner buffer state (needed by CanExecuteMovement)
			m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
			m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);
			if (timeElapsed(nLayerTimeout, tStart)) {
				bSucces = false;
				break;
			}
		}
		return bSucces;
	}

public:
	
	CPrinterConnectionState_DoExtrudeLayer(const std::string& sStateName, PPluginData pPluginData)
		: CPrinterConnectionState(getStateName(), sStateName, pPluginData)
	{

	}

	static const std::string getStateName()
	{
		return "doextrudelayer";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);
		bool bSucces = true;
		std::stringstream sNoSuccessMsg;

		double dStatusUpdateInterval = pStateEnvironment->GetDoubleParameter("printerstate", "statusupdateinterval");

		auto pSignal = pStateEnvironment->RetrieveSignal("globalsignal_doextrudelayer");
		auto sJobUUID = pSignal->GetString("jobuuid");
		auto nLayerIndex = pSignal->GetInteger("layerindex");
		auto nLayerTimeout = pSignal->GetInteger("layertimeout");

		auto pToolpathAccessor = pStateEnvironment->GetBuildJob(sJobUUID)->CreateToolpathAccessor();
		auto pLayer = pToolpathAccessor->LoadLayer((uint32_t)nLayerIndex);
		auto dUnit = pLayer->GetUnits();
		auto dLayerZ = pLayer->GetZValue();
		double dZ = dLayerZ * dUnit;

		// TODO set speed values by reading data from toolptah... uncomment if data is available
		//const double dSpeedMmPerSecond = pLayer->GetSegmentProfileTypedValue(LibMCEnv::eToolpathProfileValueType::Speed);
		//const double dSpeedFastMmPerSecond = pLayer->GetSegmentProfileTypedValue(LibMCEnv::eToolpathProfileValueType::JumpSpeed);
		//const double dExtrusionFactor = pLayer->GetSegmentProfileTypedValue(LibMCEnv::eToolpathProfileValueType::ExtrusionFactor);
		const double dSpeedMmPerSecond = 20;
		const double dSpeedFastMmPerSecond = 50;
		const double dExtrusionFactor = 0.0166;
		double dE = pStateEnvironment->RetrieveDouble("ExtrudeValue");

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);

		// do initial move to read z value/layer height
		auto nSegmentCount = pLayer->GetSegmentCount();
		auto tStart = std::chrono::high_resolution_clock::now();
		if ((nSegmentCount > 0) && (dZ > 0)) {
			bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
			if (bSucces) {
				pDriver->MoveFastToZ(dZ, dSpeedFastMmPerSecond);
				m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
				m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);
			}
			else {
				sNoSuccessMsg << "Timeout while moving to layer height Z=" << dZ << " of layer " << nLayerIndex;
			}
		}
		
		for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {
			LibMCEnv::eToolpathSegmentType eSegmentType;
			uint32_t nPointCount;
			std::vector<LibMCEnv::sPosition2D> PointData;

			pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);
			pLayer->GetSegmentPointData(nSegmentIndex, PointData);
			
			if (eSegmentType != LibMCEnv::eToolpathSegmentType::Unknown && nPointCount > 0 && nPointCount == PointData.size() && dZ > 0.0) {
				// handle x/y coordinates, dependent on SegmentType, no z value provided
				// => printer head remains on z level of given layer, no retract movement handled
				switch (eSegmentType) {
				case LibMCEnv::eToolpathSegmentType::Hatch:
					if (!(nPointCount % 2)) {
						// even number draw hatch lines
						for (uint32_t i = 0; i < nPointCount; i+=2) {
							// move fast to first hatch coord (first of a pair of coord)
							bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
							if (bSucces) {
								pDriver->MoveFastToXY(PointData[i].m_Coordinates[0] * dUnit, PointData[i].m_Coordinates[1] * dUnit, dSpeedFastMmPerSecond);
								m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
								m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);

								// move to second hatch coord with extrusion (second of a pair of coord)
								bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
								if (bSucces) {
									auto dDistance = sqrt(
										pow(PointData[i + 1].m_Coordinates[0] - PointData[i].m_Coordinates[0], 2) +
										pow(PointData[i + 1].m_Coordinates[1] - PointData[i].m_Coordinates[1], 2)) * dUnit;
									dE = dE + dDistance * dExtrusionFactor;
									pDriver->MoveToXY(PointData[i + 1].m_Coordinates[0] * dUnit, PointData[i + (int)1].m_Coordinates[1] * dUnit, dE, dSpeedMmPerSecond);
									m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
									m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);
								}
								else {
									sNoSuccessMsg << "Timeout while extruding segment " << nSegmentIndex << " of type Hatch of layer " << nLayerIndex;
								}
							}
							else {
								sNoSuccessMsg << "Timeout while extruding segment " << nSegmentIndex << " of type Hatch of layer " << nLayerIndex;
							}
							if (!bSucces) {
								break;
							}
						}
					}
					else {
						// odd number => can't draw hatch lines
						bSucces = false;
						sNoSuccessMsg << "Segment " << nSegmentIndex << " of type Hatch  of layer " << nLayerIndex << " has not an odd number of coordinates";
					}
					break;

				case LibMCEnv::eToolpathSegmentType::Loop:
					// move fast to first point of loop
					bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
					if (bSucces) {
						pDriver->MoveFastToXY(PointData[0].m_Coordinates[0] * dUnit, PointData[0].m_Coordinates[1] * dUnit, dSpeedFastMmPerSecond);
						m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
						m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);

						for (uint32_t i = 1; i < nPointCount; i++) {
							// move to rest of points of loop (with extrusion)
							bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
							if (bSucces) {
								auto dDistance = sqrt(
									pow(PointData[i].m_Coordinates[0] - PointData[i - 1].m_Coordinates[0], 2) +
									pow(PointData[i].m_Coordinates[1] - PointData[i - 1].m_Coordinates[1], 2)) * dUnit;
								dE = dE + dDistance * dExtrusionFactor;
								pDriver->MoveToXY(PointData[i].m_Coordinates[0] * dUnit, PointData[i].m_Coordinates[1] * dUnit, dE, dSpeedMmPerSecond);
								m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
								m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);
							}
							else {
								sNoSuccessMsg << "Timeout while extruding segment " << nSegmentIndex << " of type Loop of layer " << nLayerIndex;
							}
							if (!bSucces) {
								break;
							}
						}
						// move from last point in list to first point to close the of loop (with extrusion)
						bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
						if (bSucces) {
							auto dDistance = sqrt(
								pow(PointData[0].m_Coordinates[0] - PointData[nPointCount - 1].m_Coordinates[0], 2) +
								pow(PointData[0].m_Coordinates[1] - PointData[nPointCount - 1].m_Coordinates[1], 2)) * dUnit;
							dE = dE + dDistance * dExtrusionFactor;
							pDriver->MoveToXY(PointData[0].m_Coordinates[0] * dUnit, PointData[0].m_Coordinates[1] * dUnit, dE, dSpeedMmPerSecond);
							m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
							m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);
						}
						else {
							sNoSuccessMsg << "Timeout while extruding segment " << nSegmentIndex << " of type Loop to close loop of layer " << nLayerIndex;
						}
					}
					else {
						sNoSuccessMsg << "Timeout while moving in segment " << nSegmentIndex << " of type Loop of layer " << nLayerIndex;
					}
					break;

				case LibMCEnv::eToolpathSegmentType::Polyline:
					// move fast to first point of Polyline
					bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
					if (bSucces) {
						pDriver->MoveFastToXY(PointData[0].m_Coordinates[0] * dUnit, PointData[0].m_Coordinates[1] * dUnit, dSpeedFastMmPerSecond);
						m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
						m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);


						for (uint32_t i = 1; i < nPointCount; i++) {
							// move to rest of points of polyline (with extrusion)
							bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
							if (bSucces) {
								auto dDistance = sqrt(
									pow(PointData[i].m_Coordinates[0] - PointData[i -1].m_Coordinates[0], 2) +
									pow(PointData[i].m_Coordinates[1] - PointData[i - 1].m_Coordinates[1], 2)) * dUnit;
								dE = dE + dDistance * dExtrusionFactor;
								pDriver->MoveToXY(PointData[i].m_Coordinates[0] * dUnit, PointData[i].m_Coordinates[1] * dUnit, dE, dSpeedMmPerSecond);
								m_pPluginData->updatePositionStateFromDriver(pDriver, pStateEnvironment);
								m_pPluginData->handleSignals(pDriver, pStateEnvironment, true, true, true, true);

							}
							else {
								sNoSuccessMsg << "Timeout while extruding segment " << nSegmentIndex << " of type Polyline of layer " << nLayerIndex;
							}
							if (!bSucces) {
								break;
							}
						}
					}
					else {
						sNoSuccessMsg << "Timeout while moving in segment " << nSegmentIndex << " of type Polyline of layer " << nLayerIndex;
					}
					break;
				}

			}
			if (!bSucces) {
				break;
			}
		}

		// save current extrusion value => to be available/used in next layer
		pStateEnvironment->StoreDouble("ExtrudeValue", dE);
		
		if (!bSucces) {
			pStateEnvironment->LogWarning("Extrude error: " + sNoSuccessMsg.str());
		}
		pSignal->SetBoolResult("success", bSucces);
		pSignal->SignalHandled();

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName)
{
	m_pPluginData = std::make_shared<CPrinterConnectionData>();
}


IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CPrinterConnectionState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPrinterConnectionState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPrinterConnectionState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CPrinterConnectionState_DoExtrudeLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}

