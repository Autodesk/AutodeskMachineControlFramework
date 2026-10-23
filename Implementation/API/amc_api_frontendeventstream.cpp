/*++

Copyright (C) 2026 Autodesk Inc.

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

Abstract: This is the class definition of CAPIFrontendEventStream.

*/

#define __AMCIMPL_API_CONSTANTS

#include "amc_api_frontendeventstream.hpp"
#include "amc_api_sessionhandler.hpp"
#include "amc_api_constants.hpp"
#include "amc_systemstate.hpp"
#include "amc_ui_handler.hpp"
#include "amc_statemachinedata.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"
#include "common_utils.hpp"
#include "common_chrono.hpp"

#include <iostream>

using namespace AMC;

_sFrontendEventStreamMetrics::_sFrontendEventStreamMetrics()
	: m_WindowStart(std::chrono::steady_clock::now()),
	m_nWindowStartMicros(0),
	m_nEventCount(0),
	m_dEventSumMS(0.0),
	m_dEventMinMS(0.0),
	m_dEventMaxMS(0.0),
	m_dEventSumSqMS(0.0),
	m_nPayloadSumBytes(0),
	m_nPayloadMaxBytes(0),
	m_dBuildSumMS(0.0)
{
}

_sFrontendEventStreamClient::_sFrontendEventStreamClient()
	: m_bScoped(false),
	m_sScope("*"),
	m_LastUseTime(std::chrono::steady_clock::now())
{
}

CAPIFrontendEventStream::CAPIFrontendEventStream(const std::string& sUUID, PAPIAuth pAuth, std::shared_ptr<CSystemState> pSystemState, std::shared_ptr<CAPISessionHandler> pSessionHandler)
	: CJSONEventStreamInstance(sUUID),
	m_pAuth(pAuth),
	m_pSystemState(pSystemState),
	m_pSessionHandler(pSessionHandler),
	m_LastSessionRefresh(std::chrono::steady_clock::now()),
	m_bMetricsWindowStarted(false)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pSessionHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIFrontendEventStream::~CAPIFrontendEventStream()
{
}

uint64_t CAPIFrontendEventStream::createInitialRevision()
{
	// Event IDs of different streams fall into disjoint ranges with high probability, so that a
	// Last-Event-ID of another session never resumes from this log. IDs stay below 2^53 for JavaScript clients.
	std::string sRandomHex = AMCCommon::CUtils::calculateRandomSHA256String(1);
	uint64_t nEpoch = std::stoull(sRandomHex.substr(0, 5), nullptr, 16) + 1;
	return nEpoch << 32;
}

std::string CAPIFrontendEventStream::joinNames(const std::set<std::string>& names)
{
	std::string sJoined;
	for (auto& sName : names) {
		if (!sJoined.empty())
			sJoined += ",";
		sJoined += sName;
	}
	return sJoined;
}

PAPIAuth CAPIFrontendEventStream::getAuth()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	return m_pAuth;
}

void CAPIFrontendEventStream::removeExpiredClientsNoLock(std::chrono::steady_clock::time_point now)
{
	for (auto iter = m_Clients.begin(); iter != m_Clients.end(); ) {
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - iter->second.m_LastUseTime).count() >= AMC_API_FRONTENDEVENTSTREAM_CLIENTEXPIRY_MS)
			iter = m_Clients.erase(iter);
		else
			++iter;
	}
}

sFrontendEventStreamClient& CAPIFrontendEventStream::findOrCreateClientNoLock(const std::string& sClientID, std::chrono::steady_clock::time_point now)
{
	auto iIter = m_Clients.find(sClientID);
	if (iIter != m_Clients.end()) {
		iIter->second.m_LastUseTime = now;
		return iIter->second;
	}

	removeExpiredClientsNoLock(now);
	while (m_Clients.size() >= AMC_API_FRONTENDEVENTSTREAM_MAXCLIENTS) {
		auto iOldest = m_Clients.begin();
		for (auto iter = m_Clients.begin(); iter != m_Clients.end(); iter++) {
			if (iter->second.m_LastUseTime < iOldest->second.m_LastUseTime)
				iOldest = iter;
		}
		m_Clients.erase(iOldest);
	}

	sFrontendEventStreamClient client;
	client.m_pRevisionLog = std::make_shared<CUIFrontendRevisionLog>(AMC_UI_FRONTEND_REVISIONLOG_DEPTH, createInitialRevision());
	client.m_LastUseTime = now;

	return m_Clients.insert(std::make_pair(sClientID, client)).first->second;
}

