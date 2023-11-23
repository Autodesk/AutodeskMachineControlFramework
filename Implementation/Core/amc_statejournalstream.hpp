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
#include <mutex>
#include <vector>
#include <map>

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

	class CStateJournalStreamChunk;
	typedef std::shared_ptr< CStateJournalStreamChunk> PStateJournalStreamChunk;


	class CStateJournalStream
	{
	private:
		std::mutex m_ChunkChangeMutex;
		PStateJournalStreamChunk m_pCurrentChunk;
		std::vector<PStateJournalStreamChunk> m_ChunksToWrite;

		uint32_t m_nChunkIndex;
		uint64_t m_nStartTimeStamp;
		uint64_t m_nCurrentTimeStamp;
		uint64_t m_nChunkCapacityInBytes;
		uint32_t m_nJournalFlushInterval;

		std::mutex m_JournalSessionMutex;
		LibMCData::PJournalSession m_pJournalSession;

	public:
		CStateJournalStream(LibMCData::PJournalSession pJournalSession);
		virtual ~CStateJournalStream();

		virtual void writeTimeStamp(const uint64_t nAbsoluteTimeStamp);
		virtual void writeBool(const uint32_t nID, bool bValue);
		virtual void writeInt64Delta(const uint32_t nID, int64_t nDelta);
		virtual void writeDoubleDelta(const uint32_t nID, int64_t nDelta);
		virtual void writeString(const uint32_t nID, const std::string & sValue);
		virtual void writeUnits(const uint32_t nID, double dUnits);

		virtual void writeNameDefinition(const uint32_t nID, const std::string& sName);

		void startNewChunk();

		uint32_t getJournalFlushInterval ();

		// Threaded function to write chunk buffers to disk!
		void writeChunksToDiskThreaded();

	};
	typedef std::shared_ptr<CStateJournalStream> PStateJournalStream;

	
}


#endif //__AMC_STATEJOURNALSTREAM

