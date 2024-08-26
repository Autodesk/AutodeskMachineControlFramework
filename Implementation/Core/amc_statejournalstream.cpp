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

#include <iostream>
#include <stdexcept>

#define STATEJOURNALSTREAMMINCAPACITY 65536

#define STATEJOURNALSTORAGE_RAW 1
#define STATEJOURNALSTORAGE_DELTA 2

#define STATEJOURNALSTORAGE_MAXENTRIESPERCHUNK (128UL * 1024UL * 1024UL)

namespace AMC {



	

	class CStateJournalStreamChunk_Dynamic : public CStateJournalStreamChunk
	{
	private:

		uint64_t m_nChunkIndex;
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;
		uint64_t m_nCurrentTimeStampInMicroSeconds;

		std::vector<std::map<uint32_t, int64_t>> m_Data;

	public:

		CStateJournalStreamChunk_Dynamic(uint64_t nChunkIndex, uint64_t nStartTimeStampInMicroSeconds, uint64_t nEndTimeStampInMicroSeconds, uint32_t nVariableCount)
			: CStateJournalStreamChunk(), m_nChunkIndex(nChunkIndex), m_nStartTimeStampInMicroSeconds(nStartTimeStampInMicroSeconds), m_nEndTimeStampInMicroSeconds(nEndTimeStampInMicroSeconds), m_nCurrentTimeStampInMicroSeconds(nStartTimeStampInMicroSeconds)
		{
			m_Data.resize(nVariableCount);
			//std::cout << "creating dynamic chunk " << m_nChunkIndex << std::endl;
		}


		virtual ~CStateJournalStreamChunk_Dynamic()
		{
			//std::cout << "destroying dynamic chunk " << m_nChunkIndex << std::endl;
		}

		uint64_t getChunkIndex() override
		{
			return m_nChunkIndex;
		}

		uint64_t getStartTimeStampInMicroSeconds() override
		{
			return m_nStartTimeStampInMicroSeconds;
		}

		uint64_t getEndTimeStampInMicroSeconds() override
		{
			return m_nEndTimeStampInMicroSeconds;
		}



		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) override
		{
			if ((nAbsoluteTimeStampInMicroseconds < m_nStartTimeStampInMicroSeconds) || (nAbsoluteTimeStampInMicroseconds > m_nEndTimeStampInMicroSeconds))
				throw ELibMCInterfaceException (LIBMC_ERROR_JOURNALSAMPLINGOUTSIDEOFRECORDINGINTERVAL);

			uint64_t nTimeOffset = m_nStartTimeStampInMicroSeconds;
			uint64_t nRelativeTime = nAbsoluteTimeStampInMicroseconds - nTimeOffset;

			if (nStorageIndex >= m_Data.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			const auto& variableData = m_Data.at (nStorageIndex);
			auto it = variableData.upper_bound((uint32_t)nRelativeTime);

			if (it == variableData.begin()) {
				return it->second;
			}

			if (it == variableData.end()) {
				return variableData.rbegin()->second;
			}

			return std::prev(it)->second;
		}


		void writeEntry (uint32_t nStorageIndex, uint64_t nAbsoluteTimeStampInMicroseconds, int64_t nValue)
		{
			if (nStorageIndex >= m_Data.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			if ((nAbsoluteTimeStampInMicroseconds < m_nStartTimeStampInMicroSeconds) || (nAbsoluteTimeStampInMicroseconds > m_nEndTimeStampInMicroSeconds))
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALWRITINGOUTSIDEOFRECORDINGINTERVAL);

			if (nAbsoluteTimeStampInMicroseconds < m_nCurrentTimeStampInMicroSeconds)
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALWRITINGISNOTINCREMENTAL);

			m_nCurrentTimeStampInMicroSeconds = nAbsoluteTimeStampInMicroseconds;
			uint64_t nTimeOffset = m_nStartTimeStampInMicroSeconds;
			uint64_t nRelativeTime = nAbsoluteTimeStampInMicroseconds - nTimeOffset;

			m_Data[nStorageIndex][(uint32_t)nRelativeTime] = nValue;
		}