sFrontendEventStreamClient CAPIFrontendEventStream::useClient(const std::string& sClientID)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	return findOrCreateClientNoLock(sClientID, std::chrono::steady_clock::now());
}

void CAPIFrontendEventStream::setClientScope(const std::string& sClientID, const std::set<std::string>& activePageNames, const std::set<std::string>& activeDialogNames)
{
	if (!AMCCommon::CUtils::stringIsUUIDString(sClientID))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto& client = findOrCreateClientNoLock(sClientID, std::chrono::steady_clock::now());
		client.m_bScoped = true;
		client.m_ActivePageNames = activePageNames;
		client.m_ActiveDialogNames = activeDialogNames;
		client.m_sScope = joinNames(activePageNames) + "|" + joinNames(activeDialogNames);
	}

	notifyChange();
}

bool CAPIFrontendEventStream::refreshSessionIfDue(const std::string& sSessionUUID)
{
	auto now = std::chrono::steady_clock::now();
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastSessionRefresh).count() < AMC_API_FRONTENDEVENTSTREAM_SESSIONREFRESHINTERVAL_MS)
			return true;
		m_LastSessionRefresh = now;
	}

	auto pSessionHandler = m_pSessionHandler.lock();
	if (pSessionHandler.get() == nullptr)
		return false;

	return pSessionHandler->refreshSessionActivity(sSessionUUID);
}

void CAPIFrontendEventStream::endStream()
{
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		m_pAuth = nullptr;
	}

	CJSONEventStreamInstance::endStream();
}

std::string CAPIFrontendEventStream::waitForNextEvent(sJSONEventStreamCursor& cursor)
{
	bool bNotified = true;
	if (cursor.m_bStarted) {
		uint64_t nChangeCounter = waitForChange(cursor.m_nChangeCounter, AMC_API_FRONTENDEVENTSTREAM_BUILDINTERVAL_MS);
		bNotified = (nChangeCounter != cursor.m_nChangeCounter);
		cursor.m_nChangeCounter = nChangeCounter;
	}
	else {
		cursor.m_nChangeCounter = getChangeCounter();
		cursor.m_bStarted = true;
	}

	if (!isActive())
		return "";

	auto pAuth = getAuth();
	auto pSystemState = m_pSystemState.lock();
	if ((pAuth.get() == nullptr) || (pSystemState.get() == nullptr)) {
		endStream();
		return "";
	}

	std::string sSessionUUID = pAuth->getSessionUUID();
	if (!refreshSessionIfDue(sSessionUUID)) {
		endStream();
		return "";
	}

	// The counter is read before the build, so that a change during the build triggers another one.
	auto buildStart = std::chrono::steady_clock::now();
	uint64_t nDataChangeCounter = pSystemState->stateMachineData()->getFrontendChangeCounter()->get();
	bool bBuildDue = std::chrono::duration_cast<std::chrono::milliseconds>(buildStart - cursor.m_LastBuildTime).count() >= AMC_API_FRONTENDEVENTSTREAM_MAXBUILDINTERVAL_MS;
	if (!bNotified && !bBuildDue && (nDataChangeCounter == cursor.m_nDataChangeCounter))
		return "";

	cursor.m_nDataChangeCounter = nDataChangeCounter;
	cursor.m_LastBuildTime = buildStart;
	sUIFrontendBuildEpoch epoch(nDataChangeCounter, sUIFrontendBuildEpoch::currentTimeSlot());

	std::string sEvent = buildNextEvent(cursor, pAuth, pSystemState, epoch);
	double dBuildMS = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - buildStart).count();

	recordMetrics(dBuildMS, sEvent.length(), sSessionUUID, pSystemState);

	return sEvent;
}

