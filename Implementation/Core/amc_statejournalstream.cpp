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

#define STATEJOURNALSTREAMMINCAPACITY 65536

#define STATEJOURNALSTORAGE_RAW 1
#define STATEJOURNALSTORAGE_DELTA 2

#define STATEJOURNALSTORAGE_MAXENTRIESPERCHUNK (128UL * 1024UL * 1024UL)

namespace AMC {

	typedef struct {
		uint32_t m_RelativeTimeStampInMicroseconds;
		int64_t m_IntegerValue;
	} sJournalChunkIntegerEntry;


	class CStateJournalStreamChunk_Dynamic : public CStateJournalStreamChunk
	{
	private:

		uint64_t m_nChunkIndex;
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;
		uint64_t m_nCurrentTimeStampInMicroSeconds;

		std::vector<std::list<sJournalChunkIntegerEntry>> m_Entries;

	public:

		CStateJournalStreamChunk_Dynamic(uint64_t nChunkIndex, uint64_t nStartTimeStampInMicroSeconds, uint64_t nEndTimeStampInMicroSeconds, uint32_t nVariableCount)
			: CStateJournalStreamChunk(), m_nChunkIndex(nChunkIndex), m_nStartTimeStampInMicroSeconds(nStartTimeStampInMicroSeconds), m_nEndTimeStampInMicroSeconds(nEndTimeStampInMicroSeconds), m_nCurrentTimeStampInMicroSeconds(nStartTimeStampInMicroSeconds)
		{
			m_Entries.resize(nVariableCount);
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


		/*void readRawIntegerData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStampInMicroSeconds, uint64_t nIntervalEndTimeStampInMicroSeconds, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream) override
		{

			uint64_t nTimeOffset = m_nStartTimeStampInMicroSeconds;

			uint64_t nClampedIntervalStartTime = nIntervalStartTimeStampInMicroSeconds;
			if (nClampedIntervalStartTime < m_nStartTimeStampInMicroSeconds)
				nClampedIntervalStartTime = m_nStartTimeStampInMicroSeconds;
			if (nClampedIntervalStartTime > m_nEndTimeStampInMicroSeconds)
				nClampedIntervalStartTime = m_nEndTimeStampInMicroSeconds;

			uint64_t nClampedIntervalEndTime = nIntervalEndTimeStampInMicroSeconds;
			if (nClampedIntervalEndTime < m_nStartTimeStampInMicroSeconds)
				nClampedIntervalEndTime = m_nStartTimeStampInMicroSeconds;
			if (nClampedIntervalEndTime > m_nEndTimeStampInMicroSeconds)
				nClampedIntervalEndTime = m_nEndTimeStampInMicroSeconds;

			uint64_t nRelativeIntervalStartTime = nClampedIntervalStartTime - nTimeOffset;
			uint64_t nRelativeIntervalEndTime = nClampedIntervalEndTime - nTimeOffset;

			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			int64_t nCurrentValue = 0;
			uint64_t nCurrentRelativeTime = 0;
			bool isFirst = true;

			auto& variable = m_Entries.at(nStorageIndex);
			for (auto& entry : variable) {
				if (entry.m_RelativeTimeStampInMicroseconds <= nRelativeIntervalEndTime) {
					if (entry.m_RelativeTimeStampInMicroseconds > nRelativeIntervalStartTime) {

						if (isFirst) {
							sJournalTimeStreamInt64Entry firstEntry;
							firstEntry.m_nTimeStampInMicroSeconds = nRelativeIntervalStartTime + nTimeOffset;
							firstEntry.m_nValue = nCurrentValue;
							rawTimeStream.push_back(firstEntry);

							isFirst = false;
						}

						sJournalTimeStreamInt64Entry resultEntry;
						resultEntry.m_nTimeStampInMicroSeconds = entry.m_RelativeTimeStampInMicroseconds + nTimeOffset;
						resultEntry.m_nValue = entry.m_IntegerValue;
						rawTimeStream.push_back(resultEntry);
					}

					nCurrentRelativeTime = entry.m_RelativeTimeStampInMicroseconds;
					nCurrentValue = entry.m_IntegerValue;

				}
				else {
					break;
				}

			}

			if (nCurrentRelativeTime < nRelativeIntervalEndTime) {
				sJournalTimeStreamInt64Entry lastEntry;
				lastEntry.m_nTimeStampInMicroSeconds = nRelativeIntervalEndTime + nTimeOffset;
				lastEntry.m_nValue = nCurrentValue;
				rawTimeStream.push_back(lastEntry);
			}

		}


		void readRawDoubleData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStampInMicroSeconds, uint64_t nIntervalEndTimeStampInMicroSeconds, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream) override
		{
			uint64_t nTimeOffset = m_nStartTimeStampInMicroSeconds;

			uint64_t nClampedIntervalStartTime = nIntervalStartTimeStampInMicroSeconds;
			if (nClampedIntervalStartTime < m_nStartTimeStampInMicroSeconds)
				nClampedIntervalStartTime = m_nStartTimeStampInMicroSeconds;
			if (nClampedIntervalStartTime > m_nEndTimeStampInMicroSeconds)
				nClampedIntervalStartTime = m_nEndTimeStampInMicroSeconds;

			uint64_t nClampedIntervalEndTime = nIntervalEndTimeStampInMicroSeconds;
			if (nClampedIntervalEndTime < m_nStartTimeStampInMicroSeconds)
				nClampedIntervalEndTime = m_nStartTimeStampInMicroSeconds;
			if (nClampedIntervalEndTime > m_nEndTimeStampInMicroSeconds)
				nClampedIntervalEndTime = m_nEndTimeStampInMicroSeconds;

			uint64_t nRelativeIntervalStartTime = nClampedIntervalStartTime - nTimeOffset;
			uint64_t nRelativeIntervalEndTime = nClampedIntervalEndTime - nTimeOffset;

			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			int64_t nCurrentValue = 0;
			uint64_t nCurrentRelativeTime = 0;
			bool isFirst = true;

			auto& variable = m_Entries.at(nStorageIndex);
			for (auto& entry : variable) {
				if (entry.m_RelativeTimeStampInMicroseconds <= nRelativeIntervalEndTime) {
					if (entry.m_RelativeTimeStampInMicroseconds > nRelativeIntervalStartTime) {

						if (isFirst) {
							sJournalTimeStreamDoubleEntry firstEntry;
							firstEntry.m_nTimeStampInMicroSeconds = nRelativeIntervalStartTime + nTimeOffset;
							firstEntry.m_dValue = nCurrentValue * dUnits;
							rawTimeStream.push_back(firstEntry);

							isFirst = false;
						}

						sJournalTimeStreamDoubleEntry resultEntry;
						resultEntry.m_nTimeStampInMicroSeconds = entry.m_RelativeTimeStampInMicroseconds + nTimeOffset;
						resultEntry.m_dValue = entry.m_IntegerValue * dUnits;
						rawTimeStream.push_back(resultEntry);
					}

					nCurrentRelativeTime = entry.m_RelativeTimeStampInMicroseconds;
					nCurrentValue = entry.m_IntegerValue;

				}
				else {
					break;
				}

			}

			if (nCurrentRelativeTime < nRelativeIntervalEndTime) {
				sJournalTimeStreamDoubleEntry lastEntry;
				lastEntry.m_nTimeStampInMicroSeconds = nRelativeIntervalEndTime + nTimeOffset;
				lastEntry.m_dValue = nCurrentValue * dUnits;
				rawTimeStream.push_back(lastEntry);
			}


		} */
	

		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) override
		{
			if ((nAbsoluteTimeStampInMicroseconds < m_nStartTimeStampInMicroSeconds) || (nAbsoluteTimeStampInMicroseconds > m_nEndTimeStampInMicroSeconds))
				return 0;

			uint64_t nTimeOffset = m_nStartTimeStampInMicroSeconds;
			uint64_t nRelativeTime = nAbsoluteTimeStampInMicroseconds - nTimeOffset;

			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			auto& variable = m_Entries.at(nStorageIndex);
			auto iIter = variable.begin();
			if (iIter != variable.end()) {
				auto pPrevEntry = &*iIter;
				iIter++;

				while (iIter != variable.end()) {
					auto pCurrentEntry = &*iIter;
					iIter++;

					if ((nRelativeTime >= pPrevEntry->m_RelativeTimeStampInMicroseconds) &&
						(nRelativeTime < pCurrentEntry->m_RelativeTimeStampInMicroseconds)) {
						return pPrevEntry->m_IntegerValue;
					}

					pPrevEntry = pCurrentEntry;
				}

				return pPrevEntry->m_IntegerValue;
			}

			return 0;
		}


