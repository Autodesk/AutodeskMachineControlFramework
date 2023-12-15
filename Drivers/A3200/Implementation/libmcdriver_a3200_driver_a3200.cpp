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


Abstract: This is a stub class definition of CDriver_A3200

*/

#include "libmcdriver_a3200_driver_a3200.hpp"
#include "libmcdriver_a3200_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_A3200::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_A3200 
**************************************************************************************************************************/

void CDriver_A3200::SetToSimulationMode()
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

bool CDriver_A3200::IsSimulationMode()
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::SetCustomSDKResource(const std::string & sCoreResourceName, const std::string & sSystemResourceName, const std::string & sCompilerResourceName, const std::string & sUtilitiesResourceName, const std::string & sLicenseDecoderResourceName)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::SetCustomSDK(const LibMCDriver_A3200_uint64 nCoreSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pCoreSDKBufferBuffer, const LibMCDriver_A3200_uint64 nSystemSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pSystemSDKBufferBuffer, const LibMCDriver_A3200_uint64 nCompilerSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pCompilerSDKBufferBuffer, const LibMCDriver_A3200_uint64 nUtilitiesSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pUtilitiesSDKBufferBuffer, const LibMCDriver_A3200_uint64 nLicenseDecoderSDKBufferBufferSize, const LibMCDriver_A3200_uint8 * pLicenseDecoderSDKBufferBuffer)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::Connect(const LibMCDriver_A3200_uint32 nTimeout)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::Disconnect()
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

bool CDriver_A3200::VariableExists(const std::string & sVariableName)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_A3200_int64 CDriver_A3200::ReadIntegerValue(const std::string & sVariableName)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::WriteIntegerValue(const std::string & sVariableName, const LibMCDriver_A3200_int64 nValue)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_A3200_double CDriver_A3200::ReadFloatValue(const std::string & sVariableName)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::WriteFloatValue(const std::string & sVariableName, const LibMCDriver_A3200_double dValue)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

bool CDriver_A3200::ReadBoolValue(const std::string & sVariableName)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::WriteBoolValue(const std::string & sVariableName, const bool bValue)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

std::string CDriver_A3200::ReadStringValue(const std::string & sVariableName)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::WriteStringValue(const std::string & sVariableName, const std::string & sValue)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

void CDriver_A3200::GetVariableBounds(const std::string & sVariableName, LibMCDriver_A3200_int64 & nMinValue, LibMCDriver_A3200_int64 & nMaxValue)
{
	throw ELibMCDriver_A3200InterfaceException(LIBMCDRIVER_A3200_ERROR_NOTIMPLEMENTED);
}

