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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CTelemetryReader

*/

#include "libmcdata_telemetryreader.hpp"
#include "libmcdata_interfaceexception.hpp"

#include "common_importstream_native.hpp"
#include "common_utils.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"
#include "amcdata_journal.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CTelemetryReaderChannel
**************************************************************************************************************************/

CTelemetryReaderChannel::CTelemetryReaderChannel(const std::string& sUUID, eTelemetryChannelType eChannelType, uint32_t nChannelIndex, const std::string& sIdentifier, const std::string& sDescription)
    : m_sUUID(sUUID), m_eChannelType(eChannelType), m_nChannelIndex(nChannelIndex), m_sIdentifier(sIdentifier), m_sDescription(sDescription)
{
}

CTelemetryReaderChannel::~CTelemetryReaderChannel()
{
}

std::string CTelemetryReaderChannel::getUUID() const
{
    return m_sUUID;
}

LibMCData::eTelemetryChannelType CTelemetryReaderChannel::getChannelType() const
{
    return m_eChannelType;
}

uint32_t CTelemetryReaderChannel::getChannelIndex() const
{
    return m_nChannelIndex;
}

std::string CTelemetryReaderChannel::getIdentifier() const
{
    return m_sIdentifier;
}

std::string CTelemetryReaderChannel::getDescription() const
{
    return m_sDescription;
}


/*************************************************************************************************************************
 Class definition of CTelemetryReaderFile
**************************************************************************************************************************/

CTelemetryReaderFile::CTelemetryReaderFile(int64_t nFileIndex, const std::string& sAbsoluteFileName)
    : m_nFileIndex(nFileIndex), m_sAbsoluteFileName(sAbsoluteFileName)
{
}

CTelemetryReaderFile::~CTelemetryReaderFile()
{
}

int64_t CTelemetryReaderFile::getFileIndex() const
{
    return m_nFileIndex;
}

std::string CTelemetryReaderFile::getAbsoluteFileName() const
{
    return m_sAbsoluteFileName;
}

void CTelemetryReaderFile::readBuffer(uint64_t nDataOffset, uint8_t* pBuffer, uint64_t nDataLength)
{
    if (m_pImportStream.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALREADERFILENOTOPEN);

    std::lock_guard<std::mutex> lockGuard(m_ImportStreamMutex);
    m_pImportStream->seekPosition(nDataOffset, true);
    m_pImportStream->readBuffer(pBuffer, nDataLength, true);
}

void CTelemetryReaderFile::ensureFileIsOpen()
{
    if (m_pImportStream.get() == nullptr)
        m_pImportStream = std::make_shared<AMCCommon::CImportStream_Native>(m_sAbsoluteFileName);
}

void CTelemetryReaderFile::closeFile()
{
    m_pImportStream = nullptr;
}


/*************************************************************************************************************************
 Class definition of CTelemetryReaderChunk
**************************************************************************************************************************/

CTelemetryReaderChunk::CTelemetryReaderChunk(int64_t nChunkIndex, PTelemetryReaderFile pDataFile, int64_t nStartTimeStamp, int64_t nEndTimeStamp, int64_t nEntryCount, int64_t nDataOffset, int64_t nDataLength)
    : m_nChunkIndex(nChunkIndex),
    m_pDataFile(pDataFile),
    m_nStartTimeStamp(nStartTimeStamp),
    m_nEndTimeStamp(nEndTimeStamp),
    m_nEntryCount(nEntryCount),
    m_nDataOffset(nDataOffset),
    m_nDataLength(nDataLength)
{
}

CTelemetryReaderChunk::~CTelemetryReaderChunk()
{
}

int64_t CTelemetryReaderChunk::getChunkIndex() const
{
    return m_nChunkIndex;
}

PTelemetryReaderFile CTelemetryReaderChunk::getDataFile() const
{
    return m_pDataFile;
}

int64_t CTelemetryReaderChunk::getStartTimeStamp() const
{
    return m_nStartTimeStamp;
}

