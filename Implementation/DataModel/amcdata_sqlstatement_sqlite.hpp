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


#ifndef __AMCDATA_SQLSTATEMENT_SQLITE
#define __AMCDATA_SQLSTATEMENT_SQLITE

#include <memory>
#include <string>

#include "amcdata_sqlstatement.hpp"
#include "amcdata_sqlhandler_sqlite.hpp"

namespace AMCData {


	class CSQLHandler_SQLite;
	typedef std::shared_ptr<CSQLHandler_SQLite> PSQLHandler_SQLite;

	class CSQLStatement_SQLite;
	typedef std::shared_ptr<CSQLStatement_SQLite> PSQLStatement_SQLite;

	class CSQLStatement_SQLite : public CSQLStatement {
	private:
		CSQLHandler_SQLite* m_pHandler; 
		void* m_pStmtHandle;

		bool m_bAllowNext;
		bool m_bHasColumn;
		bool m_bHadRow;
		

	protected:
	public:

		CSQLStatement_SQLite() = delete;
		CSQLStatement_SQLite(CSQLHandler_SQLite* pHandler, void* pStmtHandle);

		virtual ~CSQLStatement_SQLite();
			
		void setString (uint32_t nArgNo, const std::string & sValue) override;
		void setInt (uint32_t nArgNo, const int32_t nValue) override;
		void setInt64 (uint32_t nArgNo, const int64_t nValue) override;
		void setDouble (uint32_t nArgNo, const double dValue) override;
		void setNull (uint32_t nArgNo) override;

		bool nextRow() override;
		void execute() override;

		std::string getColumnString (uint32_t nIdx) override;
		double getColumnDouble (uint32_t nIdx) override;
		int32_t getColumnInt (uint32_t nIdx) override;
		int64_t getColumnInt64 (uint32_t nIdx) override;
		eSQLColumnType getColumnType (uint32_t nIdx) override;
		std::string getColumnUUID(uint32_t nIdx) override;

		static void checkSQLiteError(int nError);
		
	};

	
}


#endif //__AMCDATA_SQLSTATEMENT_SQLITE

