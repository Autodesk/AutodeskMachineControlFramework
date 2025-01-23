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


Abstract: This is a stub class definition of CJournalReader

*/

#include "libmcdata_journalreader.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "libmcdata_journalchunkintegerdata.hpp"

#include "common_importstream_native.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"
#include "amcdata_journal.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CJournalReader 
**************************************************************************************************************************/


CJournalReaderVariable::CJournalReaderVariable(int64_t nVariableIndex, int64_t nVariableID, eParameterDataType variableType, const std::string& sVariableName, double dUnits)
    : m_nVariableIndex (nVariableIndex), m_nVariableID (nVariableID), m_VariableType (variableType), m_sVariableName (sVariableName), m_dUnits (dUnits)
{

}

CJournalReaderVariable::~CJournalReaderVariable()
{

}

int64_t CJournalReaderVariable::getVariableIndex()
{
    return m_nVariableIndex;
}

int64_t CJournalReaderVariable::getVariableID()
{
    return m_nVariableID;
}

LibMCData::eParameterDataType CJournalReaderVariable::getVariableType()
{
    return m_VariableType;
}

std::string CJournalReaderVariable::getVariableName()
{
    return m_sVariableName;
}

double CJournalReaderVariable::getUnits()
{
    return m_dUnits;
}



CJournalReaderAlias::CJournalReaderAlias(const std::string& sAliasName, PJournalReaderVariable pSourceVariable)
    : m_sAliasName (sAliasName), m_pSourceVariable (pSourceVariable)
{
    if (pSourceVariable.get())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

CJournalReaderAlias::~CJournalReaderAlias()
{

}

std::string CJournalReaderAlias::getAliasName()
{
    return m_sAliasName;
}

PJournalReaderVariable CJournalReaderAlias::getSourceVariable()
{
    return m_pSourceVariable;
}

std::string CJournalReaderAlias::getSourceVariableName()
{
    return m_pSourceVariable->getVariableName();
}

CJournalReaderFile::CJournalReaderFile(int64_t nFileIndex, const std::string& sAbsoluteFileName)
    : m_nFileIndex (nFileIndex), m_sAbsoluteFileName (sAbsoluteFileName)
{    

}

CJournalReaderFile::~CJournalReaderFile()
{

}

int64_t CJournalReaderFile::getFileIndex()
{
    return m_nFileIndex;
}

std::string CJournalReaderFile::getAbsoluteFileName()
{
    return m_sAbsoluteFileName;
}


void CJournalReaderFile::readBuffer(uint64_t nDataOffset, uint8_t* pBuffer, uint64_t nDataLength)
{
    if (m_pImportStream.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALREADERFILENOTOPEN);

    std::lock_guard<std::mutex> lockGuard(m_ImportStreamMutex);
    m_pImportStream->seekPosition(nDataOffset, true);
    m_pImportStream->readBuffer(pBuffer, nDataLength, true);

}


void CJournalReaderFile::ensureChunkFileIsOpen()
{
    if (m_pImportStream.get() == nullptr)
        m_pImportStream = std::make_shared<AMCCommon::CImportStream_Native>(m_sAbsoluteFileName);
}

void CJournalReaderFile::closeChunkFile()
{
    m_pImportStream = nullptr;
}



CJournalReaderChunk::CJournalReaderChunk(int64_t nChunkIndex, PJournalReaderFile pJournalDataFile, int64_t nStartTimeStamp, int64_t nEndTimeStamp, int64_t nDataOffset, int64_t nDataLength)
    : m_nChunkIndex (nChunkIndex), 
    m_pJournalDataFile (pJournalDataFile), 
    m_nStartTimeStamp (nStartTimeStamp), 
    m_nEndTimeStamp (nEndTimeStamp),
    m_nDataOffset (nDataOffset), 
    m_nDataLength (nDataLength)
{

}

CJournalReaderChunk::~CJournalReaderChunk()
{

}

int64_t CJournalReaderChunk::getChunkIndex()
{
    return m_nChunkIndex;
}

PJournalReaderFile CJournalReaderChunk::getDataFile()
{
    return m_pJournalDataFile;
}

int64_t CJournalReaderChunk::getStartTimeStamp()
{
    return m_nStartTimeStamp;
}

int64_t CJournalReaderChunk::getEndTimeStamp()
{
    return m_nEndTimeStamp;
}

int64_t CJournalReaderChunk::getDataOffset()
{
    return m_nDataOffset;
}

int64_t CJournalReaderChunk::getDataLength()
{
    return m_nDataLength;
}




CJournalReader::CJournalReader(AMCData::PSQLHandler pSQLHandler, const std::string& sJournalUUID, const std::string& sJournalBasePath)
    : m_pSQLHandler (pSQLHandler), 
    m_sJournalUUID (AMCCommon::CUtils::normalizeUUIDString (sJournalUUID)), 
    m_sJournalBasePath (sJournalBasePath), 
    m_nSchemaVersion (0),
    m_nLifeTimeInMicroseconds (0)
{
    if (pSQLHandler.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    auto pStatement = pSQLHandler->prepareStatement("SELECT starttime, logfilename, schemaversion FROM journals WHERE uuid=?");
    pStatement->setString(1, m_sJournalUUID);

    if (!pStatement->nextRow ())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDJOURNALUUID, "could not find journal uuid: " + m_sJournalUUID);

    m_sStartTime = pStatement->getColumnString(1);
    std::string sLogFileName = pStatement->getColumnString(2);
    m_nSchemaVersion = pStatement->getColumnInt(3);

    pStatement = nullptr;

    std::string sJournalFileName = m_sJournalBasePath + "/" + sLogFileName;

    m_pJournalSQLHandler = std::make_shared<AMCData::CSQLHandler_SQLite>(sJournalFileName);

    size_t nVariableCount = 0;

    auto pVariableStatement = m_pJournalSQLHandler->prepareStatement("SELECT variableindex, variableid, variabletype, name, units FROM journal_variables ORDER BY variableindex");
    while (pVariableStatement->nextRow()) {
        int64_t nVariableIndex = pVariableStatement->getColumnInt64(1);
        int64_t nVariableID = pVariableStatement->getColumnInt64(2);
        std::string sVariableType = pVariableStatement->getColumnString(3);
        std::string sVariableName = pVariableStatement->getColumnString(4);
        double dUnits = pVariableStatement->getColumnDouble(5);

        if (nVariableIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALVARIABLEINDEX, "Negative journal variable index: " + std::to_string(nVariableIndex));
        if (nVariableID < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NONPOSITIVEJOURNALVARIABLEID, "Non-positive journal variable ID: " + std::to_string(nVariableID));
        if (sVariableName.empty ())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYJOURNALVARIABLENAME);

        auto variableType = AMCData::CJournal::convertStringToDataType(sVariableType);
        auto pVariable = std::make_shared<CJournalReaderVariable>(nVariableIndex, nVariableID, variableType, sVariableName, dUnits);

        auto iIDIter = m_VariableIDMap.find(nVariableID);
        if (iIDIter != m_VariableIDMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALVARIABLEID, "Duplicate Journal Variable ID: " + std::to_string (nVariableID));

        auto iIndexIter = m_VariableIndexMap.find(nVariableIndex);
        if (iIndexIter != m_VariableIndexMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALVARIABLEINDEX, "Duplicate Journal Variable Index: " + std::to_string(nVariableIndex));

        auto iNameIter = m_VariableNameMap.find(sVariableName);
        if (iNameIter != m_VariableNameMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALVARIABLENAME, "Duplicate Journal Variable Name: " + sVariableName);


        m_VariableIDMap.insert(std::make_pair (nVariableID, pVariable));
        m_VariableIndexMap.insert(std::make_pair(nVariableIndex, pVariable));
        m_VariableNameMap.insert(std::make_pair(sVariableName, pVariable));

        if (nVariableIndex >= (int64_t)nVariableCount)
            nVariableCount = (uint64_t)nVariableIndex + 1;

    }

    auto pAliasStatement = m_pJournalSQLHandler->prepareStatement("SELECT aliasname, sourcename FROM journal_variablealiases ORDER BY aliasname");
    while (pAliasStatement->nextRow()) {
        std::string sAliasName = pAliasStatement->getColumnString(1);
        std::string sSourceName = pAliasStatement->getColumnString(2);

        auto iIter = m_VariableNameMap.find(sSourceName);
        if (iIter == m_VariableNameMap.end ())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SOURCEOFJOURNALALIASNOTFOUND, "Source of variable alias not found: " + sSourceName);

        auto pAlias = std::make_shared<CJournalReaderAlias>(sAliasName, iIter->second);
        m_Aliases.push_back(pAlias);
        m_AliasNameMap.insert(std::make_pair (sAliasName, pAlias));

    }


    // Linearize Variable map
    m_Variables.resize(nVariableCount);
    for (auto iIndexIter : m_VariableIndexMap) {
        m_Variables.at(iIndexIter.first) = iIndexIter.second;
    }

    pVariableStatement = nullptr;

    auto pFileStatement = m_pJournalSQLHandler->prepareStatement("SELECT fileindex, filename FROM journal_datafiles ORDER BY fileindex");
    while (pFileStatement->nextRow()) {
        int64_t nFileIndex = pFileStatement->getColumnInt64(1);
        std::string sFileName = pFileStatement->getColumnString(2);

        if (nFileIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALFILEINDEX, "Negative journal file index: " + std::to_string(nFileIndex));


        std::string sAbsoluteFileName = m_sJournalBasePath + "/" + sFileName;

        auto pFile = std::make_shared<CJournalReaderFile>(nFileIndex, sAbsoluteFileName);

        auto iFileIter = m_FileMap.find(nFileIndex);
        if (iFileIter != m_FileMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALFILEINDEX, "Duplicate journal file index: " + std::to_string (nFileIndex));

        m_FileMap.insert(std::make_pair (nFileIndex, pFile));
        m_Files.push_back(pFile);

    }
    pFileStatement = nullptr;

    size_t nChunkCount = 0;

    auto pChunkStatement = m_pJournalSQLHandler->prepareStatement("SELECT chunkindex, fileindex, starttimestamp, endtimestamp, dataoffset, datalength FROM journal_chunks ORDER BY chunkindex");
    while (pChunkStatement->nextRow()) {
        int64_t nChunkIndex = pChunkStatement->getColumnInt64(1);
        int64_t nFileIndex = pChunkStatement->getColumnInt64(2);
        int64_t nStartTimeStamp = pChunkStatement->getColumnInt64(3);
        int64_t nEndTimeStamp = pChunkStatement->getColumnInt64(4);
        int64_t nDataOffset = pChunkStatement->getColumnInt64(5);
        int64_t nDataLength = pChunkStatement->getColumnInt64(6);

        if (nChunkIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALCHUNKINDEX, "Negative journal chunk index: " + std::to_string(nChunkIndex));
        if (nFileIndex < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALFILEINDEX, "Negative journal file index: " + std::to_string(nFileIndex) + " (chunk #" + std::to_string(nChunkIndex) + ")");
        if (nStartTimeStamp < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALSTARTTIMESTAMP, "Negative journal start time stamp: " + std::to_string(nStartTimeStamp) + " (chunk #" + std::to_string(nChunkIndex) + ")");
        if (nEndTimeStamp < nStartTimeStamp)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNALENDTIMESTAMP, "Invalid journal end time stamp: " + std::to_string(nEndTimeStamp) + " (chunk #" + std::to_string(nChunkIndex) + ")");
        if (nDataOffset < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALDATAOFFSET, "Negative journal data offset: " + std::to_string(nDataOffset) + " (chunk #" + std::to_string(nChunkIndex) + ")");
        if (nDataLength < 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NEGATIVEJOURNALDATALENGTH, "Negative journal data length: " + std::to_string(nDataLength) + " (chunk #" + std::to_string(nChunkIndex) + ")");

        if ((uint64_t)nStartTimeStamp < m_nLifeTimeInMicroseconds)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALTIMESTAMPSNOTINCREASING, "Journal timestamps not increasing: " + std::to_string(nStartTimeStamp) + " (chunk #" + std::to_string(nChunkIndex) + ")");

        m_nLifeTimeInMicroseconds = (uint32_t)nEndTimeStamp;

        auto iChunkIter = m_ChunkMap.find(nChunkIndex);
        if (iChunkIter != m_ChunkMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_DUPLICATEJOURNALCHUNKINDEX, "Duplicate journal chunk index: " + std::to_string(nChunkIndex));

        auto iFileIter = m_FileMap.find(nFileIndex);
        if (iFileIter == m_FileMap.end())
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALFILEINDEXNOTFOUND, "Journal file index not found: " + std::to_string(nFileIndex));

        auto pChunk = std::make_shared<CJournalReaderChunk>(nChunkIndex, iFileIter->second, nStartTimeStamp, nEndTimeStamp, nDataOffset, nDataLength);
        m_ChunkMap.insert(std::make_pair (nChunkIndex, pChunk));

        if (nChunkIndex >= (int64_t)nChunkCount)
            nChunkCount = (uint64_t)nChunkIndex + 1;
    }

    // Linearize chunk map
    m_Chunks.resize(nChunkCount);
    for (auto iChunkIter : m_ChunkMap) {
        m_Chunks.at(iChunkIter.first) = iChunkIter.second;
    }

    pChunkStatement = nullptr;

}

