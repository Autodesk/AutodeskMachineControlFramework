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
	: CAPIHandler(pSystemState->getClientHash ()), m_pSystemState(pSystemState)
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

APIHandler_BuildType CAPIHandler_Build::parseRequest(const std::string& sURI, const eAPIRequestType requestType, std::string& paramUUID)
{
	// Leave away base URI
 	auto sParameterString = AMCCommon::CUtils::toLowerString (sURI.substr(9));
	paramUUID = "";

	if (requestType == eAPIRequestType::rtPost) {

		if ((sParameterString == "/prepare") || (sParameterString == "/prepare/")) {
			return btStartPrepareJob;
		}

		if ((sParameterString == "/toolpath") || (sParameterString == "/toolpath/")) {
			return btToolpath;
		}

	}

	if (requestType == eAPIRequestType::rtGet) {

		if ((sParameterString == "/") || (sParameterString == "")) {
			return btListJobs;
		}

		if (sParameterString.substr(0, 15) == "/listbuilddata/") {
			paramUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(15));
			return btListBuildData;
		}

		if (sParameterString.substr (0, 6) == "/data/") {
			paramUUID = AMCCommon::CUtils::normalizeUUIDString (sParameterString.substr(6));
			return btGetBuildData;
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
		case btToolpath:
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

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);

	pBuildJob->StartValidating();

	auto pServiceHandler = m_pSystemState->serviceHandler();
	auto pLib3MFWrapper = m_pSystemState->toolpathHandler()->getLib3MFWrapper();

	pServiceHandler->addServiceToQueue (std::make_shared <CService_BuildFileParsing> (pServiceHandler, pDataModel, pBuildJob->GetUUID(), pLib3MFWrapper, pAuth->getUserUUID(), m_pSystemState->getGlobalChronoInstance ()));

	writer.addString(AMC_API_KEY_UPLOAD_BUILDJOBNAME, pBuildJob->GetName());

}


void CAPIHandler_Build::handleToolpathRequest(CJSONWriter& writer, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pBodyData == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	CAPIJSONRequest jsonRequest(pBodyData, nBodyDataSize);
	auto sBuildUUID = jsonRequest.getUUID(AMC_API_KEY_BUILDUUID, LIBMC_ERROR_INVALIDBUILDUUID);

	CJSONWriterArray segmentArray(writer);

	if (sBuildUUID != AMCCommon::CUtils::createEmptyUUID()) {


		auto pDataModel = m_pSystemState->getDataModelInstance();
		auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
		auto pBuildJob = pBuildJobHandler->RetrieveJob(sBuildUUID);
		auto sStreamUUID = pBuildJob->GetStorageStreamUUID();


		auto pToolpathHandler = m_pSystemState->toolpathHandler();

		auto pToolpath = pToolpathHandler->findToolpathEntity(sStreamUUID, false);
		if (pToolpath == nullptr) {
			pToolpath = pToolpathHandler->loadToolpathEntity(sStreamUUID);
		}

		auto nLayerCount = pToolpath->getLayerCount();

		auto nLayerIndex = jsonRequest.getUint64(AMC_API_KEY_LAYERINDEX, 0, nLayerCount, LIBMC_ERROR_INVALIDLAYERINDEX);

		if (nLayerIndex < nLayerCount) {

			auto pLayerData = pToolpath->readLayer((uint32_t)nLayerIndex);
			auto dUnits = pLayerData->getUnits();

			auto nSegmentCount = pLayerData->getSegmentCount();
			for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {
				auto segmentType = pLayerData->getSegmentType(nSegmentIndex);
				auto nPointCount = pLayerData->getSegmentPointCount(nSegmentIndex);

				CJSONWriterObject segmentObject(writer);

				if (nPointCount > 0) {

					std::vector<LibMCEnv::sPosition2D> Points;
					Points.resize(nPointCount);
					pLayerData->storePointsToBufferInUnits(nSegmentIndex, Points.data());

					CJSONWriterArray pointArray(writer);

					switch (segmentType) {
					case LibMCEnv::eToolpathSegmentType::Hatch:
						segmentObject.addString(AMC_API_KEY_TYPE, AMC_API_KEY_HATCH);
						break;
					case LibMCEnv::eToolpathSegmentType::Loop:
						segmentObject.addString(AMC_API_KEY_TYPE, AMC_API_KEY_LOOP);
						break;
					case LibMCEnv::eToolpathSegmentType::Polyline:
						segmentObject.addString(AMC_API_KEY_TYPE, AMC_API_KEY_POLYLINE);
						break;
					}

					for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {

						auto pPoint = &Points[nPointIndex];

						CJSONWriterObject pointObject(writer);
						pointObject.addDouble(AMC_API_KEY_X, pPoint->m_Coordinates[0] * dUnits);
						pointObject.addDouble(AMC_API_KEY_Y, pPoint->m_Coordinates[1] * dUnits);

						pointArray.addObject(pointObject);

					}

					segmentObject.addArray(AMC_API_KEY_POINTS, pointArray);


				}

				segmentArray.addObject(segmentObject);

			}
		}
	}


	writer.addArray(AMC_API_KEY_SEGMENTS, segmentArray);
}


