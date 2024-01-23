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


#include "amc_statemachineinstance.hpp"
#include "amc_statemachinedata.hpp"

#include "libmc_exceptiontypes.hpp"

#include "libmcenv_abi.hpp"
#include "libmcplugin_exceptiontranslator.hpp"

#include <iostream>

typedef LibMCPlugin::ETranslator_StateFactory<ELibMCInterfaceException, LibMCResult, LIBMC_ERROR_INVALIDPARAM> tryMCPlugin_StateFactory;


namespace AMC {

	CStateMachineInstance::CStateMachineInstance(const std::string& sName, const std::string& sDescription, LibMCEnv::PLibMCEnvWrapper pEnvironmentWrapper, AMC::PSystemState pSystemState, AMC::PStateJournal pStateJournal)
		: m_sName(sName), m_pEnvironmentWrapper(pEnvironmentWrapper), m_pSystemState(pSystemState), m_pStateJournal (pStateJournal)
	{
		LibMCAssertNotNull(pEnvironmentWrapper.get());
		LibMCAssertNotNull(pSystemState.get());
		LibMCAssertNotNull(pStateJournal.get());

		m_ParameterHandler = std::make_shared<CParameterHandler>(sDescription);
		m_pSystemState->stateMachineData()->registerParameterHandler (sName, m_ParameterHandler);

	}


	CStateMachineInstance::~CStateMachineInstance()	
	{
		m_pInitState = nullptr;
		m_pCurrentState = nullptr;
		m_pFailedState = nullptr;
		m_pSuccessState = nullptr;
		m_pSystemState = nullptr;
		m_ParameterHandler = nullptr;
		m_pStateJournal = nullptr;
		m_pEnvironmentWrapper = nullptr;

		m_States.clear();
		m_StateList.clear();
		m_pStateFactory = nullptr;
	}


	PStateMachineState CStateMachineInstance::addState(std::string sStateName, uint32_t nRepeatDelayInMS)
	{
		// Only accessible if thread is not running
		if (threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName);


		if (findStateInternal(sStateName, false).get() != nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESTATENAME, m_sName + "/" + sStateName);
		if (sStateName.length() == 0)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSTATENAME, m_sName);

		auto pResult = std::make_shared<CStateMachineState>(m_sName, sStateName, nRepeatDelayInMS, m_pEnvironmentWrapper, m_pSystemState->getGlobalChronoInstance());

		m_States.insert(std::make_pair(sStateName, pResult));
		m_StateList.push_back(pResult);

