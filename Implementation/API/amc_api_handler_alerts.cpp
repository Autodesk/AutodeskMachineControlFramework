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

#include "amc_api_handler_alerts.hpp"
#include "libmc_interfaceexception.hpp"
#include "libmcdata_dynamic.hpp"

#include <vector>
#include <memory>
#include <string>

using namespace AMC;

CAPIHandler_Alerts::CAPIHandler_Alerts(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_Alerts::~CAPIHandler_Alerts()
{
}

std::string CAPIHandler_Alerts::getBaseURI()
{
	return "api/alerts";
}

void CAPIHandler_Alerts::handleListAlertsRequest(CJSONWriter& writer)
{
	auto pDataModel = m_pSystemState->getDataModelInstance();
	auto pAlertSession = pDataModel->CreateAlertSession();

	writer.addInteger(AMC_API_KEY_ALERTS_HEADID, (int64_t) pAlertSession->GetAlertHeadID());

	auto pAlertIterator = pAlertSession->RetrieveAlerts(false);

	CJSONWriterArray alertArray(writer);

	while (pAlertIterator->MoveNext()) {
		auto pAlert = pAlertIterator->GetCurrentAlert();

		std::string sAlertCaption;
		std::string sDescriptionIdentifier = pAlert->GetDescriptionIdentifier();
		if (!sDescriptionIdentifier.empty()) {
			sAlertCaption = sDescriptionIdentifier;
		}
		else {
			sAlertCaption = pAlert->GetDescription();
		}

		CJSONWriterObject entryObject(writer);
		entryObject.addString("alertuuid", pAlert->GetUUID());
		entryObject.addString("alertidentifier", pAlert->GetIdentifier());
		entryObject.addString("alerttimestamp", pAlert->GetTimestampUTC());
		entryObject.addString("alertcaption", sAlertCaption);
		entryObject.addString("alertcontext", pAlert->GetReadableContextInformation());
		entryObject.addString("alertlevel", pAlert->GetLevelString());
		entryObject.addBool("alertactive", pAlert->IsActive());
		entryObject.addBool("alertneedsacknowledge", pAlert->GetNeedsAcknowledgement());

		alertArray.addObject(entryObject);
	}

	writer.addArray("alerts", alertArray);
}

PAPIResponse CAPIHandler_Alerts::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	if (requestType == eAPIRequestType::rtGet) {
		CJSONWriter writer;
		writeJSONHeader(writer, AMC_API_PROTOCOL_STATUS);
		handleListAlertsRequest(writer);
		return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
	}

	throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}
