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

#include <iostream>
using namespace LibMCPlugin::Impl;


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
typedef LibMCDriver_ScanLab::PDriver_ScanLab_RTC5 PDriver_ScanLab_RTC5;
typedef LibMCEnv::CDriverCast <LibMCDriver_ScanLab::CDriver_ScanLab_RTC5, LibMCDriver_ScanLab::CWrapper> PDriverCast_ScanLab;

/*************************************************************************************************************************
 Class definition of CLaserData
**************************************************************************************************************************/
class CLaserData : public virtual CPluginData {
private:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_ScanLab m_DriverCast_ScanLab;

public:

	PDriver_ScanLab_RTC5 acquireDriver(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_ScanLab.acquireDriver(pStateEnvironment, "scanlab");
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
		: CLaserState (getStateName (), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		auto pDriver = m_pPluginData->acquireDriver (pStateEnvironment);
		std::string sName = pDriver->GetName();

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

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		LibMCEnv::PSignalHandler pSignal;
		if (pStateEnvironment->WaitForSignal("signal_drawlayer", 10, pSignal)) {

			pStateEnvironment->StoreSignal("global_drawsignal", pSignal.get());
			pStateEnvironment->SetNextState("drawlayer");

		}
		else {
			pStateEnvironment->SetNextState("idle");
		}

	}

};



/*************************************************************************************************************************
 Class definition of CLaserState_DrawLayer
**************************************************************************************************************************/

class CLaserState_DrawLayer : public virtual CLaserState {
public:

	CLaserState_DrawLayer(const std::string& sStateName, PPluginData pPluginData)
		: CLaserState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "drawlayer";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->LogMessage("Drawing layer...");

		pStateEnvironment->SetBoolParameter("laserstate", "laseron", true);

		auto pSignal = pStateEnvironment->RetrieveSignal("global_drawsignal");
		auto sJobUUID = pSignal->GetString("jobuuid");
		auto nLayerIndex = pSignal->GetInteger("layerindex");

		auto pToolpathAccessor = pStateEnvironment->CreateToolpathAccessor(sJobUUID);
		auto pLayer = pToolpathAccessor->LoadLayer((uint32_t) nLayerIndex);

		auto nSegmentCount = pLayer->GetSegmentCount();
		for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {
			LibMCEnv::eToolpathSegmentType eSegmentType;
			uint32_t nPointCount;
			std::vector<LibMCEnv::sPosition2D> PointData;

			pLayer->GetSegmentInfo(nSegmentIndex, eSegmentType, nPointCount);
			pLayer->GetSegmentPointData(nSegmentIndex, PointData);

			switch (eSegmentType) {
			case LibMCEnv::eToolpathSegmentType::Hatch:
				std::cout << PointData.size() << "  points..." << std::endl;


				break;

			case LibMCEnv::eToolpathSegmentType::Loop:
				break;

			case LibMCEnv::eToolpathSegmentType::Polyline:
				break;

			}
		}


		pSignal->SetBoolResult("success", true);
		pSignal->SignalHandled();

		pStateEnvironment->SetBoolParameter("laserstate", "laseron", false);

		pStateEnvironment->SetNextState("idle");

	}

};



/*************************************************************************************************************************
 Class definition of CLaserState_FatalError
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

	if (createStateInstanceByName<CLaserState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CLaserState_DrawLayer>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


#ifdef _MSC_VER
#pragma warning(pop)
#endif
