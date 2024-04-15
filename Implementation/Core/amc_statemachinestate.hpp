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


#ifndef __AMC_STATEMACHINESTATE
#define __AMC_STATEMACHINESTATE

#include <vector>
#include <string>
#include <memory>
#include <map>
#include "common_chrono.hpp"

#include "libmcplugin_dynamic.hpp"
#include "libmcenv_stateenvironment.hpp"

#include "amc_parameterhandler.hpp"

#define AMC_MINREPEATDELAY_MS 1 // 1 milliseconds minimum repeat delay
#define AMC_MAXREPEATDELAY_MS 10000 // 10000 milliseconds maximum repeat delay
#define AMC_DEFAULTREPEATDELAY_FOR_TESTING_MS 100 // for test environment: 100 milliseconds default repeat delay
#define AMC_REPEATDELAYCHUNK_MS 10 // 10 milliseconds sleep if waiting for repeat

namespace AMC {

	class CStateMachineState;
	typedef std::shared_ptr<CStateMachineState> PStateMachineState;

	class CStateMachineState {
	private:
		std::string m_sName;
		std::string m_sInstanceName;
		std::map <std::string, PStateMachineState> m_OutStates;
		std::vector <PStateMachineState> m_OutStateList;

		AMCCommon::PChrono m_pGlobalChrono;

		uint64_t m_LastExecutionTimeStampInMicroseconds;
		uint32_t m_nRepeatDelay;

		LibMCPlugin::PState m_pPluginState;
		LibMCEnv::PLibMCEnvWrapper m_pEnvironmentWrapper;

		void updateExecutionTime();
		void ensureExecutionDelay(uint32_t chunkInMilliseconds);
		
	public:

		CStateMachineState(const std::string & sInstanceName, const std::string& sName, uint32_t nRepeatDelay, LibMCEnv::PLibMCEnvWrapper pEnvironmentWrapper, AMCCommon::PChrono pGlobalChrono);
		virtual ~CStateMachineState();

		std::string getName() const;
		uint32_t getOutstateCount() const;
		std::string getOutstateName(uint32_t nIndex) const;

		void addOutState (PStateMachineState pState);
		bool hasOutState (std::string sName);

		void setPluginState(LibMCPlugin::PState pPluginState);

		void execute(std::string& sNextState, PSystemState pSystemState, PParameterHandler pParameterHandler, uint64_t nEndTimeOfPreviousStateInMicroseconds, const std::string& sPreviousStateName);

	};

	
}


#endif //__AMC_STATEMACHINESTATE

