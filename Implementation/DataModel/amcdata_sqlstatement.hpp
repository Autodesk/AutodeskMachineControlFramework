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


#ifndef __AMCDATA_SQLSTATEMENT
#define __AMCDATA_SQLSTATEMENT

#include <memory>
#include <string>

#include "amcdata_sqlhandler.hpp"
#include "amcdata_sqltransaction.hpp"

namespace AMCData {

	enum class eSQLColumnType : uint32_t {
		String = 1,
		Int = 2,
		Int64 = 3,
		Double = 4,
		Blob = 5,
		Null = 6,
		Unknown = 7
	};

	class CSQLHandler;
	typedef std::shared_ptr<CSQLHandler> PSQLHandler;

	class CSQLStatement;
	typedef std::shared_ptr<CSQLStatement> PSQLStatement;

	class CSQLStatement {
	private:

	public:

		CSQLStatement()
		{}

		virtual ~CSQLStatement()
		{}
		
		virtual void setString (uint32_t nArgNo, const std::string & sValue) = 0;
		virtual void setInt (uint32_t nArgNo, const int32_t nValue) = 0;
		virtual void setInt64 (uint32_t nArgNo, const int64_t nValue) = 0;
		virtual void setDouble (uint32_t nArgNo, const double dValue) = 0;
		virtual void setNull (uint32_t nArgNo) = 0;

		virtual bool nextRow() = 0;
		virtual void execute() = 0;

		virtual std::string getColumnString(uint32_t nIdx) = 0;
		virtual double getColumnDouble(uint32_t nIdx) = 0;
		virtual int32_t getColumnInt(uint32_t nIdx) = 0;
		virtual int64_t getColumnInt64(uint32_t nIdx) = 0;
		virtual eSQLColumnType getColumnType(uint32_t nIdx) = 0;
		virtual std::string getColumnUUID(uint32_t nIdx) = 0;


	};

	
}


#endif //__AMCDATA_SQLSTATEMENT