int64_t CTelemetryReaderChunk::getEndTimeStamp() const
{
    return m_nEndTimeStamp;
}

int64_t CTelemetryReaderChunk::getEntryCount() const
{
    return m_nEntryCount;
}

int64_t CTelemetryReaderChunk::getDataOffset() const
{
    return m_nDataOffset;
}

int64_t CTelemetryReaderChunk::getDataLength() const
{
    return m_nDataLength;
}


/*************************************************************************************************************************
 Class definition of CTelemetryReader
**************************************************************************************************************************/

CTelemetryReader::CTelemetryReader(AMCData::PSQLHandler pSQLHandler, const std::string& sJournalUUID, const std::string& sJournalBasePath)
    : m_pSQLHandler(pSQLHandler),
    m_sSessionUUID(AMCCommon::CUtils::normalizeUUIDString(sJournalUUID)),
    m_sJournalBasePath(sJournalBasePath),
    m_nGlobalStartTimeStamp(0),
    m_nGlobalEndTimeStamp(0)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto pStatement = pSQLHandler->prepareStatement("SELECT starttime, logfilename FROM journals WHERE uuid=?");
    pStatement->setString(1, m_sSessionUUID);

    if (!pStatement->nextRow())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDJOURNALUUID, "could not find journal uuid: " + m_sSessionUUID);

    m_sStartTime = pStatement->getColumnString(1);
    std::string sLogFileName = pStatement->getColumnString(2);

    pStatement = nullptr;

    std::string sJournalFileName = m_sJournalBasePath + "/" + sLogFileName;

    m_pJournalSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sJournalFileName);

    // Load channels
    auto pChannelStatement = m_pJournalSQLHandler->prepareStatement("SELECT uuid, channeltype, channelindex, identifier, description FROM telemetry_channels ORDER BY channelindex");
    while (pChannelStatement->nextRow()) {
        std::string sUUID = pChannelStatement->getColumnString(1);
        std::string sChannelType = pChannelStatement->getColumnString(2);
        int64_t nChannelIndex = pChannelStatement->getColumnInt64(3);
        std::string sIdentifier = pChannelStatement->getColumnString(4);
        std::string sDescription = pChannelStatement->getColumnString(5);

        auto eChannelType = AMCData::CJournal::convertStringToTelemetryType(sChannelType);

        auto pChannel = std::make_shared<CTelemetryReaderChannel>(sUUID, eChannelType, (uint32_t)nChannelIndex, sIdentifier, sDescription);
        m_Channels.push_back(pChannel);
        m_ChannelsByIdentifier.insert(std::make_pair(sIdentifier, pChannel));
        m_ChannelsByIndex.insert(std::make_pair((uint32_t)nChannelIndex, pChannel));
    }
    pChannelStatement = nullptr;

    // Load data files
    auto pFileStatement = m_pJournalSQLHandler->prepareStatement("SELECT fileindex, filename FROM telemetry_datafiles ORDER BY fileindex");
    while (pFileStatement->nextRow()) {
        int64_t nFileIndex = pFileStatement->getColumnInt64(1);
        std::string sFileName = pFileStatement->getColumnString(2);

        if (nFileIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALFILEINDEX, "Negative telemetry file index: " + std::to_string(nFileIndex));

        std::string sAbsoluteFileName = m_sJournalBasePath + "/" + sFileName;

        auto pFile = std::make_shared<CTelemetryReaderFile>(nFileIndex, sAbsoluteFileName);

        auto iFileIter = m_FileMap.find(nFileIndex);
        if (iFileIter != m_FileMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALFILEINDEX, "Duplicate telemetry file index: " + std::to_string(nFileIndex));

        m_FileMap.insert(std::make_pair(nFileIndex, pFile));
        m_Files.push_back(pFile);
    }
    pFileStatement = nullptr;

    // Load chunks
    size_t nChunkCount = 0;

    auto pChunkStatement = m_pJournalSQLHandler->prepareStatement("SELECT chunkindex, fileindex, starttimestamp, endtimestamp, entrycount, dataoffset, datalength FROM telemetry_chunks ORDER BY chunkindex");
    while (pChunkStatement->nextRow()) {
        int64_t nChunkIndex = pChunkStatement->getColumnInt64(1);
        int64_t nFileIndex = pChunkStatement->getColumnInt64(2);
        int64_t nStartTimeStamp = pChunkStatement->getColumnInt64(3);
        int64_t nEndTimeStamp = pChunkStatement->getColumnInt64(4);
        int64_t nEntryCount = pChunkStatement->getColumnInt64(5);
        int64_t nDataOffset = pChunkStatement->getColumnInt64(6);
        int64_t nDataLength = pChunkStatement->getColumnInt64(7);

        if (nChunkIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALCHUNKINDEX, "Negative telemetry chunk index: " + std::to_string(nChunkIndex));
        if (nFileIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALFILEINDEX, "Negative telemetry file index: " + std::to_string(nFileIndex));
        if (nStartTimeStamp < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALSTARTTIMESTAMP, "Negative telemetry start time stamp: " + std::to_string(nStartTimeStamp));
        if (nEndTimeStamp < nStartTimeStamp)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNALENDTIMESTAMP, "Invalid telemetry end time stamp: " + std::to_string(nEndTimeStamp));
        if (nDataOffset < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALDATAOFFSET, "Negative telemetry data offset: " + std::to_string(nDataOffset));
        if (nDataLength < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALDATALENGTH, "Negative telemetry data length: " + std::to_string(nDataLength));

        // Compute global lifetime
        if (nChunkCount == 0) {
            m_nGlobalStartTimeStamp = nStartTimeStamp;
            m_nGlobalEndTimeStamp = nEndTimeStamp;
        }
        else {
            if ((uint64_t)nEndTimeStamp > m_nGlobalEndTimeStamp)
                m_nGlobalEndTimeStamp = nEndTimeStamp;
        }

        auto iChunkIter = m_ChunkMap.find(nChunkIndex);
        if (iChunkIter != m_ChunkMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALCHUNKINDEX, "Duplicate telemetry chunk index: " + std::to_string(nChunkIndex));

        auto iFileIter = m_FileMap.find(nFileIndex);
        if (iFileIter == m_FileMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALFILEINDEXNOTFOUND, "Telemetry file index not found: " + std::to_string(nFileIndex));

        auto pChunk = std::make_shared<CTelemetryReaderChunk>(nChunkIndex, iFileIter->second, nStartTimeStamp, nEndTimeStamp, nEntryCount, nDataOffset, nDataLength);
        m_ChunkMap.insert(std::make_pair(nChunkIndex, pChunk));

        if (nChunkIndex >= (int64_t)nChunkCount)
            nChunkCount = (size_t)nChunkIndex + 1;
    }

    // Linearize chunk map
    m_Chunks.resize(nChunkCount);
    for (auto iChunkIter : m_ChunkMap) {
        m_Chunks.at(iChunkIter.first) = iChunkIter.second;
    }

    pChunkStatement = nullptr;
}

