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

#define __AMCIMPL_API_CONSTANTS

#include "amc_api_handler_executions.hpp"
#include "amc_api_constants.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"
#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>

using namespace AMC;

CAPIHandler_Executions::CAPIHandler_Executions(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Executions::~CAPIHandler_Executions()
{
}

std::string CAPIHandler_Executions::getBaseURI()
{
	return "api/executions";
}

void CAPIHandler_Executions::handleListExecutionsRequest(CJSONWriter& writer)
{
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pGlobalChrono = m_pSystemState->globalChrono();

	writer.addInteger(AMC_API_KEY_EXECUTIONS_HEADID, (int64_t) pBuildJobHandler->GetExecutionListHeadID());

	auto pExecutionIterator = pBuildJobHandler->ListJobExecutions("", "", "");

	CJSONWriterArray executionArray(writer);

	while (pExecutionIterator->MoveNext()) {
		auto pExecution = pExecutionIterator->GetCurrentJobExecution();

		LibMCData::eBuildJobExecutionStatus status = pExecution->GetStatus();
		std::string sStatusString = pExecution->GetStatusString();

		uint64_t nStartTimeStamp = pExecution->GetStartTimeStampInMicroseconds();
		uint64_t nEndTimeStamp = 0;
		if (status == LibMCData::eBuildJobExecutionStatus::Finished) {
			nEndTimeStamp = pExecution->GetEndTimeStampInMicroseconds();
		}

		int64_t nDurationInSeconds = pExecution->ComputeElapsedTimeInMicroseconds(pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970(), false) / 1000000LL;

		CJSONWriterObject entryObject(writer);
		entryObject.addString("executionuuid", pExecution->GetExecutionUUID());
		entryObject.addString("executionname", pExecution->GetJobName());
		entryObject.addString("executiondescription", pExecution->GetDescription());
		entryObject.addString("executionstarttimestamp", AMCCommon::CChrono::convertToISO8601TimeUTC(nStartTimeStamp));
		entryObject.addString("executionendtimestamp", AMCCommon::CChrono::convertToISO8601TimeUTC(nEndTimeStamp));
		entryObject.addInteger("executionduration", nDurationInSeconds);
		entryObject.addString("executionstatus", sStatusString);
		entryObject.addString("executionbuildstatus", pExecution->GetJobStatusString());
		entryObject.addInteger("executionlayercount", pExecution->GetJobLayerCount());

		std::string sJobUUID = pExecution->GetJobUUID();
		entryObject.addString("jobuuid", sJobUUID);

		try {
			auto pBuildJob = pBuildJobHandler->RetrieveJob(sJobUUID);
			if (pBuildJob->HasThumbnailStream())
				entryObject.addString("executionthumbnail", pBuildJob->GetThumbnailStreamUUID());
		}
		catch (...) {
			// Job may have been deleted; skip thumbnail
		}

		executionArray.addObject(entryObject);
	}

	writer.addArray("executions", executionArray);
}

PAPIResponse CAPIHandler_Executions::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	if (requestType == eAPIRequestType::rtGet) {
		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_BUILD);
		handleListExecutionsRequest(writer);
		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}

	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}
