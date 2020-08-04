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


#ifndef __AMC_LOGGER_STDOUT
#define __AMC_LOGGER_STDOUT

#include <memory>
#include <string>
#include <vector>

#include "amc_logger.hpp"

namespace AMC {

	class CLogger_StdOut;
	typedef std::shared_ptr<CLogger_StdOut> PLogger_StdOut;

	class CLogger_StdOut : public CLogger {
	private:
		
	public:

		CLogger_StdOut();
		virtual ~CLogger_StdOut();

		void logMessageEx(const std::string& sMessage, const std::string& sSubSystem, const eLogLevel logLevel, const std::string& sTimeStamp) override;
		
		void retrieveLogMessages(std::vector<CLoggerEntry>& entryBuffer, const uint32_t startID, const uint32_t endID, const eLogLevel eMinLogLevel) override;

	};

	
}


#endif //__AMC_LOGGER_STDOUT

