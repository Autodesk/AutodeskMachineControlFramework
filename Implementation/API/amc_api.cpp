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

#include "amc_api.hpp"
#include "amc_api_handler.hpp"
#include "amc_api_response.hpp"
#include "libmc_interfaceexception.hpp"

#include <vector>
#include <memory>
#include <string>


using namespace AMC;

CAPI::CAPI()
{

}

CAPI::~CAPI()
{
}

PAPIResponse CAPI::handleGetRequest(const std::string& sURI, uint32_t& nHTTPCode)
{

	try {

		for (auto pHandler : m_ApiHandlers) {
			std::string sBaseURI = pHandler->getBaseURI();
			bool bIsMatch;

			if (sBaseURI.length() > 0) {
				if (sBaseURI.length() < sURI.length()) {
					std::string sSlashURI = sBaseURI + "/";

					bIsMatch = (sSlashURI == sURI.substr(0, sSlashURI.length()));
				}
				else {
					bIsMatch = (sBaseURI == sURI);
				}
			}
			else {
				bIsMatch = true;
			}

			if (bIsMatch) {
				auto pResponse = pHandler->handleGetRequest(sURI);
				if (pResponse.get() != nullptr) {
					nHTTPCode = AMC_API_HTTP_SUCCESS;
					return pResponse;
				}

				break;
			}
		}

		nHTTPCode = AMC_API_HTTP_NOTFOUND;
		return makeError (LIBMC_ERROR_URLNOTFOUND, "url not found: " + sURI);
	}
	catch (ELibMCInterfaceException & IntfException) {
		nHTTPCode = AMC_API_HTTP_BADREQUEST;
		return makeError(IntfException.getErrorCode(), IntfException.what ());
	}
	catch (std::exception& StdException) {
		nHTTPCode = AMC_API_HTTP_BADREQUEST;
		return makeError(LIBMC_ERROR_GENERICBADREQUEST, StdException.what());
	}
}

PAPIResponse CAPI::handlePostRequest(const std::string& sURI, const uint8_t* pBody, const size_t nStreamSize, uint32_t& nHTTPCode)
{

	try {

		for (auto pHandler : m_ApiHandlers) {
			std::string sBaseURI = pHandler->getBaseURI();
			bool bIsMatch;

			if (sBaseURI.length() < sURI.length()) {
				std::string sSlashURI = sBaseURI + "/";

				bIsMatch = (sSlashURI == sURI.substr(0, sSlashURI.length()));
			}
			else {
				bIsMatch = (sBaseURI == sURI);
			}

			if (bIsMatch) {
				PAPIResponse pResponse = pHandler->handlePostRequest(sURI, pBody, nStreamSize);
				if (pResponse.get() != nullptr) {
					nHTTPCode = AMC_API_HTTP_SUCCESS;
					return pResponse;
				}

				break;
			}
		}

		nHTTPCode = AMC_API_HTTP_NOTFOUND;
		return makeError(LIBMC_ERROR_URLNOTFOUND, "url not found" + sURI);
	}
	catch (ELibMCInterfaceException& IntfException) {
		nHTTPCode = AMC_API_HTTP_BADREQUEST;
		return makeError(IntfException.getErrorCode(), IntfException.what());
	}
	catch (std::exception& StdException) {
		nHTTPCode = AMC_API_HTTP_BADREQUEST;
		return makeError(LIBMC_ERROR_GENERICBADREQUEST, StdException.what());
	}
}

void CAPI::registerHandler(PAPIHandler pAPIHandler)
{
	if (pAPIHandler.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	m_ApiHandlers.push_back(pAPIHandler);
}


PAPIResponse CAPI::makeError(LibMCResult errorCode, const std::string& sErrorString)
{
	CJSONWriter writer;
	writer.addString(AMC_API_KEY_PROTOCOL, AMC_API_PROTOCOL_ERROR);
	writer.addString(AMC_API_KEY_VERSION, AMC_API_PROTOCOL_VERSION);
	writer.addInteger(AMC_API_KEY_ERRORCODE, errorCode);
	writer.addString(AMC_API_KEY_MESSAGE, sErrorString);
	return std::make_shared<CAPIStringResponse> (AMC_API_CONTENTTYPE, writer.saveToString () );
}
