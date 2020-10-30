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

// TODO uncomment to activate camera driver
//#include "libmcdriver_camera_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
// TODO uncomment to activate camera driver
//typedef LibMCDriver_Camera::PDriver_RaspiCamera PDriver_RaspiCamera;
//typedef LibMCEnv::CDriverCast <LibMCDriver_Camera::CDriver_RaspiCamera, LibMCDriver_Camera::CWrapper> PDriverCast_RaspiCamera;

/*************************************************************************************************************************
 Class definition of CMainData
**************************************************************************************************************************/
class CMainData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	// TODO uncomment to activate camera driver
	//PDriverCast_RaspiCamera m_DriverCast_RaspiCamera;

public:

	// TODO uncomment to activate camera driver
	//PDriver_RaspiCamera acquireCameraDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	//{
	//	return m_DriverCast_RaspiCamera.acquireDriver(pStateEnvironment, "camera");
	//}

	bool deviceSetFanSpeed(LibMCEnv::PStateEnvironment pStateEnvironment, uint32_t nFanId, double dFanSpeed)
	{
		// set temperature without waiting => already heating while homing
		bool bSuccess = true;
		pStateEnvironment->LogMessage("Set fan speed. ");
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_setfanspeed");
		pSignal->SetInteger("fanid", nFanId);
		pSignal->SetDouble("fanspeed", dFanSpeed);
		pSignal->Trigger();

		if (pSignal->WaitForHandling(1000)) {
			bSuccess = pSignal->GetBoolResult("success");
			if (bSuccess) {
				pStateEnvironment->LogMessage("Fan speed set successful. ");
			}
			else {
				pStateEnvironment->LogWarning("Set fan speed failure. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Set fan speed timeout!");
			bSuccess = false;
		}
		return bSuccess;
	}

	bool deviceInitExtruder(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		// do extruder initialization 
		bool bSuccess = true;
		auto pSignalDoExtruderInit = pStateEnvironment->PrepareSignal("printerconnection", "signal_doextruderinit");
		pSignalDoExtruderInit->Trigger();

		if (pSignalDoExtruderInit->WaitForHandling((uint32_t)(5000))) {
			bSuccess = pSignalDoExtruderInit->GetBoolResult("success");
			if (bSuccess) {
				pStateEnvironment->LogMessage("Extruder initilalized. ");
			}
			else {
				pStateEnvironment->LogWarning("Extruder initialization error. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Extruder initialization timeout error. ");
			bSuccess = false;
		}
		return bSuccess;
	}

	bool deviceDoFinalizeExtrude(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		// send signal to finalize extrude process
		bool bSuccess = true;
		pStateEnvironment->LogMessage("Finalize extrude. ");
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_dofinalizeextrude");
		pSignal->Trigger();

		if (pSignal->WaitForHandling(5000)) {
			bSuccess = pSignal->GetBoolResult("success");
			if (bSuccess) {
				pStateEnvironment->LogMessage("Finalize extrude successful. ");
			}
			else {
				pStateEnvironment->LogWarning("Finalize extrude failure. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Finalize extrudetimeout!");
			bSuccess = false;
		}
		return bSuccess;
	}

	bool deviceSetTemperature(LibMCEnv::PStateEnvironment pStateEnvironment, uint32_t nExtruderId, double dExtruderTargetTemperature, double dBedTargetTemperature, bool bDoWait)
	{
		bool bSuccess = true;
		// set temperature without waiting => already heating while homing
		if (bDoWait) {
			pStateEnvironment->LogMessage("Set bed and extruder temperature and wait. ");
		}
		else {
			pStateEnvironment->LogMessage("Set bed and extruder temperature and do not wait. ");
		}
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
			bSuccess = pSignal->GetBoolResult("success");
			if (bSuccess) {
				pStateEnvironment->LogMessage("Temperature set successful. ");
			}
			else {
				pStateEnvironment->LogWarning("Set Temperature failure. ");
			}
		}
		else {
			pStateEnvironment->LogWarning("Set Temperature timeout!");
			bSuccess = false;
		}


		// wait for defined temperature, if defined
		if (bSuccess && bDoWait) {
			LibMCEnv::PSignalHandler pSignalGetTemperature;
			double dBedTemperature = -1;
			double dExtruderTemperature = -1;

			while ((dExtruderTemperature < dExtruderTargetTemperature) || (dBedTemperature < dBedTargetTemperature)) {
				if (pStateEnvironment->WaitForSignal("signal_gettemperature", 50, pSignalGetTemperature)) {
					bool bBedGetValue = pSignalGetTemperature->GetDouble("bedgetvalue");
					if (bBedGetValue) {
						dBedTemperature = pSignalGetTemperature->GetDouble("bedtemperature");
					}

					int32_t nExtruderId = -1;
					bool bExtruderGetValue = pSignalGetTemperature->GetDouble("extrudergetvalue");
					if (bExtruderGetValue) {
						nExtruderId = pSignalGetTemperature->GetDouble("extruderid");
						dExtruderTemperature = pSignalGetTemperature->GetDouble("extrudertemperature");
					}
					pSignalGetTemperature->SetBoolResult("success", true);
					pSignalGetTemperature->SignalHandled();

					pStateEnvironment->LogMessage("Wait for temperature: Eid" + std::to_string(nExtruderId) + "=" + std::to_string(dExtruderTemperature) + " B=" + std::to_string(dBedTemperature));
				}
				// TODO add timeout/cancellation of "Wait for temperature"
			}
		}
		return bSuccess;
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

		//auto pCameraDriver = m_pPluginData->acquireCameraDriver(pStateEnvironment);
		//pCameraDriver->Initialize("", 600, 400, LibMCDriver_Camera::eImagePixelFormat::RGB32);

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

		// TODO uncomment to activate PID control
		//auto pSignal = pStateEnvironment->PrepareSignal("pidcontrol_extruder", "signal_startcontrolling");
		//pSignal->Trigger();
		//pSignal->WaitForHandling(1000);
		
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);
		pStateEnvironment->SetBoolParameter("jobinfo", "autostart", false);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", true);

		// read timeout is for getting first response from printer (used in printer connection) => give main more time 
		auto nConnectTimeout = pStateEnvironment->GetIntegerParameter("comdata", "connecttimeout");
		auto pSignalConnected = pStateEnvironment->PrepareSignal("printerconnection", "signal_connect");
		pSignalConnected->Trigger();
		if (pSignalConnected->WaitForHandling((uint32_t)(nConnectTimeout * 10))) {

			if (pSignalConnected->GetBoolResult("success")) {
				pStateEnvironment->LogMessage("Printer connected. ");
				// Printer connected
				// 
				// read and store some values => to be used in other state of main state machine (HandleTemperature)
				auto nExtruderId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "extruderid");
				auto dExtruderTargetTemperature = pStateEnvironment->GetDoubleParameter("temperaturecontrol", "targetextrudertemperature");
				auto dBedTargetTemperature = pStateEnvironment->GetDoubleParameter("temperaturecontrol", "targetbedtemperature");

				auto nFanId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "fanid");
				auto dFanSpeed = pStateEnvironment->GetDoubleParameter("temperaturecontrol", "fanspeed");

				// set desired temperature and don't wait (in the meantime to printer could do homing, if necessary)
				if (m_pPluginData->deviceSetTemperature(pStateEnvironment, nExtruderId, dExtruderTargetTemperature, dBedTargetTemperature, false)) {
					// check if printer is homed and do homing if necessary
					auto pSignalDoHoming = pStateEnvironment->PrepareSignal("printerconnection", "signal_dohoming");
					pSignalDoHoming->Trigger();

					if (pSignalDoHoming->WaitForHandling((uint32_t)(20000))) {

						if (pSignalDoHoming->GetBoolResult("success")) {
							pStateEnvironment->LogMessage("Printer homed. ");

							// set desired temperature and don wait (in the meantime to printer could do homing, if necessary)
							if (m_pPluginData->deviceSetTemperature(pStateEnvironment, nExtruderId, dExtruderTargetTemperature, dBedTargetTemperature, true)) {

								// initialize extruder
								if (m_pPluginData->deviceInitExtruder(pStateEnvironment)) {

									// set fan speed
									if (m_pPluginData->deviceSetFanSpeed(pStateEnvironment, nFanId, dFanSpeed)) {
										// and now extrude layer
										pStateEnvironment->SetNextState("extrudelayer");
									}
								}
							}
						}
						else {
							pStateEnvironment->LogWarning("Printer not homed. ");
							pStateEnvironment->SetNextState("fatalerror");
						}
					}
				}
			}
			else {
				pStateEnvironment->LogWarning("Printer not connected");
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

		// finalize extrude/print process (retract...)
		m_pPluginData->deviceDoFinalizeExtrude(pStateEnvironment);
		// stop fan
		auto nFanId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "fanid");
		m_pPluginData->deviceSetFanSpeed(pStateEnvironment, nFanId, 0);
		// switch off heating
		auto nExtruderId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "extruderid");
		m_pPluginData->deviceSetTemperature(pStateEnvironment, nExtruderId, 0.0, 0.0, false);

		// TODO following commands disconnect automatically. maybe this should be done on demand
		auto pSignal = pStateEnvironment->PrepareSignal("printerconnection", "signal_disconnect");
		pSignal->Trigger();

		// Unload Toolpath from memory
		auto sJobUUID = pStateEnvironment->GetStringParameter("jobinfo", "jobuuid");
		pStateEnvironment->GetBuildJob(sJobUUID)->UnloadToolpath ();

		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);

		pStateEnvironment->LogMessage("Process finished");

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
		// TODO just for testing set  LayCount = to small value => print stops earlier !!!
		//nLayerCount = 5;

		
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
		//TODO uncomment to activate camera driver 
		//pBuild->AddBinaryData("image_layer_" + std::to_string(nCurrentLayer) + ".png", "image/png", Buffer);

		if (nCurrentLayer < (nLayerCount - 1)) {

			// proceed to next layer and wait for temperature
			pStateEnvironment->LogMessage("Advancing to layer #" + std::to_string(nCurrentLayer + 1) + "...");
			pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", nCurrentLayer + 1);

			pStateEnvironment->SetNextState("extrudelayer");

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

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