CJournalReader::~CJournalReader()
{
    m_pJournalSQLHandler = nullptr;
    m_pSQLHandler = nullptr;

}


std::string CJournalReader::GetJournalUUID()
{
    return m_sJournalUUID;
}

std::string CJournalReader::GetStartTime()
{
    return m_sStartTime;
}

IJournalChunkIntegerData * CJournalReader::ReadChunkIntegerData(const LibMCData_uint32 nChunkIndex)
{
    auto pResult = std::make_unique<CJournalChunkIntegerData>(nChunkIndex);

    auto iChunkIter = m_ChunkMap.find(nChunkIndex);
    if (iChunkIter == m_ChunkMap.end())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALCHUNKNOTFOUND);

    auto pChunk = iChunkIter->second;

    pResult->setTimeInterval(pChunk->getStartTimeStamp (), pChunk->getEndTimeStamp());

    auto pDataFile = pChunk->getDataFile();
    pDataFile->ensureChunkFileIsOpen();
    pDataFile->readJournalChunkIntegerData(pChunk->getDataOffset(), pChunk->getDataLength(), pResult->getVariableInfoInternal(), pResult->getTimeStampsInternal(), pResult->getValueDataInternal());

    return pResult.release();
}

LibMCData_uint64 CJournalReader::GetLifeTimeInMicroseconds()
{
    return m_nLifeTimeInMicroseconds;
}

