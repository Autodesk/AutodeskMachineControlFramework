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


#ifndef __AMC_LOGGERENTRY
#define __AMC_LOGGERENTRY

#include <memory>
#include <string>

#include "libmcdata_types.hpp"

namespace AMC {

	typedef LibMCData::eLogLevel eLogLevel;

	class CLoggerEntry;
	typedef std::shared_ptr<CLoggerEntry> PLoggerEntry;

	class CLoggerEntry {
	private:
		uint32_t m_nID;
		std::string m_sMessage;
		std::string m_sSubSystem;
		eLogLevel m_logLevel;
		std::string m_sTimeStamp;
		
	public:

		CLoggerEntry(const uint32_t nID, const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel, const std::string& sTimeStamp)
			: m_nID (nID), m_sMessage (sMessage), m_sSubSystem (sSubSystem), m_logLevel (logLevel), m_sTimeStamp (sTimeStamp)
		{			
		}

		virtual ~CLoggerEntry()
		{			
		}

		uint32_t getID() const
		{
			return m_nID;
		}

		std::string getMessage () const
		{
			return m_sMessage;
		}
		
		std::string getSubSystem () const
		{
			return m_sSubSystem;
		}
		
		eLogLevel getlogLevel () const
		{
			return m_logLevel;
		}
		
		std::string getTimeStamp () const
		{
			return m_sTimeStamp;
		}
		

	};

	
}


#endif //__AMC_LOGGERENTRY