		size_t getVariableCount() {
			return m_Data.size();
		}

		void serialize(std::vector<LibMCData::sJournalChunkVariableInfo>& variableBuffer, std::vector<uint32_t> & timeStampBuffer, std::vector<int64_t> & valueBuffer)
		{

			size_t nVariableCount = getVariableCount();
			if (nVariableCount > 0) {
				size_t nTotalCount = 0;

				variableBuffer.resize(nVariableCount);

				for (size_t nVariableIndex = 0; nVariableIndex < nVariableCount; nVariableIndex++) {

					auto& sourceVariable = m_Data.at(nVariableIndex);
					auto& targetVariable = variableBuffer.at(nVariableIndex);

					if (sourceVariable.size() > STATEJOURNALSTORAGE_MAXENTRIESPERCHUNK)
						throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALCHUNKHASTOOMANYENTRIES);

					targetVariable.m_VariableIndex = (uint32_t) nVariableIndex;
					targetVariable.m_StorageType = 0;
					targetVariable.m_EntryStartIndex = (uint32_t) nTotalCount;
					targetVariable.m_EntryCount = (uint32_t)sourceVariable.size();
					nTotalCount += sourceVariable.size();
				}

				timeStampBuffer.resize(nTotalCount);
				valueBuffer.resize(nTotalCount);

				size_t nTotalIndex = 0;

				for (size_t nVariableIndex = 0; nVariableIndex < nVariableCount; nVariableIndex++) {

					auto& sourceVariable = m_Data.at(nVariableIndex);
					auto& targetVariable = variableBuffer.at(nVariableIndex);

					for (auto & iSourceIter : sourceVariable) {
						timeStampBuffer.at(nTotalIndex) = iSourceIter.first;
						valueBuffer.at(nTotalIndex) = iSourceIter.second;
						nTotalIndex++;
					}
				}

				if (nTotalIndex != nTotalCount)
					throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTSERIALIZEJOURNALENTRIES);

			} 

		}

	};


	class CStateJournalStreamChunk_InMemory : public CStateJournalStreamChunk
	{
	private:

		uint64_t m_nChunkIndex;
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;

		std::vector<LibMCData::sJournalChunkVariableInfo> m_VariableBuffer;
		std::vector<uint32_t> m_TimeStampBuffer;
		std::vector<int64_t> m_ValueBuffer;

	public:

		CStateJournalStreamChunk_InMemory(CStateJournalStreamChunk_Dynamic* pDynamicChunk)
		{
			if (pDynamicChunk == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

			m_nChunkIndex = pDynamicChunk->getChunkIndex();
			m_nStartTimeStampInMicroSeconds = pDynamicChunk->getStartTimeStampInMicroSeconds();
			m_nEndTimeStampInMicroSeconds = pDynamicChunk->getEndTimeStampInMicroSeconds();

			pDynamicChunk->serialize(m_VariableBuffer, m_TimeStampBuffer, m_ValueBuffer);

			//std::cout << "creating in memory chunk " << m_nChunkIndex << ":  " << std::endl;				
		}

		CStateJournalStreamChunk_InMemory(LibMCData::PJournalChunkIntegerData pIntegerData)
		{
			if (pIntegerData.get() == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

			m_nChunkIndex = pIntegerData->GetChunkIndex();
			m_nStartTimeStampInMicroSeconds = pIntegerData->GetStartTimeStamp();
			m_nEndTimeStampInMicroSeconds = pIntegerData->GetEndTimeStamp();

			pIntegerData->GetVariableInfo(m_VariableBuffer);
			pIntegerData->GetTimeStampData(m_TimeStampBuffer);
			pIntegerData->GetValueData(m_ValueBuffer);

		}

		virtual ~CStateJournalStreamChunk_InMemory()
		{
			//std::cout << "destroying in memory chunk " << m_nChunkIndex << std::endl;

		}

		uint64_t getStartTimeStampInMicroSeconds() override
		{
			return m_nStartTimeStampInMicroSeconds;
		}

		uint64_t getEndTimeStampInMicroSeconds() override
		{
			return m_nEndTimeStampInMicroSeconds;
		}

		uint64_t getChunkIndex() override
		{
			return m_nChunkIndex;
		}

		void writeToJournal(LibMCData::PJournalSession pJournalSession)
		{
			if (pJournalSession.get() == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);
				
			//std::cout << "writing chunk #" << m_nChunkIndex << " to disk " << std::endl;

			 pJournalSession->WriteJournalChunkIntegerData((uint32_t)m_nChunkIndex, m_nStartTimeStampInMicroSeconds, m_nEndTimeStampInMicroSeconds, m_VariableBuffer, m_TimeStampBuffer, m_ValueBuffer);
		}

		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) override
		{
			if (nStorageIndex >= m_VariableBuffer.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			if ((nAbsoluteTimeStampInMicroseconds < m_nStartTimeStampInMicroSeconds) || (nAbsoluteTimeStampInMicroseconds > m_nEndTimeStampInMicroSeconds))
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALWRITINGOUTSIDEOFRECORDINGINTERVAL);

			uint64_t nTimeOffset = m_nStartTimeStampInMicroSeconds;
			uint64_t nRelativeTime = nAbsoluteTimeStampInMicroseconds - nTimeOffset;

			auto & variableInfo = m_VariableBuffer.at (nStorageIndex);

			size_t startIndex = variableInfo.m_EntryStartIndex;
			size_t count = variableInfo.m_EntryCount;

			auto it = std::lower_bound(m_TimeStampBuffer.begin() + startIndex, m_TimeStampBuffer.begin() + startIndex + count, nRelativeTime);

			if (it == m_TimeStampBuffer.begin() + startIndex) {
				return m_ValueBuffer.at(startIndex); // If nTimeStamp is before the first timestamp
			}

			if (it == m_TimeStampBuffer.begin() + startIndex + count) {
				return m_ValueBuffer.at (startIndex + count - 1); // If nTimeStamp is beyond the last timestamp
			}

			if (*it == nTimeOffset) {
				return m_ValueBuffer.at (it - m_TimeStampBuffer.begin());
			}

			return m_ValueBuffer.at ((it - m_TimeStampBuffer.begin()) - 1);
		}

		uint64_t getMemoryUsage()
		{
			return m_ValueBuffer.size() * sizeof(int64_t) + m_TimeStampBuffer.size() * sizeof(uint32_t) + m_VariableBuffer.size() * sizeof(LibMCData::sJournalChunkVariableInfo);
		}

	};

	class CStateJournalStreamChunk_OnDisk : public CStateJournalStreamChunk
	{
	private:
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;
		uint64_t m_nChunkIndex;

		PStateJournalStreamCache m_pStreamCache;

	public:
		CStateJournalStreamChunk_OnDisk(uint64_t nStartTimeStampInMicroSeconds, uint64_t nEndTimeStampInMicroSeconds, uint64_t nChunkIndex, PStateJournalStreamCache pStreamCache)
			: CStateJournalStreamChunk(),
			m_nStartTimeStampInMicroSeconds(nStartTimeStampInMicroSeconds),
			m_nEndTimeStampInMicroSeconds(nEndTimeStampInMicroSeconds),
			m_nChunkIndex(nChunkIndex),
			m_pStreamCache (pStreamCache)
		{
			if (pStreamCache.get() == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

			std::cout << "creating on disk chunk " << m_nChunkIndex << std::endl;

		}

		virtual ~CStateJournalStreamChunk_OnDisk()
		{
			//std::cout << "destroying on disk chunk " << m_nChunkIndex << std::endl;

		}

		uint64_t getStartTimeStampInMicroSeconds() override
		{
			return m_nStartTimeStampInMicroSeconds;
		}

		uint64_t getEndTimeStampInMicroSeconds() override
		{
			return m_nEndTimeStampInMicroSeconds;
		}

		uint64_t getChunkIndex() override
		{
			return m_nChunkIndex;
		}

		/*void readRawIntegerData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStampInMicroSeconds, uint64_t nIntervalEndTimeStampInMicroSeconds, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream) override
		{

		}


		void readRawDoubleData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStampInMicroSeconds, uint64_t nIntervalEndTimeStampInMicroSeconds, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream) override
		{

		} */

		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) override
		{
			auto pEntry = m_pStreamCache->retrieveEntry((uint32_t) m_nChunkIndex);
			if (pEntry.get() != nullptr) {
				return pEntry->sampleIntegerData(nStorageIndex, nAbsoluteTimeStampInMicroseconds);
			} 

			std::cout << "cache miss! " << m_nChunkIndex << std::endl;

			pEntry = m_pStreamCache->loadEntryFromJournal((uint32_t)m_nChunkIndex);
			return pEntry->sampleIntegerData(nStorageIndex, nAbsoluteTimeStampInMicroseconds);
		}


	};

	CStateJournalStreamChunk::CStateJournalStreamChunk()
	{

	}

	CStateJournalStreamChunk::~CStateJournalStreamChunk()
	{

	}


	CStateJournalStreamCache::CStateJournalStreamCache(uint64_t nMemoryQuota, LibMCData::PJournalSession pJournalSession)
		: m_nMemoryQuota(nMemoryQuota),
		m_nMemoryUsage(0),
		m_pJournalSession (pJournalSession)
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}

	CStateJournalStreamCache::~CStateJournalStreamCache()
	{

	}

	uint64_t CStateJournalStreamCache::getMemoryQuota()
	{
		return m_nMemoryQuota;
	}

	uint64_t CStateJournalStreamCache::getCurrentMemoryUsage()
	{
		return m_nMemoryUsage;
	}

	void CStateJournalStreamCache::writeToJournal(PStateJournalStreamChunk_InMemory pChunk)
	{
		if (pChunk.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		{
			std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
			pChunk->writeToJournal(m_pJournalSession);
		}

		addEntry(pChunk);
	}

	PStateJournalStreamChunk_InMemory CStateJournalStreamCache::loadEntryFromJournal(uint32_t nTimeChunkIndex)
	{

		PStateJournalStreamChunk_InMemory pChunk;
		{
			std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
			auto pChunkIntegerData = m_pJournalSession->ReadChunkIntegerData(nTimeChunkIndex);
			pChunk = std::make_shared<CStateJournalStreamChunk_InMemory>(pChunkIntegerData);


		}

		addEntry(pChunk);

		return pChunk;

	}



	void CStateJournalStreamCache::addEntry(PStateJournalStreamChunk_InMemory pChunk)
	{
		if (pChunk.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		uint64_t nChunkMemoryUsage = pChunk->getMemoryUsage();
		uint32_t nTimeChunkIndex = (uint32_t)pChunk->getChunkIndex();



		if (nChunkMemoryUsage > m_nMemoryQuota)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALCHUNKMEMORYEXCEEDSQUOTA);
		if (nChunkMemoryUsage == 0)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALCHUNKMEMORYISZERO);

		// If the entry already exists, remove it first (we'll update it)
		auto it = m_CacheMap.find(nTimeChunkIndex);
		if (it != m_CacheMap.end()) {
			removeEntry(nTimeChunkIndex);
		}

		std::cout << "adding to cache: " << nTimeChunkIndex << " (memory use: " << nChunkMemoryUsage + m_nMemoryUsage << ")" << std::endl;

		{
			std::lock_guard<std::mutex> lockGuard(m_CacheMutex);

			enforceMemoryQuotaInternal(nChunkMemoryUsage);

			// Add new entry to the front of the list
			m_CacheList.push_front({ nTimeChunkIndex, pChunk });
			m_CacheMap[nTimeChunkIndex] = m_CacheList.begin();

			m_nMemoryUsage += nChunkMemoryUsage;
		}

	}

	void CStateJournalStreamCache::enforceMemoryQuotaInternal(uint64_t nAdditionalMemory)
	{
		while (((m_nMemoryUsage + nAdditionalMemory) > m_nMemoryQuota) && !(m_CacheList.empty())) {
			auto last = m_CacheList.back();
			removeEntryInternal(last.first);  // Remove the least recently used entry
		}

	}

	void CStateJournalStreamCache::removeEntry(uint32_t nTimeChunkIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_CacheMutex);
		removeEntryInternal(nTimeChunkIndex);

	}

	void CStateJournalStreamCache::removeEntryInternal(uint32_t nTimeChunkIndex)
	{
		auto it = m_CacheMap.find(nTimeChunkIndex);
		if (it != m_CacheMap.end()) {
			size_t nChunkMemoryUsage = it->second->second->getMemoryUsage();
			m_CacheList.erase(it->second);  // Remove from list in O(1)
			m_CacheMap.erase(it);           // Remove from map in O(1)

			if (nChunkMemoryUsage > m_nMemoryUsage)
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALCHUNKINTERNALMEMORYBOOKKEEPINGERROR);
			if (nChunkMemoryUsage == 0)
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALCHUNKMEMORYISZERO);

			m_nMemoryUsage -= nChunkMemoryUsage;

			std::cout << "removed from cache: " << nTimeChunkIndex << " (memory use: " << m_nMemoryUsage << ")" << std::endl;

		}
	}


	PStateJournalStreamChunk_InMemory CStateJournalStreamCache::retrieveEntry(uint32_t nTimeChunkIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_CacheMutex);

		auto it = m_CacheMap.find(nTimeChunkIndex);
		if (it == m_CacheMap.end()) {
			return nullptr; // Entry not found
		}

		// Move the accessed entry to the front of the list
		m_CacheList.splice(m_CacheList.begin(), m_CacheList, it->second);

		return it->second->second;
	}

	CStateJournalStream::CStateJournalStream(LibMCData::PJournalSession pJournalSession)
		: m_nChunkIntervalInMicroseconds(0), m_nCurrentTimeStampInMicroseconds (0)
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		
		m_nChunkIntervalInMicroseconds = pJournalSession->GetChunkIntervalInMicroseconds();

		m_Cache = std::make_shared<CStateJournalStreamCache>(1024ULL * 1024ULL * 1ULL, pJournalSession);
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

		//std::cout << "starting new chunk " << nNewChunkIndex << std::endl;

		auto pNewChunk = std::make_shared <CStateJournalStreamChunk_Dynamic>(nNewChunkIndex, nNewChunkStart, nNewChunkEnd, (uint32_t)m_CurrentVariableValues.size ());

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
			m_pCurrentChunk->writeEntry (nStorageIndex, (uint32_t) nNewChunkStart, m_CurrentVariableValues.at(nStorageIndex));

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

			auto pMemoryChunk = std::make_shared<CStateJournalStreamChunk_InMemory>(pChunkToSerialize.get ());
			{
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				m_ChunksToWrite.push(pMemoryChunk);

				//std::cout << "serializing chunk " << pMemoryChunk->getChunkIndex () << std::endl;
				
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
				//std::cout << "writing chunk " << pChunkToWrite->getChunkIndex() << std::endl;

				m_Cache->writeToJournal (pChunkToWrite);

				// Push disk chunk to the archive queue
				auto pOnDiskChunk = std::make_shared<CStateJournalStreamChunk_OnDisk>(pChunkToWrite->getStartTimeStampInMicroSeconds(), pChunkToWrite->getEndTimeStampInMicroSeconds(), pChunkToWrite->getChunkIndex(), m_Cache);
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

}


