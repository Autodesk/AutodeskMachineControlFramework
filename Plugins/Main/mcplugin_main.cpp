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

#include "libmcdriver_camera_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
typedef LibMCDriver_Camera::PDriver_RaspiCamera PDriver_RaspiCamera;
typedef LibMCEnv::CDriverCast <LibMCDriver_Camera::CDriver_RaspiCamera, LibMCDriver_Camera::CWrapper> PDriverCast_RaspiCamera;

/*************************************************************************************************************************
 Class definition of CMainData
**************************************************************************************************************************/
class CMainData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_RaspiCamera m_DriverCast_RaspiCamera;

public:

	PDriver_RaspiCamera acquireCameraDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_RaspiCamera.acquireDriver(pStateEnvironment, "camera");
	}

	void setFanSpeed(LibMCEnv::PStateEnvironment pStateEnvironment, uint32_t nFanId, double dFanSpeed)
	{
		// set temperature without waiting => already heating while homing
		pStateEnvironment->LogMessage("Set fan speed. ");
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_setfanspeed");
		pSignal->SetInteger("fanid", nFanId);
		pSignal->SetDouble("fanspeed", dFanSpeed);
		pSignal->Trigger();

		if (pSignal->WaitForHandling(1000)) {
			if (pSignal->GetBoolResult("success")) {
				pStateEnvironment->LogMessage("Fan speed set successful. ");
			}
			else {
				pStateEnvironment->LogWarning("Set fan speed failure. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Set fan speed timeout!");
		}
	}

	void doFinalizeExtrude(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		// send signal to finalize extrude process
		pStateEnvironment->LogMessage("Finalize extrude. ");
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_dofinalizeextrude");
		pSignal->Trigger();

		if (pSignal->WaitForHandling(5000)) {
			if (pSignal->GetBoolResult("success")) {
				pStateEnvironment->LogMessage("Finalize extrude successful. ");
			}
			else {
				pStateEnvironment->LogWarning("Finalize extrude failure. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Finalize extrudetimeout!");
		}
	}

	void setTemperatureWithoutWaiting(LibMCEnv::PStateEnvironment pStateEnvironment, uint32_t nExtruderId, double dExtruderTargetTemperature, double dBedTargetTemperature)
	{
		// set temperature without waiting => already heating while homing
		pStateEnvironment->LogMessage("Set bed and extruder temperature without waiting. ");
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_settemperature");
		pSignal->SetBool("bedsetvalue", true);
		pSignal->SetDouble("bedtemperature", dBedTargetTemperature);
		pSignal->SetBool("beddowait", false);
		pSignal->SetBool("extrudersetvalue", true);
		pSignal->SetInteger("extruderid", nExtruderId);
		pSignal->SetDouble("extrudertemperature", dExtruderTargetTemperature);
		pSignal->SetBool("extruderdowait", false);
		pSignal->Trigger();

		if (pSignal->WaitForHandling(1000)) {
			if (pSignal->GetBoolResult("success")) {
				pStateEnvironment->LogMessage("Temperature set successful. ");
			}
			else {
				pStateEnvironment->LogWarning("Set Temperature failure. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Set Temperature timeout!");
		}
	}

};

/*************************************************************************************************************************
 Class definition of CMainState
**************************************************************************************************************************/
typedef CState<CMainData> CMainState;




/*************************************************************************************************************************
 Class definition of CMainState_Init
**************************************************************************************************************************/
class CMainState_Init : public virtual CMainState {
public:

	CMainState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CMainState (getStateName (), sStateName, pPluginData)
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

		pStateEnvironment->LogMessage("Initializing...");

		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		pStateEnvironment->SetNextState("idle");
	}

};


/*************************************************************************************************************************
 Class definition of CMainState_Idle
**************************************************************************************************************************/

class CMainState_Idle : public virtual CMainState {
public:

	CMainState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
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


		LibMCEnv::PSignalHandler pHandlerInstance;
		if (pStateEnvironment->GetBoolParameter("jobinfo", "autostart")) {
			pStateEnvironment->SetNextState("startprocess");

		}
		else {
			if (pStateEnvironment->WaitForSignal("signal_startjob", 0, pHandlerInstance)) {

				pStateEnvironment->LogMessage("Starting job..");
				try {
					auto sJobName = pHandlerInstance->GetString("jobname");
					auto	 sJobUUID = pHandlerInstance->GetString("jobuuid");

					if (sJobName == "")
						throw std::runtime_error ("empty job name!");
					if (sJobName.length () > 64)
						throw std::runtime_error("invalid job name: " + sJobName);

					// Check if build job exists
					pStateEnvironment->GetBuildJob(sJobUUID);

					pStateEnvironment->SetStringParameter("jobinfo", "jobname", sJobName);
					pStateEnvironment->SetUUIDParameter("jobinfo", "jobuuid", sJobUUID);
					pHandlerInstance->SetBoolResult("success", true);

					pStateEnvironment->SetNextState("startprocess");
				}
				catch (std::exception& E) {
					pStateEnvironment->LogWarning(std::string ("Could not start job: ") + E.what ());
					pHandlerInstance->SetBoolResult("success", false);
					pStateEnvironment->SetNextState("idle");
				}

				pHandlerInstance->SignalHandled();

			}
			else {
				pStateEnvironment->SetNextState("idle");
			}
		}

	}

};



