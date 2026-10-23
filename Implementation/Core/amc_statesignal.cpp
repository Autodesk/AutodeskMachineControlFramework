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


#include "amc_statesignal.hpp"
#include "common_utils.hpp"
#include "amc_telemetry.hpp"
#include "libmc_exceptiontypes.hpp"

#include <iterator>

namespace AMC {
	
	CStateSignalMessage::CStateSignalMessage(const std::string& sUUID, uint32_t nReactionTimeoutInMS, PTelemetryChannel pQueueTelemetryChannel, PTelemetryChannel pInProcessTelemetryChannel, PTelemetryChannel pAcknowledgeTelemetryChannel)
		: m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)),
		m_nReactionTimeoutInMS(nReactionTimeoutInMS),
		m_MessagePhase(eAMCSignalPhase::InQueue),
		m_pInProcessTelemetryChannel(pInProcessTelemetryChannel),
		m_pAcknowledgeTelemetryChannel(pAcknowledgeTelemetryChannel),
		m_nCreationTimestamp(0),
		m_nTerminalTimestamp(0)

	{
		if (pQueueTelemetryChannel.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pInProcessTelemetryChannel.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pAcknowledgeTelemetryChannel.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_pTelemetryInQueueMarker = pQueueTelemetryChannel->startIntervalMarker(0);
		m_nCreationTimestamp = m_pTelemetryInQueueMarker->getStartTimestamp();
	}

	CStateSignalMessage::~CStateSignalMessage()
	{
		m_pTelemetryInQueueMarker = nullptr;
		m_pTelemetryInProcessMarker = nullptr;
		m_pTelemetryAcknowledgedMarker = nullptr;

	}

	std::string CStateSignalMessage::getUUID() const
	{
		return m_sUUID;
	}

	uint64_t CStateSignalMessage::getCreationTimestamp() const
	{
		return m_nCreationTimestamp;
	}

	uint64_t CStateSignalMessage::getTerminalTimestamp() const
	{		
		return m_nTerminalTimestamp;
	}

	bool CStateSignalMessage::setPhase(AMC::eAMCSignalPhase messagePhase)
	{
		if (m_pTelemetryInQueueMarker.get() != nullptr) {
			m_pTelemetryInQueueMarker->finishMarker();
			m_pTelemetryInQueueMarker = nullptr;
		}
		
		if (m_pTelemetryInProcessMarker.get () != nullptr) {
			m_pTelemetryInProcessMarker->finishMarker();
			m_nTerminalTimestamp = m_pTelemetryInProcessMarker->getFinishTimestamp();
			m_pTelemetryInProcessMarker = nullptr;
		}

		if (m_pTelemetryAcknowledgedMarker.get() != nullptr) {
			m_pTelemetryAcknowledgedMarker->finishMarker();
			m_pTelemetryAcknowledgedMarker = nullptr;
		}


		switch (messagePhase) {
			case AMC::eAMCSignalPhase::InProcess:
				m_pTelemetryInProcessMarker = m_pInProcessTelemetryChannel->startIntervalMarker(0);
				m_MessagePhase = messagePhase;
				return true;

			case AMC::eAMCSignalPhase::Handled:
			case AMC::eAMCSignalPhase::Failed:
			case AMC::eAMCSignalPhase::TimedOut:
			case AMC::eAMCSignalPhase::Cleared:
				m_pTelemetryAcknowledgedMarker = m_pAcknowledgeTelemetryChannel->startIntervalMarker(0);
				m_MessagePhase = messagePhase;
				return true;

			case AMC::eAMCSignalPhase::Archived:
				m_MessagePhase = messagePhase;
				return true;

			default:
				return false;

		}


		
	}

	AMC::eAMCSignalPhase CStateSignalMessage::getPhase() const
	{
		return m_MessagePhase;
	}

	uint32_t CStateSignalMessage::getReactionTimeoutInMS() const
	{
		return m_nReactionTimeoutInMS;
	}

	bool CStateSignalMessage::hadReactionTimeout(uint64_t nGlobalTimestamp)
	{
		uint64_t nTimeoutTimestamp = m_nCreationTimestamp + (uint64_t)m_nReactionTimeoutInMS * 1000;
		return (nGlobalTimestamp >= nTimeoutTimestamp);
	}

	std::string CStateSignalMessage::getResultDataJSON() const
	{
		return m_sResultDataJSON;
	}

	std::string CStateSignalMessage::getParameterDataJSON() const
	{
		return m_sParameterDataJSON;
	}

	std::string CStateSignalMessage::getErrorMessage() const
	{
		return m_sErrorMessage;
	}

	void CStateSignalMessage::setErrorMessage(const std::string & sErrorMessage) 
	{
		m_sErrorMessage = sErrorMessage;
	}


	void CStateSignalMessage::setResultDataJSON(const std::string& sResultDataJSON)
	{
		m_sResultDataJSON = sResultDataJSON;
	}

	void CStateSignalMessage::setParameterDataJSON(const std::string& sParameterDataJSON)
	{
		m_sParameterDataJSON = sParameterDataJSON;
	}


	CStateSignalSlot::CStateSignalSlot(const std::string& sInstanceName, const std::string& sName, const std::vector<CStateSignalParameter>& Parameters, const std::vector<CStateSignalParameter>& Results, uint32_t nSignalDefaultReactionTimeOutInMS, uint32_t nSignalAutomaticArchiveTimeInMS, uint32_t nSignalQueueSize, PParameterGroup pSignalInformationGroup, CStateSignalRegistry* pRegistry)
		: m_sInstanceName (sInstanceName), 
		m_sName (sName),  
		m_ParameterDefinitions(Parameters), 
		m_ResultDefinitions(Results),
		m_nSignalDefaultReactionTimeOutInMS(nSignalDefaultReactionTimeOutInMS),
		m_nSignalAutomaticArchiveTimeInMS (nSignalAutomaticArchiveTimeInMS),
		m_nSignalQueueSize (nSignalQueueSize),
		m_pSignalInformationGroup (pSignalInformationGroup),
		m_nTriggerCount (0),
		m_nHandledCount (0),
		m_nFailedCount (0),
		m_nTimedOutCount (0),
		m_nArchivedCount (0),
		m_nMaxReactionTime (0),
		m_pRegistry (pRegistry)

	{

		if (pRegistry == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "invalid signal registry parameter");

		if (pSignalInformationGroup.get() != nullptr) {
			pSignalInformationGroup->addNewIntParameter("triggered_" + m_sName, m_sName + " was triggered", 0);
			pSignalInformationGroup->addNewIntParameter("handled_" + m_sName, m_sName + " was handled", 0);
			pSignalInformationGroup->addNewIntParameter("failed_" + m_sName, m_sName + " has failed", 0);
			pSignalInformationGroup->addNewIntParameter("timeout_" + m_sName, m_sName + " timed out", 0);
			pSignalInformationGroup->addNewIntParameter("archived_" + m_sName, m_sName + " archived", 0);
			pSignalInformationGroup->addNewIntParameter("reactiontime_" + m_sName, m_sName + " max reaction time (microseconds)", 0);
			pSignalInformationGroup->addNewIntParameter("finishtime_" + m_sName, m_sName + " max finish time (microseconds)", 0);
			pSignalInformationGroup->addNewIntParameter("acknowledgetime_" + m_sName, m_sName + " max acknowledge time (microseconds)", 0);
		}

		m_pQueueTelemetryChannel = m_pRegistry->registerTelemetryChannel (getSignalTelemetryQueueIdentifier (), "Signal Telemetry for " + m_sInstanceName + "." + m_sName + " (in queue)", LibMCData::eTelemetryChannelType::SignalQueue);
		m_pProcessingTelemetryChannel = m_pRegistry->registerTelemetryChannel(getSignalTelemetryProcessingIdentifier(), "Signal Telemetry for " + m_sInstanceName + "." + m_sName + " (processing)", LibMCData::eTelemetryChannelType::SignalProcessing);
		m_pAcknowledgementTelemetryChannel = m_pRegistry->registerTelemetryChannel(getSignalTelemetryAcknowledgementIdentifier(), "Signal Telemetry for " + m_sInstanceName + "." + m_sName + " (acknowledgement)", LibMCData::eTelemetryChannelType::SignalAcknowledgement);


	}
	
	CStateSignalSlot::~CStateSignalSlot()
	{
	}

	std::string CStateSignalSlot::getNameInternal() const
	{
		return m_sName;
	}
	
	std::string CStateSignalSlot::getInstanceNameInternal() const
	{
		return m_sInstanceName;
	}

	std::string CStateSignalSlot::getSignalTelemetryQueueIdentifier() const
	{
		return m_sInstanceName + "." + m_sName + ".queue";
	}
	std::string CStateSignalSlot::getSignalTelemetryProcessingIdentifier() const
	{
		return m_sInstanceName + "." + m_sName + ".processing";
	}

	std::string CStateSignalSlot::getSignalTelemetryAcknowledgementIdentifier() const
	{
		return m_sInstanceName + "." + m_sName + ".acknowledgement";
	}

	CStateSignalMessage *CStateSignalSlot::getMessageByUUIDNoMutex(const std::string& sMessageUUIDNormalized)
	{

		auto it = m_MessageMap.find(sMessageUUIDNormalized);
		if (it == m_MessageMap.end()) 
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "getMessageByUUIDNoMutex: Signal UUID not found: " + sMessageUUIDNormalized);
		
		return it->second.get ();
	}

	void CStateSignalSlot::updateTimingStatistics()
	{
		if (m_pSignalInformationGroup.get() != nullptr) {

			m_pSignalInformationGroup->setIntParameterValueByName("reactiontime_" + m_sName, (int64_t)m_pQueueTelemetryChannel->getMaxDurationInMicroseconds());
			m_pSignalInformationGroup->setIntParameterValueByName("finishtime_" + m_sName, (int64_t)m_pProcessingTelemetryChannel->getMaxDurationInMicroseconds());
			m_pSignalInformationGroup->setIntParameterValueByName("acknowledgetime_" + m_sName, (int64_t)m_pAcknowledgementTelemetryChannel->getMaxDurationInMicroseconds());
		}

	}


	void CStateSignalSlot::increaseTriggerCount()
	{
		m_nTriggerCount++;
		if (m_pSignalInformationGroup.get() != nullptr) {
			m_pSignalInformationGroup->setIntParameterValueByName("triggered_" + m_sName, (int64_t)m_nTriggerCount);
		}

		updateTimingStatistics();
	}

	void CStateSignalSlot::increaseHandledCount()
	{
		m_nHandledCount++;
		if (m_pSignalInformationGroup.get() != nullptr) {
			m_pSignalInformationGroup->setIntParameterValueByName("handled_" + m_sName, (int64_t)m_nHandledCount);
		}

		updateTimingStatistics();
	}

	void CStateSignalSlot::increaseFailedCount()
	{
		m_nFailedCount++;
		if (m_pSignalInformationGroup.get() != nullptr) {
			m_pSignalInformationGroup->setIntParameterValueByName("failed_" + m_sName, (int64_t)m_nFailedCount);
		}

		updateTimingStatistics();
	}

	void CStateSignalSlot::increaseTimeoutCount()
	{
		m_nTimedOutCount++;
		if (m_pSignalInformationGroup.get() != nullptr) {
			m_pSignalInformationGroup->setIntParameterValueByName("timeout_" + m_sName, (int64_t)m_nTimedOutCount);
		}

		updateTimingStatistics();
	}

	void CStateSignalSlot::increaseArchivedCount()
	{
		m_nArchivedCount++;
		if (m_pSignalInformationGroup.get() != nullptr) {
			m_pSignalInformationGroup->setIntParameterValueByName("archived_" + m_sName, (int64_t)m_nArchivedCount);
		}

	}


	bool CStateSignalSlot::queueIsFullNoMutex()
	{
		return (m_Queue.size() >= m_nSignalQueueSize);
	}

	bool CStateSignalSlot::queueIsFull()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return queueIsFullNoMutex();
	}

	bool CStateSignalSlot::queueIsEmpty()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return m_Queue.size() == 0;
	}

	uint32_t CStateSignalSlot::getAvailableSignalQueueEntriesInternal()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (m_Queue.size() < m_nSignalQueueSize)
			return m_nSignalQueueSize - (uint32_t)m_Queue.size();
		else
			return 0;
	}

	uint32_t CStateSignalSlot::getTotalSignalQueueSizeInternal()
	{
		return m_nSignalQueueSize;
	}


	bool CStateSignalSlot::eraseMessage(const std::string& sUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		bool bErased = false;
		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			if (auto iQueueIterator = m_QueueMap.find(sNormalizedUUID); iQueueIterator != m_QueueMap.end()) {
				m_Queue.erase(iQueueIterator->second);
				m_QueueMap.erase(iQueueIterator);
			}
			m_InProcess.erase(sNormalizedUUID);
			m_Handled.erase(sNormalizedUUID);
			m_Failed.erase(sNormalizedUUID);
			m_TimedOut.erase(sNormalizedUUID);
			m_Cleared.erase(sNormalizedUUID);

			bErased = m_MessageMap.erase(sNormalizedUUID) > 0;
		}

		m_pRegistry->unregisterMessage(sNormalizedUUID);

		return bErased;

	}

	size_t CStateSignalSlot::clearQueueInternal()
	{

		size_t nCount = 0;
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		while (m_Queue.size() > 0) {
			auto pMessage = m_Queue.front();
			std::string sUUID = pMessage->getUUID();
			m_Queue.pop_front();
			m_QueueMap.erase(sUUID);

			pMessage->setPhase(AMC::eAMCSignalPhase::Cleared);
			m_Cleared.insert(sUUID);

			nCount++;
			

		}

		return nCount;

	}

	PStateSignalMessage CStateSignalSlot::addNewInQueueSignalInternal(const std::string& sMessageUUID, const std::string& sParameterData, uint32_t nReactionTimeoutInMS, uint64_t nTimeStamp)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);
		PStateSignalMessage pMessage;

		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			if (queueIsFullNoMutex()) {
				// Queue is full, cannot add new signal
				return nullptr;
			}

			auto iIterator = m_MessageMap.find(sNormalizedUUID);
			if (iIterator != m_MessageMap.end()) {
				// Signal already exists, cannot add again
				return nullptr;
			}

			pMessage = std::make_shared<CStateSignalMessage>(sNormalizedUUID, nReactionTimeoutInMS, m_pQueueTelemetryChannel, m_pProcessingTelemetryChannel, m_pAcknowledgementTelemetryChannel);
			pMessage->setParameterDataJSON(sParameterData);

			// should be outside the lock, but we need to make sure that registration and addition are atomic
			try {
				m_pRegistry->registerMessage(sNormalizedUUID, this);
			}
			catch (...) {
				eraseMessage(sNormalizedUUID);
				throw;
			};

			increaseTriggerCount();
			m_MessageMap.insert(std::make_pair(sNormalizedUUID, pMessage));

			// Adding to queue will start the signal processing
			m_Queue.push_back(pMessage);
			m_QueueMap.insert(std::make_pair(sNormalizedUUID, std::prev(m_Queue.end())));

			
		}

		return pMessage;
	}

	bool CStateSignalSlot::changeSignalPhaseToInProcessInternal(const std::string& sMessageUUID, uint64_t nTimeStamp)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);
		AMC::eAMCSignalPhase messagePhase = pMessage->getPhase();

		if (messagePhase == eAMCSignalPhase::InQueue) {

			auto iQueueIter = m_QueueMap.find(sNormalizedUUID);
			if (iQueueIter == m_QueueMap.end())
				return false;

			m_Queue.erase(iQueueIter->second);
			m_QueueMap.erase(iQueueIter);

			pMessage->setPhase(eAMCSignalPhase::InProcess);
			m_InProcess.insert(sNormalizedUUID);

			updateTimingStatistics();

			return true;
		}

		return false;		

	}

	bool CStateSignalSlot::changeSignalPhaseToHandledInternal(const std::string & sMessageUUID, const std::string& sResultData, uint64_t nTimeStamp)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);
		AMC::eAMCSignalPhase messagePhase = pMessage->getPhase();

		if (messagePhase == eAMCSignalPhase::InQueue) {

			auto iQueueIter = m_QueueMap.find(sNormalizedUUID);
			if (iQueueIter == m_QueueMap.end())
				return false;

			m_Queue.erase(iQueueIter->second);
			m_QueueMap.erase(iQueueIter);

			pMessage->setResultDataJSON(sResultData);
			pMessage->setPhase(eAMCSignalPhase::Handled);
			m_Handled.insert(sNormalizedUUID);

			increaseHandledCount();

			return true;
		}

		if (messagePhase == eAMCSignalPhase::InProcess) {
			pMessage->setResultDataJSON(sResultData);
			pMessage->setPhase(eAMCSignalPhase::Handled);
			m_Handled.insert(sNormalizedUUID);
			m_InProcess.erase(sNormalizedUUID);

			increaseHandledCount();

			return true;
		}

		return false;

	}

	bool CStateSignalSlot::changeSignalPhaseToArchivedInternal(const std::string& sMessageUUID, uint64_t nTimeStamp, bool bFailIfNotExisting)
	{

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);
		bool bToArchive = false;

		{

			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			auto iMessageIter = m_MessageMap.find(sNormalizedUUID);
			if (iMessageIter == m_MessageMap.end()) {
				if (bFailIfNotExisting)
					throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "getMessageByUUIDNoMutex: Signal UUID not found: " + sNormalizedUUID);

				return false;
			}

			auto pMessage = iMessageIter->second;
			AMC::eAMCSignalPhase messagePhase = pMessage->getPhase();


			// Only failed, handled, cleared and timedout signals can be archived
			if (messagePhase == eAMCSignalPhase::Failed) {
				m_Failed.erase(sNormalizedUUID);
				bToArchive = true;
			}

			if (messagePhase == eAMCSignalPhase::Handled) {
				m_Handled.erase(sNormalizedUUID);
				bToArchive = true;
			}

			if (messagePhase == eAMCSignalPhase::TimedOut) {
				m_TimedOut.erase(sNormalizedUUID);
				bToArchive = true;
			}

			if (messagePhase == eAMCSignalPhase::Cleared) {
				m_Cleared.erase(sNormalizedUUID);
				bToArchive = true;
			}

			if (bToArchive) {

				increaseArchivedCount();

				pMessage->setPhase(eAMCSignalPhase::Archived);
				m_MessagesToArchive.push_back(pMessage);
				m_MessageMap.erase(iMessageIter);
			}

			updateTimingStatistics();
		}

		if (bToArchive) {
			m_pRegistry->unregisterMessage(sNormalizedUUID);
		}

		return bToArchive;

	}

	bool CStateSignalSlot::changeSignalPhaseToInFailedInternal(const std::string& sMessageUUID, const std::string& sResultData, const std::string& sErrorMessage, uint64_t nTimeStamp)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);
		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);
		AMC::eAMCSignalPhase messagePhase = pMessage->getPhase();

		if (messagePhase == eAMCSignalPhase::InQueue) {
			auto iQueueIter = m_QueueMap.find(sNormalizedUUID);
			if (iQueueIter == m_QueueMap.end())
				return false;
			m_Queue.erase(iQueueIter->second);
			m_QueueMap.erase(iQueueIter);
			
			pMessage->setResultDataJSON(sResultData);
			pMessage->setPhase(eAMCSignalPhase::Failed);
			pMessage->setErrorMessage(sErrorMessage);
			m_Failed.insert(sNormalizedUUID);

			increaseFailedCount();

			return true;
		}

		if (messagePhase == eAMCSignalPhase::InProcess) {
			pMessage->setPhase(eAMCSignalPhase::Failed);
			pMessage->setResultDataJSON(sResultData);
			pMessage->setErrorMessage(sErrorMessage);
			m_Failed.insert(sNormalizedUUID);
			m_InProcess.erase(sNormalizedUUID);

			increaseFailedCount();

			return true;
		}

		return false;

	}

	AMC::eAMCSignalPhase CStateSignalSlot::getSignalPhaseInternal(const std::string& sMessageUUID)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);
		return pMessage->getPhase();
	}


	
	uint32_t CStateSignalSlot::getDefaultReactionTimeoutInternal()
	{
		return m_nSignalDefaultReactionTimeOutInMS;
	}

	uint32_t CStateSignalSlot::getReactionTimeoutInternal(const std::string& sMessageUUID)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);

		return pMessage->getReactionTimeoutInMS();

	}

	void CStateSignalSlot::checkForReactionTimeoutsNoMutex(uint64_t nGlobalTimestamp)
	{
		auto it = m_Queue.begin();
		while (it != m_Queue.end()) {
			auto pMessage = (*it);
			if (pMessage->hadReactionTimeout(nGlobalTimestamp)) {
				// Signal has timed out
				std::string sUUID = pMessage->getUUID();
				pMessage->setPhase(eAMCSignalPhase::TimedOut);
				m_TimedOut.insert(sUUID);

				it = m_Queue.erase(it);
				m_QueueMap.erase(sUUID);

				increaseTimeoutCount();

			}
			else {
				++it;
			}
		}


	}

	void CStateSignalSlot::checkForReactionTimeouts(uint64_t nGlobalTimestamp)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		checkForReactionTimeoutsNoMutex(nGlobalTimestamp);
	}

	void CStateSignalSlot::autoArchiveMessages(uint64_t nGlobalTimestamp)
	{
		std::deque <std::string> messagesToArchive;
		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);
			for (auto& it : m_MessageMap) {
				auto pMessage = it.second;
				AMC::eAMCSignalPhase messagePhase = pMessage->getPhase();

				if ((messagePhase == eAMCSignalPhase::Handled) ||
					(messagePhase == eAMCSignalPhase::Failed) ||
					(messagePhase == eAMCSignalPhase::TimedOut) ||
					(messagePhase == eAMCSignalPhase::Cleared)) {
					uint64_t nArchiveTimestamp = 0;
					uint64_t nMessageTerminalTimestamp = pMessage->getTerminalTimestamp();

					if (nMessageTerminalTimestamp > 0) {
						nArchiveTimestamp = nMessageTerminalTimestamp + (uint64_t)m_nSignalAutomaticArchiveTimeInMS * 1000;
					}
					else {
						// This should never happen, but as a fallback, we use the creation timestamp
						nArchiveTimestamp = pMessage->getCreationTimestamp() + (uint64_t)m_nSignalAutomaticArchiveTimeInMS * 1000;
					}

					if (nGlobalTimestamp >= nArchiveTimestamp) {
						increaseArchivedCount();
						messagesToArchive.push_back(it.first);
					}
				}
			}
		}
		for (auto& sUUID : messagesToArchive) {
			changeSignalPhaseToArchivedInternal(sUUID, nGlobalTimestamp, false);
		}
	}


	void CStateSignalSlot::writeMessagesToArchive(CStateSignalArchiveWriter* pArchiveWriter)
	{

		std::deque <PStateSignalMessage> messagesToArchive;
		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);
			messagesToArchive.swap (m_MessagesToArchive);
		}

		if (pArchiveWriter != nullptr) {
			// If Archive Writer is null, we just clear the archive queue

			for (auto pMessage : messagesToArchive) {
				pArchiveWriter->writeSignalMessageToArchive(m_sInstanceName, m_sName, pMessage.get());
			}

		}

	}


	PStateSignalMessage CStateSignalSlot::claimMessageFromQueueInternal(bool bCheckForReactionTimeout, uint64_t nGlobalTimestamp, uint64_t nTimeStamp, bool bChangePhaseToInprocess)
	{
		(void)nTimeStamp;
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (bCheckForReactionTimeout) {
			checkForReactionTimeoutsNoMutex(nGlobalTimestamp);
		}

		if (m_Queue.empty())
			return nullptr;

		auto pMessage = m_Queue.front();
		std::string sUUID = pMessage->getUUID();

		if (bChangePhaseToInprocess) {

			// Change the phase to Inprocess in an atomic way
			m_Queue.pop_front();
			m_QueueMap.erase(sUUID);

			pMessage->setPhase(eAMCSignalPhase::InProcess);
			m_InProcess.insert(sUUID);

			updateTimingStatistics();

		}

		return pMessage;
	}


	std::string CStateSignalSlot::getResultDataJSONInternal(const std::string& sMessageUUID)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);
		
		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);

		return pMessage->getResultDataJSON();

	}

	std::string CStateSignalSlot::getParameterDataJSONInternal(const std::string& sMessageUUID)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);

		return pMessage->getParameterDataJSON();

	}

	bool CStateSignalSlot::getParameterPropertiesInternal(const std::string& sMessageUUID, std::string& sInstanceName, std::string& sSignalName, std::string& sParameterDataJSON)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		auto pMessage = getMessageByUUIDNoMutex(sNormalizedUUID);

		sInstanceName = m_sInstanceName;
		sSignalName = m_sName;
		sParameterDataJSON = pMessage->getParameterDataJSON();

		return true;

	}


	void CStateSignalSlot::populateParameterGroup(CParameterGroup* pParameterGroup)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		LibMCAssertNotNull(pParameterGroup);

		for (auto & Definition : m_ParameterDefinitions) {
			std::string sType = Definition.getType();
			std::string sName = Definition.getName();
			pParameterGroup->addNewTypedParameter(sName, sType, "", "", "");			
		}

	}

	void CStateSignalSlot::populateResultGroup(CParameterGroup* pResultGroup)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		LibMCAssertNotNull(pResultGroup);

		for (auto & Definition : m_ResultDefinitions) {
			std::string sType = Definition.getType();
			std::string sName = Definition.getName();
			pResultGroup->addNewTypedParameter(sName, sType, "", "", "");
		}
	}


}
