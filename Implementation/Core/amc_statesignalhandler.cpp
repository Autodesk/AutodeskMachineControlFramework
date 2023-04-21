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


#include "amc_statesignalhandler.hpp"

#define _STATESIGNAL_HEADERPROTECTION 

#include "amc_statesignal.hpp"

#include "libmc_exceptiontypes.hpp"

#include "common_utils.hpp"

namespace AMC {
	
	
	CStateSignalHandler::CStateSignalHandler()
	{
	}
	

	CStateSignalHandler::~CStateSignalHandler()
	{
	}
	
	void CStateSignalHandler::addSignalDefinition(const std::string& sInstanceName, const std::string& sSignalName, const std::list<CStateSignalParameter>& Parameters, const std::list<CStateSignalParameter>& Results)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		if (sSignalName.length() == 0)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSIGNALNAME, sInstanceName);


		auto iter = m_SignalMap.find(std::make_pair (sInstanceName, sSignalName));
		if (iter != m_SignalMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESIGNAL, sInstanceName + "/" + sSignalName);

		auto pSignal = std::make_shared<CStateSignal>(sInstanceName, sSignalName, Parameters, Results);
		m_SignalMap.insert(std::make_pair(std::make_pair (sInstanceName, sSignalName), pSignal));
	}

	bool CStateSignalHandler::triggerSignal(const std::string& sInstanceName, const std::string& sSignalName, const std::string& sParameterData, std::string& sSignalUUID)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalMap.find(std::make_pair(sInstanceName, sSignalName));
		if (iter == m_SignalMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sInstanceName + "/" + sSignalName);


		if (iter->second->triggerSignalInternal(sParameterData, sSignalUUID)) {
			m_SignalUUIDLookupMap.insert(std::make_pair(sSignalUUID, iter->second));
			return true;
		}

		return false;
	}


	bool CStateSignalHandler::hasSignalDefinition(const std::string& sInstanceName, const std::string& sSignalName)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalMap.find(std::make_pair(sInstanceName, sSignalName));
		return (iter != m_SignalMap.end());
	}


	bool CStateSignalHandler::checkSignal(const std::string& sInstanceName, const std::string& sSignalName, std::string& sCurrentSignalUUID)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalMap.find(std::make_pair(sInstanceName, sSignalName));
		if (iter == m_SignalMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sInstanceName + "/" + sSignalName);

		return iter->second->checkSignalInternal(sCurrentSignalUUID);
	}

	bool CStateSignalHandler::checkSignalUUID(const std::string& sInstanceName, std::string sCurrentSignalUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sCurrentSignalUUID);
		std::lock_guard <std::mutex> lockGuard(m_Mutex);
		auto iIter = m_SignalUUIDLookupMap.find(sNormalizedUUID);
		if (iIter == m_SignalUUIDLookupMap.end())
			return false;

		return iIter->second->checkSignalInternal(sNormalizedUUID);
	}


	bool CStateSignalHandler::canTrigger(const std::string& sInstanceName, const std::string& sSignalName)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalMap.find(std::make_pair(sInstanceName, sSignalName));
		if (iter == m_SignalMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sInstanceName + "/" + sSignalName);

		return iter->second->canTriggerInternal();
	}


	void CStateSignalHandler::markSignalAsHandled(const std::string& sSignalUUID, const std::string& sResultData)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalUUIDLookupMap.find(sSignalUUID);
		if (iter == m_SignalUUIDLookupMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sSignalUUID);

		iter->second->markSignalAsHandledInternal(sResultData);
	}

	bool CStateSignalHandler::signalHasBeenHandled(const std::string& sSignalUUID, const bool clearAllResults, std::string& sResultData)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalUUIDLookupMap.find(sSignalUUID);
		if (iter == m_SignalUUIDLookupMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sSignalUUID);

		PStateSignal pSignal = iter->second;
		bool bHasBeendHandled = pSignal->signalHasBeenHandledInternal(sSignalUUID, clearAllResults, sResultData);

		if (bHasBeendHandled && clearAllResults)
			m_SignalUUIDLookupMap.erase(sSignalUUID);

		return bHasBeendHandled;

		


	}


	bool CStateSignalHandler::findSignalPropertiesByUUID(const std::string& sSignalUUID, std::string& sInstanceName, std::string& sSignalName, std::string& sParameterData)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		auto iter = m_SignalUUIDLookupMap.find(sSignalUUID);
		if (iter != m_SignalUUIDLookupMap.end()) {
			if (iter->second->getCurrentUUID() == sSignalUUID) {
				sInstanceName = iter->second->getInstanceNameInternal();
				sSignalName = iter->second->getNameInternal();
				sParameterData = iter->second->getParameterDataInternal();
				return true;
			}
		
		}

		return false;

	}


	void CStateSignalHandler::populateParameterGroup(const std::string& sInstanceName, const std::string& sSignalName, CParameterGroup* pParameterGroup)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		LibMCAssertNotNull(pParameterGroup);

		auto iter = m_SignalMap.find(std::make_pair(sInstanceName, sSignalName));
		if (iter == m_SignalMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sInstanceName + "/" + sSignalName);

		iter->second->populateParameterGroup(pParameterGroup);
	}

	void CStateSignalHandler::populateResultGroup(const std::string& sInstanceName, const std::string& sSignalName, CParameterGroup* pResultGroup)
	{
		std::lock_guard <std::mutex> lockGuard(m_Mutex);

		LibMCAssertNotNull(pResultGroup);

		auto iter = m_SignalMap.find(std::make_pair(sInstanceName, sSignalName));
		if (iter == m_SignalMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, sInstanceName + "/" + sSignalName);

		iter->second->populateResultGroup(pResultGroup);
	}



}


