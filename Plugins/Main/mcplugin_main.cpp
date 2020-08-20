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

/*************************************************************************************************************************
 Class definition of CMainData
**************************************************************************************************************************/
class CMainData : public virtual CPluginData {
public:
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
					auto sJobUUID = pHandlerInstance->GetString("jobuuid");

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
					pStateEnvironment->LogMessage (std::string ("Could not start job: ") + E.what ());
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
		pSignal->WaitForHandling(10000);
		
		pStateEnvironment->SetIntegerParameter("jobinfo", "currentlayer", 0);
		pStateEnvironment->SetIntegerParameter("jobinfo", "layercount", nLayerCount);
		pStateEnvironment->SetBoolParameter("jobinfo", "autostart", false);
		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", true);
		pStateEnvironment->SetNextState("extrudelayer");
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

		pStateEnvironment->SetBoolParameter("jobinfo", "printinprogress", false);
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

		auto pSignal = pStateEnvironment->PrepareSignal("movement", "signal_doextrudelayer");
		pSignal->SetInteger("layertimeout", nLayerTimeout);
		pSignal->SetInteger("layerindex", nCurrentLayer);
		pSignal->SetString("jobuuid", sJobUUID);
		pSignal->Trigger();

		if (pSignal->WaitForHandling((uint32_t) (nLayerTimeout + nLayerTimeoutGraceTime))) {
			auto bSuccess = pSignal->GetBoolResult("success");
			std::string bSuccessMsg;
			//auto bSuccess = pSignal->GetBoolResult("successextrude");
			//auto bSuccessMsg = pSignal->GetStringResult("successextrudemessage");

			if (bSuccess) {
				pStateEnvironment->LogMessage("Extruding success. ");
				pStateEnvironment->SetNextState("nextlayer");
			}
			else {
				pStateEnvironment->LogMessage("Extruding failure: " + bSuccessMsg);
				pStateEnvironment->SetNextState("fatalerror");
			}
		}
		else {

			pStateEnvironment->LogMessage("Extruding timeout!");
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


		auto nCurrentLayer = pStateEnvironment->GetIntegerParameter("jobinfo", "currentlayer");
		auto nLayerCount = pStateEnvironment->GetIntegerParameter("jobinfo", "layercount");

		if (nCurrentLayer < (nLayerCount - 1)) {
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