void CAPIHandler_Build::handleListJobsRequest(CJSONWriter& writer, PAPIAuth pAuth)
{	
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);


	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildJobIterator = pBuildJobHandler->ListJobsByStatus(LibMCData::eBuildJobStatus::Validated);

	CJSONWriterArray jobJSONArray(writer);

	while (pBuildJobIterator->MoveNext()) {
		auto pBuildJob = pBuildJobIterator->GetCurrentJob();

		CJSONWriterObject jobJSON(writer);
		jobJSON.addString(AMC_API_KEY_UPLOAD_BUILDJOBUUID, pBuildJob->GetUUID());
		jobJSON.addString(AMC_API_KEY_UPLOAD_BUILDJOBSTORAGESTREAM, pBuildJob->GetStorageStreamUUID());
		jobJSON.addString(AMC_API_KEY_UPLOAD_BUILDJOBNAME, pBuildJob->GetName ());
		jobJSON.addInteger(AMC_API_KEY_UPLOAD_BUILDJOBLAYERCOUNT, pBuildJob->GetLayerCount());

		jobJSONArray.addObject(jobJSON);
	}

	writer.addArray(AMC_API_KEY_UPLOAD_BUILDJOBARRAY, jobJSONArray);
}

void CAPIHandler_Build::handleListBuildDataRequest(CJSONWriter& writer, PAPIAuth pAuth, std::string& buildUUID)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(buildUUID);

	CJSONWriterArray dataJSONArray(writer);

	auto pJobDataIterator = pBuildJob->ListJobData();
	while (pJobDataIterator->MoveNext()) {
		auto pJobData = pJobDataIterator->GetCurrentJobData();
		auto sUUID = pJobData->GetDataUUID();
		auto sDataType = pJobData->GetDataTypeAsString();
		auto sMimeType = pJobData->GetMIMEType();
		auto sName = pJobData->GetName();
		auto sTimeStamp = pJobData->GetTimeStamp();
		auto nSize = pJobData->GetStorageStreamSize();
		auto sSHA2 = pJobData->GetStorageStreamSHA2();

		CJSONWriterObject dataObject (writer);
		dataObject.addString(AMC_API_KEY_UPLOAD_UUID, sUUID);
		dataObject.addString(AMC_API_KEY_UPLOAD_DATATYPE, sDataType);
		dataObject.addString(AMC_API_KEY_UPLOAD_MIMETYPE, sMimeType);
		dataObject.addString(AMC_API_KEY_UPLOAD_NAME, sName);
		dataObject.addString(AMC_API_KEY_UPLOAD_TIMESTAMP, sTimeStamp);
		dataObject.addInteger(AMC_API_KEY_UPLOAD_SIZE, nSize);
		dataObject.addString(AMC_API_KEY_UPLOAD_SHA256, sSHA2);

		dataJSONArray.addObject(dataObject);
	}

	writer.addArray(AMC_API_KEY_UPLOAD_BUILDDATAARRAY, dataJSONArray);

}


PAPIResponse CAPIHandler_Build::handleGetBuildDataRequest(PAPIAuth pAuth, std::string& buildDataUUID)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pBuildJobHandler = pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->FindJobOfData(buildDataUUID);
	auto pBuildJobData = pBuildJob->RetrieveJobData(buildDataUUID);

	auto pStorageStream = pBuildJobData->GetStorageStream();

	auto pResponse = std::make_shared<CAPIFixedBufferResponse>(pBuildJobData->GetMIMEType());
	pStorageStream->GetContent (pResponse->getBuffer());

	return pResponse;
	

}


PAPIResponse CAPIHandler_Build::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	std::string paramUUID;

	auto buildType = parseRequest(sURI, requestType, paramUUID);

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_BUILD);

	switch (buildType) {
	case btStartPrepareJob:
		handlePrepareJobRequest(writer, pBodyData, nBodyDataSize, pAuth);
		break;
	case btListJobs:
		handleListJobsRequest(writer, pAuth);
		break;
	case btToolpath:
		handleToolpathRequest(writer, pBodyData, nBodyDataSize, pAuth);
		break;

	case btListBuildData:
		handleListBuildDataRequest(writer, pAuth, paramUUID);
		break;

	case btGetBuildData:
		return handleGetBuildDataRequest(pAuth, paramUUID);
		break;

	default:
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());

	return nullptr;
}



		

