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


#include "amc_statemachinestate.hpp"
#include "amc_driverhandler.hpp"

#include "common_utils.hpp"

#include "libmc_interfaceexception.hpp"

#include "libmcenv_stateenvironment.hpp"
#include <thread>

namespace AMC {

	CStateMachineState::CStateMachineState(const std::string& sInstanceName, const std::string& sName, uint32_t nRepeatDelay, LibMCEnv::PLibMCEnvWrapper pEnvironmentWrapper)
		: m_sInstanceName(sInstanceName), m_sName (sName), m_pEnvironmentWrapper (pEnvironmentWrapper), m_nRepeatDelay (nRepeatDelay)
	{
		if (pEnvironmentWrapper.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		if ((nRepeatDelay < AMC_MINREPEATDELAY_MS) || (nRepeatDelay > AMC_MAXREPEATDELAY_MS))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDREPEATDELAY);


		updateExecutionTime();

	}

	CStateMachineState::~CStateMachineState()
	{

	}

	std::string CStateMachineState::getName() const
	{
		return std::string (m_sName.c_str());
	}

	uint32_t CStateMachineState::getOutstateCount() const
	{
		return (uint32_t) m_OutStateList.size();
	}

	std::string CStateMachineState::getOutstateName(uint32_t nIndex) const
	{
		if (nIndex >= m_OutStateList.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pOutState = m_OutStateList[nIndex];
		return pOutState->getName();
	}


	void CStateMachineState::updateExecutionTime()
	{
		m_LastExecutionTime = std::chrono::high_resolution_clock::now();
	}


	void CStateMachineState::addOutState(PStateMachineState pState)
	{
		if (pState.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::string sName = pState->getName();

		if (hasOutState (sName))
			throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEOUTSTATE);


		m_OutStates.insert(std::make_pair (sName, pState));
		m_OutStateList.push_back(pState);
	}

	bool CStateMachineState::hasOutState(std::string sName)
	{
		auto iter = m_OutStates.find(sName);
		return (iter != m_OutStates.end());
	}

	void CStateMachineState::setPluginState(LibMCPlugin::PState pPluginState)
	{
		if (pPluginState.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_pPluginState = pPluginState;
	}

	template <class C> std::shared_ptr<C> mapInternalStateEnvInstance(std::shared_ptr<LibMCEnv::Impl::IBase> pImplInstance, LibMCEnv::PWrapper pWrapper)
	{
		auto pExternalInstance = std::make_shared <C>(pWrapper.get(), (LibMCEnv::Impl::IBase*) (pImplInstance.get()));
		pImplInstance->IncRefCount();
		return pExternalInstance;
	}

	void CStateMachineState::execute(std::string& sNextState, PSystemState pSystemState, PParameterHandler pParameterHandler)
	{
		if (pSystemState.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pParameterHandler.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		if (m_pPluginState.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOPLUGINSTATE);


		auto pInternalEnvironment = std::make_shared<LibMCEnv::Impl::CStateEnvironment>(pSystemState, pParameterHandler, m_sInstanceName);
		auto pExternalEnvironment = mapInternalStateEnvInstance<LibMCEnv::CStateEnvironment>  (pInternalEnvironment, m_pEnvironmentWrapper);
					
		sNextState = "";

		try {

			ensureExecutionDelay(AMC_REPEATDELAYCHUNK_MS);

			m_pPluginState->Execute(pExternalEnvironment.get());

			sNextState = pInternalEnvironment->getNextState();

			updateExecutionTime();

			pSystemState->driverHandler()->releaseDriverLocks(m_sInstanceName);

		}
		catch (std::exception & E) {

			updateExecutionTime();
			pSystemState->driverHandler()->releaseDriverLocks(m_sInstanceName);
			throw ELibMCInterfaceException (LIBMC_ERROR_COULDNOTEXECUTEPLUGIN, E.what());
		}
		
	}

	void CStateMachineState::ensureExecutionDelay(uint32_t chunkInMilliseconds)
	{

		if (chunkInMilliseconds < AMC_MINREPEATDELAY_MS)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		
		auto currentExecutionTime = std::chrono::high_resolution_clock::now();
		auto deltaExecutionTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentExecutionTime - m_LastExecutionTime).count();

		while (deltaExecutionTime < m_nRepeatDelay) {		
			AMCCommon::CUtils::sleepMilliseconds(chunkInMilliseconds);

			// TODO: Check for potential kill Signal
			currentExecutionTime = std::chrono::high_resolution_clock::now();

			auto newDeltaExecutionTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentExecutionTime - m_LastExecutionTime).count();
			if (!(deltaExecutionTime < newDeltaExecutionTime))
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDEXECUTIONDELAY);

			deltaExecutionTime = newDeltaExecutionTime;
		}

	}

}


