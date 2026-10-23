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

#include "amc_api_handler_logs.hpp"
#include "libmc_interfaceexception.hpp"
#include "amc_loggerentry.hpp"

#include <vector>
#include <memory>
#include <string>

using namespace AMC;

#define APIHANDLER_LOGS_MAXENTRIES 128

CAPIHandler_Logs::CAPIHandler_Logs(PLogger pLogger, const std::string& sClientHash)
	: CAPIHandler (sClientHash), m_pLogger (pLogger)
{
	if (pLogger.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	
}

CAPIHandler_Logs::~CAPIHandler_Logs()
{
	
}
				
std::string CAPIHandler_Logs::getBaseURI () 
{
	return "api/logs";
}

void CAPIHandler_Logs::handleListLogsRequest(CJSONWriter& writer, uint32_t nStartID)
{
	CJSONWriterArray logArray(writer);

	if (m_pLogger->supportsLogMessagesRetrieval()) {
		uint32_t nHeadID = m_pLogger->getLogMessageHeadID();

		uint32_t nEndID = nHeadID;
		if (nStartID == 0) {
			// No startID: return latest entries
			nStartID = (nEndID > APIHANDLER_LOGS_MAXENTRIES) ? (nEndID - APIHANDLER_LOGS_MAXENTRIES) : 0;
		}

		if (nStartID < nEndID) {
			std::vector<CLoggerEntry> logEntries;
			m_pLogger->retrieveLogMessages(logEntries, nStartID, nEndID, eLogLevel::Message);

			for (auto& entry : logEntries) {
				CJSONWriterObject entryObject(writer);
				entryObject.addInteger("id", entry.getID());
				entryObject.addString("subsystem", entry.getSubSystem());
				entryObject.addString("timestamp", entry.getTimeStamp());
				entryObject.addString("message", entry.getMessage());
				entryObject.addString("loglevel", entry.getlogLevelString());
				logArray.addObject(entryObject);
			}
		}
	}

	writer.addArray("logentries", logArray);
}


PAPIResponse CAPIHandler_Logs::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (requestType == eAPIRequestType::rtGet) {
		// Parse optional startID from URI: api/logs or api/logs/{startID}
		std::string sURIParam = sURI.substr(8); // remove "api/logs" from the URI
		if (!sURIParam.empty() && sURIParam[0] == '/')
			sURIParam = sURIParam.substr(1);

		uint32_t nStartID = 0;
		if (!sURIParam.empty()) {
			nStartID = std::stoul(sURIParam);
		}

		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_LOGS);
		handleListLogsRequest(writer, nStartID);
		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}

	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

		
	