		void writeEntry (uint32_t nStorageIndex, uint64_t nAbsoluteTimeStamp, int64_t nValue)
		{
			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			auto& variable = m_Entries.at(nStorageIndex);

			if ((nAbsoluteTimeStamp >= m_nStartTimeStampInMicroSeconds) && (nAbsoluteTimeStamp <= m_nEndTimeStampInMicroSeconds)) {

				sJournalChunkIntegerEntry newEntry;
				newEntry.m_RelativeTimeStampInMicroseconds = (uint32_t) (nAbsoluteTimeStamp - m_nStartTimeStampInMicroSeconds);
				newEntry.m_IntegerValue = nValue;

				variable.push_back(newEntry);
			}
		}

		size_t getVariableCount() {
			return m_Entries.size();
		}

		void serialize(std::vector<LibMCData::sJournalChunkVariableInfo>& variableBuffer, std::vector<uint32_t> & timeStampBuffer, std::vector<int64_t> & valueBuffer)
		{

			size_t nVariableCount = getVariableCount();
			if (nVariableCount > 0) {
				size_t nTotalCount = 0;

				variableBuffer.resize(nVariableCount);

				for (size_t nVariableIndex = 0; nVariableIndex < nVariableCount; nVariableIndex++) {

					auto& entry = m_Entries.at(nVariableIndex);
					auto& variable = variableBuffer.at(nVariableIndex);

					if (entry.size() > STATEJOURNALSTORAGE_MAXENTRIESPERCHUNK)
						throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALCHUNKHASTOOMANYENTRIES);

					variable.m_VariableIndex = (uint32_t) nVariableIndex;
					variable.m_EntryStartIndex = (uint32_t) nTotalCount;
					variable.m_EntryCount = (uint32_t)entry.size();
					nTotalCount += entry.size();
				}

				timeStampBuffer.resize(nTotalCount);
				valueBuffer.resize(nTotalCount);

				size_t nTotalIndex = 0;

				for (size_t nVariableIndex = 0; nVariableIndex < nVariableCount; nVariableIndex++) {

					auto& entry = m_Entries.at(nVariableIndex);
					auto& variable = variableBuffer.at(nVariableIndex);

					for (auto & bufferItem : entry) {
						timeStampBuffer.at(nTotalIndex) = bufferItem.m_RelativeTimeStampInMicroseconds;
						valueBuffer.at(nTotalIndex) = bufferItem.m_IntegerValue;
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
			return 0;
		}


	};

	class CStateJournalStreamChunk_OnDisk : public CStateJournalStreamChunk
	{
	private:
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;
		uint64_t m_nChunkIndex;

	public:
		CStateJournalStreamChunk_OnDisk(uint64_t nStartTimeStampInMicroSeconds, uint64_t nEndTimeStampInMicroSeconds, uint64_t nChunkIndex)
			: CStateJournalStreamChunk(), m_nStartTimeStampInMicroSeconds(nStartTimeStampInMicroSeconds), m_nEndTimeStampInMicroSeconds(nEndTimeStampInMicroSeconds), m_nChunkIndex (nChunkIndex)
		{
			//std::cout << "creating on disk chunk " << m_nChunkIndex << std::endl;

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
			return 0;
		}


	};

	CStateJournalStreamChunk::CStateJournalStreamChunk()
	{

	}

	CStateJournalStreamChunk::~CStateJournalStreamChunk()
	{

	}


	CStateJournalStream::CStateJournalStream(LibMCData::PJournalSession pJournalSession)
		: m_nChunkSizeInMicroseconds(5000000), m_pJournalSession(pJournalSession), m_nCurrentTimeStampInMicroseconds (0)
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		
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
		uint64_t nNewChunkIndex = nAbsoluteTimeStampInMicroSeconds / m_nChunkSizeInMicroseconds;
		uint64_t nNewChunkStart = nNewChunkIndex * m_nChunkSizeInMicroseconds;
		uint64_t nNewChunkEnd = nNewChunkStart + m_nChunkSizeInMicroseconds - 1;

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
			m_ChunkTimeline.resize(nNewChunkIndex + 1);
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

				std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
				pChunkToWrite->writeToJournal(m_pJournalSession);

				// Push disk chunk to the archive queue
				auto pOnDiskChunk = std::make_shared<CStateJournalStreamChunk_OnDisk>(pChunkToWrite->getStartTimeStampInMicroSeconds(), pChunkToWrite->getEndTimeStampInMicroSeconds(), pChunkToWrite->getChunkIndex());
				m_ChunksToArchive.push(pOnDiskChunk);

				m_ChunkTimeline.at(pChunkToWrite->getChunkIndex()) = pOnDiskChunk;

			}

			
		}
		
	}

	/*void CStateJournalStream::readRawIntegerData(const uint32_t nStorageIndex, const sStateJournalInterval& interval, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream)
	{
		if (interval.m_nStartTimeInMicroSeconds > interval.m_nEndTimeInMicroSeconds)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALSTREAMQUERY);

		uint64_t nStartChunkIndex = interval.m_nStartTimeInMicroSeconds / m_nChunkSizeInMicroseconds;
		uint64_t nEndChunkIndex = interval.m_nEndTimeInMicroSeconds / m_nChunkSizeInMicroseconds;

		uint64_t nChunkCount = m_ChunkTimeline.size();

		for (uint64_t nChunkIndex = nStartChunkIndex; nChunkIndex <= nEndChunkIndex; nChunkIndex++) {
			PStateJournalStreamChunk pChunk;
			if (nChunkIndex < nChunkCount) {
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				pChunk = m_ChunkTimeline.at(nChunkIndex);
			}

			if (pChunk.get () != nullptr)
				pChunk->readRawIntegerData(nStorageIndex, interval.m_nStartTimeInMicroSeconds, interval.m_nEndTimeInMicroSeconds, rawTimeStream);
		}


	} 

	void CStateJournalStream::readRawDoubleData(const uint32_t nStorageIndex, const sStateJournalInterval& interval, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream)
	{
		if (interval.m_nStartTimeInMicroSeconds > interval.m_nEndTimeInMicroSeconds)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALSTREAMQUERY);

		uint64_t nStartChunkIndex = interval.m_nStartTimeInMicroSeconds / m_nChunkSizeInMicroseconds;
		uint64_t nEndChunkIndex = interval.m_nEndTimeInMicroSeconds / m_nChunkSizeInMicroseconds;
		uint64_t nChunkCount = m_ChunkTimeline.size();

		for (uint64_t nChunkIndex = nStartChunkIndex; nChunkIndex <= nEndChunkIndex; nChunkIndex++) {
			PStateJournalStreamChunk pChunk;
			if (nChunkIndex < nChunkCount) {
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				pChunk = m_ChunkTimeline.at(nChunkIndex);
			}

			if (pChunk.get() != nullptr)
				pChunk->readRawDoubleData(nStorageIndex, interval.m_nStartTimeInMicroSeconds, interval.m_nEndTimeInMicroSeconds, dUnits, rawTimeStream);
		}


	} */

	int64_t CStateJournalStream::sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds)
	{
		uint64_t nChunkIndex = nAbsoluteTimeStampInMicroseconds / m_nChunkSizeInMicroseconds;
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
		uint64_t nChunkIndex = nAbsoluteTimeStampInMicroseconds / m_nChunkSizeInMicroseconds;
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
		uint64_t nChunkIndex = nAbsoluteTimeStampInMicroseconds / m_nChunkSizeInMicroseconds;
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


