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


Abstract: This is the class declaration of CTelemetryReader

*/


#ifndef __LIBMCDATA_TELEMETRYREADER
#define __LIBMCDATA_TELEMETRYREADER

#include "libmcdata_interfaces.hpp"
#include "libmcdata_telemetrychunkdata.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "common_importstream_native.hpp"
#include "amcdata_sqlhandler.hpp"
#include <map>
#include <mutex>

namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CTelemetryReader 
**************************************************************************************************************************/

class CTelemetryReaderChannel {
private:
    std::string m_sUUID;
    eTelemetryChannelType m_eChannelType;
    uint32_t m_nChannelIndex;
    std::string m_sIdentifier;
    std::string m_sDescription;

public:

    CTelemetryReaderChannel(const std::string& sUUID, eTelemetryChannelType eChannelType, uint32_t nChannelIndex, const std::string& sIdentifier, const std::string& sDescription);

    virtual ~CTelemetryReaderChannel();

    std::string getUUID() const;

    eTelemetryChannelType getChannelType() const;

    uint32_t getChannelIndex() const;

    std::string getIdentifier() const;

    std::string getDescription() const;

};

typedef std::shared_ptr<CTelemetryReaderChannel> PTelemetryReaderChannel;


class CTelemetryReaderFile {
private:
    int64_t m_nFileIndex;
    std::string m_sAbsoluteFileName;

    std::mutex m_ImportStreamMutex;
    AMCCommon::PImportStream m_pImportStream;

public:

    CTelemetryReaderFile(int64_t nFileIndex, const std::string& sAbsoluteFileName);

    virtual ~CTelemetryReaderFile();

    int64_t getFileIndex() const;

    std::string getAbsoluteFileName() const;

    void readBuffer(uint64_t nDataOffset, uint8_t* pBuffer, uint64_t nDataLength);

    void ensureFileIsOpen();

    void closeFile();

};

typedef std::shared_ptr<CTelemetryReaderFile> PTelemetryReaderFile;


class CTelemetryReaderChunk {
private:
    int64_t m_nChunkIndex;
    PTelemetryReaderFile m_pDataFile;
    int64_t m_nStartTimeStamp;
    int64_t m_nEndTimeStamp;
    int64_t m_nEntryCount;
    int64_t m_nDataOffset;
    int64_t m_nDataLength;

public:

    CTelemetryReaderChunk(int64_t nChunkIndex, PTelemetryReaderFile pDataFile, int64_t nStartTimeStamp, int64_t nEndTimeStamp, int64_t nEntryCount, int64_t nDataOffset, int64_t nDataLength);

    virtual ~CTelemetryReaderChunk();

    int64_t getChunkIndex() const;

    PTelemetryReaderFile getDataFile() const;

    int64_t getStartTimeStamp() const;

    int64_t getEndTimeStamp() const;

    int64_t getEntryCount() const;

    int64_t getDataOffset() const;

    int64_t getDataLength() const;

};

typedef std::shared_ptr<CTelemetryReaderChunk> PTelemetryReaderChunk;


class CTelemetryReader : public virtual ITelemetryReader, public virtual CBase {
private:
    AMCData::PSQLHandler m_pSQLHandler;

    AMCData::PSQLHandler m_pJournalSQLHandler;

    std::string m_sSessionUUID;

    std::string m_sJournalBasePath;

    std::string m_sStartTime;

    uint64_t m_nGlobalStartTimeStamp;
    uint64_t m_nGlobalEndTimeStamp;

    std::vector<PTelemetryReaderChannel> m_Channels;
    std::map<std::string, PTelemetryReaderChannel> m_ChannelsByIdentifier;
    std::map<uint32_t, PTelemetryReaderChannel> m_ChannelsByIndex;

    std::map<int64_t, PTelemetryReaderFile> m_FileMap;
    std::vector<PTelemetryReaderFile> m_Files;

    std::map<int64_t, PTelemetryReaderChunk> m_ChunkMap;
    std::vector<PTelemetryReaderChunk> m_Chunks;

    void readTelemetryChunkEntries(PTelemetryReaderChunk pChunk, std::vector<sTelemetryChunkEntry>& entries);

public:

    CTelemetryReader(AMCData::PSQLHandler pSQLHandler, const std::string& sJournalUUID, const std::string& sJournalBasePath);

    virtual ~CTelemetryReader();

    std::string GetSessionUUID() override;

    std::string GetStartTime() override;

    LibMCData_uint64 GetLifeTimeInMicroseconds() override;

    LibMCData_uint32 GetChannelCount() override;

    void GetChannelInformation(const LibMCData_uint32 nChannelIndex, std::string& sChannelUUID, LibMCData::eTelemetryChannelType& eChannelType, std::string& sIdentifier, std::string& sDescription) override;

    bool FindChannelByIdentifier(const std::string& sIdentifier, LibMCData_uint32& nChannelIndex) override;

    LibMCData_uint32 GetChunkCount() override;

    void GetChunkInformation(const LibMCData_uint32 nChunkIndex, LibMCData_uint64& nStartTimeStamp, LibMCData_uint64& nEndTimeStamp, LibMCData_uint64& nEntryCount) override;

    ITelemetryChunkData* ReadChunkData(const LibMCData_uint32 nChunkIndex) override;

    void FindChunksInTimeRange(const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, LibMCData_uint64 nChunkIndicesBufferSize, LibMCData_uint64* pChunkIndicesNeededCount, LibMCData_uint32* pChunkIndicesBuffer) override;

    void QueryIntervals(const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, const LibMCData_uint32 nChannelIndex, LibMCData_uint64 nIntervalsBufferSize, LibMCData_uint64* pIntervalsNeededCount, LibMCData::sTelemetryIntervalData* pIntervalsBuffer) override;

    void QueryInstantMarkers(const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, const LibMCData_uint32 nChannelIndex, LibMCData_uint64 nEntriesBufferSize, LibMCData_uint64* pEntriesNeededCount, LibMCData::sTelemetryChunkEntry* pEntriesBuffer) override;

    void GetChannelStatistics(const LibMCData_uint32 nChannelIndex, const LibMCData_uint64 nStartTimeStampInMicroseconds, const LibMCData_uint64 nEndTimeStampInMicroseconds, LibMCData_uint64& nIntervalCount, LibMCData_uint64& nInstantMarkerCount, LibMCData_uint64& nTotalDurationInMicroseconds, LibMCData_uint64& nMinDurationInMicroseconds, LibMCData_uint64& nMaxDurationInMicroseconds, LibMCData_uint64& nAvgDurationInMicroseconds) override;

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_TELEMETRYREADER
