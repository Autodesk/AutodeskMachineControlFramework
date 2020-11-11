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


Abstract: This is a stub class definition of CBuild

*/

#include "libmcenv_build.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_toolpathaccessor.hpp"

// Include custom headers here.
#include "amc_systemstate.hpp"
#include "amc_toolpathhandler.hpp"

#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CBuild 
**************************************************************************************************************************/

CBuild::CBuild(LibMCData::PBuildJob pBuildJob, AMC::PSystemState pSystemState)
	: m_pBuildJob(pBuildJob), m_pSystemState (pSystemState)
{
	if (pSystemState.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pBuildJob.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}


std::string CBuild::GetName()
{
	return m_pBuildJob->GetName();
}

std::string CBuild::GetBuildUUID()
{
	return m_pBuildJob->GetUUID();
}

std::string CBuild::GetStorageUUID()
{
	return m_pBuildJob->GetStorageStreamUUID();
}

std::string CBuild::GetStorageSHA256()
{
	return m_pBuildJob->GetStorageStream()->GetSHA2 ();

}

LibMCEnv_uint32 CBuild::GetLayerCount()
{
	return m_pBuildJob->GetLayerCount();
}

IToolpathAccessor * CBuild::CreateToolpathAccessor()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	return new CToolpathAccessor(sStreamUUID, m_pSystemState->getToolpathHandlerInstance());
}


void CBuild::LoadToolpath()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	m_pSystemState->toolpathHandler()->loadToolpathEntity(sStreamUUID);
}


void CBuild::UnloadToolpath()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	m_pSystemState->toolpathHandler()->unloadToolpathEntity(sStreamUUID);
}

bool CBuild::ToolpathIsLoaded()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	return (m_pSystemState->toolpathHandler()->findToolpathEntity(sStreamUUID, false) != nullptr);
}

std::string CBuild::AddBinaryData(const std::string & sName, const std::string & sMIMEType, const LibMCEnv_uint64 nContentBufferSize, const LibMCEnv_uint8 * pContentBuffer)
{
	auto sDataUUID = AMCCommon::CUtils::createUUID();
	auto sSystemUserID = m_pSystemState->getSystemUserID ();

	auto pStorage = m_pSystemState->storage();
	pStorage->StoreNewStream(sDataUUID, m_pBuildJob->GetUUID(), sName, sMIMEType, LibMCData::CInputVector<uint8_t>(pContentBuffer, nContentBufferSize), sSystemUserID);

	auto pStorageStream = pStorage->RetrieveStream(sDataUUID);
	m_pBuildJob->AddJobData(sName, pStorageStream, LibMCData::eBuildJobDataType::CustomBinaryData, sSystemUserID);

	return sDataUUID;


}