/*************************************************************************************************************************
 Class definition of CMainState_StartProcess
**************************************************************************************************************************/

class CMainState_StartProcess : public virtual CMainState {
public:

	CMainState_StartProcess(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "startprocess";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Starting process...");

		// Load Toolpath into memory
		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		auto pBuildJob = pStateEnvironment->GetBuildJob(sJobUUID);
		pBuildJob->LoadToolpath();

		// Find out layer count
		auto nLayerCount = pBuildJob->GetLayerCount();
		
		auto pSignal = pStateEnvironment->PrepareSignal("pidcontrol_extruder", "signal_startcontrolling");
		pSignal->Trigger();
		pSignal->WaitForHandling(1000);
		
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);
		pStateEnvironment->SetBoolParameter("jobinfo", "autostart", false);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", true);
		
		auto nExtruderId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "extruderid");
		auto dExtruderTargetTemperature = pStateEnvironment->GetDoubleParameter("temperaturecontrol", "targetextrudertemperature");
		auto dBedTargetTemperature = pStateEnvironment->GetDoubleParameter("temperaturecontrol", "targetbedtemperature");
		// store read values => to be used in other state of main state machine (waitfortemperature)
		pStateEnvironment->StoreInteger("extruderid", nExtruderId);
		pStateEnvironment->StoreDouble("extrudertargettemperature", dExtruderTargetTemperature);
		pStateEnvironment->StoreDouble("bedtargettemperature", dBedTargetTemperature);

		auto nFanId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "fanid");
		auto dFanSpeed = pStateEnvironment->GetDoubleParameter("temperaturecontrol", "fanspeed");
		// store read values => to be used in other state of main state machine (finish process)
		pStateEnvironment->StoreInteger("fanid", nFanId);
		pStateEnvironment->StoreDouble("fanspeed", dFanSpeed);

		auto pSignalIsConnected = pStateEnvironment->PrepareSignal("printerconnection", "signal_isconnected");
		pSignalIsConnected->Trigger();

		if (pSignalIsConnected->WaitForHandling((uint32_t)(5000))) {
			auto bSuccess = pSignalIsConnected->GetBoolResult("success");

			if (bSuccess) {
				pStateEnvironment->LogMessage("Printer connected. ");
				// set desired temperature and don't wait 
				m_pPluginData->setTemperatureWithoutWaiting(pStateEnvironment, nExtruderId, dExtruderTargetTemperature, dBedTargetTemperature);
				
				// check if printer is homed and do homing if necessary
				auto pSignalDoHoming = pStateEnvironment->PrepareSignal("printerconnection", "signal_dohoming");
				pSignalDoHoming->Trigger();

				if (pSignalDoHoming->WaitForHandling((uint32_t)(20000))) {
					bSuccess = pSignalDoHoming->GetBoolResult("success");

					if (bSuccess) {
						pStateEnvironment->LogMessage("Printer homed. ");

						// do some initialization stuff
						// set flag to tell state "waitfortemperature" to initialize extruder (will be done just once, while starting)
						pStateEnvironment->StoreBool("extruderdoinit", true);
						// set fan speed
						m_pPluginData->setFanSpeed(pStateEnvironment, nFanId, dFanSpeed);
						// and now wait for temperature
						pStateEnvironment->SetNextState("waitfortemperature");
					}
					else {
						pStateEnvironment->LogWarning("Printer not homed. ");
						pStateEnvironment->SetNextState("fatalerror");
					}
				}
			}
			else {
				pStateEnvironment->LogWarning("Printer not connected. ");
				pStateEnvironment->SetNextState("fatalerror");
			}
		}
		else {
			pStateEnvironment->LogWarning("Signal 'Printer is connected' timeout. ");
			pStateEnvironment->SetNextState("fatalerror");
		}


	}

};


