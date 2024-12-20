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


#include "amc_statejournalstream.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmcdata_interfaces.hpp"


#include <stdexcept>


namespace AMC {





	CStateJournalStreamCache_Current::CStateJournalStreamCache_Current(uint64_t nMemoryQuota, LibMCData::PJournalSession pJournalSession, PLogger pDebugLogger)
		: CStateJournalStreamCache (nMemoryQuota, pDebugLogger),
		m_pJournalSession (pJournalSession)
		
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}

	CStateJournalStreamCache_Current::~CStateJournalStreamCache_Current()
	{

	}


	void CStateJournalStreamCache_Current::createVariableInJournalDB(const std::string& sName, uint32_t nVariableID, uint32_t nVariableIndex, LibMCData::eParameterDataType eVariableType, double dUnits)
	{
		std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
		m_pJournalSession->CreateVariableInJournalDB(sName, nVariableID, nVariableIndex, eVariableType, dUnits);

	}


	void CStateJournalStreamCache_Current::writeToJournal(PStateJournalStreamChunk_InMemory pChunk)
	{
		if (pChunk.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		{
			std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
			pChunk->writeToJournal(m_pJournalSession);
		}

		addEntry(pChunk);
	}

	PStateJournalStreamChunk_InMemory CStateJournalStreamCache_Current::loadEntryFromJournal(uint32_t nTimeChunkIndex)
	{

		PStateJournalStreamChunk_InMemory pChunk;
		{
			std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
			auto pChunkIntegerData = m_pJournalSession->ReadChunkIntegerData(nTimeChunkIndex);
			pChunk = std::make_shared<CStateJournalStreamChunk_InMemory>(pChunkIntegerData, m_pDebugLogger);

		}

		addEntry(pChunk);

		return pChunk;

	}




	CStateJournalStream::CStateJournalStream(LibMCData::PJournalSession pJournalSession, PLogger pDebugLogger, bool bEnableDebugLogging)
		: m_nChunkIntervalInMicroseconds(0), m_nCurrentTimeStampInMicroseconds (0)
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		if (bEnableDebugLogging)
			m_pDebugLogger = pDebugLogger;
		
		m_nChunkIntervalInMicroseconds = pJournalSession->GetChunkIntervalInMicroseconds();
		uint64_t nCacheMemoryQuota = pJournalSession->GetChunkCacheQuota();

		m_Cache = std::make_shared<CStateJournalStreamCache_Current>(nCacheMemoryQuota, pJournalSession, m_pDebugLogger);
	}

	CStateJournalStream::~CStateJournalStream()
	{

	}

	
	void CStateJournalStream::ensureChunk(const uint64_t nAbsoluteTimeStampInMicroSeconds)
	{
		if (nAbsoluteTimeStampInMicroSeconds < m_nCurrentTimeStampInMicroseconds)
			throw ELibMCInterfaceException(LIBMC_ERROR_CHUNKTIMESTREAMNOTCONTINUOUS);

		m_nCurrentTimeStampInMicroseconds = nAbsoluteTimeStampInMicroSeconds;

		if (m_pCurrentChunk == nullptr) {
			startNewChunk(nAbsoluteTimeStampInMicroSeconds);
		}
		else {
			if (nAbsoluteTimeStampInMicroSeconds > m_pCurrentChunk->getEndTimeStampInMicroSeconds ())
				startNewChunk(nAbsoluteTimeStampInMicroSeconds);
		}

	}


	void CStateJournalStream::startNewChunk(const uint64_t nAbsoluteTimeStampInMicroSeconds)
	{

		// Create new chunk in memory
		uint64_t nNewChunkIndex = nAbsoluteTimeStampInMicroSeconds / m_nChunkIntervalInMicroseconds;
		uint64_t nNewChunkStart = nNewChunkIndex * m_nChunkIntervalInMicroseconds;
		uint64_t nNewChunkEnd = nNewChunkStart + m_nChunkIntervalInMicroseconds - 1;

		auto pNewChunk = std::make_shared <CStateJournalStreamChunk_Dynamic>(nNewChunkIndex, nNewChunkStart, nNewChunkEnd, (uint32_t)m_CurrentVariableValues.size (), m_pDebugLogger);

		std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);

		// Determine new chunk index. Timeline should always increase
		if (nNewChunkIndex < m_ChunkTimeline.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_CHUNKTIMESTREAMNOTCONTINUOUS);

		// Finished chunks should always be properly serialized in memory
		if (m_pCurrentChunk.get() != nullptr) {
			m_ChunksToSerialize.push(m_pCurrentChunk);
			m_pCurrentChunk = nullptr;
		}

		// Assign current chunk
		m_pCurrentChunk = pNewChunk;

		// Store chunk in total timeline array
		if (m_ChunkTimeline.size() < nNewChunkIndex) {
			// In this case the timeline has gaps with no data...
			size_t nNewSize = (size_t)nNewChunkIndex + 1;
			m_ChunkTimeline.resize(nNewSize);
			m_ChunkTimeline.at(nNewChunkIndex) = pNewChunk;
		}
		else {
			// Default case: m_ChunkTimeline.size() == nNewChunkIndex;
			m_ChunkTimeline.push_back(pNewChunk);
		}

		uint32_t nVariableCount = (uint32_t) m_CurrentVariableValues.size();
		for (uint32_t nStorageIndex = 0; nStorageIndex < nVariableCount; nStorageIndex++)
			m_pCurrentChunk->writeEntry (nStorageIndex, nNewChunkStart, m_CurrentVariableValues.at(nStorageIndex));

	}

	void CStateJournalStream::writeBool_MicroSecond(const uint64_t nAbsoluteTimeStampInMicroSeconds, const uint32_t nStorageIndex, bool bValue)
	{
		ensureChunk(nAbsoluteTimeStampInMicroSeconds);

		if (bValue) {
			m_pCurrentChunk->writeEntry (nStorageIndex, nAbsoluteTimeStampInMicroSeconds, 1);
			m_CurrentVariableValues.at(nStorageIndex) = 1;
		}
		else {
			m_pCurrentChunk->writeEntry (nStorageIndex, nAbsoluteTimeStampInMicroSeconds, 0);
			m_CurrentVariableValues.at(nStorageIndex) = 0;
		}

	}


	void CStateJournalStream::writeInt64_MicroSecond(const uint64_t nAbsoluteTimeStampInMicroSeconds, const uint32_t nStorageIndex, int64_t nValue)
	{
		ensureChunk(nAbsoluteTimeStampInMicroSeconds);

		m_pCurrentChunk->writeEntry(nStorageIndex, nAbsoluteTimeStampInMicroSeconds, nValue);
		m_CurrentVariableValues.at(nStorageIndex) = nValue;

	}
	
	void CStateJournalStream::writeDouble_MicroSecond(const uint64_t nAbsoluteTimeStampInMicroSeconds, const uint32_t nStorageIndex, int64_t nValue)
	{
		ensureChunk(nAbsoluteTimeStampInMicroSeconds);

		m_pCurrentChunk->writeEntry(nStorageIndex, nAbsoluteTimeStampInMicroSeconds, nValue);
		m_CurrentVariableValues.at(nStorageIndex) = nValue;

	}

	void CStateJournalStream::serializeChunksThreaded()
	{
		while (!m_ChunksToSerialize.empty()) {
			PStateJournalStreamChunk_Dynamic pChunkToSerialize;
			{
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				pChunkToSerialize = m_ChunksToSerialize.front();
				m_ChunksToSerialize.pop();
			}

			auto pMemoryChunk = std::make_shared<CStateJournalStreamChunk_InMemory>(pChunkToSerialize.get (), m_pDebugLogger);
			{
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				m_ChunksToWrite.push(pMemoryChunk);
				
				m_ChunkTimeline.at(pMemoryChunk->getChunkIndex()) = pMemoryChunk;
 
			}

		}
	}


	void CStateJournalStream::writeChunksToDiskThreaded()
	{
	
		while (!m_ChunksToWrite.empty()) {

			// Get Chunk on top of queue
			PStateJournalStreamChunk_InMemory pChunkToWrite;
			{
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				pChunkToWrite = m_ChunksToWrite.front();
				m_ChunksToWrite.pop();
			}

			// Write Chunk to Journal
			{
				m_Cache->writeToJournal (pChunkToWrite);

				// Push disk chunk to the archive queue
				auto pOnDiskChunk = std::make_shared<CStateJournalStreamChunk_OnDisk>(pChunkToWrite->getStartTimeStampInMicroSeconds(), pChunkToWrite->getEndTimeStampInMicroSeconds(), pChunkToWrite->getChunkIndex(), m_Cache, m_pDebugLogger);
				m_ChunksToArchive.push(pOnDiskChunk);

				m_ChunkTimeline.at(pChunkToWrite->getChunkIndex()) = pOnDiskChunk;

			}

			
		}
		
	}

	int64_t CStateJournalStream::sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds)
	{
		uint64_t nChunkIndex = nAbsoluteTimeStampInMicroseconds / m_nChunkIntervalInMicroseconds;
		uint64_t nChunkCount = m_ChunkTimeline.size();

		PStateJournalStreamChunk pChunk;
		if (nChunkIndex < nChunkCount) {
			std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
			pChunk = m_ChunkTimeline.at(nChunkIndex);
		}

		if (pChunk.get() != nullptr)
			return pChunk->sampleIntegerData(nStorageIndex, nAbsoluteTimeStampInMicroseconds);

		return 0;
	}

	double CStateJournalStream::sampleDoubleData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds, double dUnits)
	{
		uint64_t nChunkIndex = nAbsoluteTimeStampInMicroseconds / m_nChunkIntervalInMicroseconds;
		uint64_t nChunkCount = m_ChunkTimeline.size();

		PStateJournalStreamChunk pChunk;
		if (nChunkIndex < nChunkCount) {
			std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
			pChunk = m_ChunkTimeline.at(nChunkIndex);
		}

		if (pChunk.get() != nullptr)
			return pChunk->sampleIntegerData(nStorageIndex, nAbsoluteTimeStampInMicroseconds) * dUnits;

		return 0.0;
	}

	bool CStateJournalStream::sampleBoolData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds)
	{
		uint64_t nChunkIndex = nAbsoluteTimeStampInMicroseconds / m_nChunkIntervalInMicroseconds;
		uint64_t nChunkCount = m_ChunkTimeline.size();

		PStateJournalStreamChunk pChunk;
		if (nChunkIndex < nChunkCount) {
			std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
			pChunk = m_ChunkTimeline.at(nChunkIndex);
		}

		if (pChunk.get() != nullptr)
			return pChunk->sampleIntegerData(nStorageIndex, nAbsoluteTimeStampInMicroseconds) != 0;

		return false;

	}



	void CStateJournalStream::setVariableCount(size_t nVariableCount)
	{
		m_CurrentVariableValues.resize (nVariableCount);
		for (auto& value : m_CurrentVariableValues)
			value = 0;

	}

	PStateJournalStreamCache_Current CStateJournalStream::getCache()
	{
		return m_Cache;
	}




}


