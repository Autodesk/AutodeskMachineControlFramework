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

#include "amc_api_handler_build.hpp"
#include "amc_api_jsonrequest.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include "amc_service_buildfileparsing.hpp"
#include "amc_toolpathhandler.hpp"

#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>


using namespace AMC;

CAPIHandler_Build::CAPIHandler_Build(PSystemState pSystemState)
	: m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CAPIHandler_Build::~CAPIHandler_Build()
{
	
}
				
std::string CAPIHandler_Build::getBaseURI () 
{
	return "api/build";
}

APIHandler_BuildType CAPIHandler_Build::parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string& jobUUID)
{
	// Leave away base URI
 	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr(9));
	jobUUID = "";

	if (requestType == eAPIRequestType::rtPost) {

		if ((sParameterString == "/prepare") || (sParameterString == "/prepare/")) {
			return btStartPrepareJob;
		}
	
	}

	if (requestType == eAPIRequestType::rtGet) {

		if ((sParameterString == "/") || (sParameterString == "")) {
			return btListJobs;
		}

	}


	return btUnknown;
}


bool CAPIHandler_Build::expectsRawBody(const std::string& sURI, const eAPIRequestType requestType)
{
	std::string jobUUID;

	switch (parseRequest(sURI, requestType, jobUUID)) {
		case btStartPrepareJob:
			return true;

		default:
			return false;
	}
}


uint32_t CAPIHandler_Build::getFormDataFieldCount(const std::string& sURI, const eAPIRequestType requestType)
{
	return 0;	
}

CAPIFieldDetails CAPIHandler_Build::getFormDataFieldDetails(const std::string& sURI, const eAPIRequestType requestType, const uint32_t nFieldIndex)
{
	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);
}

void CAPIHandler_Build::handlePrepareJobRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);

	auto sBuildUUID = jsonRequest.getUUID(AMC_API_KEY_BUILDUUID, LIBMC_ERROR_INVALIDBUILDUUID);

	auto pBuildJobHandler = m_pSystemState->buildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);

	pBuildJob->StartValidating();

	auto pServiceHandler = m_pSystemState->serviceHandler();
	auto pLib3MFWrapper = m_pSystemState->toolpathHandler()->getLib3MFWrapper();

	pServiceHandler->addServiceToQueue (std::make_shared <CService_BuildFileParsing> (pServiceHandler, pBuildJob, pLib3MFWrapper, pAuth->getUserName()));

	writer.addString(AMC_API_KEY_UPLOAD_BUILDJOBNAME, pBuildJob->GetName());

}


void CAPIHandler_Build::handleListJobsRequest(CJSONWriter& writer, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);


	auto pBuildJobHandler = m_pSystemState->buildJobHandler();
	auto pBuildJobIterator = pBuildJobHandler->ListJobsByStatus(LibMCData::eBuildJobStatus::Validated);

	CJSONWriterArray jobJSONArray(writer);

	while (pBuildJobIterator->MoveNext()) {
		auto pBuildJob = pBuildJobIterator->GetCurrentJob();

		CJSONWriterObject jobJSON(writer);
		jobJSON.addString(AMC_API_KEY_UPLOAD_BUILDJOBNAME, pBuildJob->GetName ());
		jobJSON.addInteger(AMC_API_KEY_UPLOAD_BUILDJOBLAYERCOUNT, pBuildJob->GetLayerCount());

		jobJSONArray.addObject(jobJSON);
	}

	writer.addArray(AMC_API_KEY_UPLOAD_BUILDJOBARRAY, jobJSONArray);
}



PAPIResponse CAPIHandler_Build::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	std::string jobUUID;

	auto buildType = parseRequest(sURI, requestType, jobUUID);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_BUILD);

	switch (buildType) {
	case btStartPrepareJob:
		handlePrepareJobRequest(writer, pBodyData, nBodyDataSize, pAuth);
		break;
	case btListJobs:
		handleListJobsRequest(writer, pAuth);
		break;

	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());

	return nullptr;
}



		