/*************************************************************************************************************************
 Class definition of CMainState_WaitForTemperature
**************************************************************************************************************************/

class CMainState_WaitForTemperature : public virtual CMainState {
public:

	CMainState_WaitForTemperature(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "waitfortemperature";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Wait for temperature...");
		uint32_t nExtruderId = pStateEnvironment->RetrieveBool("extruderid");
		double dExtruderTargetTemperature = pStateEnvironment->RetrieveDouble("extrudertargettemperature");
		double dBedTargetTemperature = pStateEnvironment->RetrieveDouble("bedtargettemperature");
		
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_settemperature");
		pSignal->SetBool("bedsetvalue", true);
		pSignal->SetDouble("bedtemperature", dBedTargetTemperature);
		pSignal->SetBool("beddowait", false);
		pSignal->SetBool("extrudersetvalue", true);
		pSignal->SetInteger("extruderid", nExtruderId);
		pSignal->SetDouble("extrudertemperature", dExtruderTargetTemperature);
		pSignal->SetBool("extruderdowait", false);
		pSignal->Trigger();

		if (pSignal->WaitForHandling(10000)) {
			auto bSuccess = pSignal->GetBoolResult("success");

			// wait for defined temperature 
			LibMCEnv::PSignalHandler pSignalHandler;
			double dBedTemperature = -1;
			double dExtruderTemperature = -1;

			while ((dExtruderTemperature < dExtruderTargetTemperature) || (dBedTemperature < dBedTargetTemperature)) {
				if (pStateEnvironment->WaitForSignal("signal_gettemperature", 0, pSignalHandler)) {
					bool bBedGetValue = pSignalHandler->GetDouble("bedgetvalue");
					if (bBedGetValue) {
						dBedTemperature = pSignalHandler->GetDouble("bedtemperature");
					}
					uint32_t nExtruderId = -1;
					bool bExtruderGetValue = pSignalHandler->GetDouble("extrudergetvalue");
					if (bExtruderGetValue) {
						nExtruderId = pSignalHandler->GetDouble("extruderid");
						dExtruderTemperature = pSignalHandler->GetDouble("extrudertemperature");
					}
					pSignalHandler->SetBoolResult("success", true);
					pSignalHandler->SignalHandled();

					pStateEnvironment->LogMessage("Wait for temperature: Eid" + std::to_string(nExtruderId) + "=" + std::to_string(dExtruderTemperature) + " B=" + std::to_string(dBedTemperature));
				}
				pStateEnvironment->Sleep(250);

				// TODO add timeout/cancellation of "Wait for temperature"
			}

			if (bSuccess) {
				pStateEnvironment->LogMessage("Temperature set successful. ");

				bool bSuccessExtruderInit = true;
				// if called first time extruderdoinit should be set (by "startprocess") => initialize extruder
				if (pStateEnvironment->RetrieveBool("extruderdoinit")) {
					// do extruder initialization just once (at the beginning/first call) of "waitfortemperature" => reset flag
					pStateEnvironment->StoreBool("extruderdoinit", false);
					auto pSignalDoExtruderInit = pStateEnvironment->PrepareSignal("printerconnection", "signal_doextruderinit");
					pSignalDoExtruderInit->Trigger();

					if (pSignalDoExtruderInit->WaitForHandling((uint32_t)(5000))) {
						bSuccessExtruderInit = pSignalDoExtruderInit->GetBoolResult("success");
						if (bSuccessExtruderInit) {
							pStateEnvironment->LogMessage("Extruder initilalized. ");
						}
					}
					else {
						bSuccessExtruderInit = false;
					}
				}
				
				if (bSuccessExtruderInit) {
					// proceed to extrude current layer
					pStateEnvironment->SetNextState("extrudelayer");
				}
				else {
					pStateEnvironment->LogWarning("Extruder initialization error. ");
					pStateEnvironment->SetNextState("fatalerror");
				}
			}
			else {
				pStateEnvironment->LogWarning("Set Temperature failure. ");
				pStateEnvironment->SetNextState("fatalerror");
			}
		}
		else {
			pStateEnvironment->LogWarning("Set Temperature timeout!");
			pStateEnvironment->SetNextState("fatalerror");
		}
	}

};



