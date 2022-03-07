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

// TODO uncomment to activate camera/MQTT driver
//#include "libmcdriver_camera_dynamic.hpp"
//#include "libmcdriver_mqtt_dynamic.hpp"
#include "libmcenv_drivercast.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
// TODO uncomment to activate camera/MQQTT driver
//typedef LibMCDriver_Camera::PDriver_RaspiCamera PDriver_RaspiCamera;
//typedef LibMCEnv::CDriverCast <LibMCDriver_Camera::CDriver_RaspiCamera, LibMCDriver_Camera::CWrapper> PDriverCast_RaspiCamera;
//typedef LibMCDriver_MQTT::PDriver_MQTT PDriver_MQTT;
//typedef LibMCEnv::CDriverCast <LibMCDriver_MQTT::CDriver_MQTT, LibMCDriver_MQTT::CWrapper> PDriverCast_MQTT;

/*************************************************************************************************************************
 Class definition of CMainData
**************************************************************************************************************************/
class CMainData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	// TODO uncomment to activate camera/MQTT driver
	//PDriverCast_RaspiCamera m_DriverCast_RaspiCamera;
//	PDriverCast_MQTT m_DriverCast_MQTT;

public:

	// TODO uncomment to activate camera/MQTT driver
	//PDriver_RaspiCamera acquireCameraDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	//{
	//	return m_DriverCast_RaspiCamera.acquireDriver(pStateEnvironment, "camera");
	//}
	//PDriver_MQTT acquireMQTTDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	//{
	//	return m_DriverCast_MQTT.acquireDriver(pStateEnvironment, "mqtt");
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
					dBedTemperature = pSignalGetTemperature->GetDouble("bedtemperature");
					dExtruderTemperature = pSignalGetTemperature->GetDouble("extrudertemperature");

					pSignalGetTemperature->SetBoolResult("success", true);
					pSignalGetTemperature->SignalHandled();

					//pStateEnvironment->LogMessage("Wait for temperature: Eid" + std::to_string(nExtruderId) + "=" + std::to_string(dExtruderTemperature) + " B=" + std::to_string(dBedTemperature));
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

		// TODO uncomment to activate camera/MQTT driver
		//pStateEnvironment->LogMessage("Establishing MQTT Connection...");
		//auto pMQTTDriver = m_pPluginData->acquireMQTTDriver(pStateEnvironment);
		//pMQTTDriver->Connect();
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

		// TODO activate MQTT sending...
		//auto pMQTTDriver = m_pPluginData->acquireMQTTDriver(pStateEnvironment);
		//pMQTTDriver->SendMQTTMessage ("TestMessage for testing :-) ");
		//pStateEnvironment->LogMessage("SendMQTTMessage done");


		LibMCEnv::PSignalHandler pHandlerInstance;
		if (pStateEnvironment->GetBoolParameter("jobinfo", "autostart")) {
			pStateEnvironment->SetNextState("startprocess");

		}
		else {
			if (pStateEnvironment->WaitForSignal("signal_startjob", 0, pHandlerInstance)) {

				pStateEnvironment->LogMessage("Starting job..");
				try {
					auto sJobUUID = pHandlerInstance->GetString("jobuuid");

					if (sJobUUID.empty())
						throw std::runtime_error ("empty job UUID!");

					// Check if build job exists
					pStateEnvironment->GetBuildJob(sJobUUID);

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
				LibMCEnv::PSignalHandler pSignalHandler;
				if (pStateEnvironment->WaitForSignal("signal_resetfatalerror", 0, pSignalHandler)) {
					// in state idle nothing to do if signal resetfatalerror received
					pSignalHandler->SetBoolResult("success", true);
					pSignalHandler->SignalHandled();
				}
				if (pStateEnvironment->WaitForSignal("signal_cleartemperatureandfan", 0, pSignalHandler)) {
					// reset bed/extruder temperature and fan speed
					auto nFanId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "fanid");
					m_pPluginData->deviceSetFanSpeed(pStateEnvironment, nFanId, 0);
					// switch off heating
					auto nExtruderId = pStateEnvironment->GetIntegerParameter("temperaturecontrol", "extruderid");
					m_pPluginData->deviceSetTemperature(pStateEnvironment, nExtruderId, 0.0, 0.0, false);

					pSignalHandler->SetBoolResult("success", true);
					pSignalHandler->SignalHandled();
				}

				pStateEnvironment->SetNextState("idle");
			}
		}

	}

};



