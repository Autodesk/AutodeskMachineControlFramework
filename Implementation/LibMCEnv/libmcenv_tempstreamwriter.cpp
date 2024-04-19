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


Abstract: This is a stub class definition of CTempStreamWriter

*/

#include "libmcenv_tempstreamwriter.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CTempStreamWriter 
**************************************************************************************************************************/

CTempStreamWriter::CTempStreamWriter(LibMCData::PDataModel pDataModel, const std::string& sName, const std::string& sMIMEType, const std::string& sCurrentUserUUID, AMCCommon::PChrono pGlobalChrono)
    : m_pDataModel(pDataModel), m_sName(sName), m_sMIMEType(sMIMEType), m_nWritePosition (0), m_bIsFinished (false), m_pGlobalChrono (pGlobalChrono)
{
    if (pDataModel.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pGlobalChrono.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_sUUID = AMCCommon::CUtils::createUUID();

    auto pJournalSession = pDataModel->CreateJournalSession();

    std::string sJournalUUID = pJournalSession->GetSessionUUID();

    m_pStorage = m_pDataModel->CreateStorage();
    m_pStorage->BeginRandomWriteStream(m_sUUID, sName, sMIMEType, sCurrentUserUUID, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());
    m_pStorage->AttachStreamToJournal(m_sUUID, sJournalUUID);
}


CTempStreamWriter::~CTempStreamWriter()
{
    m_pStorage = nullptr;
    m_pDataModel = nullptr;
}


std::string CTempStreamWriter::GetUUID()
{
    return m_sUUID;
}

std::string CTempStreamWriter::GetName()
{
    return m_sName;
}

std::string CTempStreamWriter::GetMIMEType()
{
    return m_sMIMEType;
}

LibMCEnv_uint64 CTempStreamWriter::GetSize()
{
    return m_pStorage->GetRandomWriteStreamSize(m_sUUID);
}

LibMCEnv_uint64 CTempStreamWriter::GetWritePosition()
{
    return m_nWritePosition;
}

void CTempStreamWriter::Seek(const LibMCEnv_uint64 nWritePosition)
{
    m_nWritePosition = nWritePosition;
}

bool CTempStreamWriter::IsFinished()
{
    return m_bIsFinished;
}

void CTempStreamWriter::WriteData(const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8 * pDataBuffer)
{
    if (nDataBufferSize > 0) {
        m_pStorage->StoreRandomWriteStream(m_sUUID, m_nWritePosition, LibMCData::CInputVector<uint8_t>(pDataBuffer, nDataBufferSize));
        m_nWritePosition += nDataBufferSize;
    }
}

void CTempStreamWriter::WriteString(const std::string & sData)
{
    if (!sData.empty()) {
        WriteData(sData.length(), (const uint8_t*)sData.c_str());
    }
}

void CTempStreamWriter::WriteLine(const std::string & sLine)
{
    std::string sData = sLine + "\n";
    WriteString(sData);
}

void CTempStreamWriter::Finish()
{
    m_pStorage->FinishRandomWriteStream(m_sUUID);
    m_bIsFinished = true;
}

void CTempStreamWriter::CopyFrom(IStreamReader* pStreamReader)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}