/*************************************************************************************************************************
 Class definition of CMainState_FinishProcess
**************************************************************************************************************************/

class CMainState_FinishProcess : public virtual CMainState {
public:

	CMainState_FinishProcess(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "finishprocess";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Process finished");

		// Unload Toolpath from memory
		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		pStateEnvironment->GetBuildJob(sJobUUID)->UnloadToolpath ();

		// stop fan
		auto nFanId = pStateEnvironment->RetrieveInteger("fanid");
		m_pPluginData->setFanSpeed(pStateEnvironment, nFanId, 0);
		// switch off heating
		uint32_t nExtruderId = pStateEnvironment->RetrieveBool("extruderid");
		m_pPluginData->setTemperatureWithoutWaiting(pStateEnvironment, nExtruderId, 0.0, 0.0);
		
		// finalize extrude/print process (retract...)
		m_pPluginData->doFinalizeExtrude(pStateEnvironment);


		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		// TODO following commands disconnect automatically. maybe this should be done on demand
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_disconnect");
		pSignal->Trigger();
		
		pStateEnvironment->SetNextState("idle");

	}

};


/*************************************************************************************************************************
 Class definition of CMainState_ExtrudeLayer
**************************************************************************************************************************/

class CMainState_ExtrudeLayer : public virtual CMainState {
public:

	CMainState_ExtrudeLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "extrudelayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		auto nCurrentLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
		auto nLayerTimeoutGraceTime = pStateEnvironment->GetIntegerParameter("jobinfo", "layertimeoutgracetime");

		if (nLayerTimeoutGraceTime < 0)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		// send signal to restore filament to internally stored absolute E axis value ("ExtrudeValue") before extruding current layer
		// retract filament is done by send signal "signal_retractfilament" (see NextLayer)
		auto pSignalRestoreFilament = pStateEnvironment->PrepareSignal("printerconnection", "signal_restorefilament");
		pSignalRestoreFilament->Trigger();

		if (pSignalRestoreFilament->WaitForHandling((uint32_t)(2000))) {
			const auto bSuccess = pSignalRestoreFilament->GetBoolResult("success");

			if (bSuccess) {
				pStateEnvironment->LogMessage("Filament restored. ");
				// proceed to extrude layer 

				// TODO get/calc timeout (from layer length, given speed....)
				int64_t nLayerTimeout = 200000;

				pStateEnvironment->LogMessage("Extrude layer #" + std::to_string(nCurrentLayer) + "...");

				auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_doextrudelayer");
				pSignal->SetInteger("layertimeout", nLayerTimeout);
				pSignal->SetInteger("layerindex", nCurrentLayer);
				pSignal->SetString("jobuuid", sJobUUID);
				pSignal->Trigger();

				if (pSignal->WaitForHandling((uint32_t)(nLayerTimeout + nLayerTimeoutGraceTime))) {
					auto bSuccess = pSignal->GetBoolResult("success");

					if (bSuccess) {
						pStateEnvironment->LogMessage("Extruding success. ");
						pStateEnvironment->SetNextState("nextlayer");
					}
					else {
						pStateEnvironment->LogWarning("Extruding failure. ");
						pStateEnvironment->SetNextState("fatalerror");
					}
				}
				else {

					pStateEnvironment->LogWarning("Extruding timeout!");
					pStateEnvironment->SetNextState("fatalerror");
				}
			}
			else {
				pStateEnvironment->LogWarning("Filament restore error.");
				pStateEnvironment->SetNextState("fatalerror");
			}
		}
		else {
			pStateEnvironment->LogWarning("Filament restore timeout. ");
			pStateEnvironment->SetNextState("fatalerror");
		}
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_NextLayer
**************************************************************************************************************************/

class CMainState_NextLayer : public virtual CMainState {
public:

	CMainState_NextLayer(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "nextlayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		auto nCurrentLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
		auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");

		// TODO activate/uncomment following lines to test emergency stop when proceeding to layer 3
		//if (nCurrentLayer > 1) {
		//	pStateEnvironment->LogMessage("Just for testing. call EMERGENCY STOP if layer #" + std::to_string(nCurrentLayer + 1) + " is next layer");
		//	auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_emergencystop");
		//	pSignal->Trigger();
		//	pStateEnvironment->SetNextState("fatalerror");
		//}

		// TODO uncomment to activate camera driver pStateEnvironment->LogMessage("Getting Camera Image");
		//auto pCameraDriver = m_pPluginData->acquireCameraDriver(pStateEnvironment);
		//auto pPNGImage = pCameraDriver->CapturePNGImage();
		//
		//std::vector<uint8_t> Buffer;
		//pPNGImage->GetRawData(Buffer);

		auto pBuild = pStateEnvironment->GetBuildJob(sJobUUID);
		//TODO uncomment to activate camera driver pBuild->AddBinaryData("image_layer_" + std::to_string(nCurrentLayer) + ".png", "image/png", Buffer);

		if (nCurrentLayer < (nLayerCount - 1)) {
			// send signal to retract filament relatively before waiting for temperature (and extruding new layer)
			// restore filament is done by send signal "signal_restorefilament" (see ExtrudeLayer)
			auto pSignalRetractFilament = pStateEnvironment->PrepareSignal("printerconnection", "signal_retractfilament");
			pSignalRetractFilament->Trigger();

			if (pSignalRetractFilament->WaitForHandling((uint32_t)(2000))) {
				const auto bSuccess = pSignalRetractFilament->GetBoolResult("success");

				if (bSuccess) {
					pStateEnvironment->LogMessage("Filament retracted. ");
					// proceed to next layer and wait for temperature
					pStateEnvironment->LogMessage("Advancing to layer #" + std::to_string(nCurrentLayer + 1) + "...");
					pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", nCurrentLayer + 1);

					pStateEnvironment->SetNextState("waitfortemperature");
				}
				else {
					pStateEnvironment->LogWarning("Filament retract error.");
					pStateEnvironment->SetNextState("fatalerror");
				}
			}
			else {
				pStateEnvironment->LogWarning("Filament retract timeout. ");
				pStateEnvironment->SetNextState("fatalerror");
			}

		}
		else {
			pStateEnvironment->LogMessage("Finishing process...");
			pStateEnvironment->SetNextState("finishprocess");
		}

	}

};





/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CMainState_FatalError : public virtual CMainState {
public:

	CMainState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
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

		// Unload all toolpathes that might be in memory
		pStateEnvironment->UnloadAllToolpathes();

		pStateEnvironment->SetNextState("fatalerror");
	}

};



/*************************************************************************************************************************
 Class definition of CStateFactory 
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName)
{
	m_pPluginData = std::make_shared<CMainData>();
}

IState * CStateFactory::CreateState(const std::string & sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CMainState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_Idle>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_StartProcess>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_FinishProcess>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_ExtrudeLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_NextLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CMainState_WaitForTemperature>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
