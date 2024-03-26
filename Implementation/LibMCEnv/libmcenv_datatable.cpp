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


Abstract: This is a stub class definition of CDataTable

*/

#include "libmcenv_datatable.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDataTable 
**************************************************************************************************************************/

CDataTable::CDataTable()
{

}

CDataTable::~CDataTable()
{

}

void CDataTable::AddColumn(const std::string & sIdentifier, const std::string & sDescription, const LibMCEnv::eDataTableColumnType eColumnType)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::RemoveColumn(const std::string & sIdentifier)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CDataTable::HasColumn(const std::string & sIdentifier)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint32 CDataTable::GetRowCount()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint32 CDataTable::GetColumnCount()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CDataTable::GetColumnIdentifier(const LibMCEnv_uint32 nColumnIndex)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CDataTable::GetColumnDescription(const LibMCEnv_uint32 nColumnIndex)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv::eDataTableColumnType CDataTable::GetColumnType(const LibMCEnv_uint32 nColumnIndex)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::GetColumnInformation(const std::string & sIdentifier, std::string & sDescription, LibMCEnv::eDataTableColumnType & eColumnType)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::GetDoubleColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_double * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::GetInt32ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_int32 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::GetInt64ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_int64 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::GetUint32ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_uint32 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::GetUint64ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_uint64 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::SetDoubleColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_double * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::SetInt32ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_int32 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::SetInt64ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_int64 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::SetUint32ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_uint32 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CDataTable::SetUint64ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_uint64 * pValuesBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

