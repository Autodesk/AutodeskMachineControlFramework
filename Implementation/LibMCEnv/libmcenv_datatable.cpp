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
#include "common_utils.hpp"
#include <vector>

#include "amc_constants.hpp"

using namespace LibMCEnv::Impl;


class CDataTableColumn_Double : public CDataTableColumn 
{
private:

	std::vector <double> m_Rows;

public:

	CDataTableColumn_Double (const std::string& sIdentifier, const std::string& sDescription)
		: CDataTableColumn (sIdentifier, sDescription)
	{

	}

	virtual ~CDataTableColumn_Double()
	{

	}

	size_t getRowCount()
	{
		return m_Rows.size();
	}

	LibMCEnv::eDataTableColumnType getColumnType()
	{
		return LibMCEnv::eDataTableColumnType::DoubleColumn;
	}

	void copyDataTo(double* pBuffer, size_t nBufferSize)
	{
		if (nBufferSize > 0) {
			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			size_t nCopyCount = nBufferSize;
			if (nCopyCount > m_Rows.size())
				nCopyCount = m_Rows.size();

			for (size_t nIndex = 0; nIndex < nCopyCount; nIndex++)
				pBuffer[nIndex] = m_Rows.at(nIndex);

			// Fill up buffer with 0.0 as value, when buffer is larger than the column's rowcount
			for (size_t nFillIndex = nCopyCount; nFillIndex < nBufferSize; nFillIndex++)
				pBuffer[nFillIndex] = 0.0;
		}
	}

	void copyDataFrom(const double* pBuffer, size_t nBufferSize)
	{
		m_Rows.resize(nBufferSize);

		if (nBufferSize > 0) {

			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			for (size_t nIndex = 0; nIndex < nBufferSize; nIndex++)
				m_Rows.at(nIndex) = pBuffer[nIndex];

		}
	}


};



class CDataTableColumn_Uint32 : public CDataTableColumn
{
private:

	std::vector <uint32_t> m_Rows;

public:

	CDataTableColumn_Uint32(const std::string& sIdentifier, const std::string& sDescription)
		: CDataTableColumn(sIdentifier, sDescription)
	{

	}

	virtual ~CDataTableColumn_Uint32()
	{

	}

	size_t getRowCount()
	{
		return m_Rows.size();
	}

	LibMCEnv::eDataTableColumnType getColumnType()
	{
		return LibMCEnv::eDataTableColumnType::Uint32Column;
	}

	void copyDataTo(uint32_t* pBuffer, size_t nBufferSize)
	{
		if (nBufferSize > 0) {
			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			size_t nCopyCount = nBufferSize;
			if (nCopyCount > m_Rows.size())
				nCopyCount = m_Rows.size();

			for (size_t nIndex = 0; nIndex < nCopyCount; nIndex++)
				pBuffer[nIndex] = m_Rows.at(nIndex);

			// Fill up buffer with 0.0 as value, when buffer is larger than the column's rowcount
			for (size_t nFillIndex = nCopyCount; nFillIndex < nBufferSize; nFillIndex++)
				pBuffer[nFillIndex] = 0.0;
		}
	}

	void copyDataFrom(const uint32_t* pBuffer, size_t nBufferSize)
	{
		m_Rows.resize(nBufferSize);

		if (nBufferSize > 0) {

			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			for (size_t nIndex = 0; nIndex < nBufferSize; nIndex++)
				m_Rows.at(nIndex) = pBuffer[nIndex];

		}
	}


};


class CDataTableColumn_Uint64 : public CDataTableColumn
{
private:

	std::vector <uint64_t> m_Rows;

public:

	CDataTableColumn_Uint64(const std::string& sIdentifier, const std::string& sDescription)
		: CDataTableColumn(sIdentifier, sDescription)
	{

	}

	virtual ~CDataTableColumn_Uint64()
	{

	}

	size_t getRowCount()
	{
		return m_Rows.size();
	}

	LibMCEnv::eDataTableColumnType getColumnType()
	{
		return LibMCEnv::eDataTableColumnType::Uint64Column;
	}

	void copyDataTo(uint64_t* pBuffer, size_t nBufferSize)
	{
		if (nBufferSize > 0) {
			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			size_t nCopyCount = nBufferSize;
			if (nCopyCount > m_Rows.size())
				nCopyCount = m_Rows.size();

			for (size_t nIndex = 0; nIndex < nCopyCount; nIndex++)
				pBuffer[nIndex] = m_Rows.at(nIndex);

			// Fill up buffer with 0.0 as value, when buffer is larger than the column's rowcount
			for (size_t nFillIndex = nCopyCount; nFillIndex < nBufferSize; nFillIndex++)
				pBuffer[nFillIndex] = 0.0;
		}
	}

