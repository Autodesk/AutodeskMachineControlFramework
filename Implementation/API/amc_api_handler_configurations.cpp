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

#include "amc_api_handler_configurations.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"
#include "common_utils.hpp"

#include <vector>
#include <memory>
#include <string>

using namespace AMC;

CAPIHandler_Configurations::CAPIHandler_Configurations(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Configurations::~CAPIHandler_Configurations()
{
}

std::string CAPIHandler_Configurations::getBaseURI()
{
	return "api/configurations";
}

void CAPIHandler_Configurations::handleListConfigurationsRequest(CJSONWriter& writer, const std::string& sSchema)
{
	auto pDataModel = m_pSystemState->getDataModelInstance();

	CJSONWriterArray configArray(writer);

	if (!sSchema.empty()) {
		auto pConfigurationType = pDataModel->FindConfigurationTypeBySchema(sSchema);

		if (pConfigurationType.get() != nullptr) {
			auto pVersionIterator = pConfigurationType->ListAllConfigurationVersions();
			auto pActiveVersion = pConfigurationType->GetActiveConfigurationVersion();
			std::string sActiveUUID;
			if (pActiveVersion.get() != nullptr)
				sActiveUUID = pActiveVersion->GetVersionUUID();

			while (pVersionIterator->MoveNext()) {
				auto pVersion = pVersionIterator->GetCurrent();

				CJSONWriterObject entryObject(writer);
				entryObject.addBool("configurationactive", pVersion->GetVersionUUID() == sActiveUUID);
				entryObject.addInteger("configurationversion", pVersion->GetNumericVersion());
				entryObject.addString("username", pVersion->GetUserUUID());
				entryObject.addString("configurationuuid", pVersion->GetVersionUUID());
				entryObject.addString("configurationtimestamp", pVersion->GetTimestamp());

				configArray.addObject(entryObject);
			}
		}
	}

	writer.addArray("configurations", configArray);
}

PAPIResponse CAPIHandler_Configurations::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	if (requestType == eAPIRequestType::rtGet) {
		// Accept optional schema query parameter
		std::string sSchema = pFormFields.getRequestParameter("schema", false);

		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_STATUS);
		handleListConfigurationsRequest(writer, sSchema);
		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}

	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}
