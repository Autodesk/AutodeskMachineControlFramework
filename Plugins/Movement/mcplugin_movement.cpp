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
		} 

	}

	void updateStateFromDriver(PDriver_Marlin pDriver, LibMCEnv::PStateEnvironment pStateEnvironment, bool bDoUpdatePIDValues)
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
		double dCurrentExtruderTemperature;
		double dTargetExtruderTemperature;
		double dCurrentBedTemperature;
		double dTargetBedTemperature;
		double dP;
		double dI;
		double dD;

		if (bDoUpdatePIDValues) {
			updatePIDValues(pDriver, pStateEnvironment);
		}
		pDriver->UpdateState();
		pDriver->GetCurrentPosition(dCurrentX, dCurrentY, dCurrentZ);
		pDriver->GetTargetPosition(dTargetX, dTargetY, dTargetZ);
		pDriver->GetExtruderTemperature(0, dTargetExtruderTemperature, dCurrentExtruderTemperature);
		pDriver->GetHeatedBedTemperature(dTargetBedTemperature, dCurrentBedTemperature);
		pDriver->GetPidParameters(dP, dI, dD);

		pStateEnvironment->SetDoubleParameter("movementstate", "targetx", dTargetX);
		pStateEnvironment->SetDoubleParameter("movementstate", "targety", dTargetY);
		pStateEnvironment->SetDoubleParameter("movementstate", "targetz", dTargetZ);
		pStateEnvironment->SetDoubleParameter("movementstate", "currentx", dCurrentX);
		pStateEnvironment->SetDoubleParameter("movementstate", "currenty", dCurrentY);
		pStateEnvironment->SetDoubleParameter("movementstate", "currentz", dCurrentZ);
		pStateEnvironment->SetDoubleParameter("movementstate", "currenttemperatureextruder", dCurrentExtruderTemperature);
		pStateEnvironment->SetDoubleParameter("movementstate", "targettemperatureextruder", dTargetExtruderTemperature);
		pStateEnvironment->SetDoubleParameter("movementstate", "currenttemperaturebed", dCurrentBedTemperature);
		pStateEnvironment->SetDoubleParameter("movementstate", "targettemperaturebed", dTargetBedTemperature);
		pStateEnvironment->SetBoolParameter("movementstate", "ishomed", pDriver->IsHomed());
		pStateEnvironment->SetBoolParameter("movementstate", "ismoving", pDriver->IsMoving());
		pStateEnvironment->SetBoolParameter("movementstate", "bufferavailable", pDriver->CanExecuteMovement());
		pStateEnvironment->SetDoubleParameter("movementstate", "pidvaluep", dP);
		pStateEnvironment->SetDoubleParameter("movementstate", "pidvaluei", dI);
		pStateEnvironment->SetDoubleParameter("movementstate", "pidvalued", dD);
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
		auto dTimerInterval = pStateEnvironment->GetDoubleParameter("movementstate", "statusupdateinterval");

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		pDriver->Connect(sCOMPort, (uint32_t) nBaudRate, dTimerInterval);
		m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, false);

		if (!pDriver->IsHomed()) {
			pStateEnvironment->SetNextState("homing");
		}
		else {
			pStateEnvironment->SetNextState("idle");
		}

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
		
		m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, false);
		if (pDriver->CanExecuteMovement()) {

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
 Class definition of CMovementState_DoExtrudeLayer
**************************************************************************************************************************/
class CMovementState_DoExtrudeLayer : public virtual CMovementState {
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
			m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);
			if (timeElapsed(nLayerTimeout, tStart)) {
				bSucces = false;
				break;
			}
		}
		return bSucces;
	}