	void copyDataFrom(const uint64_t* pBuffer, size_t nBufferSize)
	{
		m_Rows.resize(nBufferSize);

		if (nBufferSize > 0) {

			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			for (size_t nIndex = 0; nIndex < nBufferSize; nIndex++)
				m_Rows.at(nIndex) = pBuffer[nIndex];

		}
	}



};


class CDataTableColumn_Int32 : public CDataTableColumn
{
private:

	std::vector <int32_t> m_Rows;

public:

	CDataTableColumn_Int32(const std::string& sIdentifier, const std::string& sDescription)
		: CDataTableColumn(sIdentifier, sDescription)
	{

	}

	virtual ~CDataTableColumn_Int32()
	{

	}

	size_t getRowCount()
	{
		return m_Rows.size();
	}

	LibMCEnv::eDataTableColumnType getColumnType()
	{
		return LibMCEnv::eDataTableColumnType::Int32Column;
	}

	void copyDataTo(int32_t* pBuffer, size_t nBufferSize)
	{
		if (nBufferSize > 0) {
			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			size_t nCopyCount = nBufferSize;
			if (nCopyCount > m_Rows.size())
				nCopyCount = m_Rows.size();

			for (size_t nIndex = 0; nIndex < nCopyCount; nIndex++)
				pBuffer[nIndex] = m_Rows.at(nIndex);

			// Fill up buffer with 0.0 as value, when buffer is larger than the column's rowcount
			for (size_t nFillIndex = nCopyCount; nFillIndex < nBufferSize; nFillIndex++)
				pBuffer[nFillIndex] = 0.0;
		}
	}

	void copyDataFrom(const int32_t* pBuffer, size_t nBufferSize)
	{
		m_Rows.resize(nBufferSize);

		if (nBufferSize > 0) {

			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			for (size_t nIndex = 0; nIndex < nBufferSize; nIndex++)
				m_Rows.at(nIndex) = pBuffer[nIndex];

		}
	}

};


class CDataTableColumn_Int64 : public CDataTableColumn
{
private:

	std::vector <int64_t> m_Rows;

public:

	CDataTableColumn_Int64(const std::string& sIdentifier, const std::string& sDescription)
		: CDataTableColumn(sIdentifier, sDescription)
	{

	}

	virtual ~CDataTableColumn_Int64()
	{

	}

	size_t getRowCount()
	{
		return m_Rows.size();
	}

	LibMCEnv::eDataTableColumnType getColumnType()
	{
		return LibMCEnv::eDataTableColumnType::Int64Column;
	}

	void copyDataTo(int64_t* pBuffer, size_t nBufferSize)
	{
		if (nBufferSize > 0) {
			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			size_t nCopyCount = nBufferSize;
			if (nCopyCount > m_Rows.size())
				nCopyCount = m_Rows.size();

			for (size_t nIndex = 0; nIndex < nCopyCount; nIndex++)
				pBuffer[nIndex] = m_Rows.at(nIndex);

			// Fill up buffer with 0.0 as value, when buffer is larger than the column's rowcount
			for (size_t nFillIndex = nCopyCount; nFillIndex < nBufferSize; nFillIndex++)
				pBuffer[nFillIndex] = 0.0;
		}
	}

	void copyDataFrom(const int64_t* pBuffer, size_t nBufferSize)
	{
		m_Rows.resize(nBufferSize);

		if (nBufferSize > 0) {

			if (pBuffer == nullptr)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNBUFFERISNULL);

			for (size_t nIndex = 0; nIndex < nBufferSize; nIndex++)
				m_Rows.at(nIndex) = pBuffer[nIndex];

		}
	}

};

CDataTableColumn::CDataTableColumn(const std::string& sIdentifier, const std::string& sDescription)
	: m_sIdentifier (sIdentifier), m_sDescription (sDescription)
{
	if (sIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNIDENTIFIEREMPTY);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOLUMNIDENTIFIER, "invalid column identifier: " + sIdentifier);

}

CDataTableColumn::~CDataTableColumn()
{

}

std::string CDataTableColumn::getIdentifier()
{
	return m_sIdentifier;
}

std::string CDataTableColumn::getDescription()
{
	return m_sDescription;
}

void CDataTableColumn::setDescription(const std::string& sDescription)
{
	m_sDescription = sDescription;
}


CDataTable::CDataTable()
	: m_nMaxRowCount (0)
{

}

