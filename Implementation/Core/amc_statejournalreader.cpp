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


#include "amc_statejournalreader.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"
#include "libmc_exceptiontypes.hpp"
#include <iostream>
#include <mutex>

namespace AMC {



	CStateJournalStreamCache_Historic::CStateJournalStreamCache_Historic(uint64_t nMemoryQuota, CStateJournalReader* pOwner, PLogger pDebugLogger)
		: CStateJournalStreamCache (nMemoryQuota, pDebugLogger), m_pOwner (pOwner)
	{
		if (pOwner == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	CStateJournalStreamCache_Historic::~CStateJournalStreamCache_Historic()
	{

	}

	PStateJournalStreamChunk_InMemory CStateJournalStreamCache_Historic::loadEntryFromJournal(uint32_t nTimeChunkIndex)
	{
		PStateJournalStreamChunk_InMemory pChunk;
		{
			auto pChunkIntegerData = m_pOwner->readChunkIntegerData(nTimeChunkIndex);
			pChunk = std::make_shared<CStateJournalStreamChunk_InMemory>(pChunkIntegerData, m_pDebugLogger);

		}

		addEntry(pChunk);

		return pChunk;
	}


	CStateJournalReaderVariable::CStateJournalReaderVariable(uint32_t nVariableIndex, const std::string& sVariableName, uint32_t nVariableID, LibMCData::eParameterDataType dataType)
		: m_nVariableIndex (nVariableIndex), m_sVariableName (sVariableName), m_nVariableID (nVariableID), m_DataType (dataType)
	{

	}

	CStateJournalReaderVariable::~CStateJournalReaderVariable()
	{

	}

	uint32_t CStateJournalReaderVariable::getVariableIndex()
	{
		return m_nVariableIndex;
	}

	std::string CStateJournalReaderVariable::getVariableName()
	{
		return m_sVariableName;
	}

	uint32_t CStateJournalReaderVariable::getVariableID()
	{
		return m_nVariableID;
	}

	LibMCData::eParameterDataType CStateJournalReaderVariable::getDataType()
	{
		return m_DataType;
	}


	CStateJournalReaderChunk::CStateJournalReaderChunk(uint32_t nChunkIndex, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp)
		: m_nChunkIndex (nChunkIndex), m_nStartTimeStamp (nStartTimeStamp), m_nEndTimeStamp (nEndTimeStamp)
	{

	}

	CStateJournalReaderChunk::~CStateJournalReaderChunk()
	{

	}

	uint32_t CStateJournalReaderChunk::getChunkIndex()
	{
		return m_nChunkIndex;
	}

	uint64_t CStateJournalReaderChunk::getStartTimeStamp()
	{
		return m_nStartTimeStamp;
	}

	uint64_t CStateJournalReaderChunk::getEndTimeStamp()
	{
		return m_nEndTimeStamp;
	}



	CStateJournalReader::CStateJournalReader(LibMCData::PJournalReader pReader, uint64_t nMemoryQuota, PLogger pDebugLogger)
		: m_pJournalReader (pReader)
	{
		if (pReader.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_pStreamCache = std::make_shared<CStateJournalStreamCache_Historic>(nMemoryQuota, this, pDebugLogger);

		uint32_t nVariableCount = m_pJournalReader->GetVariableCount();
		for (uint32_t nVariableIndex = 0; nVariableIndex < nVariableCount; nVariableIndex++) {
			std::string sName;
			uint32_t nID = 0;
			LibMCData::eParameterDataType eDataType = LibMCData::eParameterDataType::Unknown;

			m_pJournalReader->GetVariableInformation(nVariableIndex, sName, nID, eDataType);

			auto pVariable = std::make_shared<CStateJournalReaderVariable>(nVariableIndex, sName, nID, eDataType);
			m_Variables.push_back(pVariable);
			if (!sName.empty()) {
				m_VariableNameMap.insert(std::make_pair (sName, pVariable));
			}
		}


		uint64_t nCurrentTimeStamp = 0;
		uint32_t nChunkCount = m_pJournalReader->GetChunkCount();
		for (uint32_t nChunkIndex = 0; nChunkIndex < nChunkCount; nChunkIndex++) 
		{
			uint64_t nStartTimeStamp = 0;
			uint64_t nEndTimeStamp = 0;
			m_pJournalReader->GetChunkInformation(nChunkIndex, nStartTimeStamp, nEndTimeStamp);

			auto pChunk = std::make_shared<CStateJournalReaderChunk>(nChunkIndex, nStartTimeStamp, nEndTimeStamp);

			// if (nStartTimeStamp == nEndTimeStamp == 0), then the data model is missing a chunk. 
			// TODO: Decide if that is a viable option to allow and just ignore. Or should there be an error triggered?.
			if (nEndTimeStamp > 0) {

				if (nEndTimeStamp <= nStartTimeStamp)
					throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDJOURNALCHUNKTIMESTAMP, "Invalid journal chunk time stamp: " + std::to_string(nStartTimeStamp) + "-" + std::to_string(nEndTimeStamp));

				if (nChunkIndex > 0) {
					if (nStartTimeStamp <= nCurrentTimeStamp) 
						throw ELibMCInterfaceException(LIBMC_ERROR_OVERLAPPINGJOURNALCHUNKTIMESTAMP, "Overlapping journal chunk time stamp: " + std::to_string (nStartTimeStamp) + "-" + std::to_string(nEndTimeStamp));
				}

				nCurrentTimeStamp = nEndTimeStamp;

				m_Chunks.push_back (pChunk);

			}
		}


	}

	CStateJournalReader::~CStateJournalReader()
	{
		m_pStreamCache = nullptr;
	}

	double CStateJournalReader::computeSample(const std::string& sName, const uint64_t nTimeStamp)
	{
		auto iVariableIter = m_VariableNameMap.find(sName);
		if (iVariableIter == m_VariableNameMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND, sName);

		auto pChunk = findChunkForTimestamp(nTimeStamp);
		if (pChunk.get() != nullptr) {
			auto pEntry = m_pStreamCache->retrieveEntry(pChunk->getChunkIndex());
			if (pEntry.get() == nullptr)
				pEntry = m_pStreamCache->loadEntryFromJournal(pChunk->getChunkIndex());

			int64_t nIntegerData = pEntry->sampleIntegerData(iVariableIter->second->getVariableIndex(), nTimeStamp);

			return (double)nIntegerData;			
		}

		return 0.0;
	}

	std::string CStateJournalReader::getStartTimeAsUTC()
	{
		std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
		return m_pJournalReader->GetStartTime();

	}

	uint64_t CStateJournalReader::getLifeTimeInMicroseconds()
	{
		std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
		return m_pJournalReader->GetLifeTimeInMicroseconds();
	}

	LibMCData::PJournalChunkIntegerData CStateJournalReader::readChunkIntegerData(uint32_t nChunkIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_JournalReaderMutex);
		return m_pJournalReader->ReadChunkIntegerData(nChunkIndex);

	}

	PStateJournalReaderChunk CStateJournalReader::findChunkForTimestamp(uint64_t targetTimestamp) 
	{

		if (m_Chunks.size() == 0)
			return nullptr;

		size_t left = 0;
		size_t right = m_Chunks.size() - 1;

		while (left <= right) {
			size_t mid = left + (right - left) / 2;
			auto & pChunk = m_Chunks.at (mid);

			if (pChunk->getStartTimeStamp() <= targetTimestamp && targetTimestamp <= pChunk->getEndTimeStamp()) {
				// Found the correct chunk
				return pChunk;
			}
			else if (targetTimestamp < pChunk->getStartTimeStamp()) {
				// Search the left half
				right = mid - 1;
			}
			else {
				// Search the right half
				left = mid + 1;
			}
		}

		// Return nullptr if no valid chunk is found
		return nullptr;
	}

}


