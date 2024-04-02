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


Abstract: This is the class declaration of CDataTable

*/


#ifndef __LIBMCENV_DATATABLE
#define __LIBMCENV_DATATABLE

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDataTable 
**************************************************************************************************************************/

class CDataTableColumn
{
private:
	
	std::string m_sIdentifier;
	std::string m_sDescription;

public:

	CDataTableColumn(const std::string& sIdentifier, const std::string & sDescription);

	virtual ~CDataTableColumn();

	virtual size_t getRowCount() = 0;

	virtual LibMCEnv::eDataTableColumnType getColumnType() = 0;

	std::string getIdentifier();

	std::string getDescription();

	void setDescription(const std::string& sDescription);

	virtual void writeCSVValue(size_t nRowIndex, std::vector<char>& buffer, size_t& nBufferPosition) = 0;

	virtual void WriteDataToStream(ITempStreamWriter* pWriter) = 0;

	virtual size_t getEntrySizeInBytes();
};

typedef std::shared_ptr<CDataTableColumn> PDataTableColumn;


class CDataTable : public virtual IDataTable, public virtual CBase {
private:

	size_t m_nMaxRowCount;

	std::map<std::string, PDataTableColumn> m_ColumnMap;
	std::vector<PDataTableColumn> m_Columns;

	void updateMaxRowCount ();

	CDataTableColumn* findColumn(const std::string& sIdentifier, bool bMustExist);

public:

	CDataTable();

	virtual ~CDataTable();

	void AddColumn(const std::string & sIdentifier, const std::string & sDescription, const LibMCEnv::eDataTableColumnType eColumnType) override;

	void RemoveColumn(const std::string & sIdentifier) override;

	bool HasColumn(const std::string & sIdentifier) override;

	LibMCEnv_uint32 GetRowCount() override;

	LibMCEnv_uint32 GetColumnCount() override;

	std::string GetColumnIdentifier(const LibMCEnv_uint32 nColumnIndex) override;

	std::string GetColumnDescription(const LibMCEnv_uint32 nColumnIndex) override;

	LibMCEnv::eDataTableColumnType GetColumnType(const LibMCEnv_uint32 nColumnIndex) override;

	void GetColumnInformation(const std::string & sIdentifier, std::string & sDescription, LibMCEnv::eDataTableColumnType & eColumnType) override;

	void GetDoubleColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_double * pValuesBuffer) override;

	void GetInt32ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_int32 * pValuesBuffer) override;

	void GetInt64ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_int64 * pValuesBuffer) override;

	void GetUint32ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_uint32 * pValuesBuffer) override;

	void GetUint64ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_uint64 * pValuesBuffer) override;

	void SetDoubleColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_double * pValuesBuffer) override;

	void SetInt32ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_int32 * pValuesBuffer) override;

	void SetInt64ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_int64 * pValuesBuffer) override;

	void SetUint32ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_uint32 * pValuesBuffer) override;

	void SetUint64ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_uint64 * pValuesBuffer) override;

	void WriteCSVToStream(ITempStreamWriter* pWriter, const std::string& sSeparator) override;

	void WriteDataToStream(ITempStreamWriter* pWriter, IDataTableWriteOptions* pOptions) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_DATATABLE