CDataTable::~CDataTable()
{

}



void CDataTable::AddColumn(const std::string & sIdentifier, const std::string & sDescription, const LibMCEnv::eDataTableColumnType eColumnType)
{
	if (sIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNIDENTIFIEREMPTY);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOLUMNIDENTIFIER, "invalid column identifier: " + sIdentifier);

	if (HasColumn (sIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DUPLICATECOLUMNIDENTIFIER, "duplicate column identifier: " + sIdentifier);

	if (m_Columns.size () >= DATATABLE_MAXCOLUMNCOUNT)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DATATABLEHASTOOMANYCOLUMS);

	PDataTableColumn pColumn;

	switch (eColumnType) {
	case LibMCEnv::eDataTableColumnType::DoubleColumn:
		pColumn = std::make_shared<CDataTableColumn_Double>(sIdentifier, sDescription);
		break;
	case LibMCEnv::eDataTableColumnType::Uint32Column:
		pColumn = std::make_shared<CDataTableColumn_Uint32>(sIdentifier, sDescription);
		break;
	case LibMCEnv::eDataTableColumnType::Uint64Column:
		pColumn = std::make_shared<CDataTableColumn_Uint64>(sIdentifier, sDescription);
		break;
	case LibMCEnv::eDataTableColumnType::Int32Column:
		pColumn = std::make_shared<CDataTableColumn_Int32>(sIdentifier, sDescription);
		break;
	case LibMCEnv::eDataTableColumnType::Int64Column:
		pColumn = std::make_shared<CDataTableColumn_Int64>(sIdentifier, sDescription);
		break;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOLUMNDATATYPE);
	}

	m_Columns.push_back(pColumn);
	m_ColumnMap.insert(std::make_pair (sIdentifier, pColumn));
}

void CDataTable::RemoveColumn(const std::string & sIdentifier)
{
	auto pFoundColumn = findColumn(sIdentifier, true);

	m_ColumnMap.erase(sIdentifier);
	m_Columns.erase(std::remove_if(m_Columns.begin(), m_Columns.end(), [pFoundColumn](PDataTableColumn & pColumn) {
		return (pFoundColumn ==  pColumn.get ());
	}), m_Columns.end());

}

bool CDataTable::HasColumn(const std::string & sIdentifier)
{
	auto iIter = m_ColumnMap.find(sIdentifier);
	return iIter != m_ColumnMap.end();
}

CDataTableColumn* CDataTable::findColumn(const std::string& sIdentifier, bool bMustExist)
{
	auto iIter = m_ColumnMap.find(sIdentifier);
	if (iIter == m_ColumnMap.end()) {
		if (bMustExist)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNIDENTIFIERNOTFOUND, "column identifier not found: " + sIdentifier);

		return nullptr;
	}

	return iIter->second.get();

}

LibMCEnv_uint32 CDataTable::GetRowCount()
{
	return m_nMaxRowCount;
}

LibMCEnv_uint32 CDataTable::GetColumnCount()
{
	return (uint32_t)m_Columns.size();
}

std::string CDataTable::GetColumnIdentifier(const LibMCEnv_uint32 nColumnIndex)
{
	if (nColumnIndex >= m_Columns.size ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOLUMNINDEX);

	return m_Columns.at(nColumnIndex)->getIdentifier();
}

std::string CDataTable::GetColumnDescription(const LibMCEnv_uint32 nColumnIndex)
{
	if (nColumnIndex >= m_Columns.size())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOLUMNINDEX);

	return m_Columns.at(nColumnIndex)->getDescription();
}

LibMCEnv::eDataTableColumnType CDataTable::GetColumnType(const LibMCEnv_uint32 nColumnIndex)
{
	if (nColumnIndex >= m_Columns.size())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOLUMNINDEX);

	return m_Columns.at(nColumnIndex)->getColumnType();
}

void CDataTable::GetColumnInformation(const std::string & sIdentifier, std::string & sDescription, LibMCEnv::eDataTableColumnType & eColumnType)
{
	auto pColumn = findColumn(sIdentifier, true);
	sDescription = pColumn->getDescription();
	eColumnType = pColumn->getColumnType();
}

void CDataTable::GetDoubleColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_double * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);

	auto pDoubleColumn = dynamic_cast<CDataTableColumn_Double*> (pColumn);

	if (pDoubleColumn == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEDOUBLE, "column is not of type double: " + sIdentifier);

	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nMaxRowCount;

	if (pValuesBuffer != nullptr) 
	{
		if (nValuesBufferSize < m_nMaxRowCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		pDoubleColumn->copyDataTo(pValuesBuffer, m_nMaxRowCount);
	}

}

