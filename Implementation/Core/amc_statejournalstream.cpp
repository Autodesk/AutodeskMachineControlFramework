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

#include <iostream>

#define STATEJOURNALSTREAMMINCAPACITY 65536

namespace AMC {

	typedef struct _sJournalStreamChunkEntry {
		uint32_t m_nRelativeTimeStamp;
		int64_t m_nValue;
	} sJournalStreamChunkEntry;



	class CStateJournalStreamChunk_Dynamic : public CStateJournalStreamChunk
	{
	private:

		uint64_t m_nChunkIndex;
		uint64_t m_nStartTimeStamp;
		uint64_t m_nEndTimeStamp;
		uint64_t m_nCurrentTimeStamp;

		std::vector<std::list<sJournalStreamChunkEntry>> m_Entries;

	public:

		CStateJournalStreamChunk_Dynamic(uint64_t nChunkIndex, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp, uint32_t nVariableCount)
			: CStateJournalStreamChunk (), m_nChunkIndex (nChunkIndex), m_nStartTimeStamp (nStartTimeStamp), m_nEndTimeStamp (nEndTimeStamp), m_nCurrentTimeStamp (nStartTimeStamp)
		{
			m_Entries.resize(nVariableCount);
		}


		uint64_t getChunkIndex()
		{
			return m_nChunkIndex;
		} 

		uint64_t getStartTimeStamp() override
		{
			return m_nStartTimeStamp;
		}

		uint64_t getEndTimeStamp() override
		{
			return m_nEndTimeStamp;
		}

		
		void readRawIntegerData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream) override
		{
			uint64_t nRelativeIntervalStart = 0;			
			if (nIntervalStartTimeStamp > m_nStartTimeStamp)
				nRelativeIntervalStart = nIntervalStartTimeStamp - (uint64_t) m_nStartTimeStamp;
				
			uint64_t nRelativeIntervalEnd = 0;
			if (nIntervalEndTimeStamp > m_nStartTimeStamp)
				nRelativeIntervalEnd = nIntervalEndTimeStamp - m_nStartTimeStamp;

			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			int64_t nStartValue = 0;

			auto& variable = m_Entries.at(nStorageIndex);
			for (auto & entry : variable) {				
					if (entry.m_nRelativeTimeStamp <= nRelativeIntervalStart) {
						nStartValue = entry.m_nValue;
					}
					else {
						if (entry.m_nRelativeTimeStamp <= nRelativeIntervalEnd) {
							if (rawTimeStream.empty() && (entry.m_nRelativeTimeStamp > nRelativeIntervalStart)) {
								sJournalTimeStreamInt64Entry firstEntry;
								firstEntry.m_nTimeStamp = m_nStartTimeStamp + nRelativeIntervalStart;
								firstEntry.m_nValue = nStartValue;
								rawTimeStream.push_back(firstEntry);

							}

							sJournalTimeStreamInt64Entry resultEntry;
							resultEntry.m_nTimeStamp = m_nStartTimeStamp + entry.m_nRelativeTimeStamp;
							resultEntry.m_nValue = entry.m_nValue;
							rawTimeStream.push_back(resultEntry);
						}
					}
			}

		}


