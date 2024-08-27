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

#define STATEJOURNAL_MAXTIMESTAMPDELTA  (256 * 1024 * 1024)

#define STATEJOURNAL_COMMANDFLAG_TIMESTAMP 0x00000000UL 

#define STATEJOURNAL_COMMANDFLAG_INTEGER 0x10000000UL
#define STATEJOURNAL_COMMANDFLAG_INTEGER_POSITIVE 0x00000000UL
#define STATEJOURNAL_COMMANDFLAG_INTEGER_NEGATIVE 0x08000000UL

#define STATEJOURNAL_COMMANDFLAG_STRING 0x20000000UL

#define STATEJOURNAL_COMMANDFLAG_DOUBLE 0x30000000UL
#define STATEJOURNAL_COMMANDFLAG_DOUBLE_POSITIVE 0x00000000UL
#define STATEJOURNAL_COMMANDFLAG_DOUBLE_NEGATIVE 0x08000000UL

#define STATEJOURNAL_COMMANDFLAG_BOOL 0x40000000UL
#define STATEJOURNAL_COMMANDFLAG_BOOL_FALSE 0x00000000UL
#define STATEJOURNAL_COMMANDFLAG_BOOL_TRUE 0x08000000UL

#define STATEJOURNAL_COMMANDFLAG_EVENT 0x50000000UL
#define STATEJOURNAL_COMMANDFLAG_DEFINITION 0x60000000UL
#define STATEJOURNAL_COMMANDFLAG_UNITS 0x70000000UL



namespace AMC {


	typedef struct _sJournalTimeStreamDoubleEntry {
		uint64_t m_nTimeStampInMicroSeconds;
		double m_dValue;
	} sJournalTimeStreamDoubleEntry;

	typedef struct _sJournalTimeStreamInt64Entry {
		uint64_t m_nTimeStampInMicroSeconds;
		int64_t m_nValue;
	} sJournalTimeStreamInt64Entry;


	typedef struct _sStateJournalInterval {
		uint64_t m_nStartTimeInMicroSeconds;
		uint64_t m_nEndTimeInMicroSeconds;
	} sStateJournalInterval;

	class CStateJournalStreamChunk
	{
	private:
		AMC::PLogger m_pDebugLogger;

	public:

		// pDebugLogger may be null
		CStateJournalStreamChunk(AMC::PLogger pDebugLogger);

		virtual ~CStateJournalStreamChunk();

		virtual uint64_t getStartTimeStampInMicroSeconds() = 0;

		virtual uint64_t getEndTimeStampInMicroSeconds() = 0;

		virtual uint64_t getChunkIndex() = 0;

		virtual int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) = 0;
		
		void debugLog(const std::string & sDebugMessage);

	};

	class CStateJournalStreamChunk_Dynamic;
	class CStateJournalStreamChunk_InMemory;
	class CStateJournalStreamChunk_OnDisk;

	typedef std::shared_ptr<CStateJournalStreamChunk> PStateJournalStreamChunk;
	typedef std::shared_ptr<CStateJournalStreamChunk_Dynamic> PStateJournalStreamChunk_Dynamic;
	typedef std::shared_ptr<CStateJournalStreamChunk_InMemory> PStateJournalStreamChunk_InMemory;
	typedef std::shared_ptr<CStateJournalStreamChunk_OnDisk> PStateJournalStreamChunk_OnDisk;

	class CStateJournalStreamCache
	{
	private:

		uint64_t m_nMemoryUsage;
		uint64_t m_nMemoryQuota;

		// Debug Logger Object, May be null
		PLogger m_pDebugLogger;

		// General Mutex for the cache
		std::mutex m_CacheMutex;

		// Special mutex for the journaling database session.
		std::mutex m_JournalSessionMutex;
		LibMCData::PJournalSession m_pJournalSession;

		// Doubly linked list to store the cache entries in LRU order
		std::list<std::pair<uint32_t, PStateJournalStreamChunk_InMemory>> m_CacheList;

		// Hash map to store the mapping from time chunk index to list iterator
		std::unordered_map<uint32_t, std::list<std::pair<uint32_t, PStateJournalStreamChunk_InMemory>>::iterator> m_CacheMap;

		// Enforces the memory quota (no mutex protection)
		void enforceMemoryQuotaInternal(uint64_t nAdditionalMemory);

		// Removes an entry (no mutex protection)
		void removeEntryInternal(uint32_t nTimeChunkIndex);

		// Adds an in memory chunk entry (mutex protected)
		void addEntry(PStateJournalStreamChunk_InMemory pChunk);

	public:

		CStateJournalStreamCache(uint64_t nMemoryQuota, LibMCData::PJournalSession pJournalSession, PLogger pDebugLogger);

		virtual ~CStateJournalStreamCache();

		uint64_t getMemoryQuota();

		uint64_t getCurrentMemoryUsage();

		void writeToJournal(PStateJournalStreamChunk_InMemory pChunk);

		void removeEntry(uint32_t nTimeChunkIndex);

		PStateJournalStreamChunk_InMemory retrieveEntry(uint32_t nTimeChunkIndex);

		PStateJournalStreamChunk_InMemory loadEntryFromJournal(uint32_t nTimeChunkIndex);

	};

	typedef std::shared_ptr<CStateJournalStreamCache> PStateJournalStreamCache;


	class CStateJournalStream
	{
	private:
		std::mutex m_ChunkChangeMutex;
		PStateJournalStreamChunk_Dynamic m_pCurrentChunk;

		// Optional Debug logger, maybe optional
		PLogger m_pDebugLogger;

		// Memory Stream Cache
		PStateJournalStreamCache m_Cache;

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
		CStateJournalStream(LibMCData::PJournalSession pJournalSession, PLogger pDebugLogger);
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
	};
	typedef std::shared_ptr<CStateJournalStream> PStateJournalStream;

	
}


#endif //__AMC_STATEJOURNALSTREAM