public:
	
	CMovementState_DoExtrudeLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
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
		
		double dStatusUpdateInterval = pStateEnvironment->GetDoubleParameter("movementstate", "statusupdateinterval");

		auto pSignal = pStateEnvironment->RetrieveSignal("globalsignal_doextrudelayer");
		auto sJobUUID = pSignal->GetString("jobuuid");
		auto nLayerIndex = pSignal->GetInteger("layerindex");
		auto nLayerTimeout = pSignal->GetInteger("layertimeout");

		auto pToolpathAccessor = pStateEnvironment->GetBuildJob(sJobUUID)->CreateToolpathAccessor();
		auto pLayer = pToolpathAccessor->LoadLayer((uint32_t)nLayerIndex);
		auto dUnit = pLayer->GetUnits();
		auto dLayerZ = pLayer->GetZValue();
		double dZ = dLayerZ * dUnit;

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		auto nSegmentCount = pLayer->GetSegmentCount();

		auto tStart = std::chrono::high_resolution_clock::now();

		// do initial move to read z value/layer height
		if ((nSegmentCount > 0) && (dZ > 0)) {
			bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
			if (bSucces) {
				// TODO check how to en/disable extrusion
				// TODO check how to set Speed/Feedrate
				pDriver->MoveToZ(dZ, 100.0);
				m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);
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
								// TODO check how to en/disable extrusion
								// TODO check how to set Speed/Feedrate
								pDriver->MoveFastToXY(PointData[i].m_Coordinates[0] * dUnit, PointData[i].m_Coordinates[1] * dUnit, 100.0);
								m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);

								// move to second hatch coord with extrusion (second of a pair of coord)
								bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
								if (bSucces) {
									// TODO check how to en/disable extrusion
									// TODO check how to set Speed/Feedrate
									pDriver->MoveToXY(PointData[i + 1].m_Coordinates[0] * dUnit, PointData[i + 1].m_Coordinates[1] * dUnit, 100.0);
									m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);
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
						// TODO check how to en/disable extrusion
						// TODO check how to set Speed/Feedrate
						pDriver->MoveFastToXY(PointData[0].m_Coordinates[0] * dUnit, PointData[0].m_Coordinates[1] * dUnit, 100.0);
						m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);


						for (uint32_t i = 1; i < nPointCount; i++) {
							// move to rest of points of loop (with extrusion)
							bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
							if (bSucces) {
								// TODO check how to en/disable extrusion
								// TODO check how to set Speed/Feedrate
								pDriver->MoveToXY(PointData[i].m_Coordinates[0] * dUnit, PointData[i].m_Coordinates[1] * dUnit, 100.0);
								m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);
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
							// TODO check how to en/disable extrusion
							// TODO check how to set Speed/Feedrate
							pDriver->MoveToXY(PointData[0].m_Coordinates[0] * dUnit, PointData[0].m_Coordinates[1] * dUnit, 100.0);
							m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);
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
						// TODO check how to en/disable extrusion
						// TODO check how to set Speed/Feedrate
						pDriver->MoveFastToXY(PointData[0].m_Coordinates[0] * dUnit, PointData[0].m_Coordinates[1] * dUnit, 100.0);
						m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);


						for (uint32_t i = 1; i < nPointCount; i++) {
							// move to rest of points of polyline (with extrusion)
							bSucces = canExecuteMovement(pStateEnvironment, dStatusUpdateInterval, pDriver, nLayerTimeout, tStart);
							if (bSucces) {
								// TODO check how to en/disable extrusion
								// TODO check how to set Speed/Feedrate
								pDriver->MoveToXY(PointData[i].m_Coordinates[0] * dUnit, PointData[i].m_Coordinates[1] * dUnit, 100.0);
								m_pPluginData->updateStateFromDriver(pDriver, pStateEnvironment, true);

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

		if (!bSucces) {
			pStateEnvironment->LogMessage("Extrude error: " + sNoSuccessMsg.str());
		}
		pSignal->SetBoolResult("success", bSucces);
		pSignal->SignalHandled();

		pStateEnvironment->SetNextState("idle");
	}

};



/*************************************************************************************************************************
 Class definition of CMovementState_Homing
**************************************************************************************************************************/
class CMovementState_Homing : public virtual CMovementState {
public:

	CMovementState_Homing(const std::string& sStateName, PPluginData pPluginData)
		: CMovementState(getStateName(), sStateName, pPluginData)
	{

	}

	static const std::string getStateName()
	{
		return "homing";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto pDriver = m_pPluginData->acquireDriver(pStateEnvironment);
		pDriver->StartHoming();

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

	if (createStateInstanceByName<CMovementState_DoExtrudeLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMovementState_Homing>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}