		return pResult;
	}


	PStateMachineState CStateMachineInstance::findStateInternal(std::string sStateName, bool bFailIfNotExisting)
	{
		auto iter = m_States.find(sStateName);

		if (iter == m_States.end()) {

			if (bFailIfNotExisting)
				throw ELibMCCustomException(LIBMC_ERROR_STATENOTFOUND, m_sName + "/" + sStateName);

			return nullptr;
		}

		return iter->second;
	}

	PStateMachineState CStateMachineInstance::findState(std::string sStateName, bool bFailIfNotExisting)
	{
		// Only accessible if thread is not running
		if (threadIsRunning ())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName + "/" + sStateName);

		return findStateInternal(sStateName, bFailIfNotExisting);
	}

	void CStateMachineInstance::setInitState(std::string sStateName)
	{
		// Only accessible if thread is not running
		if (threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName + "/" + sStateName);

		auto iter = m_States.find(sStateName);
		if (iter == m_States.end()) {
			throw ELibMCCustomException(LIBMC_ERROR_INITSTATENOTFOUND, m_sName + "/" + sStateName);
		}

		m_pInitState = iter->second;
		setCurrentStateInternal(m_pInitState);
	}

	void CStateMachineInstance::setFailedState(std::string sStateName)
	{
		// Only accessible if thread is not running
		if (threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName + "/" + sStateName);

		auto iter = m_States.find(sStateName);
		if (iter == m_States.end()) {
			throw ELibMCCustomException(LIBMC_ERROR_FAILEDSTATENOTFOUND, m_sName + "/" + sStateName);
		}

		m_pFailedState = iter->second;

	}

	void CStateMachineInstance::setSuccessState(std::string sStateName)
	{
		// Only accessible if thread is not running
		if (threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName + "/" + sStateName);

		auto iter = m_States.find(sStateName);
		if (iter == m_States.end()) {
			throw ELibMCCustomException(LIBMC_ERROR_SUCCESSSTATENOTFOUND, m_sName + "/" + sStateName);
		}

		m_pSuccessState = iter->second;

	}


	void CStateMachineInstance::setCurrentStateInternal(PStateMachineState pCurrentState)
	{
		m_pCurrentState = pCurrentState;
		if (pCurrentState.get() != nullptr) {
			m_pSystemState->stateMachineData()->setInstanceStateName(m_sName, m_pCurrentState->getName());
		}
		else {
			m_pSystemState->stateMachineData()->setInstanceStateName(m_sName, "");
		}

	}

	bool CStateMachineInstance::hasCurrentStateInternal()
	{
		return (m_pCurrentState.get() != nullptr);

	}




	void CStateMachineInstance::setStateFactory(LibMCPlugin::PStateFactory pStateFactory)
	{
		// Only accessible if thread is not running
		if (threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName);

		LibMCAssertNotNull(pStateFactory.get());

		if (threadIsRunning ())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName);

		m_pStateFactory = pStateFactory;

		m_pSystemState->logger()->logMessage ("Initialising instance " + m_sName, m_sName, eLogLevel::Message);
		for (auto pStateIter : m_States) {

			auto pState = pStateIter.second;
			std::string sStateName = pState->getName();
			auto pPluginState = tryMCPlugin_StateFactory(pStateFactory, LIBMC_ERROR_COULDNOTCREATESTATE, sStateName).CreateState(sStateName);

			pState->setPluginState(pPluginState);
		}


	}

	void CStateMachineInstance::executeStep()
	{
		if (!hasCurrentStateInternal ())
			throw ELibMCCustomException(LIBMC_ERROR_NOCURRENTSTATE, m_sName);
		if (m_pFailedState.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_NOFAILEDSTATE, m_sName);

		try {

			std::string sCurrentState = m_pCurrentState->getName();
			std::string sNextState;
			m_pCurrentState->execute(sNextState, m_pSystemState, m_ParameterHandler);

			if (sNextState.empty())
				throw ELibMCCustomException(LIBMC_ERROR_NOOUTSTATEGIVEN, m_sName + ": " + sCurrentState);
			if (!m_pCurrentState->hasOutState(sNextState) && (sNextState != m_pFailedState->getName()))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDOUTSTATEGIVEN, m_sName + ": " + sCurrentState + "->" + sNextState);

			// Set new current state		
			if (sCurrentState != sNextState)
				m_pSystemState->logger()->logMessage("state change: " + sCurrentState + "->" + sNextState, m_sName, eLogLevel::Debug);

			setCurrentStateInternal (findStateInternal (sNextState, true));

		}
		catch (std::exception & E) {

			m_pSystemState->logger()->logMessage("step execution error: " + std::string (E.what()), m_sName, eLogLevel::CriticalError);
			setCurrentStateInternal (m_pFailedState);
		}
		catch (...)
		{
			m_pSystemState->logger()->logMessage("step execution error: unknown", m_sName, eLogLevel::CriticalError);
			setCurrentStateInternal(m_pFailedState);
		}

	}

	void CStateMachineInstance::executeThread()
	{
		while (!threadShallTerminate()) {
			executeStep();
		}
	}


	void CStateMachineInstance::startThread()
	{
		m_pSystemState->logger()->logMessage("starting instance thread for " + m_sName + "...", m_sName, eLogLevel::Message);

		if (m_pInitState.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_NOINITSTATE, m_sName);
		if (m_pFailedState.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_NOFAILEDSTATE, m_sName);

		if (threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISRUNNING, m_sName);

		// Initialise signals
		setCurrentStateInternal(m_pInitState);
		m_TerminateSignal = std::promise<void>();
		m_TerminateFuture = m_TerminateSignal.get_future();
		
		// Start Thread
		m_Thread = std::thread(&CStateMachineInstance::executeThread, this);

	}


	std::string CStateMachineInstance::getCurrentStateName()
	{
		if (m_pCurrentState.get() != nullptr) {
			return m_pCurrentState->getName();
		}
		else
			return "";

	}

	bool CStateMachineInstance::currentStateIsSuccessState()
	{
		if (m_pCurrentState.get() != nullptr) {
			if (m_pCurrentState.get() == m_pSuccessState.get())
				return true;
		}
		return false;
	}

	bool CStateMachineInstance::currentStateIsFailureState()
	{
		if (m_pCurrentState.get() != nullptr) {
			if (m_pCurrentState.get() == m_pFailedState.get())
				return true;
		}
		return false;
	}


	void CStateMachineInstance::terminateThread()
	{
		m_pSystemState->logger()->logMessage("terminating instance thread ", m_sName, eLogLevel::Message);

		if (!threadIsRunning())
			throw ELibMCCustomException(LIBMC_ERROR_THREADISNOTRUNNING, m_sName);

		// Set termination flag
		m_TerminateSignal.set_value();

		// Wait for thread to finish
		m_Thread.join();

		m_pSystemState->logger()->logMessage("instance thread terminated", m_sName, eLogLevel::Message);

		// Clean up signals
		m_TerminateFuture = std::future<void>();
		m_TerminateSignal = std::promise<void>();
		setCurrentStateInternal (nullptr);
	}



	bool CStateMachineInstance::threadIsRunning()
	{
		return m_TerminateFuture.valid();
	}

	bool CStateMachineInstance::threadShallTerminate()
	{
		if (m_TerminateFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
			return false;
		return true;
	}

	std::string CStateMachineInstance::getName() const
	{
		// Return thread safe copy of instance name
		return std::string (m_sName.c_str());
	}

	std::string CStateMachineInstance::getDescription() const
	{		
		return m_ParameterHandler->getDescription ();
	}



	uint32_t CStateMachineInstance::getStateCount() const
	{
		return (uint32_t)m_StateList.size();

	}

	std::string CStateMachineInstance::getNameOfState(uint32_t nStateIndex) const
	{
		if (nStateIndex >= m_StateList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);
		auto pState = m_StateList[nStateIndex];
		return pState->getName();
	}

	uint32_t CStateMachineInstance::getOutstateCountOfState(uint32_t nStateIndex) const
	{
		if (nStateIndex >= m_StateList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);
		auto pState = m_StateList[nStateIndex];
		return pState->getOutstateCount();
	}

	std::string CStateMachineInstance::getOutstateNameOfState(uint32_t nStateIndex, uint32_t nOutstateIndex) const
	{
		if (nStateIndex >= m_StateList.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDINDEX, m_sName);
		auto pState = m_StateList[nStateIndex];
		return pState->getOutstateName(nOutstateIndex);

	}

	AMC::CParameterHandler* CStateMachineInstance::getParameterHandler()
	{
		return m_ParameterHandler.get ();
	}



}


