/*++

Copyright (C) 2022 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_UART

*/

#include "libmcdriver_uart_driver_uart.hpp"
#include "libmcdriver_uart_interfaceexception.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace LibMCDriver_UART::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_UART 
**************************************************************************************************************************/


CDriver_UART::CDriver_UART(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_pDriverEnvironment (pDriverEnvironment), m_bSimulationMode (false)
{

}

CDriver_UART::~CDriver_UART()
{

}

void CDriver_UART::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_UART::GetName()
{
	return m_sName;
}

std::string CDriver_UART::GetType()
{
	return "uart";
}

void CDriver_UART::GetVersion(LibMCDriver_UART_uint32& nMajor, LibMCDriver_UART_uint32& nMinor, LibMCDriver_UART_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_UART_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_UART_VERSION_MINOR;
	nMicro = LIBMCDRIVER_UART_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);;
}

void CDriver_UART::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_UART";
	sBaseName = "libmcdriver_uart";
}

void CDriver_UART::QueryParameters()
{
	
}



void CDriver_UART::SetToSimulationMode()
{
	m_bSimulationMode = true;
}

bool CDriver_UART::IsSimulationMode()
{
	return m_bSimulationMode;
}

void CDriver_UART::Connect(const std::string & sDeviceAddress, const LibMCDriver_UART_uint32 nTimeout)
{
}

void CDriver_UART::Disconnect()
{
}

