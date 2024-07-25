/*++

Copyright (C) 2024 Autodesk Inc.

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
#include <sstream>

#include <iomanip>
#include <cmath>

using namespace LibMCPlugin::Impl; 

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


/*************************************************************************************************************************
  State definitions
**************************************************************************************************************************/
/*************************************************************************************************************************
 Class definition of CMainData
**************************************************************************************************************************/
class CMainData : public virtual CPluginData {
protected:
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
		: CMainState(getStateName(), sStateName, pPluginData) {}

	static const std::string getStateName() {
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) {
		pStateEnvironment->LogMessage("Initializing...");

		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_Idle
**************************************************************************************************************************/
class CMainState_Idle : public virtual CMainState {
public:
	CMainState_Idle(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {}

	static const std::string getStateName() {
		return "idle";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) {
		pStateEnvironment->SetNextState("idle");
	}
};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CMainState_FatalError : public virtual CMainState {
public:
	CMainState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CMainState(getStateName(), sStateName, pPluginData) {}

	static const std::string getStateName() {
		return "fatalerror";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) {
		pStateEnvironment->Sleep(5000);
		pStateEnvironment->SetNextState("init");
	}
};


/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName) {
    m_pPluginData = std::make_shared<CMainData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName) {
    IState* pStateInstance = nullptr;

    if (createStateInstanceByName<CMainState_Init>(sStateName, pStateInstance, m_pPluginData))
        return pStateInstance;

    if (createStateInstanceByName<CMainState_Idle>(sStateName, pStateInstance, m_pPluginData))
        return pStateInstance;

    if (createStateInstanceByName<CMainState_FatalError>(sStateName, pStateInstance, m_pPluginData))
        return pStateInstance;

    throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
