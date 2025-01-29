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

#include "amcdata_sqlhandler_sqlite.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "sqlite3.h"



namespace AMCData {

	CSQLHandler_SQLite::CSQLHandler_SQLite(const std::string& sFileName)
		: m_pDBHandle (nullptr)
	{
		sqlite3* pDBHandle = nullptr;
		sqlite3_open(sFileName.c_str(), &pDBHandle);

		m_pDBHandle = pDBHandle;
	}

	CSQLHandler_SQLite::~CSQLHandler_SQLite()
	{
		sqlite3_close((sqlite3*) m_pDBHandle);
	}

	PSQLStatement CSQLHandler_SQLite::prepareStatementLocked(const std::string& sSQLString, PSQLTransactionLock pLock) 
	{
		if (sSQLString.length() > SQLITE_MAXSTATEMENTLENGTH)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		sqlite3_stmt* pStmt = nullptr;
		checkSQLiteError(sqlite3_prepare_v2((sqlite3*)m_pDBHandle, sSQLString.c_str(), (int)sSQLString.length(), &pStmt, nullptr));
			
		return std::make_shared<CSQLStatement_SQLite>(this, (void *)pStmt, pLock);
	}

    void CSQLHandler_SQLite::checkSQLiteError(int nError)
	{
		std::string sErrorMessage;
		if (nError != SQLITE_OK) {
			if (m_pDBHandle != nullptr)
				sErrorMessage = sqlite3_errmsg((sqlite3*)m_pDBHandle);
		}

		switch (nError) {
		case SQLITE_OK: return;
		case SQLITE_ERROR: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_ERROR, sErrorMessage);
		case SQLITE_INTERNAL: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_INTERNAL, sErrorMessage);
		case SQLITE_PERM: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_PERM, sErrorMessage);
		case SQLITE_ABORT: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_ABORT, sErrorMessage);
		case SQLITE_BUSY: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_BUSY, sErrorMessage);
		case SQLITE_LOCKED: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_LOCKED, sErrorMessage);
		case SQLITE_NOMEM: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_NOMEM, sErrorMessage);
		case SQLITE_READONLY: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_READONLY, sErrorMessage);
		case SQLITE_INTERRUPT: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_INTERRUPT, sErrorMessage);
		case SQLITE_IOERR: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_IOERR, sErrorMessage);
		case SQLITE_CORRUPT: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_CORRUPT, sErrorMessage);
		case SQLITE_NOTFOUND: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_NOTFOUND, sErrorMessage);
		case SQLITE_FULL: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_FULL, sErrorMessage);
		case SQLITE_CANTOPEN: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_CANTOPEN, sErrorMessage);
		case SQLITE_PROTOCOL: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_PROTOCOL, sErrorMessage);
		case SQLITE_SCHEMA: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_SCHEMA, sErrorMessage);
		case SQLITE_TOOBIG: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_TOOBIG, sErrorMessage);
		case SQLITE_CONSTRAINT: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_CONSTRAINT, sErrorMessage);
		case SQLITE_MISMATCH: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_MISMATCH, sErrorMessage);
		case SQLITE_MISUSE: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_MISUSE, sErrorMessage);
		case SQLITE_NOLFS: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_NOLFS, sErrorMessage);
		case SQLITE_AUTH: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_AUTH, sErrorMessage);
		case SQLITE_RANGE: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_RANGE, sErrorMessage);
		case SQLITE_NOTADB: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_NOTADB, sErrorMessage);
		case SQLITE_NOTICE: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_NOTICE, sErrorMessage);
		case SQLITE_WARNING: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_WARNING, sErrorMessage);
		default: throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_SQLITE_OTHER);

		}

	}

	PSQLTransaction CSQLHandler_SQLite::beginTransaction()
	{
		return std::make_shared <CSQLTransaction>(this, createLock ());
	}


}


