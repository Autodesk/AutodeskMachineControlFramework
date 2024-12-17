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


#ifndef __AMC_STATEJOURNALSTREAMCACHE
#define __AMC_STATEJOURNALSTREAMCACHE

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

#define STATEJOURNALSTORAGE_MAXENTRIESPERCHUNK (128UL * 1024UL * 1024UL)


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


	class CStateJournalStreamCache;
	typedef std::shared_ptr<CStateJournalStreamCache> PStateJournalStreamCache;

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


	class CStateJournalStreamChunk_Dynamic : public CStateJournalStreamChunk
	{
	private:

		// Index of the chunk in the journal stream
		uint64_t m_nChunkIndex;

		// Start and end timestamps for this chunk in microseconds
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;

		// The latest timestamp written to this chunk, used for ensuring sequential writes
		uint64_t m_nCurrentTimeStampInMicroSeconds;

		// Vector holding maps for each variable, mapping relative timestamps to values
		std::vector<std::map<uint32_t, int64_t>> m_Data;

	public:

		// Constructor: Initializes chunk with given index, start/end timestamps, and number of variables
		CStateJournalStreamChunk_Dynamic(uint64_t nChunkIndex, uint64_t nStartTimeStampInMicroSeconds, uint64_t nEndTimeStampInMicroSeconds, uint32_t nVariableCount, AMC::PLogger pDebugLogger);


		// Destructor: Virtual to allow proper cleanup in derived classes
		virtual ~CStateJournalStreamChunk_Dynamic();

		// Get the index of this chunk in the journal stream
		uint64_t getChunkIndex() override;
		
		// Get the start timestamp for this chunk
		uint64_t getStartTimeStampInMicroSeconds() override;
		
		// Get the start timestamp for this chunk
		uint64_t getEndTimeStampInMicroSeconds() override;
		
		// Retrieve the value for a given variable at a specific absolute timestamp
		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) override;

		// Write a new value to the journal for a specific variable at a specific timestamp
		void writeEntry(uint32_t nStorageIndex, uint64_t nAbsoluteTimeStampInMicroseconds, int64_t nValue);

		// Get the number of variables being tracked in this chunk
		size_t getVariableCount();

		// Serialize the journal data into provided buffers for efficient storage or transmission
		void serialize(std::vector<LibMCData::sJournalChunkVariableInfo>& variableBuffer, std::vector<uint32_t>& timeStampBuffer, std::vector<int64_t>& valueBuffer);

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

		CStateJournalStreamChunk_InMemory(CStateJournalStreamChunk_Dynamic* pDynamicChunk, AMC::PLogger pDebugLogger);

		CStateJournalStreamChunk_InMemory(LibMCData::PJournalChunkIntegerData pIntegerData, AMC::PLogger pDebugLogger);

		virtual ~CStateJournalStreamChunk_InMemory();

		uint64_t getStartTimeStampInMicroSeconds() override;
		
		uint64_t getEndTimeStampInMicroSeconds() override;
		
		uint64_t getChunkIndex() override;
		
		void writeToJournal(LibMCData::PJournalSession pJournalSession);
		
		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds) override;

		uint64_t getMemoryUsage();

	};

	class CStateJournalStreamChunk_OnDisk : public CStateJournalStreamChunk
	{
	private:
		uint64_t m_nStartTimeStampInMicroSeconds;
		uint64_t m_nEndTimeStampInMicroSeconds;
		uint64_t m_nChunkIndex;

		PStateJournalStreamCache m_pStreamCache;

	public:
		CStateJournalStreamChunk_OnDisk(uint64_t nStartTimeStampInMicroSeconds, uint64_t nEndTimeStampInMicroSeconds, uint64_t nChunkIndex, PStateJournalStreamCache pStreamCache, AMC::PLogger pDebugLogger);

		virtual ~CStateJournalStreamChunk_OnDisk();

		uint64_t getStartTimeStampInMicroSeconds();
		
		uint64_t getEndTimeStampInMicroSeconds();

		uint64_t getChunkIndex();

		int64_t sampleIntegerData(const uint32_t nStorageIndex, const uint64_t nAbsoluteTimeStampInMicroseconds);

	};


	typedef std::shared_ptr<CStateJournalStreamChunk> PStateJournalStreamChunk;
	typedef std::shared_ptr<CStateJournalStreamChunk_InMemory> PStateJournalStreamChunk_InMemory;
	
	class CStateJournalStreamCache
	{
	protected:

		uint64_t m_nMemoryUsage;
		uint64_t m_nMemoryQuota;

		// Debug Logger Object, May be null
		PLogger m_pDebugLogger;

		// General Mutex for the cache
		std::mutex m_CacheMutex;

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

		CStateJournalStreamCache(uint64_t nMemoryQuota, PLogger pDebugLogger);

		virtual ~CStateJournalStreamCache();

		uint64_t getMemoryQuota();

		uint64_t getCurrentMemoryUsage();

		void removeEntry(uint32_t nTimeChunkIndex);

		PStateJournalStreamChunk_InMemory retrieveEntry(uint32_t nTimeChunkIndex);

		virtual PStateJournalStreamChunk_InMemory loadEntryFromJournal(uint32_t nTimeChunkIndex) = 0;

	};


	
}


#endif //__AMC_STATEJOURNALSTREAMCACHE

