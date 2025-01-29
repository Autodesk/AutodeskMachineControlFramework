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


#ifndef __AMCDATA_SQLHANDLER
#define __AMCDATA_SQLHANDLER

#include <memory>
#include <string>
#include <thread>
#include <mutex>

#include "amcdata_sqlstatement.hpp"
#include "amcdata_sqltransaction.hpp"

namespace AMCData {


	class CSQLHandler;
	typedef std::shared_ptr<CSQLHandler> PSQLHandler;

	class CSQLStatement;
	typedef std::shared_ptr<CSQLStatement> PSQLStatement;

	class CSQLTransactionLock;
	typedef std::shared_ptr<CSQLTransactionLock> PSQLTransactionLock;

	class CSQLTransaction;
	typedef std::shared_ptr<CSQLTransaction> PSQLTransaction;

	class CSQLTransactionLock {
		private:
			std::lock_guard<std::mutex> m_lockGuard;

		public:
			CSQLTransactionLock(std::mutex& mutexToLock)
				: m_lockGuard (mutexToLock)
			{


			}

			virtual ~CSQLTransactionLock()
			{

			}

	};

	class CSQLHandler {
	private:

		std::mutex m_Mutex;
		
	public:

		CSQLHandler()
		{}

		virtual ~CSQLHandler()
		{}

		virtual PSQLStatement prepareStatement(const std::string& sSQLString)
		{
			return prepareStatementLocked(sSQLString, createLock());
		}

		virtual PSQLStatement prepareStatementLocked (const std::string& sSQLString, PSQLTransactionLock pLock) = 0;

		virtual PSQLTransaction beginTransaction() = 0;

		virtual PSQLTransactionLock createLock() {
			return std::make_shared<CSQLTransactionLock>(m_Mutex);
		}


	};

	
}


#endif //__AMCDATA_SQLHANDLER

