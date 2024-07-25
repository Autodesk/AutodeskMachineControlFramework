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


Abstract: This is a stub class definition of CZIPStreamWriter

*/

#include "libmcenv_zipstreamwriter.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_streamreader.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CZIPStreamWriter 
**************************************************************************************************************************/

#define ZIPSTREAMCOPY_CHUNKSIZE (1024ULL * 1024ULL)

CZIPEntryStreamWriter::CZIPEntryStreamWriter(LibMCData::PStorageZIPWriter pZIPWriter, uint32_t nEntryID, const std::string& sUUID, const std::string& sName)
    : m_pZIPWriter (pZIPWriter), m_nEntryID (nEntryID), m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sName (sName)
{
    if (pZIPWriter.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "zip writer is null object");
}

CZIPEntryStreamWriter::~CZIPEntryStreamWriter()
{

}

std::string CZIPEntryStreamWriter::GetUUID()
{
    return m_sUUID;
}

std::string CZIPEntryStreamWriter::GetName()
{
    return m_sName;
}

std::string CZIPEntryStreamWriter::GetMIMEType()
{
    return "application/zip";
}

LibMCEnv_uint64 CZIPEntryStreamWriter::GetSize()
{
    return m_pZIPWriter->GetEntrySize(m_nEntryID);
}

LibMCEnv_uint64 CZIPEntryStreamWriter::GetWritePosition()
{
    return m_pZIPWriter->GetEntrySize(m_nEntryID);
}

void CZIPEntryStreamWriter::Seek(const LibMCEnv_uint64 nWritePosition)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_CANNOTSEEKZIPSTREAM);
}

bool CZIPEntryStreamWriter::IsFinished()
{
    return m_pZIPWriter->IsFinished();
}

void CZIPEntryStreamWriter::WriteData(const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8* pDataBuffer)
{
    m_pZIPWriter->WriteData(m_nEntryID, LibMCData::CInputVector<uint8_t>(pDataBuffer, nDataBufferSize));
}

void CZIPEntryStreamWriter::WriteString(const std::string& sData)
{
    if (!sData.empty()) {
        WriteData(sData.length(), (const uint8_t*)sData.c_str());
    }
}

void CZIPEntryStreamWriter::WriteLine(const std::string& sLine)
{
    WriteString(sLine + "\n");
}

void CZIPEntryStreamWriter::Finish()
{
    m_pZIPWriter->Finish();
}

void CZIPEntryStreamWriter::CopyFrom(IStreamReader* pStreamReader)
{
    if (pStreamReader == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "streamreader is null object");

    pStreamReader->Seek(0);
    size_t nSize = pStreamReader->GetSize();
    if (nSize > 0) {
        std::vector<uint8_t> Buffer;
        Buffer.resize(ZIPSTREAMCOPY_CHUNKSIZE);

        while (nSize > 0) {
            size_t nChunkSize = ZIPSTREAMCOPY_CHUNKSIZE;
            if (nChunkSize > nSize)
                nChunkSize = nSize;

            uint64_t dataNeeded = 0;
            pStreamReader->ReadData(nChunkSize, nChunkSize, &dataNeeded, Buffer.data());
            WriteData(nChunkSize, Buffer.data());

            nSize -= nChunkSize;
        }

    }
}

IStreamReader* CZIPEntryStreamWriter::GetStreamReader()
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_CANNOTREADFROMZIPSTREAM);
}


CZIPStreamWriter::CZIPStreamWriter(LibMCData::PDataModel pDataModel, LibMCData::PStorageZIPWriter pZIPWriter, const std::string& sUUID, const std::string& sName, AMCCommon::PChrono pGlobalChrono)
    : m_pDataModel(pDataModel),
    m_pZIPWriter(pZIPWriter),
    m_sName(sName),
    m_sUUID(AMCCommon::CUtils::normalizeUUIDString(sUUID)),
    m_pGlobalChrono (pGlobalChrono)

{
    if (pDataModel.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pZIPWriter.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pGlobalChrono.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CZIPStreamWriter::~CZIPStreamWriter()
{

}


ITempStreamWriter * CZIPStreamWriter::CreateZIPEntry(const std::string & sFileName)
{
    uint32_t nCurrentEntryID = m_pZIPWriter->StartNewEntry(sFileName, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());

    return new CZIPEntryStreamWriter(m_pZIPWriter, nCurrentEntryID, m_sUUID, sFileName);
}

void CZIPStreamWriter::Finish()
{
    m_pZIPWriter->FinishCurrentEntry();
    m_pZIPWriter->Finish();
}

void CZIPStreamWriter::CreateZIPEntryFromStream(const std::string& sFileName, IStreamReader* pStreamReader) 
{
    m_pZIPWriter->FinishCurrentEntry();

    if (pStreamReader == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    uint32_t nCurrentEntryID = m_pZIPWriter->StartNewEntry(sFileName, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());

    auto pWriter = std::make_unique<CZIPEntryStreamWriter>(m_pZIPWriter, nCurrentEntryID, m_sUUID, sFileName);
    pStreamReader->Seek(0);
    pWriter->CopyFrom(pStreamReader);
}

std::string CZIPStreamWriter::GetUUID()
{
    return m_sUUID;
}

std::string CZIPStreamWriter::GetName()
{
    return m_sName;
}

std::string CZIPStreamWriter::GetMIMEType()
{
    return "application/zip";
}

LibMCEnv_uint64 CZIPStreamWriter::GetSize()
{
    return m_pZIPWriter->GetZIPStreamSize();
}

bool CZIPStreamWriter::IsFinished()
{
    return m_pZIPWriter->IsFinished();
}


IStreamReader* CZIPStreamWriter::GetStreamReader()
{
    m_pZIPWriter->Finish();

    auto pStorage = m_pDataModel->CreateStorage();
    auto pStream = pStorage->RetrieveStream (m_sUUID);

    return new CStreamReader(pStorage, pStream);

}