/*************************************************************************************************************************
 Class definition of CMainState_StartProcess
**************************************************************************************************************************/

class CMainState_StartProcess : public virtual CMainState {
private:
	struct OperationSetup {   
		std::string operationId;
		std::string operationName;
		std::string setupId;
		std::string setupName;
	};
	
public:

	CMainState_StartProcess(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "startprocess";
	}


	void read3mfMetaData(LibMCEnv::PStateEnvironment pStateEnvironment, std::string sJobUUID)
	{
		const std::string s3mfNamespace = "http://autodesk.com/fusion360/amcf/";
		std::string sAppVersion = "";
		std::string sAppUserName = "";
		std::string sAppUserId = "";
		std::string sProjectName = "";
		std::string sProjectId = "";
		std::string sDataFileName = "";
		std::string sDataFileId = "";
		std::string sDataFileVersionId = "";
		std::string sDataFileVersionNr = "";
		std::string sDocName = "";
		std::string sDocVersion = "";
		std::string sDocId = "";
		std::string sMachineName = "";
		std::string sMachineUUID = "";
		std::vector<OperationSetup> sOperationSetups;

		std::string sBedTemp = "";
		std::string sExt1Temp = "";

		auto pToolpathAccessor = pStateEnvironment->GetBuildJob(sJobUUID)->CreateToolpathAccessor();
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "ApplicationVersion")) {
			sAppVersion = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "ApplicationVersion");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "ApplicationUserName")) {
			sAppUserName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "ApplicationUserName");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "ApplicationUserId")) {
			sAppUserId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "ApplicationUserId");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "ProjectName")) {
			sProjectName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "ProjectName");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "ProjectId")) {
			sProjectId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "ProjectId");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "DataFileName")) {
			sDataFileName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "DataFileName");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "DataFileId")) {
			sDataFileId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "DataFileId");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "DataFileVersionId")) {
			sDataFileVersionId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "DataFileVersionId");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "DataFileVersionNr")) {
			sDataFileVersionNr = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "DataFileVersionNr");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "DocumentName")) {
			sDocName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "DocumentName");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "DocumentVersion")) {
			sDocVersion = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "DocumentVersion");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "PostProcessorParameter-document-id")) {
			sDocId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "PostProcessorParameter-document-id");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "MachineName")) {
			sMachineName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "MachineName");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "MachineUUID")) {
			sMachineUUID = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "MachineUUID");
		}
		int opIndex = 0;
		bool doLoop = true;
		while (doLoop) {
			if (pToolpathAccessor->HasMetaData(s3mfNamespace, "OperationId" + std::to_string(opIndex))) {
				OperationSetup opSetup;
				opSetup.operationId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "OperationId" + std::to_string(opIndex));
				if (pToolpathAccessor->HasMetaData(s3mfNamespace, "OperationName" + std::to_string(opIndex))) {
					opSetup.operationName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "OperationName" + std::to_string(opIndex));
				}
				if (pToolpathAccessor->HasMetaData(s3mfNamespace, "SetupId" + std::to_string(opIndex))) {
					opSetup.setupId = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "SetupId" + std::to_string(opIndex));
				}
				if (pToolpathAccessor->HasMetaData(s3mfNamespace, "setupName" + std::to_string(opIndex))) {
					opSetup.setupName = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "setupName" + std::to_string(opIndex));
				}
				sOperationSetups.push_back(opSetup);
			
				opIndex++;
				// TODO HasMetaData doesn't work => as a workaround, do loop just once => remove following line when HasMetaData works as expected
				doLoop = false;
			}
			else {
				doLoop = false;
			}
		}

		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "PostProcessorParameter-bed-temp")) {
			sBedTemp = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "PostProcessorParameter-bed-temp");
		}
		if (pToolpathAccessor->HasMetaData(s3mfNamespace, "PostProcessorParameter-ext1-temp")) {
			sExt1Temp = pToolpathAccessor->GetMetaDataValue(s3mfNamespace, "PostProcessorParameter-ext1-temp");
		}

		pStateEnvironment->LogMessage("AppVersion: " + sAppVersion);
		pStateEnvironment->LogMessage("AppUserName: " + sAppUserName);
		pStateEnvironment->LogMessage("AppUserId: " + sAppUserId);
		pStateEnvironment->LogMessage("ProjectName: " + sProjectName);
		pStateEnvironment->LogMessage("ProjectId: " + sProjectId);
		pStateEnvironment->LogMessage("DataFileName: " + sDataFileName);
		pStateEnvironment->LogMessage("DataFileId: " + sDataFileId);
		pStateEnvironment->LogMessage("DataFileVersionId: " + sDataFileVersionId);
		pStateEnvironment->LogMessage("DataFileVersionNr: " + sDataFileVersionNr);
		pStateEnvironment->LogMessage("DocName: " + sDocName);
		pStateEnvironment->LogMessage("DocVersion: " + sDocVersion);
		pStateEnvironment->LogMessage("DocId: " + sDocId);
		pStateEnvironment->LogMessage("MachineName: " + sMachineName);
		pStateEnvironment->LogMessage("MachineUUID: " + sMachineUUID);

		opIndex = 0;
		for (const auto& sOperationSetups : sOperationSetups) {
			pStateEnvironment->LogMessage("OperationId" + std::to_string(opIndex) + ": " + sOperationSetups.operationId);
			pStateEnvironment->LogMessage("OperationName" + std::to_string(opIndex) + ": " + sOperationSetups.operationName);
			pStateEnvironment->LogMessage("SetupId" + std::to_string(opIndex) + ": " + sOperationSetups.setupId);
			pStateEnvironment->LogMessage("SetupName" + std::to_string(opIndex) + ": " + sOperationSetups.setupName);
			opIndex++;
		}

		pStateEnvironment->LogMessage("BedTemp: " + sBedTemp);
		pStateEnvironment->LogMessage("Ext1Temp: " + sExt1Temp);
		// jobname not sent by UI (together with UUID) => so we need to set parameter jobname by reading it from 3mf file (meta data)
		pStateEnvironment->SetStringParameter("jobinfo", "jobname", sDocName);
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
		
		read3mfMetaData(pStateEnvironment, sJobUUID);

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
		auto nConnectAndInitialiseTimeout = pStateEnvironment->GetIntegerParameter("timeoutdata", "printerconnectandinitialise");
		auto pSignalConnected = pStateEnvironment->PrepareSignal("printerconnection", "signal_connect");
		pSignalConnected->Trigger();
		if (pSignalConnected->WaitForHandling((uint32_t)nConnectAndInitialiseTimeout)) {

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


		// TODO uncomment to activate MQTT driver
		//pStateEnvironment->LogMessage("MQTT Disconnect done");
		//auto pMQTTDriver = m_pPluginData->acquireMQTTDriver(pStateEnvironment);
		//pMQTTDriver->Disconnect();


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
		auto nLayerTimeoutGraceTime = pStateEnvironment->GetIntegerParameter("timeoutdata", "layergracetime");

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

		LibMCEnv::PSignalHandler pHandlerInstance;
		if (pStateEnvironment->WaitForSignal("signal_resetfatalerror", 0, pHandlerInstance)) {

			pStateEnvironment->LogMessage("Reset state machine main after fatal error");
			try {
				pStateEnvironment->SetNextState("init");
				pHandlerInstance->SetBoolResult("success", true);
			}
			catch (std::exception& E) {
				pStateEnvironment->LogWarning(std::string("Could not reset: ") + E.what());
				pHandlerInstance->SetBoolResult("success", false);
				pStateEnvironment->SetNextState("fatalerror");
			}
			pHandlerInstance->SignalHandled();

		}
		else {
			// Unload all toolpathes that might be in memory
			pStateEnvironment->UnloadAllToolpathes();

			pStateEnvironment->SetNextState("fatalerror");
		}
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
