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


#ifndef __AMC_LOGGER
#define __AMC_LOGGER

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

#include "common_utils.hpp"
#include "common_chrono.hpp"

#include "amc_loggerentry.hpp"

#define LOG_SUBSYSTEM_SYSTEM "system"
#define LOG_SUBSYSTEM_NETWORK "network"
#define LOG_SUBSYSTEM_UNKNOWN "unknown"

namespace AMC {

	class CLogger;
	typedef std::shared_ptr<CLogger> PLogger;

	class CLogger {
	private:
			
	public:

		CLogger()
		{}

		virtual ~CLogger()
		{}

		virtual void logMessageEx (const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel, const std::string& sTimeStamp) = 0;

		virtual bool supportsLogMessagesRetrieval()
		{
			return false;
		}

		virtual void retrieveLogMessages(std::vector<CLoggerEntry>& entryBuffer, const uint32_t startID, const uint32_t endID, const eLogLevel eMinLogLevel) = 0;
		
		void logMessage(const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel)
		{
			AMCCommon::CChrono chrono;
			logMessageEx(sMessage, sSubSystem, logLevel, chrono.getStartTimeISO8601TimeUTC());
		}



		static std::string logLevelToString(eLogLevel logLevel)
		{
			switch (logLevel) {
			case eLogLevel::FatalError: return "fatal";
			case eLogLevel::CriticalError: return "critical";
			case eLogLevel::Warning: return "warning";
			case eLogLevel::Message: return "message";
			case eLogLevel::Info: return "info";
			case eLogLevel::Debug: return "debug";
			default:
				return "unknown";
			}
		}
	};

	
}


#endif //__AMC_LOGGER