void CAPIFrontendEventStream::recordMetrics(double dBuildMS, size_t nEventBytes, const std::string& sSessionUUID, std::shared_ptr<CSystemState> pSystemState)
{
	auto now = std::chrono::steady_clock::now();
	sFrontendEventStreamMetrics completedWindow;
	bool bWindowCompleted = false;

	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (!m_bMetricsWindowStarted) {
			m_Metrics = sFrontendEventStreamMetrics();
			m_Metrics.m_WindowStart = now;
			m_Metrics.m_nWindowStartMicros = pSystemState->globalChrono()->getUTCTimeStampInMicrosecondsSince1970();
			m_bMetricsWindowStarted = true;
		}

		m_Metrics.m_dBuildSumMS += dBuildMS;

		if (nEventBytes > 0) {
			if ((m_Metrics.m_nEventCount == 0) || (dBuildMS < m_Metrics.m_dEventMinMS))
				m_Metrics.m_dEventMinMS = dBuildMS;
			if ((m_Metrics.m_nEventCount == 0) || (dBuildMS > m_Metrics.m_dEventMaxMS))
				m_Metrics.m_dEventMaxMS = dBuildMS;

			m_Metrics.m_nEventCount++;
			m_Metrics.m_dEventSumMS += dBuildMS;
			m_Metrics.m_dEventSumSqMS += dBuildMS * dBuildMS;
			m_Metrics.m_nPayloadSumBytes += nEventBytes;
			if (nEventBytes > m_Metrics.m_nPayloadMaxBytes)
				m_Metrics.m_nPayloadMaxBytes = nEventBytes;
		}

		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_Metrics.m_WindowStart).count() >= AMC_API_FRONTENDEVENTSTREAM_METRICSWINDOW_MS) {
			completedWindow = m_Metrics;
			m_bMetricsWindowStarted = false;
			bWindowCompleted = true;
		}
	}

	if (!bWindowCompleted)
		return;

	// Metrics are best effort and must never end the stream.
	try {
		auto pChrono = pSystemState->globalChrono();
		uint64_t nNowMicros = pChrono->getUTCTimeStampInMicrosecondsSince1970();

		auto pMetricsHandler = pSystemState->getDataModelInstance()->CreateSessionMetricsHandler();
		pMetricsHandler->AddFrontendMetrics(sSessionUUID, AMC_API_FRONTENDEVENTSTREAM_METRICSLABEL, completedWindow.m_nWindowStartMicros, nNowMicros,
			completedWindow.m_nEventCount, completedWindow.m_dEventSumMS, completedWindow.m_dEventMinMS, completedWindow.m_dEventMaxMS, completedWindow.m_dEventSumSqMS,
			completedWindow.m_nPayloadSumBytes, completedWindow.m_nPayloadMaxBytes, completedWindow.m_dBuildSumMS, nNowMicros);
	}
	catch (std::exception& E) {
		std::cout << "frontend event stream: could not record session metrics: " << E.what() << std::endl;
	}
}

std::string CAPIFrontendEventStream::buildNextEvent(sJSONEventStreamCursor& cursor, PAPIAuth pAuth, std::shared_ptr<CSystemState> pSystemState, const sUIFrontendBuildEpoch& epoch)
{
	auto client = useClient(cursor.m_sClientID);

	CJSONWriter statusWriter;
	if (client.m_bScoped)
		pSystemState->uiHandler()->frontendWriteStatusToJSON(statusWriter, pAuth.get(), &client.m_ActivePageNames, &client.m_ActiveDialogNames, epoch);
	else
		pSystemState->uiHandler()->frontendWriteStatusToJSON(statusWriter, pAuth.get(), nullptr, nullptr, epoch);

	CUIFrontendSnapshot snapshot;
	snapshot.readFromFrontendJSON(statusWriter.getDocument());

	// A changed scope starts a new lineage in the log, so the client receives a snapshot.
	uint64_t nBaseRevision = cursor.m_nLastEventID;
	auto publishResult = client.m_pRevisionLog->publish(snapshot, client.m_sScope, nBaseRevision);

	if (publishResult.m_bPatchAvailable) {
		if (publishResult.m_nRevision == nBaseRevision)
			return "";

		CJSONWriter patchWriter;
		patchWriter.addInteger(AMC_API_KEY_FRONTEND_REVISION, (int64_t)publishResult.m_nRevision);
		patchWriter.addInteger(AMC_API_KEY_FRONTEND_BASE, (int64_t)nBaseRevision);

		CJSONWriterObject changedObject(patchWriter);
		publishResult.m_Patch.writeToJSON(patchWriter, changedObject);
		patchWriter.addObject(AMC_API_KEY_FRONTEND_CHANGED, changedObject);

		cursor.m_nLastEventID = publishResult.m_nRevision;
		return formatEvent(AMC_API_FRONTENDEVENTSTREAM_EVENT_PATCH, publishResult.m_nRevision, patchWriter.saveToString());
	}

	CJSONWriter snapshotWriter;
	snapshotWriter.addInteger(AMC_API_KEY_FRONTEND_REVISION, (int64_t)publishResult.m_nRevision);

	CJSONWriterObject storesObject(snapshotWriter);
	snapshot.writeToJSON(snapshotWriter, storesObject);
	snapshotWriter.addObject(AMC_API_KEY_FRONTEND_STORES, storesObject);

	cursor.m_nLastEventID = publishResult.m_nRevision;
	return formatEvent(AMC_API_FRONTENDEVENTSTREAM_EVENT_SNAPSHOT, publishResult.m_nRevision, snapshotWriter.saveToString());
}