CTelemetryReader::~CTelemetryReader()
{
    m_pJournalSQLHandler = nullptr;
    m_pSQLHandler = nullptr;
}

void CTelemetryReader::readTelemetryChunkEntries(PTelemetryReaderChunk pChunk, std::vector<sTelemetryChunkEntry>& entries)
{
    if (pChunk.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto pDataFile = pChunk->getDataFile();
    pDataFile->ensureFileIsOpen();

    int64_t nEntryCount = pChunk->getEntryCount();
    int64_t nExpectedLength = nEntryCount * sizeof(sTelemetryChunkEntry);
    
    if (pChunk->getDataLength() != nExpectedLength)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "Telemetry chunk data length mismatch");

    entries.resize(nEntryCount);
    if (nEntryCount > 0) {
        pDataFile->readBuffer(pChunk->getDataOffset(), (uint8_t*)entries.data(), nExpectedLength);
    }
}

std::string CTelemetryReader::GetSessionUUID()
{
    return m_sSessionUUID;
}

std::string CTelemetryReader::GetStartTime()
{
    return m_sStartTime;
}

LibMCData_uint64 CTelemetryReader::GetLifeTimeInMicroseconds()
{
    return m_nGlobalEndTimeStamp - m_nGlobalStartTimeStamp;
}

