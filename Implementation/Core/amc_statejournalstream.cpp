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

namespace AMC {

	class CStateJournalStreamChunk
	{
	private:

		uint32_t m_nChunkIndex;
		uint64_t m_nCapacityInBytes;
		uint64_t m_nMaxDataSize;
		uint64_t m_nStartTimeStamp;
		uint64_t m_nEndTimeStamp;

		std::vector<uint8_t> m_Data;

		void writeData(const uint8_t* pData, uint32_t nLength)
		{
			LibMCAssertNotNull(pData);

			const uint8_t* pSrc = pData;
			for (uint32_t nIndex = 0; nIndex < nLength; nIndex++) {
				m_Data.push_back(*pSrc);
				pSrc++;
			}
		}

	public:

		CStateJournalStreamChunk(uint32_t nChunkIndex, uint64_t nStartTimeStamp, uint64_t nCapacityInBytes)
			: m_nCapacityInBytes (nCapacityInBytes), m_nChunkIndex (nChunkIndex), m_nStartTimeStamp (nStartTimeStamp), m_nEndTimeStamp (nStartTimeStamp)
		{
			if (nCapacityInBytes < STATEJOURNALSTREAMMINCAPACITY)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALSTREAMCAPACITY);
			if ((nCapacityInBytes % 32) != 0)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALSTREAMCAPACITY);

