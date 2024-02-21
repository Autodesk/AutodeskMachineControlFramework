/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is a stub class definition of CChannel

*/

#include "libmcdriver_tml_channel.hpp"
#include "libmcdriver_tml_axis.hpp"
#include "libmcdriver_tml_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_TML::Impl;

/*************************************************************************************************************************
 Class definition of CChannel 
**************************************************************************************************************************/

CChannel::CChannel(PTMLInstance pTMLInstance, const std::string& sChannelIdentifier, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pTMLInstance (pTMLInstance), m_sChannelIdentifier(sChannelIdentifier), m_pDriverEnvironment(pDriverEnvironment)
{
    if (pTMLInstance.get () == nullptr)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDPARAM);

    if (sChannelIdentifier.empty())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_EMPTYCHANNELIDENTIFIER);

    if (!CTMLInstance::checkIdentifierString (sChannelIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER, "invalid channel identifier: " + sChannelIdentifier);

}

CChannel::~CChannel()
{

}

std::string CChannel::GetIdentifier()
{
    return m_sChannelIdentifier;
}

IAxis* CChannel::SetupAxis(const std::string& sIdentifier, const LibMCDriver_TML_uint32 nAxisID, const LibMCDriver_TML_uint64 nConfigurationBufferSize, const LibMCDriver_TML_uint8* pConfigurationBuffer, const LibMCDriver_TML_uint32 nCountsPerMM)
{
    if (sIdentifier.empty())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_EMPTYAXISIDENTIFIER);

    if (!CTMLInstance::checkIdentifierString(sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sIdentifier);

    if ((nConfigurationBufferSize == 0) || (pConfigurationBuffer == nullptr))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISCONFIGURATIONBUFFER);

    m_pTMLInstance->setupAxis(m_sChannelIdentifier, sIdentifier, nAxisID, nConfigurationBufferSize, pConfigurationBuffer, nCountsPerMM);
    
    return new CAxis(m_pTMLInstance, m_sChannelIdentifier, sIdentifier, m_pDriverEnvironment);

}

IAxis* CChannel::FindAxis(const std::string& sIdentifier)
{
    if (sIdentifier.empty())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_EMPTYAXISIDENTIFIER);

    if (!CTMLInstance::checkIdentifierString(sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sIdentifier);

    if (!m_pTMLInstance->axisExistsInChannel (m_sChannelIdentifier, sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_AXISDOESNOTEXIST, "axis does not exist: " + sIdentifier);

    return new CAxis(m_pTMLInstance, m_sChannelIdentifier, sIdentifier, m_pDriverEnvironment);
}


bool CChannel::AxisExists(const std::string& sIdentifier)
{
    if (sIdentifier.empty())
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_EMPTYAXISIDENTIFIER);

    if (!CTMLInstance::checkIdentifierString(sIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER, "invalid axis identifier: " + sIdentifier);

    return m_pTMLInstance->axisExistsInChannel(m_sChannelIdentifier, sIdentifier);
}


void CChannel::Close()
{
    m_pTMLInstance->closeChannel(m_sChannelIdentifier);
}

