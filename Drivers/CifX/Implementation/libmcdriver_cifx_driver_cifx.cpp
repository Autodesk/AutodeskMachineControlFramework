/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_CifX

*/

#include "libmcdriver_cifx_driver_cifx.hpp"
#include "libmcdriver_cifx_interfaceexception.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_CifX::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_CifX 
**************************************************************************************************************************/

CDriver_CifX::CDriver_CifX(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_bIsSimulationMode (false), m_pDriverEnvironment (pDriverEnvironment)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_INVALIDPARAM);

}

CDriver_CifX::~CDriver_CifX()
{

}


void CDriver_CifX::Configure(const std::string& sConfigurationString)
{
	
}

std::string CDriver_CifX::GetName()
{
	return m_sName;
}

std::string CDriver_CifX::GetType()
{
	return "cifx-1.0";
}

void CDriver_CifX::GetVersion(LibMCDriver_CifX_uint32& nMajor, LibMCDriver_CifX_uint32& nMinor, LibMCDriver_CifX_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_CIFX_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_CIFX_VERSION_MINOR;
	nMicro = LIBMCDRIVER_CIFX_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);

}

void CDriver_CifX::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_CifX";
	sBaseName = "libmcdriver_cifx";
}

void CDriver_CifX::QueryParameters()
{
}


void CDriver_CifX::SetToSimulationMode()
{
}

bool CDriver_CifX::IsSimulationMode()
{
	return m_bIsSimulationMode;
}

void CDriver_CifX::SetCustomSDKResource(const std::string & sResourceName)
{
}

LibMCDriver_CifX_uint32 CDriver_CifX::EnumerateBoards()
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

IBoardInformation * CDriver_CifX::GetBoardInformation(const LibMCDriver_CifX_uint32 nBoardIndex)
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

void CDriver_CifX::Connect()
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

void CDriver_CifX::Disconnect()
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_CifX::IsConnected()
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_CifX::ValueExists(const std::string& sName)
{
	throw ELibMCDriver_CifXInterfaceException(LIBMCDRIVER_CIFX_ERROR_NOTIMPLEMENTED);
}

void CDriver_CifX::WriteIntegerValue(const std::string& sName, const LibMCDriver_CifX_int64 nValue, const LibMCDriver_CifX_uint32 nTimeOutInMs)
{

}

void CDriver_CifX::WriteBoolValue(const std::string& sName, const bool bValue, const LibMCDriver_CifX_uint32 nTimeOutInMs)
{

}

void CDriver_CifX::WriteDoubleValue(const std::string& sName, const LibMCDriver_CifX_double dValue, const LibMCDriver_CifX_uint32 nTimeOutInMs)
{

}

LibMCDriver_CifX_int64 CDriver_CifX::ReadIntegerValue(const std::string& sName)
{
	return 0;
}

bool CDriver_CifX::ReadBoolValue(const std::string& sName)
{
	return false;
}

LibMCDriver_CifX_double CDriver_CifX::ReadDoubleValue(const std::string& sName)
{
	return 0.0;
}

