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

#include "libmcplugin_impl.hpp"

using namespace LibMCPlugin::Impl;

#include <stdint.h>

/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
__NODRIVERIMPORT


/*************************************************************************************************************************
 Class definition of CSignalLoadData
**************************************************************************************************************************/
class CSignalLoadData : public virtual CPluginData {
public:
	std::string m_sInstanceName;
	bool m_bIsPing;
	bool m_bStarted;
	uint32_t m_nSent;
	uint32_t m_nReceived;
	uint32_t m_nInFlight;
	uint32_t m_nMaxCount;
	uint32_t m_nWindowSize;

	CSignalLoadData(const std::string& sInstanceName)
		: m_sInstanceName(sInstanceName), m_bIsPing(sInstanceName == "ping"), m_bStarted(false), m_nSent(0), m_nReceived(0), m_nInFlight(0), m_nMaxCount(100000), m_nWindowSize(1024)
	{
	}
};

typedef CState<CSignalLoadData> CSignalLoadState;


/*************************************************************************************************************************
 Class definition of CSignalLoadState_Init
**************************************************************************************************************************/
class CSignalLoadState_Init : public virtual CSignalLoadState {
public:

	CSignalLoadState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CSignalLoadState(getStateName(), sStateName, pPluginData)
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

		m_pPluginData->m_bStarted = false;
		m_pPluginData->m_nSent = 0;
		m_pPluginData->m_nReceived = 0;
		m_pPluginData->m_nInFlight = 0;

		pStateEnvironment->SetNextState("run");
	}

};


/*************************************************************************************************************************
 Class definition of CSignalLoadState_Run
**************************************************************************************************************************/
class CSignalLoadState_Run : public virtual CSignalLoadState {
public:

	CSignalLoadState_Run(const std::string& sStateName, PPluginData pPluginData)
		: CSignalLoadState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "run";
	}

	void handlePingSignals(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		LibMCEnv::PSignalHandler pHandler;
		while ((pHandler = pStateEnvironment->GetUnhandledSignal("signal_ping")) != nullptr) {
			int64_t nCounter = pHandler->GetInteger("counter");
			pHandler->SignalHandled();

			auto pReply = pStateEnvironment->PrepareSignal("ping", "signal_pong");
			pReply->SetInteger("counter", nCounter);
			pReply->Trigger();
		}
	}

	bool checkStopSignal(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		auto pHandler = pStateEnvironment->GetUnhandledSignal("signal_stop");
		if (pHandler.get() != nullptr) {
			pHandler->SignalHandled();
			return true;
		}
		return false;
	}

	void handlePongSignals(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		LibMCEnv::PSignalHandler pHandler;
		while ((pHandler = pStateEnvironment->GetUnhandledSignal("signal_pong")) != nullptr) {
			pHandler->SignalHandled();
			if (m_pPluginData->m_nInFlight > 0)
				m_pPluginData->m_nInFlight--;
			m_pPluginData->m_nReceived++;
		}
	}

	void sendPingBurst(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		while ((m_pPluginData->m_nSent < m_pPluginData->m_nMaxCount) && (m_pPluginData->m_nInFlight < m_pPluginData->m_nWindowSize)) {
			auto pSignal = pStateEnvironment->PrepareSignal("pong", "signal_ping");
			pSignal->SetInteger("counter", (int64_t)m_pPluginData->m_nSent);
			pSignal->Trigger();

			m_pPluginData->m_nSent++;
			m_pPluginData->m_nInFlight++;
		}
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		if (m_pPluginData->m_bIsPing) {
			if (!m_pPluginData->m_bStarted) {
				sendPingBurst(pStateEnvironment);
				m_pPluginData->m_bStarted = true;
			}

			handlePongSignals(pStateEnvironment);
			sendPingBurst(pStateEnvironment);

			if ((m_pPluginData->m_nReceived >= m_pPluginData->m_nMaxCount) && (m_pPluginData->m_nInFlight == 0)) {
				auto pStop = pStateEnvironment->PrepareSignal("pong", "signal_stop");
				pStop->Trigger();
				pStateEnvironment->SetNextState("success");
				return;
			}

			pStateEnvironment->SetNextState("run");
		}
		else {
			if (checkStopSignal(pStateEnvironment)) {
				pStateEnvironment->SetNextState("success");
				return;
			}

			handlePingSignals(pStateEnvironment);
			pStateEnvironment->SetNextState("run");
		}
	}

};


/*************************************************************************************************************************
 Class definition of CSignalLoadState_Success
**************************************************************************************************************************/
class CSignalLoadState_Success : public virtual CSignalLoadState {
public:

	CSignalLoadState_Success(const std::string& sStateName, PPluginData pPluginData)
		: CSignalLoadState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "success";
	}

	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->SetNextState("success");
	}

};


/*************************************************************************************************************************
 Class definition of CSignalLoadState_FatalError
**************************************************************************************************************************/
class CSignalLoadState_FatalError : public virtual CSignalLoadState {
public:

	CSignalLoadState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CSignalLoadState(getStateName(), sStateName, pPluginData)
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
	m_pPluginData = std::make_shared<CSignalLoadData>(sInstanceName);
}

IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CSignalLoadState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CSignalLoadState_Run>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CSignalLoadState_Success>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CSignalLoadState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}
