/*++

Copyright (C) 2025 Autodesk Inc.

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

#include "amc_api_handler_image.hpp"
#include "amc_ui_handler.hpp"
#include "amc_resourcepackage.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"
#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>

using namespace AMC;

CAPIHandler_Image::CAPIHandler_Image(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Image::~CAPIHandler_Image()
{
}

std::string CAPIHandler_Image::getBaseURI()
{
	return "api/image";
}

void CAPIHandler_Image::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession)
{
	// Images are loaded by <img> tags which cannot send Bearer tokens,
	// so this endpoint does not require authorization.
	bNeedsToBeAuthorized = false;
	bCreateNewSession = false;
}

PAPIResponse CAPIHandler_Image::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (requestType != eAPIRequestType::rtGet)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	// Extract the UUID from the URI: "api/image/{uuid}"
	auto sParameterString = sURI.substr(getBaseURI().length());
	if (sParameterString.length() < 37 || sParameterString[0] != '/')
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sParameterUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1, 36));

	// First look in core resources for UUID
	auto pCoreResourcePackage = m_pSystemState->uiHandler()->getCoreResourcePackage();
	auto pResourceEntry = pCoreResourcePackage->findEntryByUUID(sParameterUUID, false);

	if (pResourceEntry != nullptr) {
		auto apiResponse = std::make_shared<CAPIFixedBufferResponse>(pResourceEntry->getContentType());
		pCoreResourcePackage->readEntry(pResourceEntry->getName(), apiResponse->getBuffer());
		return apiResponse;
	}

	// Then look in storage for uuid
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pStorage = pDataModel->CreateStorage();
	if (pStorage->StreamIsImage(sParameterUUID)) {

		auto pStream = pStorage->RetrieveStream(sParameterUUID);
		auto sContentType = pStream->GetMIMEType();

		auto apiResponse = std::make_shared<CAPIFixedBufferResponse>(sContentType);
		pStream->GetContent(apiResponse->getBuffer());
		return apiResponse;
	}

	// If not found, return 404
	return nullptr;
}
