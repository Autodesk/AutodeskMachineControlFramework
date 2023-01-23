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


Abstract: This is a stub class definition of CDriver_BK9xxx

*/

#include "libmcdriver_bk9xxx_driver_bk9xxx.hpp"
#include "libmcdriver_bk9xxx_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_BK9xxx::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_BK9xxx 
**************************************************************************************************************************/


CDriver_BK9xxx::CDriver_BK9xxx(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), 
	m_bIsInSimulationMode (false),
	m_nPort(0), 
	m_nTimeOutInMs(0), 
	m_pDriverEnvironment (pDriverEnvironment)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDPARAM);
}

CDriver_BK9xxx::~CDriver_BK9xxx()
{

}
	

void CDriver_BK9xxx::Configure(const std::string& sConfigurationString)
{
	// TODO: Parse the XML
}

std::string CDriver_BK9xxx::GetName()
{
	return m_sName;
}


void CDriver_BK9xxx::GetVersion(LibMCDriver_BK9xxx_uint32& nMajor, LibMCDriver_BK9xxx_uint32& nMinor, LibMCDriver_BK9xxx_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_BK9XXX_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_BK9XXX_VERSION_MINOR;
	nMicro = LIBMCDRIVER_BK9XXX_VERSION_MICRO;
}

void CDriver_BK9xxx::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_BK9xxx";
	sBaseName = "libmcdriver_bk9xxx";
}

void CDriver_BK9xxx::QueryParameters()
{
	
}

void CDriver_BK9xxx::SetToSimulationMode()
{
	if (IsConnected())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_SETSIMULATIONMODEBEFORECONNECTING);

	m_bIsInSimulationMode = true;
}

bool CDriver_BK9xxx::IsSimulationMode()
{
	return m_bIsInSimulationMode;
}

void CDriver_BK9xxx::Connect(const std::string & sIPAddress, const LibMCDriver_BK9xxx_uint32 nPort, const LibMCDriver_BK9xxx_uint32 nTimeOutInMs)
{
	//TODO
}

std::string CDriver_BK9xxx::GetIPAddress()
{
	return m_sIPAddress;
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetPort()
{
	return m_nPort;
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetTimeOut()
{
	return m_nTimeOutInMs;
}

void CDriver_BK9xxx::SetErrorRecoveryMode(const LibMCDriver_BK9xxx_uint32 nReconnectionDelayInMs, const LibMCDriver_BK9xxx_uint32 nReconnectionCount)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::GetErrorRecoveryMode(LibMCDriver_BK9xxx_uint32 & nReconnectionDelayInMs, const LibMCDriver_BK9xxx_uint32 nReconnectionCount)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::Disconnect()
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::ResetDevice()
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::Reconnect()
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::IsConnected()
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_BK9xxx::eErrorState CDriver_BK9xxx::GetErrorState()
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::VariableExists(const std::string & sName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_BK9xxx::eVariableType CDriver_BK9xxx::GetVariableType(const std::string & sName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::DigitalInputExists(const std::string & sName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::DigitalOutputExists(const std::string & sName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::AnalogInputExists(const std::string & sName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::AnalogOutputExists(const std::string & sName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::GetDigitalInput(const std::string & sVariableName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

bool CDriver_BK9xxx::GetDigitalOutput(const std::string & sVariableName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetAnalogInputRaw(const std::string & sVariableName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_BK9xxx_uint32 CDriver_BK9xxx::GetAnalogOutputRaw(const std::string & sVariableName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_BK9xxx_double CDriver_BK9xxx::GetAnalogInput(const std::string & sVariableName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_BK9xxx_double CDriver_BK9xxx::GetAnalogOutput(const std::string & sVariableName)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::SetDigitalOutput(const std::string & sVariableName, const bool bValue)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::SetAnalogOutputRaw(const std::string & sVariableName, const LibMCDriver_BK9xxx_uint32 nValue)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}

void CDriver_BK9xxx::SetAnalogOutput(const std::string & sVariableName, const LibMCDriver_BK9xxx_double dValue)
{
	throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_NOTIMPLEMENTED);
}


CDriver_BK9000::CDriver_BK9000(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_BK9xxx (sName, pDriverEnvironment)
{

}

CDriver_BK9000::~CDriver_BK9000()
{

}

std::string CDriver_BK9000::GetType()
{
	return "bk9000";
}


CDriver_BK9050::CDriver_BK9050(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_BK9xxx(sName, pDriverEnvironment)
{

}

CDriver_BK9050::~CDriver_BK9050()
{

}

std::string CDriver_BK9050::GetType()
{
	return "bk9050";
}


CDriver_BK9100::CDriver_BK9100(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: CDriver_BK9xxx(sName, pDriverEnvironment)
{

}

CDriver_BK9100::~CDriver_BK9100()
{

}

std::string CDriver_BK9100::GetType()
{
	return "bk9100";
}

