/*++

Copyright (C) 2024 ASL Inc.

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


Abstract: This is a stub class definition of CDriverContext

*/

#include "libmcdriver_asl_drivercontextinstance.hpp"
#include "libmcdriver_asl_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_ASL::Impl;

/*************************************************************************************************************************
 Class definition of CDriverContext 
**************************************************************************************************************************/

CDriverContextInstance::CDriverContextInstance(const std::string& sIdentifier, const std::string& sCOMPort, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sIdentifier (sIdentifier), m_sCOMPort (sCOMPort), m_pDriverEnvironment (pDriverEnvironment)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_INVALIDPARAM);

	if (sCOMPort.empty())
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_EMPTYDRIVERCONTEXTCOMPORT);

	for (auto ch : sCOMPort)
		if (!isalnum (ch))
			throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_INVALIDDRIVERCONTEXTCOMPORT, "invalid driver context COM Port: " + sCOMPort);

	if (sIdentifier.empty())
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_EMPTYDRIVERCONTEXTIDENTIFIER);

	for (auto ch : sIdentifier)
		if (!isalnum(ch))
			throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_INVALIDDRIVERCONTEXTIDENTIFIER, "invalid driver context COM Port: " + sCOMPort);

	m_pConnection.reset(new serial::Serial(sCOMPort, 1000000));

	if (!m_pConnection->isOpen()) {
		m_pConnection.reset();
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "could not connect to COM Port: " + sCOMPort);
	}

}

CDriverContextInstance::~CDriverContextInstance()
{

}

std::string CDriverContextInstance::GetSerialNumber()
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SetPower(const bool bPower)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SetPrintheadMode(const LibMCDriver_ASL::eBoardMode eMode)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SetFrequency(const LibMCDriver_ASL_uint32 nFrequency)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SetTemperature(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_double dTemperature)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::HomeLocation()
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SetPrintStart(const LibMCDriver_ASL_uint32 nStartLocation)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SendImage(LibMCEnv::PImageData pImageObject)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::Poll()
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_double CDriverContextInstance::GetTemperature(const LibMCDriver_ASL_uint8 nIndex)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_double CDriverContextInstance::GetPrintCounts(const LibMCDriver_ASL_uint8 nIndex)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_double CDriverContextInstance::GetImageLength(const LibMCDriver_ASL_uint8 nIndex)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_double CDriverContextInstance::GetHeadState(const LibMCDriver_ASL_uint8 nIndex)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

bool CDriverContextInstance::IsHeating(const LibMCDriver_ASL_uint8 nIndex)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

bool CDriverContextInstance::GetPower()
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

