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


Abstract: This is the class declaration of CJournalReader

*/


#ifndef __LIBMCDATA_JOURNALREADER
#define __LIBMCDATA_JOURNALREADER

#include "libmcdata_interfaces.hpp"
#include "amcdata_journalchunkdatafile.hpp"
#include "libmcdata_journalchunkintegerdata.hpp"

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
 Class declaration of CJournalReader 
**************************************************************************************************************************/

class CJournalReaderVariable {
private:
    int64_t m_nVariableIndex;
    int64_t m_nVariableID;
    eParameterDataType m_VariableType;
    std::string m_sVariableName;
    double m_dUnits;

public:

    CJournalReaderVariable(int64_t nVariableIndex, int64_t nVariableID, eParameterDataType variableType, const std::string& sVariableName, double dUnits);

    virtual ~CJournalReaderVariable();

    int64_t getVariableIndex ();

    int64_t getVariableID ();

    eParameterDataType getVariableType ();

    std::string getVariableName ();

    double getUnits();
};




typedef std::shared_ptr<CJournalReaderVariable> PJournalReaderVariable;



class CJournalReaderAlias {
private:
    std::string m_sAliasName;
    PJournalReaderVariable m_pSourceVariable;

public:

    CJournalReaderAlias(const std::string & sAliasName, PJournalReaderVariable pSourceVariable);

    virtual ~CJournalReaderAlias();

    std::string getAliasName();

    PJournalReaderVariable getSourceVariable ();

    std::string getSourceVariableName();

};




typedef std::shared_ptr<CJournalReaderAlias> PJournalReaderAlias;



class CJournalReaderFile : public AMCData::CJournalChunkDataFile {
private:
    int64_t m_nFileIndex;
    std::string m_sAbsoluteFileName;

    std::mutex m_ImportStreamMutex;
    AMCCommon::PImportStream m_pImportStream;

   

public:

    CJournalReaderFile(int64_t nFileIndex, const std::string& sAbsoluteFileName);

    virtual ~CJournalReaderFile();

    int64_t getFileIndex ();

    std::string getAbsoluteFileName ();

    void readBuffer(uint64_t nDataOffset, uint8_t* pBuffer, uint64_t nDataLength) override;

    void ensureChunkFileIsOpen();

    void closeChunkFile();

};


typedef std::shared_ptr<CJournalReaderFile> PJournalReaderFile;


class CJournalReaderChunk {
private:
    int64_t m_nChunkIndex;
    PJournalReaderFile m_pJournalDataFile;
    int64_t m_nStartTimeStamp;
    int64_t m_nEndTimeStamp;
    int64_t m_nDataOffset;
    int64_t m_nDataLength;

public:

    CJournalReaderChunk(int64_t nChunkIndex, PJournalReaderFile pJournalDataFile, int64_t nStartTimeStamp, int64_t nEndTimeStamp, int64_t nDataOffset, int64_t nDataLength);

    virtual ~CJournalReaderChunk();

    int64_t getChunkIndex ();

    PJournalReaderFile getDataFile ();

    int64_t getStartTimeStamp ();

    int64_t getEndTimeStamp ();

    int64_t getDataOffset ();

    int64_t getDataLength ();

};


typedef std::shared_ptr<CJournalReaderChunk> PJournalReaderChunk;

class CJournalReader : public virtual IJournalReader, public virtual CBase {
private:
    AMCData::PSQLHandler m_pSQLHandler;

    AMCData::PSQLHandler m_pJournalSQLHandler;

    std::string m_sJournalUUID;

    std::string m_sJournalBasePath;

    std::string m_sStartTime;
    uint32_t m_nLifeTimeInMicroseconds;

    int32_t m_nSchemaVersion;

    std::map <std::string, PJournalReaderVariable> m_VariableNameMap;
    std::map <int64_t, PJournalReaderVariable> m_VariableIndexMap;
    std::map <int64_t, PJournalReaderVariable> m_VariableIDMap;
    std::vector <PJournalReaderVariable> m_Variables;

    std::vector <PJournalReaderAlias> m_Aliases;
    std::map <std::string, PJournalReaderAlias> m_AliasNameMap;

    std::map <int64_t, PJournalReaderFile> m_FileMap;
    std::vector<PJournalReaderFile> m_Files;

    std::map <int64_t, PJournalReaderChunk> m_ChunkMap;
    std::vector<PJournalReaderChunk> m_Chunks;

public:

    CJournalReader(AMCData::PSQLHandler pSQLHandler, const std::string & sJournalUUID, const std::string & sJournalBasePath);

    virtual ~CJournalReader ();

	std::string GetJournalUUID() override;

    std::string GetStartTime() override;

    LibMCData_uint64 GetLifeTimeInMicroseconds() override;

	IJournalChunkIntegerData * ReadChunkIntegerData(const LibMCData_uint32 nChunkIndex) override;

    LibMCData_uint32 GetVariableCount() override;

    void GetVariableInformation(const LibMCData_uint32 nVariableIndex, std::string& sVariableName, LibMCData_uint32& nVariableID, LibMCData::eParameterDataType& eDataType, LibMCData_double & dUnits) override;

    LibMCData_uint32 GetChunkCount() override;

    void GetChunkInformation(const LibMCData_uint32 nChunkIndex, LibMCData_uint64& nStartTimeStamp, LibMCData_uint64& nEndTimeStamp) override;

    LibMCData_uint32 GetAliasCount() override;

    void GetAliasInformation(const LibMCData_uint32 nAliasIndex, std::string& sAliasName, std::string& sSourceVariableName) override;

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_JOURNALREADER