		void readRawDoubleData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream) override
		{
			uint64_t nRelativeIntervalStart = 0;
			if (nIntervalStartTimeStamp > m_nStartTimeStamp)
				nRelativeIntervalStart = nIntervalStartTimeStamp - (uint64_t)m_nStartTimeStamp;

			uint64_t nRelativeIntervalEnd = 0;
			if (nIntervalEndTimeStamp > m_nStartTimeStamp)
				nRelativeIntervalEnd = nIntervalEndTimeStamp - m_nStartTimeStamp;

			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			int64_t nStartValue = 0;

			auto& variable = m_Entries.at(nStorageIndex);
			for (auto& entry : variable) {
				if (entry.m_nRelativeTimeStamp <= nRelativeIntervalStart) {
					nStartValue = entry.m_nValue;
				}
				else {
					if (entry.m_nRelativeTimeStamp <= nRelativeIntervalEnd) {
						if (rawTimeStream.empty() && (entry.m_nRelativeTimeStamp > nRelativeIntervalStart)) {
							sJournalTimeStreamDoubleEntry firstEntry;
							firstEntry.m_nTimeStamp = m_nStartTimeStamp + nRelativeIntervalStart;
							firstEntry.m_dValue = nStartValue * dUnits;
							rawTimeStream.push_back(firstEntry);

						}

						sJournalTimeStreamDoubleEntry resultEntry;
						resultEntry.m_nTimeStamp = m_nStartTimeStamp + entry.m_nRelativeTimeStamp;
						resultEntry.m_dValue = entry.m_nValue * dUnits;
						rawTimeStream.push_back(resultEntry);
					}
				}
			}

		}

		void writeEntry (uint32_t nStorageIndex, uint32_t nRelativeTimeStamp, int64_t nValue)
		{
			if (nStorageIndex >= m_Entries.size())
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			auto& variable = m_Entries.at(nStorageIndex);

			variable.push_back(sJournalStreamChunkEntry{ nRelativeTimeStamp, nValue });
		}

	};


	class CStateJournalStreamChunk_InMemory : public CStateJournalStreamChunk
	{
	private:

		uint64_t m_nChunkIndex;
		uint64_t m_nStartTimeStamp;
		uint64_t m_nEndTimeStamp;

	public:

		CStateJournalStreamChunk_InMemory(CStateJournalStreamChunk_Dynamic* pDynamicChunk)
		{
			if (pDynamicChunk == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

			m_nChunkIndex = pDynamicChunk->getChunkIndex();
			m_nStartTimeStamp = pDynamicChunk->getStartTimeStamp();
			m_nEndTimeStamp = pDynamicChunk->getEndTimeStamp();
		}

		virtual ~CStateJournalStreamChunk_InMemory()
		{

		}

		uint64_t getStartTimeStamp() override
		{
			return m_nStartTimeStamp;
		}

		uint64_t getEndTimeStamp() override
		{
			return m_nEndTimeStamp;
		}

		void readRawIntegerData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream) override
		{

		}

		void readRawDoubleData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream) override
		{

		}

		void writeToJournal(LibMCData::PJournalSession pJournalSession)
		{
			if (pJournalSession.get() == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND);

			std::vector<uint8_t> dataBuffer;
			dataBuffer.resize(1024 * sizeof(sJournalStreamChunkEntry));

			pJournalSession->WriteJournalChunkData((uint32_t)m_nChunkIndex, m_nStartTimeStamp, m_nEndTimeStamp, dataBuffer);
		}

	};

	class CStateJournalStreamChunk_OnDisk : public CStateJournalStreamChunk
	{
	private:
		uint64_t m_nStartTimeStamp;
		uint64_t m_nEndTimeStamp;

	public:
		CStateJournalStreamChunk_OnDisk(uint64_t nStartTimeStamp, uint64_t nEndTimeStamp)
			: CStateJournalStreamChunk(), m_nStartTimeStamp (nStartTimeStamp), m_nEndTimeStamp (nEndTimeStamp)
		{

		}

		virtual ~CStateJournalStreamChunk_OnDisk()
		{

		}

		uint64_t getStartTimeStamp() override
		{
			return m_nStartTimeStamp;
		}

		uint64_t getEndTimeStamp() override
		{
			return m_nEndTimeStamp;
		}

		void readRawIntegerData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream) override
		{

		}


		void readRawDoubleData(uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream) override
		{

		}

	};

	CStateJournalStreamChunk::CStateJournalStreamChunk()
	{

	}

	CStateJournalStreamChunk::~CStateJournalStreamChunk()
	{

	}


	CStateJournalStream::CStateJournalStream(LibMCData::PJournalSession pJournalSession)
		: m_nChunkSizeInMilliseconds(60000), m_pJournalSession(pJournalSession), m_nCurrentTimeStamp (0)
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		
	}

	CStateJournalStream::~CStateJournalStream()
	{

	}

	
	void CStateJournalStream::ensureChunk(const uint64_t nAbsoluteTimeStamp)
	{
		if (nAbsoluteTimeStamp < m_nCurrentTimeStamp)
			throw ELibMCInterfaceException(LIBMC_ERROR_CHUNKTIMESTREAMNOTCONTINUOUS);

		m_nCurrentTimeStamp = nAbsoluteTimeStamp;

		if (m_pCurrentChunk == nullptr) {
			startNewChunk(nAbsoluteTimeStamp);
		}
		else {
			if (nAbsoluteTimeStamp > m_pCurrentChunk->getEndTimeStamp ())
				startNewChunk(nAbsoluteTimeStamp);
		}

	}


	void CStateJournalStream::startNewChunk(const uint64_t nAbsoluteTimeStamp)
	{
		std::cout << "starting new chunk " << nAbsoluteTimeStamp << std::endl;

		// Create new chunk in memory
		uint64_t nNewChunkIndex = nAbsoluteTimeStamp / m_nChunkSizeInMilliseconds;
		uint64_t nNewChunkStart = nNewChunkIndex * m_nChunkSizeInMilliseconds;
		uint64_t nNewChunkEnd = nNewChunkStart + m_nChunkSizeInMilliseconds - 1;
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
			m_pCurrentChunk->writeEntry(nStorageIndex, nNewChunkStart, m_CurrentVariableValues.at(nStorageIndex));

	}

	void CStateJournalStream::writeBool(const uint64_t nAbsoluteTimeStamp, const uint32_t nStorageIndex, bool bValue)
	{
		ensureChunk(nAbsoluteTimeStamp);

		uint32_t nRelativeTimeStamp = (uint32_t) (nAbsoluteTimeStamp - m_pCurrentChunk->getStartTimeStamp());

		if (bValue) {
			m_pCurrentChunk->writeEntry (nStorageIndex, nRelativeTimeStamp, 1);
			m_CurrentVariableValues.at(nStorageIndex) = 1;
		}
		else {
			m_pCurrentChunk->writeEntry (nStorageIndex, nRelativeTimeStamp, 0);
			m_CurrentVariableValues.at(nStorageIndex) = 0;
		}

	}


	void CStateJournalStream::writeInt64(const uint64_t nAbsoluteTimeStamp, const uint32_t nStorageIndex, int64_t nValue)
	{
		ensureChunk(nAbsoluteTimeStamp);

		uint32_t nRelativeTimeStamp = (uint32_t) (nAbsoluteTimeStamp - m_pCurrentChunk->getStartTimeStamp());

		m_pCurrentChunk->writeEntry(nStorageIndex, nRelativeTimeStamp, nValue);
		m_CurrentVariableValues.at(nStorageIndex) = nValue;

	}
	
	void CStateJournalStream::writeDouble(const uint64_t nAbsoluteTimeStamp, const uint32_t nStorageIndex, int64_t nValue)
	{
		ensureChunk(nAbsoluteTimeStamp);

		uint32_t nRelativeTimeStamp = (uint32_t) (nAbsoluteTimeStamp - m_pCurrentChunk->getStartTimeStamp());

		m_pCurrentChunk->writeEntry(nStorageIndex, nRelativeTimeStamp, nValue);
		m_CurrentVariableValues.at(nStorageIndex) = nValue;

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
				std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
				pChunkToWrite->writeToJournal(m_pJournalSession);

				// Push disk chunk to the archive queue
				m_ChunksToArchive.push(std::make_shared<CStateJournalStreamChunk_OnDisk>(pChunkToWrite->getStartTimeStamp(), pChunkToWrite->getEndTimeStamp()));
			}

			// TODO: Remove Chunks from memory if necessary...

		}
		
	}

	void CStateJournalStream::readRawIntegerData(const uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, std::vector<sJournalTimeStreamInt64Entry>& rawTimeStream)
	{
		if (nIntervalStartTimeStamp > nIntervalEndTimeStamp)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALSTREAMQUERY);

		uint64_t nStartChunkIndex = nIntervalStartTimeStamp / m_nChunkSizeInMilliseconds;
		uint64_t nEndChunkIndex = nIntervalEndTimeStamp / m_nChunkSizeInMilliseconds;

		uint64_t nChunkCount = m_ChunkTimeline.size();

		for (uint64_t nChunkIndex = nStartChunkIndex; nChunkIndex <= nEndChunkIndex; nChunkIndex++) {
			PStateJournalStreamChunk pChunk;
			if (nChunkIndex < nChunkCount) {
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				pChunk = m_ChunkTimeline.at(nChunkIndex);
			}

			if (pChunk.get () != nullptr)
				pChunk->readRawIntegerData(nStorageIndex, nIntervalStartTimeStamp, nIntervalEndTimeStamp, rawTimeStream);
		}


	}

	void CStateJournalStream::readRawDoubleData(const uint32_t nStorageIndex, uint64_t nIntervalStartTimeStamp, uint64_t nIntervalEndTimeStamp, double dUnits, std::vector<sJournalTimeStreamDoubleEntry>& rawTimeStream)
	{
		if (nIntervalStartTimeStamp > nIntervalEndTimeStamp)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALSTREAMQUERY);

		uint64_t nStartChunkIndex = nIntervalStartTimeStamp / m_nChunkSizeInMilliseconds;
		uint64_t nEndChunkIndex = nIntervalEndTimeStamp / m_nChunkSizeInMilliseconds;
		uint64_t nChunkCount = m_ChunkTimeline.size();

		for (uint64_t nChunkIndex = nStartChunkIndex; nChunkIndex <= nEndChunkIndex; nChunkIndex++) {
			PStateJournalStreamChunk pChunk;
			if (nChunkIndex < nChunkCount) {
				std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
				pChunk = m_ChunkTimeline.at(nChunkIndex);
			}

			if (pChunk.get() != nullptr)
				pChunk->readRawDoubleData(nStorageIndex, nIntervalStartTimeStamp, nIntervalEndTimeStamp, dUnits, rawTimeStream);
		}


	}

	uint64_t CStateJournalStream::getChunkSizeInMilliseconds()
	{
		return m_nChunkSizeInMilliseconds;
	}

	void CStateJournalStream::setVariableCount(size_t nVariableCount)
	{
		m_CurrentVariableValues.resize (nVariableCount);
		for (auto& value : m_CurrentVariableValues)
			value = 0;

	}

}


