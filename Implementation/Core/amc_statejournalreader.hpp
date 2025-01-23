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


#ifndef __AMC_STATEJOURNALREADER
#define __AMC_STATEJOURNALREADER

#include <memory>
#include <string>
#include <mutex>

#include "libmcdata_dynamic.hpp"
#include "common_chrono.hpp"
#include "amc_statejournalstreamcache.hpp"

namespace AMC {


	class CStateJournalReader;
	typedef std::shared_ptr<CStateJournalReader> PStateJournalReader;

	class CStateJournalStreamCache_Historic : public CStateJournalStreamCache
	{
	private:

		CStateJournalReader* m_pOwner;

	public:

		CStateJournalStreamCache_Historic(uint64_t nMemoryQuota, CStateJournalReader* pOwner, PLogger pDebugLogger);

		virtual ~CStateJournalStreamCache_Historic();
		
		PStateJournalStreamChunk_InMemory loadEntryFromJournal(uint32_t nTimeChunkIndex) override;

	};

	typedef std::shared_ptr<CStateJournalStreamCache_Historic> PStateJournalStreamCache_Historic;

	class CStateJournalReaderVariable {
	private:
		uint32_t m_nVariableIndex;
		std::string m_sVariableName;
		uint32_t m_nVariableID;
		LibMCData::eParameterDataType m_DataType;
		double m_dUnits;

	public:

		CStateJournalReaderVariable (uint32_t nVariableIndex, const std::string & sVariableName, uint32_t nVariableID, LibMCData::eParameterDataType dataType, double dUnits);

		virtual ~CStateJournalReaderVariable();

		uint32_t getVariableIndex ();

		std::string getVariableName ();

		uint32_t getVariableID ();

		LibMCData::eParameterDataType getDataType ();

		double getUnits();

	};

	typedef std::shared_ptr<CStateJournalReaderVariable> PStateJournalReaderVariable;


	class CStateJournalReaderChunk {
	private:
		uint32_t m_nChunkIndex;
		uint64_t m_nStartTimeStamp;
		uint64_t m_nEndTimeStamp;

	public:

		CStateJournalReaderChunk(uint32_t nChunkIndex, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp);

		virtual ~CStateJournalReaderChunk();

		uint32_t getChunkIndex();

		uint64_t getStartTimeStamp ();

		uint64_t getEndTimeStamp ();

	};

	typedef std::shared_ptr<CStateJournalReaderChunk> PStateJournalReaderChunk;


	class CStateJournalReader {
	protected:
		

		std::mutex m_JournalReaderMutex;
		LibMCData::PJournalReader m_pJournalReader;
		PStateJournalStreamCache_Historic m_pStreamCache;

		std::vector<PStateJournalReaderChunk> m_Chunks;
		std::vector<PStateJournalReaderVariable> m_Variables;
		std::map<std::string, PStateJournalReaderVariable> m_VariableNameMap;
		std::map<std::string, PStateJournalReaderVariable> m_AliasNameMap;

		PStateJournalReaderChunk findChunkForTimestamp(uint64_t targetTimestamp);

		PStateJournalReaderVariable findVariable(const std::string & sVariableOrAliasName);

	public:

		CStateJournalReader (LibMCData::PJournalReader pReader, uint64_t nMemoryQuota, PLogger pDebugLogger);

		virtual ~CStateJournalReader();

		double computeDoubleSample(const std::string& sName, const uint64_t nTimeStamp);

		int64_t computeIntegerSample(const std::string& sName, const uint64_t nTimeStamp);

		std::string getStartTimeAsUTC();

		uint64_t getLifeTimeInMicroseconds();

		LibMCData::PJournalChunkIntegerData readChunkIntegerData (uint32_t nChunkIndex);

	};

	
}


#endif //__AMC_STATEJOURNALREADER

