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


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CBuildExecution 
**************************************************************************************************************************/

std::string CBuildExecution::GetUUID()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::GetBuildUUID()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IBuild * CBuildExecution::GetBuild()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv::eBuildExecutionStatus CBuildExecution::GetExecutionStatus()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CBuildExecution::IsInProcess()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CBuildExecution::IsFinished()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CBuildExecution::IsFailed()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CBuildExecution::SetStatusToFinished()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CBuildExecution::SetStatusToFailed()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::GetDescription()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

void CBuildExecution::SetDescription(const std::string & sDescription)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::GetJournalUUID()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

bool CBuildExecution::HasAttachedUser()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::GetUserUUID()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuildExecution::GetStartTimeInUTC()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CBuildExecution::GetStartTimeStampInMilliseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CBuildExecution::GetStartTimeStampInMicroseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CBuildExecution::GetEndTimeStampInMilliseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CBuildExecution::GetEndTimeStampInMicroseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CBuildExecution::GetElapsedTimeInMilliseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

LibMCEnv_uint64 CBuildExecution::GetElapsedTimeInMicroseconds()
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
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

