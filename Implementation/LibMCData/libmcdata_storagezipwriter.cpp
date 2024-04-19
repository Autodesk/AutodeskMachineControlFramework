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


Abstract: This is a stub class definition of CStorageZIPWriter

*/

#include "libmcdata_storagezipwriter.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CStorageZIPWriter 
**************************************************************************************************************************/


CStorageZIPWriter::CStorageZIPWriter(AMCData::PStorageWriter_ZIPStream pStorageWriter)
	: m_pStorageWriter (pStorageWriter)
{
	if (pStorageWriter.get () == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);

}

CStorageZIPWriter::~CStorageZIPWriter()
{
	m_pStorageWriter = nullptr;
}

LibMCData_uint32 CStorageZIPWriter::StartNewEntry(const std::string & sFileName, const LibMCData_uint64 nAbsoluteTimeStamp)
{
	return m_pStorageWriter->startNewEntry(sFileName, nAbsoluteTimeStamp);
}

void CStorageZIPWriter::FinishCurrentEntry()
{
	m_pStorageWriter->finishCurrentEntry();
}

LibMCData_uint32 CStorageZIPWriter::GetOpenEntryID()
{
	return m_pStorageWriter->getOpenEntryID();
}

void CStorageZIPWriter::WriteData(const LibMCData_uint32 nEntryID, const LibMCData_uint64 nDataBufferSize, const LibMCData_uint8 * pDataBuffer)
{
	m_pStorageWriter->writeToCurrentEntry(nEntryID, pDataBuffer, nDataBufferSize);
}

LibMCData_uint64 CStorageZIPWriter::GetEntrySize(const LibMCData_uint32 nEntryID)
{
	return m_pStorageWriter->getEntrySize(nEntryID);
}

void CStorageZIPWriter::Finish()
{
}

bool CStorageZIPWriter::IsFinished()
{
	return false;
}