LibMCData_uint32 CTelemetryReader::GetChannelCount()
{
    return (uint32_t)m_Channels.size();
}

void CTelemetryReader::GetChannelInformation(const LibMCData_uint32 nChannelIndex, std::string& sChannelUUID, LibMCData::eTelemetryChannelType& eChannelType, std::string& sIdentifier, std::string& sDescription)
{
    if (nChannelIndex >= m_Channels.size())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDVARIABLEINDEX, "Invalid channel index: " + std::to_string(nChannelIndex));

    auto pChannel = m_Channels.at(nChannelIndex);
    sChannelUUID = pChannel->getUUID();
    eChannelType = pChannel->getChannelType();
    sIdentifier = pChannel->getIdentifier();
    sDescription = pChannel->getDescription();
}

bool CTelemetryReader::FindChannelByIdentifier(const std::string& sIdentifier, LibMCData_uint32& nChannelIndex)
{
    auto iIter = m_ChannelsByIdentifier.find(sIdentifier);
    if (iIter != m_ChannelsByIdentifier.end()) {
        nChannelIndex = iIter->second->getChannelIndex();
        return true;
    }
    nChannelIndex = 0;
    return false;
}

LibMCData_uint32 CTelemetryReader::GetChunkCount()
{
    return (uint32_t)m_Chunks.size();
}

void CTelemetryReader::GetChunkInformation(const LibMCData_uint32 nChunkIndex, LibMCData_uint64& nStartTimeStamp, LibMCData_uint64& nEndTimeStamp, LibMCData_uint64& nEntryCount)
{
    if (nChunkIndex >= m_Chunks.size())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDCHUNKINDEX, "Invalid chunk index: " + std::to_string(nChunkIndex));

    auto pChunk = m_Chunks.at(nChunkIndex);
    if (pChunk.get() != nullptr) {
        nStartTimeStamp = pChunk->getStartTimeStamp();
        nEndTimeStamp = pChunk->getEndTimeStamp();
        nEntryCount = pChunk->getEntryCount();
    }
    else {
        nStartTimeStamp = 0;
        nEndTimeStamp = 0;
        nEntryCount = 0;
    }
}

ITelemetryChunkData* CTelemetryReader::ReadChunkData(const LibMCData_uint32 nChunkIndex)
{
    auto iChunkIter = m_ChunkMap.find(nChunkIndex);
    if (iChunkIter == m_ChunkMap.end())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALCHUNKNOTFOUND, "Telemetry chunk not found: " + std::to_string(nChunkIndex));

    auto pChunk = iChunkIter->second;

    auto pResult = std::make_unique<CTelemetryChunkData>(nChunkIndex);
    pResult->setTimeInterval(pChunk->getStartTimeStamp(), pChunk->getEndTimeStamp());

    readTelemetryChunkEntries(pChunk, pResult->getEntriesInternal());

    return pResult.release();
}