void CDataTable::GetInt32ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_int32 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);

	auto pInt32Column = dynamic_cast<CDataTableColumn_Int32*> (pColumn);

	if (pInt32Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEINT32, "column is not of type int32: " + sIdentifier);

	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nMaxRowCount;

	if (pValuesBuffer != nullptr)
	{
		if (nValuesBufferSize < m_nMaxRowCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		pInt32Column->copyDataTo(pValuesBuffer, m_nMaxRowCount);
	}
}

void CDataTable::GetInt64ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_int64 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);

	auto pInt64Column = dynamic_cast<CDataTableColumn_Int64*> (pColumn);

	if (pInt64Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEINT64, "column is not of type int64: " + sIdentifier);

	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nMaxRowCount;

	if (pValuesBuffer != nullptr)
	{
		if (nValuesBufferSize < m_nMaxRowCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		pInt64Column->copyDataTo(pValuesBuffer, m_nMaxRowCount);
	}
}

void CDataTable::GetUint32ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_uint32 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);

	auto pUint32Column = dynamic_cast<CDataTableColumn_Uint32*> (pColumn);

	if (pUint32Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEUINT32, "column is not of type uint32: " + sIdentifier);

	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nMaxRowCount;

	if (pValuesBuffer != nullptr)
	{
		if (nValuesBufferSize < m_nMaxRowCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		pUint32Column->copyDataTo(pValuesBuffer, m_nMaxRowCount);
	}
}

void CDataTable::GetUint64ColumnValues(const std::string & sIdentifier, LibMCEnv_uint64 nValuesBufferSize, LibMCEnv_uint64* pValuesNeededCount, LibMCEnv_uint64 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);

	auto pUint64Column = dynamic_cast<CDataTableColumn_Uint64*> (pColumn);

	if (pUint64Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEUINT64, "column is not of type uint64: " + sIdentifier);

	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nMaxRowCount;

	if (pValuesBuffer != nullptr)
	{
		if (nValuesBufferSize < m_nMaxRowCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		pUint64Column->copyDataTo(pValuesBuffer, m_nMaxRowCount);
	}
}

void CDataTable::SetDoubleColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_double * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);
	auto pDoubleColumn = dynamic_cast<CDataTableColumn_Double*> (pColumn);

	if (pDoubleColumn == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEDOUBLE, "column is not of type double: " + sIdentifier);

	pDoubleColumn->copyDataFrom(pValuesBuffer, nValuesBufferSize);

	updateMaxRowCount();
}

void CDataTable::SetInt32ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_int32 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);
	auto pInt32Column = dynamic_cast<CDataTableColumn_Int32*> (pColumn);

	if (pInt32Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEINT32, "column is not of type int32: " + sIdentifier);

	pInt32Column->copyDataFrom(pValuesBuffer, nValuesBufferSize);

	updateMaxRowCount();
}

void CDataTable::SetInt64ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_int64 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);
	auto pInt64Column = dynamic_cast<CDataTableColumn_Int64*> (pColumn);

	if (pInt64Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEINT64, "column is not of type int64: " + sIdentifier);

	pInt64Column->copyDataFrom(pValuesBuffer, nValuesBufferSize);

	updateMaxRowCount();
}

void CDataTable::SetUint32ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_uint32 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);
	auto pUint32Column = dynamic_cast<CDataTableColumn_Uint32*> (pColumn);

	if (pUint32Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEUINT32, "column is not of type uint32: " + sIdentifier);

	pUint32Column->copyDataFrom(pValuesBuffer, nValuesBufferSize);

	updateMaxRowCount();
}

void CDataTable::SetUint64ColumnValues(const std::string & sIdentifier, const LibMCEnv_uint64 nValuesBufferSize, const LibMCEnv_uint64 * pValuesBuffer)
{
	auto pColumn = findColumn(sIdentifier, true);
	auto pUint64Column = dynamic_cast<CDataTableColumn_Uint64*> (pColumn);

	if (pUint64Column == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COLUMNISNOTOFTYPEUINT64, "column is not of type uint64: " + sIdentifier);

	pUint64Column->copyDataFrom(pValuesBuffer, nValuesBufferSize);

	updateMaxRowCount();
}


void CDataTable::updateMaxRowCount()
{
	m_nMaxRowCount = 0;
	for (auto& iColumn : m_Columns) {
		size_t nRowCount = iColumn->getRowCount();
		if (nRowCount > m_nMaxRowCount)
			m_nMaxRowCount = nRowCount;
	}
}

