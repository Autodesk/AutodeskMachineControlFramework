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


#ifndef __AMC_STATEMACHINEINSTANCE
#define __AMC_STATEMACHINEINSTANCE

#include "amc_statemachinestate.hpp"
#include "libmcplugin_dynamic.hpp"
#include "libmcenv_stateenvironment.hpp"
#include "amc_logger.hpp"
#include "amc_parameterhandler.hpp"
#include "amc_statejournal.hpp"

#include "common_chrono.hpp"

#include <map>
#include <string>
#include <thread>
#include <future>

namespace AMC {
	
	class CStateMachineInstance;
	typedef std::shared_ptr<CStateMachineInstance> PStateMachineInstance;


	class CStateMachineInstance {
	private:

		// Functions in thread context
		std::map<std::string, PStateMachineState>  m_States;
		std::vector<PStateMachineState> m_StateList;
		PStateMachineState m_pInitState;
		PStateMachineState m_pFailedState;
		PStateMachineState m_pCurrentState;
		PStateMachineState m_pSuccessState;

		AMC::PSystemState m_pSystemState;
		AMC::PParameterHandler m_ParameterHandler;
		AMC::PStateJournal m_pStateJournal;

		LibMCPlugin::PStateFactory m_pStateFactory;
		LibMCEnv::PLibMCEnvWrapper m_pEnvironmentWrapper;		

		// Thread members
		std::thread m_Thread;
		std::promise<void> m_TerminateSignal;
		std::future<void> m_TerminateFuture;

		// Externally accessible members		
		// Machine Name (only set in the constructor)
		std::string m_sName;

		// Thread functions
		bool threadIsRunning();
		bool threadShallTerminate();
		void executeStep();
		void executeThread();

		// Find state (only call inside thread)
		PStateMachineState findStateInternal(std::string sStateName, bool bFailIfNotExisting);

		// Set current state (only call inside thread)
		void setCurrentStateInternal(PStateMachineState pCurrentState);

		// Returns if current state exists
		bool hasCurrentStateInternal();

	public:

		CStateMachineInstance(const std::string & sName, const std::string& sDescription, LibMCEnv::PLibMCEnvWrapper pEnvironmentWrapper, AMC::PSystemState pSystemState, AMC::PStateJournal pStateJournal);
		virtual ~CStateMachineInstance();

		// Initialization functions (call before startThread)
		void setInitState(std::string sStateName);
		void setFailedState(std::string sStateName);
		void setSuccessState(std::string sStateName);

		void setStateFactory (LibMCPlugin::PStateFactory pStateFactory);		
		PStateMachineState addState(std::string sStateName, uint32_t nRepeatDelayInMS);
		PStateMachineState findState(std::string sStateName, bool bFailIfNotExisting);

		// External accessors
		std::string getName() const;
		std::string getDescription() const;
		uint32_t getStateCount() const;
		std::string getNameOfState(uint32_t nStateIndex) const;
		uint32_t getOutstateCountOfState(uint32_t nStateIndex) const;
		std::string getOutstateNameOfState(uint32_t nStateIndex, uint32_t nOutstateIndex) const;

		AMC::CParameterHandler * getParameterHandler ();

		// External methods
		void startThread();
		void terminateThread();

		std::string getCurrentStateName ();
		bool currentStateIsSuccessState();
		bool currentStateIsFailureState();
	};

	
}


#endif //__AMC_STATEMACHINEINSTANCE

