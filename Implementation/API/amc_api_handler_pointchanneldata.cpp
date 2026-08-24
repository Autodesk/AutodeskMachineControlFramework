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

#include "amc_api_handler_pointchanneldata.hpp"

#include "amc_scatterplot.hpp"
#include "amc_toolpathhandler.hpp"

#include "libmc_interfaceexception.hpp"
#include "common_utils.hpp"

using namespace AMC;

CAPIHandler_PointChannelData::CAPIHandler_PointChannelData(PSystemState pSystemState)
	: CAPIHandler(pSystemState->getClientHash()), m_pSystemState(pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}

CAPIHandler_PointChannelData::~CAPIHandler_PointChannelData()
{
}

std::string CAPIHandler_PointChannelData::getBaseURI()
{
	return "api/pointchanneldata";
}

PAPIResponse CAPIHandler_PointChannelData::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields& pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{
	pFormFields;
	pBodyData;
	nBodyDataSize;

	if (pAuth.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	if (requestType != eAPIRequestType::rtGet)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	// Extract parameters from URI: "api/pointchanneldata/{uuid}/{channelname}"
	auto sParameterString = sURI.substr(getBaseURI().length());
	if ((sParameterString.length() < 39) || (sParameterString[0] != '/'))
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sScatterplotUUID = AMCCommon::CUtils::normalizeUUIDString(sParameterString.substr(1, 36));
	if (sParameterString[37] != '/')
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	std::string sChannelName = sParameterString.substr(38);
	if (sChannelName.empty())
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	auto pToolpathHandler = m_pSystemState->getToolpathHandlerInstance();
	auto pScatterplot = pToolpathHandler->restoreScatterplot(sScatterplotUUID, false);
	if (pScatterplot.get() == nullptr)
		return nullptr;

	CJSONWriter writer;
	writeJSONHeader(writer, AMC_API_PROTOCOL_STATUS);

	auto& channelEntries = pScatterplot->getChannelEntries();
	auto channelIt = channelEntries.find(sChannelName);
	if (channelIt != channelEntries.end()) {
		auto& columnEntries = channelIt->second;
		for (const auto& pair : columnEntries) {
			auto sColumnName = pair.first;
			auto& vecData = pair.second;

			CJSONWriterArray dataArray(writer);
			for (auto value : vecData)
				dataArray.addDouble("", value);

			writer.addArray(sColumnName, dataArray);
		}
	}

	return std::make_shared<CAPIStringResponse>(AMC_API_HTTP_SUCCESS, AMC_API_CONTENTTYPE, writer.saveToString());
}

