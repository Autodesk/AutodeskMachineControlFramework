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


#ifndef __AMC_STATEJOURNALSTREAM
#define __AMC_STATEJOURNALSTREAM

#include <memory>
#include <string>
#include <list>
#include <mutex>
#include <vector>
#include <map>
#include <queue>
#include <unordered_map>
#include "amc_logger.hpp"

#include "Common/common_exportstream_native.hpp"
#include "libmcdata_dynamic.hpp"

#include "amc_statejournalstreamcache.hpp"


namespace AMC {

	class CStateJournalStreamChunk_Dynamic;
	class CStateJournalStreamChunk_OnDisk;

	typedef std::shared_ptr<CStateJournalStreamChunk_Dynamic> PStateJournalStreamChunk_Dynamic;
	typedef std::shared_ptr<CStateJournalStreamChunk_OnDisk> PStateJournalStreamChunk_OnDisk;


	class CStateJournalStreamCache_Current : public CStateJournalStreamCache
	{
	private:

		// Special mutex for the journaling database session.
		std::mutex m_JournalSessionMutex;
		LibMCData::PJournalSession m_pJournalSession;

	public:

		CStateJournalStreamCache_Current(uint64_t nMemoryQuota, LibMCData::PJournalSession pJournalSession, PLogger pDebugLogger);

		virtual ~CStateJournalStreamCache_Current();

		void writeToJournal(PStateJournalStreamChunk_InMemory pChunk);

		PStateJournalStreamChunk_InMemory loadEntryFromJournal(uint32_t nTimeChunkIndex);

		void createVariableInJournalDB(const std::string & sName, uint32_t nVariableID, uint32_t nVariableIndex, LibMCData::eParameterDataType eVariableType, double dUnits);

		void createVariableAliasInJournalDB(const std::string & sAliasName, const std::string & sSourceVariableName);

	};

	typedef std::shared_ptr<CStateJournalStreamCache_Current> PStateJournalStreamCache_Current;


	class CStateJournalStream
	{
	private:
		std::mutex m_ChunkChangeMutex;
		PStateJournalStreamChunk_Dynamic m_pCurrentChunk;

		// Optional Debug logger, maybe optional
		PLogger m_pDebugLogger;

		// Memory Stream Cache
		PStateJournalStreamCache_Current m_Cache;

		// Total list of chunks of the stream
		std::vector<PStateJournalStreamChunk> m_ChunkTimeline;

		// Queue that holds all chunks that should be serialized in memory
		std::queue<PStateJournalStreamChunk_Dynamic> m_ChunksToSerialize;

		// Queue that holds all chunks that are serialized in memory but have not been written to disk.
		std::queue<PStateJournalStreamChunk_InMemory> m_ChunksToWrite;

		// Queue that holds all chunks that have been written to disk and could be freed from memory.
		std::queue<PStateJournalStreamChunk_OnDisk> m_ChunksToArchive;

		uint64_t m_nChunkIntervalInMicroseconds;
		uint64_t m_nCurrentTimeStampInMicroseconds;

		std::vector<uint64_t> m_CurrentVariableValues;

		void startNewChunk(const uint64_t nAbsoluteTimeStampInMicroseconds);
		void ensureChunk(const uint64_t nAbsoluteTimeStampInMicroseconds);

	public:
		CStateJournalStream(LibMCData::PJournalSession pJournalSession, PLogger pDebugLogger, bool bEnableDebugLogging);
		virtual ~CStateJournalStream();

		virtual void writeBool_MicroSecond(const uint64_t nAbsoluteTimeStampInMicroseconds, const uint32_t nStorageIndex, bool bValue);
		virtual void writeInt64_MicroSecond(const uint64_t nAbsoluteTimeStampInMicroseconds, const uint32_t nStorageIndex, int64_t nValue);
		virtual void writeDouble_MicroSecond(const uint64_t nAbsoluteTimeStampInMicroseconds, const uint32_t nStorageIndex, int64_t nValue);

		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds);
		double sampleDoubleData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds, double dUnits);
		bool sampleBoolData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds);

		// Threaded function to write chunk buffers to disk!
		void serializeChunksThreaded();
		void writeChunksToDiskThreaded();

		void setVariableCount (size_t nVariableCount);

		PStateJournalStreamCache_Current getCache ();
	};
	typedef std::shared_ptr<CStateJournalStream> PStateJournalStream;

	
}


#endif //__AMC_STATEJOURNALSTREAM

