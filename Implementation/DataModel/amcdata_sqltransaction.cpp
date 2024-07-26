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

#include "amcdata_sqltransaction.hpp"
#include "libmcdata_interfaceexception.hpp"



namespace AMCData {

	CSQLTransaction::CSQLTransaction(CSQLHandler* pSQLHandler, PSQLTransactionLock pLock)
		: m_pSQLHandler (pSQLHandler), m_bIsClosed(false), m_pLock (pLock)
	{
		if (!m_pSQLHandler)
			throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

		auto pStatement = m_pSQLHandler->prepareStatementLocked("BEGIN TRANSACTION;", m_pLock);
		pStatement->execute();

	}

	CSQLTransaction::~CSQLTransaction()
	{
		if ((m_pSQLHandler != nullptr) && (!m_bIsClosed))
			try {
				rollback();
			}
			catch (...) {	
			}
	}

	void CSQLTransaction::commit()
	{
		auto pStatement = m_pSQLHandler->prepareStatementLocked("COMMIT;", m_pLock);
		pStatement->execute();

		m_bIsClosed = true;
	}
	
	void CSQLTransaction::rollback()
	{
		auto pStatement = m_pSQLHandler->prepareStatementLocked("ROLLBACK;", m_pLock);
		pStatement->execute();

		m_bIsClosed = true;

	}

	PSQLStatement CSQLTransaction::prepareStatement(const std::string& sSQLString)
	{
		return m_pSQLHandler->prepareStatementLocked(sSQLString, m_pLock);
	}

	void CSQLTransaction::executeStatement(const std::string& sSQLString)
	{
		auto pStatement = m_pSQLHandler->prepareStatementLocked(sSQLString, m_pLock);
		pStatement->execute();
	}

}