LibMCData_uint32 CJournalReader::GetVariableCount()
{
    return (uint32_t)m_Variables.size();
}

void CJournalReader::GetVariableInformation(const LibMCData_uint32 nVariableIndex, std::string& sVariableName, LibMCData_uint32& nVariableID, LibMCData::eParameterDataType& eDataType, LibMCData_double & dUnits)
{
    if (nVariableIndex >= m_Variables.size())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDVARIABLEINDEX);

    auto pVariable = m_Variables.at(nVariableIndex);
    if (pVariable.get() != nullptr) {
        sVariableName = pVariable->getVariableName();
        nVariableID = (uint32_t)pVariable->getVariableID();
        eDataType = pVariable->getVariableType();
        if (eDataType == LibMCData::eParameterDataType::Double)
            dUnits = pVariable->getUnits();
        else
            dUnits = 0.0;

    }
    else {
        sVariableName = "";
        nVariableID = 0;
        eDataType = LibMCData::eParameterDataType::Unknown;
        dUnits = 0.0;

    }
}

LibMCData_uint32 CJournalReader::GetChunkCount()
{
    return (uint32_t)m_Chunks.size();
}

void CJournalReader::GetChunkInformation(const LibMCData_uint32 nChunkIndex, LibMCData_uint64& nStartTimeStamp, LibMCData_uint64& nEndTimeStamp)
{
    if (nChunkIndex >= m_Chunks.size())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDCHUNKINDEX);

    auto pChunk = m_Chunks.at(nChunkIndex);
    if (pChunk.get() != nullptr) {
        nStartTimeStamp = pChunk->getStartTimeStamp();
        nEndTimeStamp = pChunk->getEndTimeStamp();
    }
    else {
        nStartTimeStamp = 0;
        nEndTimeStamp = 0;
    }
}

LibMCData_uint32 CJournalReader::GetAliasCount()
{
    return (uint32_t)m_Aliases.size();
}

void CJournalReader::GetAliasInformation(const LibMCData_uint32 nAliasIndex, std::string& sAliasName, std::string& sSourceVariableName)
{
    if (nAliasIndex >= m_Aliases.size())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDALIASINDEX);

    auto pAlias = m_Aliases.at(nAliasIndex);

    sAliasName = pAlias->getAliasName ();
    sSourceVariableName = pAlias->getSourceVariableName ();

}


