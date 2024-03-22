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


Abstract: This is a stub class definition of CBuildExecution

*/

#include "libmcenv_buildexecution.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmcenv_build.hpp"
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CBuildExecution 
**************************************************************************************************************************/
CBuildExecution::CBuildExecution(LibMCData::PBuildJobExecution pExecution, LibMCData::PDataModel pDataModel, AMC::PToolpathHandler pToolpathHandler, const std::string& sSystemUserID)
	: m_pExecution (pExecution), 
	m_pDataModel (pDataModel), 
	m_pToolpathHandler (pToolpathHandler), 
	m_sSystemUserID (sSystemUserID)
{
	if (pExecution.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pDataModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pToolpathHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	// Cache Execution UUID
	m_sExecutionUUID = m_pExecution->GetExecutionUUID();
}

CBuildExecution::~CBuildExecution()
{
	m_pExecution = nullptr;
	m_pDataModel = nullptr;
	m_pToolpathHandler = nullptr;
}

std::string CBuildExecution::GetUUID()
{
	return m_sExecutionUUID;
}

std::string CBuildExecution::GetBuildUUID()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);

	return m_pExecution->GetJobUUID();
}

IBuild * CBuildExecution::GetBuild()
{
	std::string sBuildUUID = GetBuildUUID();
	return new CBuild(m_pDataModel, sBuildUUID, m_pToolpathHandler, m_sSystemUserID);
}

LibMCEnv::eBuildExecutionStatus CBuildExecution::GetExecutionStatus()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	LibMCData::eBuildJobExecutionStatus eStatus = m_pExecution->GetStatus();

	switch (eStatus) {
	case LibMCData::eBuildJobExecutionStatus::InProcess: return LibMCEnv::eBuildExecutionStatus::InProcess;
	case LibMCData::eBuildJobExecutionStatus::Finished: return LibMCEnv::eBuildExecutionStatus::Finished;
	case LibMCData::eBuildJobExecutionStatus::Failed: return LibMCEnv::eBuildExecutionStatus::Failed;
	default:
		return LibMCEnv::eBuildExecutionStatus::Unknown;
	}
}

bool CBuildExecution::IsInProcess()
{
	return (GetExecutionStatus() == LibMCEnv::eBuildExecutionStatus::InProcess);
}

bool CBuildExecution::IsFinished()
{
	return (GetExecutionStatus() == LibMCEnv::eBuildExecutionStatus::Finished);
}

bool CBuildExecution::IsFailed()
{
	return (GetExecutionStatus() == LibMCEnv::eBuildExecutionStatus::Failed);
}

void CBuildExecution::SetStatusToFinished()
{
	if (GetExecutionStatus() != LibMCEnv::eBuildExecutionStatus::InProcess)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_CANNOTCHANGESTATUSOFBUILDEXECUTION, "can not change status of build execution" + m_sExecutionUUID);

	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->ChangeStatus (LibMCData::eBuildJobExecutionStatus::Finished);
}

void CBuildExecution::SetStatusToFailed()
{
	if (GetExecutionStatus() != LibMCEnv::eBuildExecutionStatus::InProcess)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_CANNOTCHANGESTATUSOFBUILDEXECUTION, "can not change status of build execution" + m_sExecutionUUID);

	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->ChangeStatus(LibMCData::eBuildJobExecutionStatus::Failed);
}

std::string CBuildExecution::GetDescription()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetDescription();
}

void CBuildExecution::SetDescription(const std::string & sDescription)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->SetDescription(sDescription);
}

std::string CBuildExecution::GetJournalUUID()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetJournalUUID();
}

bool CBuildExecution::HasAttachedUser()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	std::string sUserUUID = m_pExecution->GetUserUUID();

	if (sUserUUID.empty())
		return false;

	std::string sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);

	return (sNormalizedUserUUID != AMCCommon::CUtils::createEmptyUUID());
}

std::string CBuildExecution::GetUserUUID()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	std::string sUserUUID = m_pExecution->GetUserUUID();

	if (sUserUUID.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUILDEXECUTIONHASNOATTACHEDUSER, "build execution has no attached user: " + m_sExecutionUUID);

	std::string sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	if (sNormalizedUserUUID == AMCCommon::CUtils::createEmptyUUID())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUILDEXECUTIONHASNOATTACHEDUSER, "build execution has no attached user: " + m_sExecutionUUID);

	return sNormalizedUserUUID;
}

std::string CBuildExecution::GetStartTimeInUTC()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetStartTimeInUTC();
}

LibMCEnv_uint64 CBuildExecution::GetStartTimeStampInMilliseconds()
{
	return GetStartTimeStampInMicroseconds() / 1000;
}

LibMCEnv_uint64 CBuildExecution::GetStartTimeStampInMicroseconds()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetStartTimeStampInMicroseconds();
}

LibMCEnv_uint64 CBuildExecution::GetEndTimeStampInMilliseconds()
{
	return GetEndTimeStampInMicroseconds() / 1000;
}

LibMCEnv_uint64 CBuildExecution::GetEndTimeStampInMicroseconds()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetEndTimeStampInMicroseconds();
}

LibMCEnv_uint64 CBuildExecution::GetElapsedTimeInMilliseconds()
{
	return GetElapsedTimeInMicroseconds() / 1000;
}

LibMCEnv_uint64 CBuildExecution::GetElapsedTimeInMicroseconds()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetElapsedTimeInMicroseconds();
}

std::string CBuildExecution::AddBinaryData(const std::string & sIdentifier, const std::string & sName, const std::string & sMIMEType, const LibMCEnv_uint64 nContentBufferSize, const LibMCEnv_uint8 * pContentBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IDiscreteFieldData2D * CBuildExecution::LoadDiscreteField2DByIdentifier(const std::string & sContextIdentifier)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IDiscreteFieldData2D * CBuildExecution::LoadDiscreteField2DByUUID(const std::string & sDataUUID)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::StoreDiscreteField2D(const std::string & sContextIdentifier, const std::string & sName, IDiscreteFieldData2D* pFieldDataInstance, IDiscreteFieldData2DStoreOptions* pStoreOptions)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IImageData * CBuildExecution::LoadPNGImageByIdentifier(const std::string & sContextIdentifier)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IImageData * CBuildExecution::LoadPNGImageByUUID(const std::string & sDataUUID)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::StorePNGImage(const std::string & sContextIdentifier, const std::string & sName, IImageData* pImageDataInstance, IPNGImageStoreOptions* pStoreOptions)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CBuildExecution::AddMetaDataString(const std::string & sKey, const std::string & sValue)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CBuildExecution::HasMetaDataString(const std::string & sKey)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::GetMetaDataString(const std::string & sKey)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