			// Make sure that the data blocks never exceed capacity
			m_nMaxDataSize = nCapacityInBytes;
			m_Data.reserve(m_nMaxDataSize);


		}

		void writeCommand(const uint32_t nCommand)
		{
			writeData((const uint8_t*)&nCommand, 4);
		}

		void writeUint64(const uint64_t nValue)
		{
			writeData((const uint8_t*)&nValue, 8);
		}

		void writeString(const std::string& sValue)
		{
			uint32_t nLength = (uint32_t)sValue.length();
			writeData((const uint8_t*)&nLength, 4);
			writeData((const uint8_t*) sValue.c_str (), nLength);
		}

		void writeDouble(const double dValue)
		{
			writeData((const uint8_t*)&dValue, 8);
		}


		/*void writeNameDefinition (const uint32_t nID, const std::string& sName)
		{
			auto iIter = m_NameDefinitions.find(nID);
			if (iIter != m_NameDefinitions.end())
				throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEJOURNALID);

			m_NameDefinitions.insert(std::make_pair (nID, sName));

			writeCommand(nID | STATEJOURNAL_COMMANDFLAG_DEFINITION);
			writeString(sName);

		} */

		void setEndTimeStamp(uint64_t nEndTimeStamp)
		{
			if (nEndTimeStamp < m_nEndTimeStamp)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALTIMESTAMP);

		}

		uint32_t getChunkIndex()
		{
			return m_nChunkIndex;
		}

		std::vector<uint8_t>& getDataBuffer()
		{
			return m_Data;
		}

		uint64_t getStartTimeStamp()
		{
			return m_nStartTimeStamp;
		}

		uint64_t getEndTimeStamp()
		{
			return m_nEndTimeStamp;
		}

	};

	CStateJournalStream::CStateJournalStream(LibMCData::PJournalSession pJournalSession)
		: m_nCurrentTimeStamp (0), m_nStartTimeStamp (0), m_pJournalSession(pJournalSession), m_nChunkIndex (0), m_nJournalFlushInterval (0)
	{
		if (pJournalSession.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_nChunkCapacityInBytes = pJournalSession->GetChunkCapacity();
		m_nJournalFlushInterval = pJournalSession->GetFlushInterval();

	}

	CStateJournalStream::~CStateJournalStream()
	{

	}

	
	void CStateJournalStream::writeString(const uint32_t nID, const std::string& sValue)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);

		m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_STRING);
		m_pCurrentChunk->writeString(sValue);
	}


	void CStateJournalStream::startNewChunk()
	{
		std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);

		if (m_pCurrentChunk.get() != nullptr)
			m_ChunksToWrite.push_back(m_pCurrentChunk);

		m_pCurrentChunk = nullptr;
		m_pCurrentChunk = std::make_shared <CStateJournalStreamChunk>(m_nChunkIndex, m_nCurrentTimeStamp, m_nChunkCapacityInBytes);
		m_nChunkIndex++;
		
	}

	uint32_t CStateJournalStream::getJournalFlushInterval()
	{
		return m_nJournalFlushInterval;
	}


	void CStateJournalStream::writeTimeStamp(const uint64_t nAbsoluteTimeStamp)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);

		if (nAbsoluteTimeStamp < m_nCurrentTimeStamp)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTIMESTAMP);

		uint64_t nDeltaTime = nAbsoluteTimeStamp - m_nCurrentTimeStamp;
		if (nDeltaTime >= STATEJOURNAL_MAXTIMESTAMPDELTA)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTIMESTAMP);

		m_pCurrentChunk->writeCommand(((uint32_t) nDeltaTime) | STATEJOURNAL_COMMANDFLAG_TIMESTAMP);

		m_nCurrentTimeStamp = nAbsoluteTimeStamp;
	}

	void CStateJournalStream::writeBool(const uint32_t nID, bool bValue)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);

		if (bValue) {
			m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_BOOL | STATEJOURNAL_COMMANDFLAG_BOOL_TRUE);
		}
		else {
			m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_BOOL | STATEJOURNAL_COMMANDFLAG_BOOL_FALSE);
		}

	}


	void CStateJournalStream::writeInt64Delta(const uint32_t nID, int64_t nDelta)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);

		if (nDelta > 0) {
			m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_INTEGER | STATEJOURNAL_COMMANDFLAG_INTEGER_POSITIVE);
			m_pCurrentChunk->writeUint64((uint64_t) nDelta);
		}
		else {
			m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_INTEGER | STATEJOURNAL_COMMANDFLAG_INTEGER_NEGATIVE);
			m_pCurrentChunk->writeUint64((uint64_t)(- nDelta));
		}

	}
	
	void CStateJournalStream::writeDoubleDelta(const uint32_t nID, int64_t nDelta)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);

		if (nDelta > 0) {
			m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_DOUBLE | STATEJOURNAL_COMMANDFLAG_DOUBLE_POSITIVE);
			m_pCurrentChunk->writeUint64((uint64_t)nDelta);
		}
		else {
			m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_DOUBLE | STATEJOURNAL_COMMANDFLAG_DOUBLE_NEGATIVE);
			m_pCurrentChunk->writeUint64((uint64_t)(-nDelta));
		}

	}


	void CStateJournalStream::writeNameDefinition(const uint32_t nID, const std::string& sName)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);


		//m_pCurrentChunk->writeNameDefinition(nID, sName);
	}

	void CStateJournalStream::writeUnits(const uint32_t nID, double dUnits)
	{
		if (m_pCurrentChunk == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_NOCURRENTJOURNALCHUNK);

		m_pCurrentChunk->writeCommand(nID | STATEJOURNAL_COMMANDFLAG_UNITS);
		m_pCurrentChunk->writeDouble(dUnits);

	}

	void CStateJournalStream::writeChunksToDiskThreaded()
	{
		std::vector<PStateJournalStreamChunk> chunksToWrite;
		{
			// Copy over chunk pointers thread safe!
			std::lock_guard<std::mutex> lockGuard(m_ChunkChangeMutex);
			chunksToWrite = m_ChunksToWrite;

			// Clear write pipeline
			m_ChunksToWrite.clear();
		}

		for (auto pChunk : chunksToWrite) {
			//std::cout << "writing chunk " << pChunk->getChunkIndex() << std::endl;
			std::lock_guard<std::mutex> lockGuard(m_JournalSessionMutex);
			std::vector<uint8_t> & dataBuffer = pChunk->getDataBuffer();
			m_pJournalSession->WriteJournalChunkData(pChunk->getChunkIndex(), pChunk->getStartTimeStamp(), pChunk->getEndTimeStamp(), dataBuffer);
			
		}


	}



}


