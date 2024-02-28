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

CTempStreamWriter::CTempStreamWriter(LibMCData::PDataModel pDataModel, const std::string& sName, const std::string& sMIMEType)
    : m_pDataModel(pDataModel), m_sName(sName), m_sMIMEType(sMIMEType)
{
    if (pDataModel.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_pStorage = m_pDataModel->CreateStorage();

    m_sUUID = AMCCommon::CUtils::createUUID();
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
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CTempStreamWriter::GetWritePosition()
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CTempStreamWriter::Seek(const LibMCEnv_uint64 nWritePosition)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CTempStreamWriter::IsFinished()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CTempStreamWriter::WriteData(const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8 * pDataBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CTempStreamWriter::WriteString(const std::string & sData)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CTempStreamWriter::WriteLine(const std::string & sLine)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CTempStreamWriter::Finish()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