void CTelemetryReader::FindChunksInTimeRange(const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, LibMCData_uint64 nChunkIndicesBufferSize, LibMCData_uint64* pChunkIndicesNeededCount, LibMCData_uint32* pChunkIndicesBuffer)
{
    std::vector<uint32_t> matchingChunks;

    for (auto& pChunk : m_Chunks) {
        if (pChunk.get() != nullptr) {
            uint64_t chunkStart = pChunk->getStartTimeStamp();
            uint64_t chunkEnd = pChunk->getEndTimeStamp();

            // Check if chunk overlaps with the time range
            if (chunkStart <= nEndTimeStampInMicroseconds && chunkEnd >= nStartTimeStampInMicroseconds) {
                matchingChunks.push_back((uint32_t)pChunk->getChunkIndex());
            }
        }
    }

    if (pChunkIndicesNeededCount != nullptr)
        *pChunkIndicesNeededCount = matchingChunks.size();

    if (pChunkIndicesBuffer != nullptr) {
        if (nChunkIndicesBufferSize < matchingChunks.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        uint32_t* pTarget = pChunkIndicesBuffer;
        for (auto index : matchingChunks) {
            *pTarget = index;
            pTarget++;
        }
    }
}

void CTelemetryReader::QueryIntervals(const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, const LibMCData_uint32 nChannelIndex, LibMCData_uint64 nIntervalsBufferSize, LibMCData_uint64* pIntervalsNeededCount, LibMCData::sTelemetryIntervalData* pIntervalsBuffer)
{
    // First pass: collect all entries in time range
    std::vector<sTelemetryChunkEntry> allEntries;

    for (auto& pChunk : m_Chunks) {
        if (pChunk.get() != nullptr) {
            uint64_t chunkStart = pChunk->getStartTimeStamp();
            uint64_t chunkEnd = pChunk->getEndTimeStamp();

            // Check if chunk might contain relevant data
            if (chunkStart <= nEndTimeStampInMicroseconds && chunkEnd >= nStartTimeStampInMicroseconds) {
                std::vector<sTelemetryChunkEntry> chunkEntries;
                readTelemetryChunkEntries(pChunk, chunkEntries);

                for (auto& entry : chunkEntries) {
                    if (entry.m_TimeStamp >= nStartTimeStampInMicroseconds && 
                        entry.m_TimeStamp <= nEndTimeStampInMicroseconds) {
                        if (nChannelIndex == 0xFFFFFFFF || entry.m_ChannelIndex == nChannelIndex) {
                            allEntries.push_back(entry);
                        }
                    }
                }
            }
        }
    }

    // Build map of interval start markers
    std::map<uint64_t, sTelemetryChunkEntry> startMarkers;
    std::vector<sTelemetryIntervalData> intervals;

    for (auto& entry : allEntries) {
        if (entry.m_EntryType == eTelemetryChunkEntryType::IntervalStartMarker) {
            startMarkers[entry.m_MarkerID] = entry;
        }
        else if (entry.m_EntryType == eTelemetryChunkEntryType::IntervalEndMarker) {
            auto iStart = startMarkers.find(entry.m_MarkerID);
            if (iStart != startMarkers.end()) {
                sTelemetryIntervalData interval;
                interval.m_MarkerID = entry.m_MarkerID;
                interval.m_ChannelIndex = entry.m_ChannelIndex;
                interval.m_StartTimeStamp = iStart->second.m_TimeStamp;
                interval.m_EndTimeStamp = entry.m_TimeStamp;
                interval.m_DurationInMicroseconds = entry.m_TimeStamp - iStart->second.m_TimeStamp;
                interval.m_ContextData = iStart->second.m_ContextData;
                intervals.push_back(interval);
            }
        }
    }

    if (pIntervalsNeededCount != nullptr)
        *pIntervalsNeededCount = intervals.size();

    if (pIntervalsBuffer != nullptr) {
        if (nIntervalsBufferSize < intervals.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        sTelemetryIntervalData* pTarget = pIntervalsBuffer;
        for (auto& interval : intervals) {
            *pTarget = interval;
            pTarget++;
        }
    }
}

void CTelemetryReader::QueryInstantMarkers(const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, const LibMCData_uint32 nChannelIndex, LibMCData_uint64 nEntriesBufferSize, LibMCData_uint64* pEntriesNeededCount, LibMCData::sTelemetryChunkEntry* pEntriesBuffer)
{
    std::vector<sTelemetryChunkEntry> instantMarkers;

    for (auto& pChunk : m_Chunks) {
        if (pChunk.get() != nullptr) {
            uint64_t chunkStart = pChunk->getStartTimeStamp();
            uint64_t chunkEnd = pChunk->getEndTimeStamp();

            if (chunkStart <= nEndTimeStampInMicroseconds && chunkEnd >= nStartTimeStampInMicroseconds) {
                std::vector<sTelemetryChunkEntry> chunkEntries;
                readTelemetryChunkEntries(pChunk, chunkEntries);

                for (auto& entry : chunkEntries) {
                    if (entry.m_EntryType == eTelemetryChunkEntryType::InstantMarker &&
                        entry.m_TimeStamp >= nStartTimeStampInMicroseconds &&
                        entry.m_TimeStamp <= nEndTimeStampInMicroseconds) {
                        if (nChannelIndex == 0xFFFFFFFF || entry.m_ChannelIndex == nChannelIndex) {
                            instantMarkers.push_back(entry);
                        }
                    }
                }
            }
        }
    }

    if (pEntriesNeededCount != nullptr)
        *pEntriesNeededCount = instantMarkers.size();

    if (pEntriesBuffer != nullptr) {
        if (nEntriesBufferSize < instantMarkers.size())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_BUFFERTOOSMALL);

        sTelemetryChunkEntry* pTarget = pEntriesBuffer;
        for (auto& entry : instantMarkers) {
            *pTarget = entry;
            pTarget++;
        }
    }
}

void CTelemetryReader::GetChannelStatistics(const LibMCData_uint32 nChannelIndex, const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, LibMCData_uint64& nIntervalCount, LibMCData_uint64& nInstantMarkerCount, LibMCData_uint64& nTotalDurationInMicroseconds, LibMCData_uint64& nMinDurationInMicroseconds, LibMCData_uint64& nMaxDurationInMicroseconds, LibMCData_uint64& nAvgDurationInMicroseconds)
{
    // Query intervals for statistics
    std::vector<sTelemetryIntervalData> intervals;
    QueryIntervals(nStartTimeStampInMicroseconds, nEndTimeStampInMicroseconds, nChannelIndex, 0, nullptr, nullptr);
    
    // Get actual intervals
    LibMCData_uint64 nNeededCount = 0;
    QueryIntervals(nStartTimeStampInMicroseconds, nEndTimeStampInMicroseconds, nChannelIndex, 0, &nNeededCount, nullptr);
    intervals.resize(nNeededCount);
    if (nNeededCount > 0) {
        QueryIntervals(nStartTimeStampInMicroseconds, nEndTimeStampInMicroseconds, nChannelIndex, nNeededCount, nullptr, intervals.data());
    }

    // Get instant marker count
    LibMCData_uint64 nInstantCount = 0;
    QueryInstantMarkers(nStartTimeStampInMicroseconds, nEndTimeStampInMicroseconds, nChannelIndex, 0, &nInstantCount, nullptr);

    nIntervalCount = intervals.size();
    nInstantMarkerCount = nInstantCount;
    nTotalDurationInMicroseconds = 0;
    nMinDurationInMicroseconds = 0;
    nMaxDurationInMicroseconds = 0;
    nAvgDurationInMicroseconds = 0;

    if (!intervals.empty()) {
        nMinDurationInMicroseconds = intervals[0].m_DurationInMicroseconds;
        nMaxDurationInMicroseconds = intervals[0].m_DurationInMicroseconds;

        for (auto& interval : intervals) {
            nTotalDurationInMicroseconds += interval.m_DurationInMicroseconds;
            if (interval.m_DurationInMicroseconds < nMinDurationInMicroseconds)
                nMinDurationInMicroseconds = interval.m_DurationInMicroseconds;
            if (interval.m_DurationInMicroseconds > nMaxDurationInMicroseconds)
                nMaxDurationInMicroseconds = interval.m_DurationInMicroseconds;
        }

        nAvgDurationInMicroseconds = nTotalDurationInMicroseconds / intervals.size();
    }
}
