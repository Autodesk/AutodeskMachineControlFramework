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


Abstract: This is a stub class definition of CEulerConnection

*/

#include "libmcdriver_euler_eulerconnection.hpp"
#include "libmcdriver_euler_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_Euler::Impl;

/*************************************************************************************************************************
 Class definition of CEulerConnection 
**************************************************************************************************************************/

CEulerConnection::CEulerConnection(PEulerConnectionInstance pConnectionInstance)
	: m_pConnectionInstance (pConnectionInstance)
{
	if (m_pConnectionInstance.get() == nullptr)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDPARAM);

}

CEulerConnection::~CEulerConnection()
{
	m_pConnectionInstance = nullptr;
}


void CEulerConnection::Disconnect()
{
	m_pConnectionInstance->closeConnection();
}

std::string CEulerConnection::GetIdentifier()
{
	return m_pConnectionInstance->getIdentifier();
}

std::string CEulerConnection::GetApplicationName()
{
	return m_pConnectionInstance->getApplicationName();
}

std::string CEulerConnection::GetApplicationVersion()
{
	return m_pConnectionInstance->getApplicationVersion();
}

std::string CEulerConnection::GetBaseURL()
{
	return m_pConnectionInstance->getBaseURL();
}

std::string CEulerConnection::GetAPIKey()
{
	return m_pConnectionInstance->getAPIKey();
}

std::string CEulerConnection::GetDeviceID()
{
	return m_pConnectionInstance->getDeviceID();
}

void CEulerConnection::StartHeartBeat()
{
	m_pConnectionInstance->startHeartBeat();
}

void CEulerConnection::StopHeartBeat()
{
	m_pConnectionInstance->stopHeartBeat();
}

std::string CEulerConnection::CreateBuild(const std::string & sJobName, const LibMCDriver_Euler_uint32 nLayerCount)
{
	return m_pConnectionInstance->createBuild (sJobName, nLayerCount);
}

void CEulerConnection::UploadImage(const std::string& sBuildJobID, const LibMCDriver_Euler_uint32 nLayerIndex, const LibMCDriver_Euler::eEulerImageType eImageType, LibMCEnv::PImageData pImage)
{
	m_pConnectionInstance->uploadImage(sBuildJobID, nLayerIndex, eImageType, pImage);
}

void CEulerConnection::SetJobStatus(const std::string& sBuildJobID, const LibMCDriver_Euler::eEulerJobStatus eJobStatus)
{
	m_pConnectionInstance->setJobStatus(sBuildJobID, eJobStatus);
}


