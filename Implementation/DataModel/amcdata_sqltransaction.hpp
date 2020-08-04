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


#ifndef __AMCDATA_SQLTRANSACTION
#define __AMCDATA_SQLTRANSACTION

#include "amcdata_sqlhandler.hpp"

#include <memory>
#include <string>


namespace AMCData {

	class CSQLTransaction;
	typedef std::shared_ptr<CSQLTransaction> PSQLTransaction;

	class CSQLHandler;
	typedef std::shared_ptr<CSQLHandler> PSQLHandler;

	class CSQLStatement;
	typedef std::shared_ptr<CSQLStatement> PSQLStatement;

	class CSQLTransaction {
	protected:
		bool m_bIsClosed;	
		CSQLHandler * m_pSQLHandler;
	
	public:

		CSQLTransaction(CSQLHandler * pSQLHandler);
		virtual ~CSQLTransaction();
		
		virtual void commit ();
		virtual void rollback ();

		PSQLStatement prepareStatement(const std::string& sSQLString);
		void executeStatement(const std::string& sSQLString);

	};

	
}


#endif //__AMCDATA_SQLTRANSACTION

