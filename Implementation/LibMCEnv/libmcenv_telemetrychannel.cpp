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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CTelemetryChannel

*/

#include "libmcenv_telemetrychannel.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_telemetrymarkerscope.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CTelemetryChannel 
**************************************************************************************************************************/

CTelemetryChannel::CTelemetryChannel(AMC::PTelemetryHandler pTelemetryHandler, const std::string & sParentName, const std::string & sIdentifier)
	: m_pTelemetryHandler(pTelemetryHandler), m_sParentName(sParentName), m_sIdentifier(sIdentifier)    
{
    if (pTelemetryHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CTelemetryChannel::~CTelemetryChannel()
{

}


std::string CTelemetryChannel::GetParent()
{
    return m_sParentName;
}

std::string CTelemetryChannel::GetIdentifier()
{
    return m_sIdentifier;
}

std::string CTelemetryChannel::GetGlobalIdentifier()
{
	return m_sParentName + "." + m_sIdentifier;
}

ITelemetryMarkerScope * CTelemetryChannel::StartMarkerScope(const LibMCEnv_uint64 nUserContextData)
{
	auto pChannel = m_pTelemetryHandler->findChannelByIdentifier (GetGlobalIdentifier(), true);
    auto scope = pChannel->startIntervalScope(nUserContextData);

	return new CTelemetryMarkerScope(std::move(scope), m_sParentName, m_sIdentifier);
}

LibMCEnv_uint64 CTelemetryChannel::CreateInstantMarker(const LibMCEnv_uint64 nUserContextData)
{
    auto pChannel = m_pTelemetryHandler->findChannelByIdentifier(GetGlobalIdentifier(), true);
    return pChannel->createInstantMarker(nUserContextData);

}

LibMCData::eTelemetryChannelType CTelemetryChannel::mapEnvChannelTypeToDataChannelType(LibMCEnv::eTelemetryChannelType envChannelType)
{
    switch (envChannelType) {
        case LibMCEnv::eTelemetryChannelType::CustomMarker:
			return LibMCData::eTelemetryChannelType::CustomMarker;
		case LibMCEnv::eTelemetryChannelType::RemoteQuery:
			return LibMCData::eTelemetryChannelType::RemoteQuery;
		case LibMCEnv::eTelemetryChannelType::StateExecution:
			return LibMCData::eTelemetryChannelType::StateExecution;
		case LibMCEnv::eTelemetryChannelType::StateRepeatDelay:
			return LibMCData::eTelemetryChannelType::StateRepeatDelay;
		case LibMCEnv::eTelemetryChannelType::SignalQueue:
			return LibMCData::eTelemetryChannelType::SignalQueue;
		case LibMCEnv::eTelemetryChannelType::SignalProcessing:
			return LibMCData::eTelemetryChannelType::SignalProcessing;
		case LibMCEnv::eTelemetryChannelType::SignalAcknowledgement:
            return LibMCData::eTelemetryChannelType::SignalAcknowledgement;
        default:
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "invalid telemetry channel type");
    }

}

LibMCEnv::eTelemetryChannelType CTelemetryChannel::mapDataChannelTypeToEnvChannelType(LibMCData::eTelemetryChannelType dataChannelType)
{
	switch (dataChannelType) {
	case LibMCData::eTelemetryChannelType::CustomMarker:
		return LibMCEnv::eTelemetryChannelType::CustomMarker;
	case LibMCData::eTelemetryChannelType::RemoteQuery:
		return LibMCEnv::eTelemetryChannelType::RemoteQuery;
	case LibMCData::eTelemetryChannelType::StateExecution:
		return LibMCEnv::eTelemetryChannelType::StateExecution;
	case LibMCData::eTelemetryChannelType::StateRepeatDelay:
		return LibMCEnv::eTelemetryChannelType::StateRepeatDelay;
	case LibMCData::eTelemetryChannelType::SignalQueue:
		return LibMCEnv::eTelemetryChannelType::SignalQueue;
	case LibMCData::eTelemetryChannelType::SignalProcessing:
		return LibMCEnv::eTelemetryChannelType::SignalProcessing;
	case LibMCData::eTelemetryChannelType::SignalAcknowledgement:
		return LibMCEnv::eTelemetryChannelType::SignalAcknowledgement;
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "invalid telemetry channel type");
	}

}



std::string CTelemetryChannel::mapEnvChannelTypeToTypeString(LibMCEnv::eTelemetryChannelType envChannelType)
{
	return AMC::CTelemetryChannel::mapDataChannelTypeToTypeString (mapEnvChannelTypeToDataChannelType (envChannelType));
}

static LibMCEnv::eTelemetryChannelType mapChannelTypeStringToEnvChannelType(const std::string& sTypeString)
{
	return CTelemetryChannel::mapDataChannelTypeToEnvChannelType (AMC::CTelemetryChannel::mapChannelTypeStringToDataChannelType(sTypeString));
}
