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

#include "amc_statesignal.hpp"

#include "libmc_exceptiontypes.hpp"

#include "common_utils.hpp"

#include "amc_telemetry.hpp"

namespace AMC {
	

	CStateSignalInstance::CStateSignalInstance(const std::string& sInstanceName, CStateSignalRegistry* pRegistry)
		: m_sInstanceName (sInstanceName), m_pRegistry (pRegistry)
	{

		if (pRegistry == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		if (sInstanceName.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYSIGNALINSTANCENAME);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sInstanceName))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSIGNALINSTANCENAME, sInstanceName);

	}

	CStateSignalInstance::~CStateSignalInstance()
	{

	}

	std::string CStateSignalInstance::getInstanceName() const
	{
		return m_sInstanceName;
	}

	PStateSignalSlot CStateSignalInstance::getSignalSlot(const std::string& sSignalName)
	{
		std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
		auto iIter = m_Slots.find(sSignalName);
		if (iIter == m_Slots.end())
			throw ELibMCCustomException(LIBMC_ERROR_COULDNOTFINDSIGNAL, sSignalName);

		return iIter->second;
	}

	PStateSignalSlot CStateSignalInstance::getSignalSlotOrNull(const std::string& sSignalName)
	{
		std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
		auto iIter = m_Slots.find(sSignalName);
		if (iIter == m_Slots.end())
			return nullptr;

		return iIter->second;

	}


	bool CStateSignalInstance::hasSignalDefinition(const std::string& sSignalName)
	{
		std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
		auto iIter = m_Slots.find(sSignalName);
		return (iIter != m_Slots.end());

	}

	PStateSignalSlot CStateSignalInstance::addSignalDefinition(const std::string& sSignalName, const std::vector<CStateSignalParameter>& Parameters, const std::vector<CStateSignalParameter>& Results, uint32_t nSignalReactionTimeOutInMS, uint32_t nAutomaticArchiveTimeInMS, uint32_t nSignalQueueSize, PParameterGroup pSignalInformationGroup)
	{
		if (sSignalName.empty())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSIGNALNAME, sSignalName);

		std::lock_guard<std::mutex> lockGuard(m_SlotMutex);

		auto iter = m_Slots.find(sSignalName);
		if (iter != m_Slots.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESIGNAL, m_sInstanceName + "/" + sSignalName);

		auto pSignal = std::make_shared<CStateSignalSlot>(m_sInstanceName, sSignalName, Parameters, Results, nSignalReactionTimeOutInMS, nAutomaticArchiveTimeInMS, nSignalQueueSize, pSignalInformationGroup, m_pRegistry);
		m_Slots.emplace(sSignalName, pSignal);

		return pSignal;

	}


	bool CStateSignalInstance::addNewInQueueSignal(const std::string& sSignalName, const std::string& sSignalUUID, const std::string& sParameterData, uint32_t nResponseTimeOutInMS, uint64_t nTimestamp)
	{

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);
		if (m_pRegistry->findSignalSlotOfMessage(sNormalizedUUID) != nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALALREADYTRIGGERED, sNormalizedUUID);

		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		auto pMessage = pSlot->addNewInQueueSignalInternal(sNormalizedUUID, sParameterData, nResponseTimeOutInMS, nTimestamp);
		if (pMessage.get() != nullptr) {
			return true;
		}

		return false;

	}

	void CStateSignalInstance::clearUnhandledSignals(uint64_t nTimestamp)
	{
		std::vector<AMC::PStateSignalSlot> slotList;
		{
			slotList.reserve(m_Slots.size());

			std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
			for (auto it = m_Slots.begin(); it != m_Slots.end(); it++) {
				slotList.push_back(it->second);
			}
		}


		for (auto pSlot : slotList) {
			pSlot->clearQueueInternal();
		}


	}

	void CStateSignalInstance::clearUnhandledSignalsOfType(const std::string& sSignalTypeName, uint64_t nTimestamp)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalTypeName);
		pSlot->clearQueueInternal();
	}

	bool CStateSignalInstance::canTrigger(const std::string& sSignalName)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);
		return !pSlot->queueIsFull();
	}

	bool CStateSignalInstance::queueIsEmpty(const std::string& sSignalName)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);
		return pSlot->queueIsEmpty();
	}



	bool CStateSignalInstance::claimSignalMessage(const std::string& sSignalName, bool bCheckForReactionTimeout, uint64_t nGlobalTimestamp, uint64_t nTimeStamp, std::string& sSignalUUID, std::string& sParameterDataJSON, bool bChangePhaseToInprocess)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		auto pMessage = pSlot->claimMessageFromQueueInternal(bCheckForReactionTimeout, nGlobalTimestamp, nTimeStamp, bChangePhaseToInprocess);
		if (pMessage.get() == nullptr)
			return false;

		sSignalUUID = pMessage->getUUID();
		sParameterDataJSON = pMessage->getParameterDataJSON();
		return true;
	}


	uint32_t CStateSignalInstance::getAvailableSignalQueueEntryCount(const std::string& sSignalName)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		return pSlot->getAvailableSignalQueueEntriesInternal();
	}

	uint32_t CStateSignalInstance::getTotalSignalQueueSize(const std::string& sSignalName)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		return pSlot->getTotalSignalQueueSizeInternal();
	}

	uint32_t CStateSignalInstance::getDefaultReactionTimeout(const std::string& sSignalName)
	{
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		return pSlot->getDefaultReactionTimeoutInternal();
	}

	void CStateSignalInstance::populateParameterGroup(const std::string& sSignalName, CParameterGroup* pParameterGroup)
	{
		LibMCAssertNotNull(pParameterGroup);
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		pSlot->populateParameterGroup(pParameterGroup);

	}

	void CStateSignalInstance::populateResultGroup(const std::string& sSignalName, CParameterGroup* pResultGroup)
	{
		LibMCAssertNotNull(pResultGroup);
		AMC::PStateSignalSlot pSlot = getSignalSlot(sSignalName);

		pSlot->populateResultGroup(pResultGroup);

	}

	void CStateSignalInstance::checkForReactionTimeouts(uint64_t nGlobalTimestamp)
	{
		std::vector<PStateSignalSlot> slots;
		{
			std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
			for (auto iIterator : m_Slots) {
				slots.push_back(iIterator.second);
			}
		}

		for (auto pSlot : slots) {
			pSlot->checkForReactionTimeouts(nGlobalTimestamp);
		}
	}

	void CStateSignalInstance::autoArchiveMessages(uint64_t nGlobalTimestamp)
	{
		std::vector<PStateSignalSlot> slots;
		{
			std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
			for (auto iIterator : m_Slots) {
				slots.push_back(iIterator.second);
			}
		}

		for (auto pSlot : slots) {
			pSlot->autoArchiveMessages(nGlobalTimestamp);
		}

	}

	void CStateSignalInstance::writeMessagesToArchive(CStateSignalArchiveWriter* pArchiveWriter)
	{
		std::vector<PStateSignalSlot> slots;
		{
			std::lock_guard<std::mutex> lockGuard(m_SlotMutex);
			for (auto iIterator : m_Slots) {
				slots.push_back(iIterator.second);
			}
		}

		for (auto pSlot : slots) {
			pSlot->writeMessagesToArchive(pArchiveWriter);
		}

	}

	
	CStateSignalHandler::CStateSignalHandler(PTelemetryHandler pTelemetryHandler)
		: m_pTelemetryHandler(pTelemetryHandler)
	{
		if (pTelemetryHandler.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}
	

	CStateSignalHandler::~CStateSignalHandler()
	{
	}
	

	PStateSignalInstance CStateSignalHandler::registerInstance(const std::string& sInstanceName)
	{
		std::lock_guard<std::mutex> lockGuard(m_SignalInstanceMutex);

		auto iIter = m_Instances.find(sInstanceName);
		if (iIter != m_Instances.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESIGNALINSTANCENAME, sInstanceName);

		auto pInstance = std::make_shared<CStateSignalInstance>(sInstanceName, this);

		m_Instances.emplace(sInstanceName, pInstance);

		return pInstance;
	}

	PStateSignalInstance CStateSignalHandler::getInstance(const std::string& sInstanceName)
	{
		std::lock_guard<std::mutex> lockGuard(m_SignalInstanceMutex);

		auto iIter = m_Instances.find(sInstanceName);
		if (iIter == m_Instances.end())
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALINSTANCENOTFOUND, sInstanceName);

		return iIter->second;
	}

	void CStateSignalHandler::registerMessage(const std::string& sMessageUUID, CStateSignalSlot* pSignalSlot)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString (sMessageUUID);

		if (pSignalSlot == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::string sInstanceName = pSignalSlot->getInstanceNameInternal();
		std::string sSignalName = pSignalSlot->getNameInternal();

		auto pInstance = getInstance(sInstanceName);
		auto pSlot = pInstance->getSignalSlot(sSignalName);
		{
			std::lock_guard<std::mutex> lockGuard(m_MessageMapMutex);
			auto insertResult = m_MessageSlotMap.emplace(sNormalizedUUID, pSlot);
			if (!insertResult.second)
				throw ELibMCCustomException(LIBMC_ERROR_SIGNALALREADYTRIGGERED, sNormalizedUUID);
		}

	}

	void CStateSignalHandler::unregisterMessage(const std::string& sMessageUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);

		std::lock_guard<std::mutex> lockGuard(m_MessageMapMutex);

		m_MessageSlotMap.erase(sNormalizedUUID);

	}

	PStateSignalSlot CStateSignalHandler::findSignalSlotOfMessage(const std::string& sMessageUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sMessageUUID);
		std::weak_ptr<CStateSignalSlot> pwSlot;

		std::lock_guard<std::mutex> lockGuard(m_MessageMapMutex);

		auto iMessageIter = m_MessageSlotMap.find(sNormalizedUUID);
		if (iMessageIter == m_MessageSlotMap.end())
			return nullptr;

		pwSlot = iMessageIter->second;

		auto pSlot = pwSlot.lock();
		if (pSlot.get() == nullptr) {
			// Slot has been deleted, prune dead entry
			m_MessageSlotMap.erase(iMessageIter);
		}

		return pSlot;

	}

	PTelemetryChannel CStateSignalHandler::registerTelemetryChannel(const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType)
	{
		return m_pTelemetryHandler->registerChannel(sChannelIdentifier, sChannelDescription, channelType);
	}


	bool CStateSignalHandler::finalizeSignal(const std::string& sUUID) {

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

		PStateSignalSlot pSlot = findSignalSlotOfMessage (sNormalizedUUID);
		if (pSlot != nullptr)
			return pSlot->eraseMessage(sNormalizedUUID);

		return false;
	}


	void CStateSignalHandler::changeSignalPhaseToHandled(const std::string& sSignalUUID, const std::string& sResultData, uint64_t nTimestamp)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "signal not found while changing phase to handled (" + sNormalizedUUID + ")");

		pSlot->changeSignalPhaseToHandledInternal(sNormalizedUUID, sResultData, nTimestamp);
	}

	void CStateSignalHandler::changeSignalPhaseToInProcess(const std::string& sSignalUUID, uint64_t nTimestamp)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "signal not found while changing phase to inprocess (" + sNormalizedUUID + ")");

		pSlot->changeSignalPhaseToInProcessInternal (sNormalizedUUID, nTimestamp);
	}

	void CStateSignalHandler::changeSignalPhaseToFailed(const std::string& sSignalUUID, const std::string& sResultData, const std::string& sErrorMessage, uint64_t nTimestamp)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "signal not found while changing phase to failed (" + sNormalizedUUID + ")");

		pSlot->changeSignalPhaseToInFailedInternal(sNormalizedUUID, sResultData, sErrorMessage, nTimestamp);
	}

	AMC::eAMCSignalPhase CStateSignalHandler::getSignalPhase(const std::string& sSignalUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "signal not found while getting signal phase (" + sNormalizedUUID + ")");


		return pSlot->getSignalPhaseInternal (sNormalizedUUID);
	}


	void CStateSignalHandler::checkForReactionTimeouts(uint64_t nGlobalTimestamp)
	{
		std::vector<AMC::PStateSignalInstance> instances;
		{
			instances.reserve(m_Instances.size());

			std::lock_guard<std::mutex> lockGuard(m_SignalInstanceMutex);
			for (auto iIter : m_Instances)
				instances.push_back(iIter.second);
		}

		for (auto pInstance : instances) {
			pInstance->checkForReactionTimeouts(nGlobalTimestamp);
		}
	}

	void CStateSignalHandler::autoArchiveMessages(uint64_t nGlobalTimestamp)
	{

		std::vector<AMC::PStateSignalInstance> instances;
		{
			instances.reserve(m_Instances.size());

			std::lock_guard<std::mutex> lockGuard(m_SignalInstanceMutex);
			for (auto iIter : m_Instances)
				instances.push_back(iIter.second);
		}

		for (auto pInstance : instances) {
			pInstance->autoArchiveMessages(nGlobalTimestamp);
		}
	}

	void CStateSignalHandler::writeMessagesToArchive(CStateSignalArchiveWriter* pArchiveWriter)
	{
		std::vector<AMC::PStateSignalInstance> instances;
		{
			instances.reserve(m_Instances.size());

			std::lock_guard<std::mutex> lockGuard(m_SignalInstanceMutex);
			for (auto iIter : m_Instances)
				instances.push_back(iIter.second);
		}

		for (auto pInstance : instances) {
			pInstance->writeMessagesToArchive(pArchiveWriter);
		}

	}



	uint32_t CStateSignalHandler::getReactionTimeout(const std::string& sSignalUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		AMC::PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "signal not found while getting reaction timeout (" + sNormalizedUUID + ")");

		return pSlot->getReactionTimeoutInternal(sNormalizedUUID);

	}

	std::string CStateSignalHandler::getResultDataJSON(const std::string& sSignalUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		AMC::PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_SIGNALNOTFOUND, "signal not found while getting result data JSON (" + sNormalizedUUID + ")");

		return pSlot->getResultDataJSONInternal(sNormalizedUUID);
	}

	bool CStateSignalHandler::findSignalPropertiesByUUID(const std::string& sSignalUUID, std::string& sInstanceName, std::string& sSignalName, std::string& sParameterData)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sSignalUUID);

		AMC::PStateSignalSlot pSlot = findSignalSlotOfMessage(sNormalizedUUID);
		if (pSlot != nullptr) {
			return pSlot->getParameterPropertiesInternal(sNormalizedUUID, sInstanceName, sSignalName, sParameterData);
		}

		return false;

	}



}


