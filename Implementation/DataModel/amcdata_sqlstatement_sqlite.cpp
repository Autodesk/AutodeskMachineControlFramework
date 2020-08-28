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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "amcdata_sqlstatement_sqlite.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "sqlite3.h"
#include "common_utils.hpp"

namespace AMCData {

	CSQLStatement_SQLite::CSQLStatement_SQLite(CSQLHandler_SQLite* pHandler, void* pStmtHandle)
		: m_pHandler (pHandler), m_pStmtHandle (pStmtHandle), m_bAllowNext (true), m_bHasColumn (false), m_bHadRow (false)
	{
		if (pHandler == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
		if (pStmtHandle == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	}


	CSQLStatement_SQLite::~CSQLStatement_SQLite()
	{
		sqlite3_finalize((sqlite3_stmt*) m_pStmtHandle);
		m_bHasColumn = false;
	}

	void CSQLStatement_SQLite::setString(uint32_t nArgNo, const std::string & sValue)
	{
		if (sValue.length () > SQLITE_MAXSTATEMENTLENGTH)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		checkSQLiteError(sqlite3_bind_text ((sqlite3_stmt*)m_pStmtHandle, (int)nArgNo, sValue.c_str(), (int)sValue.length(), SQLITE_TRANSIENT));
	}

	void CSQLStatement_SQLite::setInt(uint32_t nArgNo, const int32_t nValue)
	{
		checkSQLiteError(sqlite3_bind_int((sqlite3_stmt*)m_pStmtHandle, (int)nArgNo, nValue));
	}

	void CSQLStatement_SQLite::setInt64(uint32_t nArgNo, const int64_t nValue)
	{
		checkSQLiteError(sqlite3_bind_int64((sqlite3_stmt*)m_pStmtHandle, (int)nArgNo, nValue));
	}

	void CSQLStatement_SQLite::setDouble(uint32_t nArgNo, const double dValue)
	{
		checkSQLiteError(sqlite3_bind_double((sqlite3_stmt*)m_pStmtHandle, (int)nArgNo, dValue));
	}

	void CSQLStatement_SQLite::setNull(uint32_t nArgNo)
	{
		checkSQLiteError(sqlite3_bind_null((sqlite3_stmt*)m_pStmtHandle, (int)nArgNo));
	}

	void CSQLStatement_SQLite::checkSQLiteError(int nError)
	{
		CSQLHandler_SQLite::checkSQLiteError(nError);
	}

	bool CSQLStatement_SQLite::nextRow()
	{
		m_bHasColumn = false;
		m_bHadRow = true;

		if (!m_bAllowNext)
			return false;

		int nResult = sqlite3_step((sqlite3_stmt*)m_pStmtHandle);
		switch (nResult) {
		case SQLITE_DONE:
			m_bAllowNext = false;
			m_bHasColumn = false;
			return false;
		case SQLITE_ROW:
			m_bAllowNext = true;
			m_bHasColumn = true;
			return true;
		default:
			checkSQLiteError(nResult);
			return false;
		}	
	}

	void CSQLStatement_SQLite::execute()
	{
		m_bHasColumn = false;
		m_bHadRow = true;

		if (!m_bAllowNext)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_TOOMANYRESULTROWS);

		int nResult = sqlite3_step((sqlite3_stmt*)m_pStmtHandle);
		switch (nResult) {
		case SQLITE_DONE:
			m_bAllowNext = false;
			m_bHasColumn = false;
			break;
		case SQLITE_ROW:
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_TOOMANYRESULTROWS);
		default:
			checkSQLiteError(nResult);
		}

	}



	std::string CSQLStatement_SQLite::getColumnString(uint32_t nIdx)
	{
		if (!m_bHasColumn)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCOLUMNAVAILABLE);
		if (nIdx == 0)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDINDEX);

		auto pText = sqlite3_column_text((sqlite3_stmt*)m_pStmtHandle, nIdx - 1);
		if (pText == nullptr)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDTEXTCOLUMN);
		return std::string((const char*) pText);

	}

	std::string CSQLStatement_SQLite::getColumnUUID(uint32_t nIdx)
	{
		return AMCCommon::CUtils::normalizeUUIDString (getColumnString (nIdx));
	}

	double CSQLStatement_SQLite::getColumnDouble(uint32_t nIdx)
	{
		if (!m_bHasColumn)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCOLUMNAVAILABLE);
		if (nIdx == 0)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDINDEX);

		return sqlite3_column_double((sqlite3_stmt*)m_pStmtHandle, nIdx - 1);
	}

	int32_t CSQLStatement_SQLite::getColumnInt(uint32_t nIdx)
	{
		if (!m_bHasColumn)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCOLUMNAVAILABLE);
		if (nIdx == 0)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDINDEX);

		return sqlite3_column_int((sqlite3_stmt*)m_pStmtHandle, nIdx - 1);
	}


	int64_t CSQLStatement_SQLite::getColumnInt64(uint32_t nIdx)
	{
		if (!m_bHasColumn)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCOLUMNAVAILABLE);
		if (nIdx == 0)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDINDEX);

		return sqlite3_column_int64((sqlite3_stmt*)m_pStmtHandle, nIdx - 1);
	}


	eSQLColumnType CSQLStatement_SQLite::getColumnType(uint32_t nIdx)
	{
		if (!m_bHasColumn)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOCOLUMNAVAILABLE);
		if (nIdx == 0)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDINDEX);

		int columnType = sqlite3_column_type((sqlite3_stmt*)m_pStmtHandle, nIdx - 1);
		switch (columnType) {
		case SQLITE_INTEGER:
			return eSQLColumnType::Int64;
		case SQLITE_FLOAT:
			return eSQLColumnType::Double;
		case SQLITE_TEXT:
			return eSQLColumnType::String;
		case SQLITE_BLOB:
			return eSQLColumnType::Blob;
		case SQLITE_NULL:
			return eSQLColumnType::Null;
		default:
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDCOLUMNTYPE);
		}

	}


}


