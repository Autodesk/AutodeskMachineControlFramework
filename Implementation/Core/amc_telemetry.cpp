/*++

Copyright (C) 2023 Autodesk Inc.

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


#include "amc_telemetry.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"

#include "libmcdata_dynamic.hpp"

#define TELEMETRY_DEFAULT_CHUNKINTERVAL_MICROSECONDS 60000000 // 60 seconds
#define TELEMETRY_ONEHOURINMICROSECONDS 3600000000ULL

namespace AMC {

	CTelemetryDataChunk::CTelemetryDataChunk(CTelemetryWriter* pWriter, uint64_t nChunkID, uint64_t nChunkStartTimestamp, uint64_t nChunkEndTimestamp)
		: m_nTelemetryChunkID(nChunkID), m_nMinTimestamp(0), m_nMaxTimestamp(0), m_nMinMarkerID(0), m_nMaxMarkerID(0),
		m_pWriter(pWriter), m_nChunkStartTimestamp(nChunkStartTimestamp), m_nChunkEndTimestamp(nChunkEndTimestamp), m_bChunkIsReadonly (false), m_bChunkHasBeenArchived (false)
	{
		if (pWriter == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		if (nChunkStartTimestamp >= nChunkEndTimestamp)
			throw ELibMCInterfaceException(LIBMC_ERROR_TELEMETRYCHUNKSTARTTIMESTAMPAFTEREND);

	}

	CTelemetryDataChunk::~CTelemetryDataChunk()
	{

	}

	uint64_t CTelemetryDataChunk::getChunkID() const
	{
		return m_nTelemetryChunkID;
	}

	bool CTelemetryDataChunk::isEmpty() const
	{
		std::lock_guard<std::mutex> lock(m_ChunkMutex);

		return m_Entries.empty();
	}

	void CTelemetryDataChunk::writeEntry(const LibMCData::sTelemetryChunkEntry& entry)
	{
		uint32_t nEntryIndex;

		{
			std::lock_guard<std::mutex> lock(m_ChunkMutex);

			if (m_bChunkIsReadonly)
				throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHUNKISREADONLY, std::to_string(m_nTelemetryChunkID));

			m_Entries.emplace_back(entry);
			// entry index shall be one-based
			nEntryIndex = static_cast<uint32_t> (m_Entries.size());

			if (nEntryIndex == 1) {
				m_nMinMarkerID = entry.m_MarkerID;
				m_nMaxMarkerID = entry.m_MarkerID;
				m_nMinTimestamp = entry.m_TimeStamp;
				m_nMaxTimestamp = entry.m_TimeStamp;
			}
			else {
				if (entry.m_MarkerID < m_nMinMarkerID)
					m_nMinMarkerID = entry.m_MarkerID;
				if (entry.m_MarkerID > m_nMaxMarkerID)
					m_nMaxMarkerID = entry.m_MarkerID;
				if (entry.m_TimeStamp < m_nMinTimestamp)
					m_nMinTimestamp = entry.m_TimeStamp;
				if (entry.m_TimeStamp > m_nMaxTimestamp)
					m_nMaxTimestamp = entry.m_TimeStamp;

			}

		}

		if (entry.m_EntryType == LibMCData::eTelemetryChunkEntryType::IntervalStartMarker) {
			m_pWriter->registerOpenInterval (entry.m_MarkerID, (uint32_t)m_nTelemetryChunkID, nEntryIndex);
		}
		else if (entry.m_EntryType == LibMCData::eTelemetryChunkEntryType::IntervalEndMarker) {
			m_pWriter->eraseOpenInterval(entry.m_MarkerID);
		}

	}

	void CTelemetryDataChunk::makeReadOnly()
	{
		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		m_bChunkIsReadonly = true;
	}

	bool CTelemetryDataChunk::isReadOnly() const
	{
		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		return m_bChunkIsReadonly;
	}

	bool CTelemetryDataChunk::hasBeenArchived() const
	{
		std::lock_guard<std::mutex> lock(m_ChunkMutex);
		return m_bChunkHasBeenArchived;
	}

	void CTelemetryDataChunk::writeToArchive(LibMCData::PTelemetrySession pTelemetrySession)
	{
		if (pTelemetrySession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::vector<LibMCData::sTelemetryChunkEntry> chunkEntries;
		uint64_t nChunkStartTimestamp;
		uint64_t nChunkEndTimestamp;
		uint64_t nChunkID;

		{
			std::lock_guard<std::mutex> lock(m_ChunkMutex);
			if (!m_bChunkIsReadonly)
				throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHUNKSCANONLYBEARCHIVEDIFREADONLY, std::to_string(m_nTelemetryChunkID));

			if (m_bChunkHasBeenArchived)
				return;

			chunkEntries.swap(m_Entries);

			nChunkStartTimestamp = m_nChunkStartTimestamp;
			nChunkEndTimestamp = m_nChunkEndTimestamp;
			nChunkID = m_nTelemetryChunkID;
			m_bChunkHasBeenArchived = true;
		}

		try {
			pTelemetrySession->WriteTelemetryChunk(nChunkID, nChunkStartTimestamp, nChunkEndTimestamp, chunkEntries);
		}
		catch (...) {
			// Restore entries on failure
			std::lock_guard<std::mutex> lock(m_ChunkMutex);
			chunkEntries.swap(m_Entries);
			m_bChunkHasBeenArchived = false;

			// Propagate exception to archiving routine
			throw;
		}



	}



	CTelemetryWriter::CTelemetryWriter(LibMCData::PTelemetrySession pTelemetrySession, AMCCommon::PChrono pGlobalChrono)
		: m_pTelemetrySession(pTelemetrySession), m_nNextMarkerID(1), m_pGlobalChrono (pGlobalChrono),
		m_nChunkIntervalInMicroseconds (TELEMETRY_DEFAULT_CHUNKINTERVAL_MICROSECONDS)
	{
		if (pGlobalChrono.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pTelemetrySession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		extendChunksUntil(1);
	}

	CTelemetryWriter::~CTelemetryWriter()
	{

	}

	PTelemetryDataChunk CTelemetryWriter::getOrCreateChunkByTimestamp(uint64_t nTimestamp)
	{

		uint64_t nChunkIndexZeroBased = nTimestamp / m_nChunkIntervalInMicroseconds;
		uint64_t nChunkIndexOneBased = nChunkIndexZeroBased + 1;

		// Telemetry chunk IDs are one-based
		// telemetry session is per-process-lifetime, and
		// the DB schema expects chunk ranges per session
		extendChunksUntil(nChunkIndexOneBased);

		std::lock_guard<std::mutex> lock(m_ChunkMutex);

		// Telemetry array is zero-based
		auto pChunk = m_Chunks.at (nChunkIndexZeroBased);

		if (pChunk->getChunkID() != nChunkIndexOneBased)
			throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHUNKIDMISMATCH, std::to_string (pChunk->getChunkID()) + " != " + std::to_string (nChunkIndexOneBased));

		return pChunk;

	}

	void CTelemetryWriter::extendChunksUntil(uint64_t nMaxChunkIndexOneBased)
	{
		// Reasonable max chunk index based on current time is maximum an hour in the future!
		uint64_t nReasonableMaxChunkIndex = ((m_pGlobalChrono->getElapsedMicroseconds() + TELEMETRY_ONEHOURINMICROSECONDS) / m_nChunkIntervalInMicroseconds) + 1;
		if (nMaxChunkIndexOneBased > nReasonableMaxChunkIndex)
			throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHUNKINDEXOUTOFRANGE, std::to_string(nMaxChunkIndexOneBased));

		std::lock_guard<std::mutex> lock(m_ChunkMutex);		
		for (uint64_t nZeroBasedIndexToAdd = m_Chunks.size(); nZeroBasedIndexToAdd < nMaxChunkIndexOneBased; nZeroBasedIndexToAdd++) {
			uint64_t nChunkStartTimestamp = nZeroBasedIndexToAdd * m_nChunkIntervalInMicroseconds;
			uint64_t nChunkEndTimestamp = nChunkStartTimestamp + m_nChunkIntervalInMicroseconds - 1;

			uint64_t nOneBasedChunkIndex = nZeroBasedIndexToAdd + 1;

			auto pChunk = std::make_shared<CTelemetryDataChunk>(this, nOneBasedChunkIndex, nChunkStartTimestamp, nChunkEndTimestamp);
			m_Chunks.emplace_back(pChunk);

			// We make all chunks read-only except for the last two chunks (which might get some writing due to timing race conditions)
			if (nZeroBasedIndexToAdd >= 2) {
				uint64_t nZeroBasedMakeReadonly = nZeroBasedIndexToAdd - 2;
				auto pChunkToMakeReadonly = m_Chunks.at(nZeroBasedMakeReadonly);
				pChunkToMakeReadonly->makeReadOnly();

				std::lock_guard<std::mutex> archiveLock(m_ArchiveQueueMutex);
				m_ArchiveQueue.push (pChunkToMakeReadonly);
			}
		}
	}

	void CTelemetryWriter::writeEntry(const LibMCData::sTelemetryChunkEntry& entry)
	{
		auto pChunk = getOrCreateChunkByTimestamp(entry.m_TimeStamp);
		pChunk->writeEntry(entry);

	}

	void CTelemetryWriter::registerOpenInterval(uint64_t nMarkerID, uint32_t nChunkID, uint32_t nChunkEntryIndex)
	{
		std::lock_guard<std::mutex> lock(m_OpenIntervalMutex);
		sTelemetryOpenIntervalMarker openIntervalMarker;
		openIntervalMarker.m_nChunkID = nChunkID;
		openIntervalMarker.m_nEntryIndex = nChunkEntryIndex;
		auto insertResult = m_OpenIntervalMarkers.emplace(nMarkerID, openIntervalMarker);
		if (insertResult.second == false)
			throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYMARKERALREADYREGISTERED, std::to_string(nMarkerID));
	}

	void CTelemetryWriter::eraseOpenInterval(uint64_t nMarkerID)
	{
		std::lock_guard<std::mutex> lock(m_OpenIntervalMutex);
		m_OpenIntervalMarkers.erase(nMarkerID);

	}

	void CTelemetryWriter::createChannelInDB(const std::string& sUUID, LibMCData::eTelemetryChannelType channelType, uint32_t nChannelIndex, const std::string& sChannelIdentifier, const std::string& sChannelDescription)
	{
		std::lock_guard<std::mutex> lock(m_DataMutex);
		m_pTelemetrySession->CreateChannelInDB (sUUID, channelType, nChannelIndex, sChannelIdentifier, sChannelDescription);
	}

	void CTelemetryWriter::archiveOldChunksToDB()
	{

		bool bIsEmpty = false;
		while (!bIsEmpty) {

			PTelemetryDataChunk pChunkToArchive;
			{
				std::lock_guard<std::mutex> archiveLock(m_ArchiveQueueMutex);
				bIsEmpty = m_ArchiveQueue.empty ();
				if (bIsEmpty)
					break;

				pChunkToArchive = m_ArchiveQueue.front();
				m_ArchiveQueue.pop();
			}

			{
				std::lock_guard<std::mutex> lock(m_DataMutex);
				pChunkToArchive->writeToArchive(m_pTelemetrySession);
			}
		}

	}


	uint64_t CTelemetryWriter::createMarkerID()
	{
		return m_nNextMarkerID.fetch_add(1, std::memory_order_relaxed);
	}

	uint64_t CTelemetryWriter::getCurrentTimestamp()
	{
		return m_pGlobalChrono->getElapsedMicroseconds();
	}


	CTelemetryMarker::CTelemetryMarker(WTelemetryChannel pChannel, uint64_t nMarkerID, bool bMarkInstant, uint64_t nContextData)
		: m_pChannel(pChannel), m_nMarkerID(nMarkerID), m_nStartTimestamp(0), m_nFinishTimestamp(0), m_nContextData(nContextData)
	{

		auto pLockedChannel = m_pChannel.lock();
		if (pLockedChannel.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Telemetry Channel is null");

		m_nStartTimestamp = pLockedChannel->getCurrentTimestamp();
		uint32_t nChannelIndex = pLockedChannel->getChannelIndex();
		auto pWriter = pLockedChannel->getWriter();

		if (bMarkInstant) {
			m_nFinishTimestamp = m_nStartTimestamp;
			pWriter->writeEntry({ LibMCData::eTelemetryChunkEntryType::InstantMarker, nChannelIndex, m_nMarkerID, m_nStartTimestamp, m_nContextData });
		}
		else {
			pWriter->writeEntry({ LibMCData::eTelemetryChunkEntryType::IntervalStartMarker, nChannelIndex, m_nMarkerID, m_nStartTimestamp, m_nContextData });

		}

	}

	CTelemetryMarker::~CTelemetryMarker()
	{

	}

	WTelemetryChannel CTelemetryMarker::getChannel() const
	{
		return m_pChannel;
	}

	void CTelemetryMarker::finishMarker()
	{
		auto pLockedChannel = m_pChannel.lock();
		if (pLockedChannel.get () == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Telemetry Channel is null");

		const uint64_t nTimestamp = pLockedChannel->getCurrentTimestamp();
		uint32_t nChannelIndex = pLockedChannel->getChannelIndex();
		auto pWriter = pLockedChannel->getWriter();

		if (nTimestamp < m_nStartTimestamp)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDTELEMETRYMARKERFINISHTIMESTAMP,
				pLockedChannel->getChannelIdentifier() + " / " + std::to_string(m_nMarkerID));

		uint64_t expected = 0;
		if (!m_nFinishTimestamp.compare_exchange_strong(
			expected, nTimestamp,
			std::memory_order_release,
			std::memory_order_relaxed))
		{
			// expected now contains the current (non-zero) finish timestamp
			throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYMARKERHASALREADYBEENFINISHED,
				pLockedChannel->getChannelIdentifier() + " / " + std::to_string(m_nMarkerID));
		}

		pWriter->writeEntry({ LibMCData::eTelemetryChunkEntryType::IntervalEndMarker, nChannelIndex, m_nMarkerID, nTimestamp, m_nContextData });

		uint64_t nDuration = nTimestamp - m_nStartTimestamp;
		pLockedChannel->unregisterMarker(m_nMarkerID, nDuration);
	}



	bool CTelemetryMarker::isFinished() const
	{
		auto nFinishTimestamp = m_nFinishTimestamp.load(std::memory_order_acquire);
		return (nFinishTimestamp != 0);
	}
	 
	uint64_t CTelemetryMarker::getDuration() const
	{
		auto nFinishTimestamp = m_nFinishTimestamp.load(std::memory_order_acquire);
		if (nFinishTimestamp == 0)
			throw ELibMCCustomException(LIBMC_ERROR_UNFINISHEDTELEMETRYMARKERSHAVENODURATION, std::to_string(m_nMarkerID));

		return (nFinishTimestamp - m_nStartTimestamp);
	}

	uint64_t CTelemetryMarker::getMarkerID() const
	{
		return m_nMarkerID;
	}

	uint64_t CTelemetryMarker::getStartTimestamp() const
	{
		return m_nStartTimestamp;
	}

	uint64_t CTelemetryMarker::getFinishTimestamp() const
	{
		auto nFinishTimestamp = m_nFinishTimestamp.load(std::memory_order_acquire);
		return nFinishTimestamp;
	}

	uint64_t CTelemetryMarker::getContextData() const
	{
		return m_nContextData;
	}

	CTelemetryChannel::CTelemetryChannel(const std::string& sChannelUUID, uint32_t nChannelIndex, const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType, PTelemetryWriter pTelemetryWriter)
		: m_sChannelUUID(sChannelUUID), 
		m_nChannelIndex(nChannelIndex), 
		m_sChannelIdentifier(sChannelIdentifier), 
		m_sChannelDescription(sChannelDescription), 
		m_eChannelType(channelType), 
		m_pTelemetryWriter (pTelemetryWriter),
		m_nTotalMarkersCreated (0),
		m_nMaxDurationInMicroseconds (0)
	{

		if (pTelemetryWriter.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}

	CTelemetryChannel::~CTelemetryChannel()
	{

	}
	 
	std::string CTelemetryChannel::getChannelUUID() const
	{
		return m_sChannelUUID;
	}

	uint32_t CTelemetryChannel::getChannelIndex() const
	{
		return m_nChannelIndex;
	}

	std::string CTelemetryChannel::getChannelIdentifier() const
	{
		return m_sChannelIdentifier;
	}

	std::string CTelemetryChannel::getChannelDescription() const
	{
		return m_sChannelDescription;
	}

	LibMCData::eTelemetryChannelType CTelemetryChannel::getChannelType() const
	{
		return m_eChannelType;
	}

	PTelemetryMarker CTelemetryChannel::createMarkerEx(bool bMarkInstant, uint64_t nContextData)
	{

		uint64_t nMarkerID = m_pTelemetryWriter->createMarkerID();

		// Retrieve the shared pointer I am living in...
		auto selfPtr = shared_from_this();

		auto pMarker = std::make_shared<CTelemetryMarker>(selfPtr, nMarkerID, bMarkInstant, nContextData);

		if (!bMarkInstant)
		{
			std::lock_guard<std::mutex> lockGuard(m_MarkerMutex);
			m_Markers.emplace(nMarkerID, pMarker);
			m_nTotalMarkersCreated++;
		}
		else {
			std::lock_guard<std::mutex> lockGuard(m_MarkerMutex);
			m_nTotalMarkersCreated++;
		}

		return pMarker;

	}

	uint64_t CTelemetryChannel::getTotalMarkersCreated()
	{
		std::lock_guard<std::mutex> lockGuard(m_MarkerMutex);
		return m_nTotalMarkersCreated;

	}

	uint64_t CTelemetryChannel::getMaxDurationInMicroseconds()
	{
		std::lock_guard<std::mutex> lockGuard(m_MarkerMutex);
		return m_nMaxDurationInMicroseconds;
	}

	void CTelemetryChannel::getStatistics(uint64_t& nTotalMarkersCreated, uint64_t& nMaxDurationInMicroseconds)
	{
		std::lock_guard<std::mutex> lockGuard(m_MarkerMutex);
		nTotalMarkersCreated = m_nTotalMarkersCreated;
		nMaxDurationInMicroseconds = m_nMaxDurationInMicroseconds;
	}

	uint64_t CTelemetryChannel::createInstantMarker(uint64_t nContextData)
	{
		auto pMarker = createMarkerEx(true, nContextData);
		return pMarker->getMarkerID();
	}


	CTelemetryScope CTelemetryChannel::startIntervalScope(uint64_t nContextData)
	{
		return CTelemetryScope (createMarkerEx(false, nContextData));
	}

	PTelemetryMarker CTelemetryChannel::startIntervalMarker(uint64_t nContextData)
	{
		return createMarkerEx(false, nContextData);
	}


	uint64_t CTelemetryChannel::getCurrentTimestamp()
	{
		uint64_t nTimestamp = m_pTelemetryWriter->getCurrentTimestamp();
		// Ensure Timestamp is never zero (should never happen anyhow)..
		if (nTimestamp == 0)
			nTimestamp = 1;

		return nTimestamp;
	}

	void CTelemetryChannel::unregisterMarker(uint64_t markerID, uint64_t nDurationInMicroseconds)
	{
		std::lock_guard<std::mutex> lock(m_MarkerMutex);
		m_Markers.erase(markerID);

		if (nDurationInMicroseconds > m_nMaxDurationInMicroseconds)
			m_nMaxDurationInMicroseconds = nDurationInMicroseconds;
	}

	PTelemetryWriter CTelemetryChannel::getWriter() const
	{
		return m_pTelemetryWriter;
	}


	std::string CTelemetryChannel::mapDataChannelTypeToTypeString(LibMCData::eTelemetryChannelType dataChannelType)
	{
		switch (dataChannelType) {
		case LibMCData::eTelemetryChannelType::CustomMarker:
			return "custommarker";
		case LibMCData::eTelemetryChannelType::RemoteQuery:
			return "remotequery";
		case LibMCData::eTelemetryChannelType::StateExecution:
			return "stateexecution";
		case LibMCData::eTelemetryChannelType::StateRepeatDelay:
			return "staterepeatdelay";
		case LibMCData::eTelemetryChannelType::SignalQueue:
			return "signalqueue";
		case LibMCData::eTelemetryChannelType::SignalProcessing:
			return "signalprocessing";
		case LibMCData::eTelemetryChannelType::SignalAcknowledgement:
			return "signalacknowledgement";
		default:
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTELEMETRYCHANNELTYPE, "invalid telemetry channel type");
		}
	}

	LibMCData::eTelemetryChannelType CTelemetryChannel::mapChannelTypeStringToDataChannelType(const std::string& sTypeString)
	{
		if (sTypeString == "custommarker")
			return LibMCData::eTelemetryChannelType::CustomMarker;
		if (sTypeString == "remotequery")
			return LibMCData::eTelemetryChannelType::RemoteQuery;
		if (sTypeString == "stateexecution")
			return LibMCData::eTelemetryChannelType::StateExecution;
		if (sTypeString == "staterepeatdelay")
			return LibMCData::eTelemetryChannelType::StateRepeatDelay;
		if (sTypeString == "signalqueue")
			return LibMCData::eTelemetryChannelType::SignalQueue;
		if (sTypeString == "signalprocessing")
			return LibMCData::eTelemetryChannelType::SignalProcessing;
		if (sTypeString == "signalacknowledgement")
			return LibMCData::eTelemetryChannelType::SignalAcknowledgement;

		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTELEMETRYCHANNELTYPE, "invalid telemetry channel type string: " + sTypeString);
	}

	

	CTelemetryScope::CTelemetryScope(PTelemetryMarker pMarker)
		: m_pMarker(pMarker)
	{
		if (pMarker.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPARAM, "Telemetry Marker is null");
	}

	CTelemetryScope::~CTelemetryScope()
	{
		if (m_pMarker.get() != nullptr) {
			try {
				m_pMarker->finishMarker();
			}
			catch (...) {
				// ignore in destructor
			}
		}
	}

	uint64_t CTelemetryScope::getMarkerID() const
	{
		return m_pMarker->getMarkerID();
	}

	uint64_t CTelemetryScope::getStartTimeStamp() const
	{
		return m_pMarker->getStartTimestamp();
	}


	CTelemetryHandler::CTelemetryHandler(PTelemetryWriter pWriter)
		: m_nNextChannelIndex (1),  m_pTelemetryWriter (pWriter)
	{
		if (pWriter.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	CTelemetryHandler::~CTelemetryHandler()
	{
	}

	PTelemetryChannel CTelemetryHandler::registerChannel(const std::string& sChannelIdentifier, const std::string& sChannelDescription, LibMCData::eTelemetryChannelType channelType)
	{
		if (!AMCCommon::CUtils::stringIsValidAlphanumericPathString(sChannelIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDTELEMETRYCHANNELIDENTIFIER, sChannelIdentifier);

		std::lock_guard<std::mutex> lockGuard(m_ChannelMutex);

		auto iIdentifierIter = m_ChannelsByIdentifier.find(sChannelIdentifier);
		if (iIdentifierIter != m_ChannelsByIdentifier.end())
			throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHANNELALREADYEXISTS, sChannelIdentifier);

		std::string sUUID = AMCCommon::CUtils::createUUID();
		uint32_t nChannelIndex = m_nNextChannelIndex;
		m_nNextChannelIndex++;

		auto pChannel = std::make_shared<CTelemetryChannel>(sUUID, nChannelIndex, sChannelIdentifier, sChannelDescription, channelType, m_pTelemetryWriter);

		m_ChannelsByIdentifier.emplace(sChannelIdentifier, pChannel);
		m_ChannelsByUUID.emplace(sUUID, pChannel);

		// We want to keep the lock of the channel mutex while creating the channel in the DB, to be able to roll back in case of error.
		try 
		{
			m_pTelemetryWriter->createChannelInDB(sUUID, channelType, nChannelIndex, sChannelIdentifier, sChannelDescription);
		}
		catch (...)
		{
			// roll back changes in case of error...
			m_ChannelsByIdentifier.erase(sChannelIdentifier);
			m_ChannelsByUUID.erase(sUUID);

			throw;
		}


		return pChannel;

	}

	PTelemetryChannel CTelemetryHandler::findChannelByUUID(const std::string& sChannelUUID)
	{
		std::lock_guard<std::mutex> lockGuard(m_ChannelMutex);

		auto iUUIDIter = m_ChannelsByUUID.find(sChannelUUID);
		if (iUUIDIter == m_ChannelsByUUID.end())
			throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHANNELNOTFOUND, sChannelUUID);
		return iUUIDIter->second;
	}

	PTelemetryChannel CTelemetryHandler::findChannelByIdentifier(const std::string& sChannelIdentifier, bool bFailIfNotExisting)
	{

		std::lock_guard<std::mutex> lockGuard(m_ChannelMutex);

		auto iIdentifierIter = m_ChannelsByIdentifier.find(sChannelIdentifier);	
		if (iIdentifierIter == m_ChannelsByIdentifier.end()) {
			if (bFailIfNotExisting)
				throw ELibMCCustomException(LIBMC_ERROR_TELEMETRYCHANNELNOTFOUND, sChannelIdentifier);
			return nullptr;
		}
		return iIdentifierIter->second;
	}


	void CTelemetryHandler::archiveOldChunksToDB()
	{
		m_pTelemetryWriter->archiveOldChunksToDB();
	}

}




