/*++

Copyright (C) 2026 Autodesk Inc.

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

#include "amc_api_handler_journal.hpp"
#include "amc_statejournal.hpp"

#include "libmc_interfaceexception.hpp"
#include "common_utils.hpp"

#include <sstream>
#include <algorithm>

using namespace AMC;

#define AMC_API_PROTOCOL_JOURNAL "com.autodesk.machinecontrol.journal"

#define JOURNAL_DEFAULT_BUCKETCOUNT 1000
#define JOURNAL_MIN_BUCKETCOUNT 1
#define JOURNAL_MAX_BUCKETCOUNT 20000

namespace
{
	std::string parameterDataTypeToString(LibMCData::eParameterDataType eType)
	{
		switch (eType) {
		case LibMCData::eParameterDataType::Bool: return "bool";
		case LibMCData::eParameterDataType::Integer: return "integer";
		case LibMCData::eParameterDataType::Double: return "double";
		case LibMCData::eParameterDataType::String: return "string";
		default: return "unknown";
		}
	}

	uint64_t parseUint64Parameter(CAPIFormFields& formFields, const std::string& sName, uint64_t nDefaultValue)
	{
		if (!formFields.hasRequestParameter(sName))
			return nDefaultValue;

		std::string sValue = formFields.getRequestParameter(sName, false);
		if (sValue.empty())
			return nDefaultValue;

		try {
			return std::stoull(sValue);
		}
		catch (std::exception&) {
			return nDefaultValue;
		}
	}

	std::vector<std::string> splitCommaSeparated(const std::string& sValue)
	{
		std::vector<std::string> result;
		std::stringstream stream(sValue);
		std::string sItem;
		while (std::getline(stream, sItem, ',')) {
			// trim surrounding whitespace
			size_t nStart = sItem.find_first_not_of(" \t\r\n");
			size_t nEnd = sItem.find_last_not_of(" \t\r\n");
			if (nStart != std::string::npos)
				result.push_back(sItem.substr(nStart, nEnd - nStart + 1));
		}
		return result;
	}
}

CAPIHandler_Journal::CAPIHandler_Journal(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Journal::~CAPIHandler_Journal()
{
}

std::string CAPIHandler_Journal::getBaseURI()
{
	return "api/journal";
}

PAPIResponse CAPIHandler_Journal::handleCurrentRequest()
{
	auto pStateJournal = m_pSystemState->getStateJournalInstance();

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_JOURNAL);

	if (pStateJournal.get() != nullptr) {
		writer.addString("starttime", pStateJournal->getStartTimeAsUTC());
		writer.addInteger("lifetimeinmicroseconds", (int64_t)pStateJournal->getLifeTimeInMicroseconds());
		writer.addInteger("currenttime", (int64_t)pStateJournal->getLifeTimeInMicroseconds());
	}
	else {
		writer.addString("starttime", "");
		writer.addInteger("lifetimeinmicroseconds", 0);
		writer.addInteger("currenttime", 0);
	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}

PAPIResponse CAPIHandler_Journal::handleVariablesRequest()
{
	auto pStateJournal = m_pSystemState->getStateJournalInstance();

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_JOURNAL);

	CJSONWriterArray variablesArray(writer);

	if (pStateJournal.get() != nullptr) {
		uint32_t nVariableCount = pStateJournal->getVariableCount();
		for (uint32_t nIndex = 0; nIndex < nVariableCount; nIndex++) {
			std::string sName;
			LibMCData::eParameterDataType eDataType;
			double dUnits = 0.0;
			pStateJournal->getVariableInformation(nIndex, sName, eDataType, dUnits);

			CJSONWriterObject variableObject(writer);
			variableObject.addString("name", sName);
			variableObject.addString("type", parameterDataTypeToString(eDataType));
			variableObject.addDouble("units", dUnits);
			variablesArray.addObject(variableObject);
		}
	}

	writer.addArray("variables", variablesArray);
	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}

PAPIResponse CAPIHandler_Journal::handleEnvelopeRequest(CAPIFormFields& pFormFields)
{
	auto pStateJournal = m_pSystemState->getStateJournalInstance();

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_JOURNAL);

	uint64_t nLifeTime = 0;
	if (pStateJournal.get() != nullptr)
		nLifeTime = pStateJournal->getLifeTimeInMicroseconds();

	uint64_t nStartTimeStamp = parseUint64Parameter(pFormFields, "starttimestamp", 0);
	uint64_t nEndTimeStamp = parseUint64Parameter(pFormFields, "endtimestamp", nLifeTime);
	uint64_t nCount = parseUint64Parameter(pFormFields, "count", JOURNAL_DEFAULT_BUCKETCOUNT);

	// Guard against a degenerate range (e.g. an empty journal). Produce a valid but empty response.
	if (nEndTimeStamp <= nStartTimeStamp)
		nEndTimeStamp = nStartTimeStamp + 1;

	if (nCount < JOURNAL_MIN_BUCKETCOUNT)
		nCount = JOURNAL_MIN_BUCKETCOUNT;
	if (nCount > JOURNAL_MAX_BUCKETCOUNT)
		nCount = JOURNAL_MAX_BUCKETCOUNT;

	std::vector<std::string> variableNames;
	if (pFormFields.hasRequestParameter("variables"))
		variableNames = splitCommaSeparated(pFormFields.getRequestParameter("variables", false));
	else if (pFormFields.hasRequestParameter("variable"))
		variableNames.push_back(pFormFields.getRequestParameter("variable", false));

	writer.addInteger("starttimestamp", (int64_t)nStartTimeStamp);
	writer.addInteger("endtimestamp", (int64_t)nEndTimeStamp);

	CJSONWriterArray seriesArray(writer);

	if (pStateJournal.get() != nullptr) {
		for (auto& sVariableName : variableNames) {
			CJSONWriterObject seriesObject(writer);
			seriesObject.addString("name", sVariableName);

			std::vector<sJournalEnvelopeSample> envelope;
			bool bSuccess = true;
			try {
				pStateJournal->sampleVariableEnvelope(sVariableName, nStartTimeStamp, nEndTimeStamp, (uint32_t)nCount, envelope);
			}
			catch (std::exception&) {
				bSuccess = false;
			}

			CJSONWriterArray bucketsArray(writer);
			if (bSuccess) {
				for (auto& sample : envelope) {
					CJSONWriterObject bucketObject(writer);
					bucketObject.addInteger("t", (int64_t)sample.m_nTimeStampInMicroSeconds);
					bucketObject.addDouble("min", sample.m_dMinValue);
					bucketObject.addDouble("max", sample.m_dMaxValue);
					bucketObject.addDouble("avg", sample.m_dAverageValue);
					bucketObject.addDouble("last", sample.m_dLastValue);
					bucketsArray.addObject(bucketObject);
				}
			}
			seriesObject.addBool("available", bSuccess);
			seriesObject.addArray("buckets", bucketsArray);
			seriesArray.addObject(seriesObject);
		}
	}

	writer.addArray("series", seriesArray);
	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}

PAPIResponse CAPIHandler_Journal::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	pBodyData;
	nBodyDataSize;

	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (requestType != eAPIRequestType::rtGet)
		return nullptr;

	auto sParameterString = sURI.substr(getBaseURI().length());

	// Normalize any trailing slash
	if (!sParameterString.empty() && (sParameterString.back() == '/'))
		sParameterString = sParameterString.substr(0, sParameterString.length() - 1);

	// GET /api/journal/current
	if (sParameterString == "/current")
		return handleCurrentRequest();

	// GET /api/journal/variables
	if (sParameterString == "/variables")
		return handleVariablesRequest();

	// GET /api/journal/envelope?variables=a,b&starttimestamp=..&endtimestamp=..&count=..
	if (sParameterString == "/envelope")
		return handleEnvelopeRequest(pFormFields);

	return nullptr;
}
