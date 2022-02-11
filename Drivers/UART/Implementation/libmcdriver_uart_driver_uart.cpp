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
#include "serial/serial.h"

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_UART::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_UART 
**************************************************************************************************************************/


CDriver_UART::CDriver_UART(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName (sName), m_pDriverEnvironment (pDriverEnvironment), m_bSimulationMode (false),
		m_Parity (LibMCDriver_UART::eUARTParity::Even),
		m_StopBits(LibMCDriver_UART::eUARTStopBits::OneStopBit),
		m_ByteSize(LibMCDriver_UART::eUARTByteSize::EightBits),
		m_FlowControl(LibMCDriver_UART::eUARTFlowControl::NoFlowControl)
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

void CDriver_UART::Connect(const std::string& sDeviceAddress, const LibMCDriver_UART_uint32 nBaudRate, const LibMCDriver_UART_uint32 nTimeout)
{
	m_pConnection.reset();

	serial::bytesize_t byteSize;
	switch (m_ByteSize) {
		case eUARTByteSize::FiveBits: byteSize = serial::fivebits; break;
		case eUARTByteSize::SixBits: byteSize = serial::sixbits; break;
		case eUARTByteSize::SevenBits: byteSize = serial::sevenbits; break;
		case eUARTByteSize::EightBits: byteSize = serial::eightbits; break;
		default:
			throw ELibMCDriver_UARTInterfaceException(LIBMCDRIVER_UART_ERROR_INVALIDBYTESIZE);
	}

	serial::stopbits_t stopBits;
	switch (m_StopBits) {
	case eUARTStopBits::OneStopBit: stopBits = serial::stopbits_one; break;
	case eUARTStopBits::TwoStopBits: stopBits = serial::stopbits_two; break;
	case eUARTStopBits::OnePointFiveStopBits: stopBits = serial::stopbits_one_point_five; break;
	default:
		throw ELibMCDriver_UARTInterfaceException(LIBMCDRIVER_UART_ERROR_INVALIDSTOPBITS);
	}

	serial::parity_t parity;
	switch (m_Parity) {
	case eUARTParity::None: parity = serial::parity_none; break;
	case eUARTParity::Odd: parity = serial::parity_odd; break;
	case eUARTParity::Even: parity = serial::parity_even; break;
	case eUARTParity::Mark: parity = serial::parity_mark; break;
	case eUARTParity::Space: parity = serial::parity_space; break;
	default:
		throw ELibMCDriver_UARTInterfaceException(LIBMCDRIVER_UART_ERROR_INVALIDPARITY);
	}

	serial::flowcontrol_t flowControl;
	switch (m_FlowControl) {
	case eUARTFlowControl::NoFlowControl: flowControl = serial::flowcontrol_none; break;
	case eUARTFlowControl::Software: flowControl = serial::flowcontrol_software; break;
	case eUARTFlowControl::Hardware: flowControl = serial::flowcontrol_hardware; break;
	default:
		throw ELibMCDriver_UARTInterfaceException(LIBMCDRIVER_UART_ERROR_INVALIDFLOWCONTROL);
	}

	m_pConnection.reset(new serial::Serial(sDeviceAddress, nBaudRate, serial::Timeout::simpleTimeout(nTimeout), byteSize, parity, stopBits, flowControl));

	if (!m_pConnection->isOpen()) {
		m_pConnection.reset();
		throw ELibMCDriver_UARTInterfaceException(LIBMCDRIVER_UART_ERROR_COULDNOTCONNECT);
	}

}

void CDriver_UART::Disconnect()
{
	m_pConnection.reset();
}

bool CDriver_UART::IsConnected()
{
	if (m_pConnection.get() != nullptr) {
		return m_pConnection->available();
	}

	return false;
}

std::string CDriver_UART::SendLine(const std::string& sLineToSend, const LibMCDriver_UART_uint32 nTimeout)
{
	if (m_pConnection.get() == nullptr) 
		throw ELibMCDriver_UARTInterfaceException(LIBMCDRIVER_UART_ERROR_DRIVERNOTCONNECTED);

	m_pConnection->write(sLineToSend);
	return m_pConnection->readline();
}


void CDriver_UART::SetParity(const LibMCDriver_UART::eUARTParity eParity)
{
	m_Parity = eParity;
}

LibMCDriver_UART::eUARTParity CDriver_UART::GetParity()
{
	return m_Parity;
}

void CDriver_UART::SetStopBits(const LibMCDriver_UART::eUARTStopBits eStopBits)
{
	m_StopBits = eStopBits;
}

LibMCDriver_UART::eUARTStopBits CDriver_UART::GetStopBits()
{
	return m_StopBits;
}

void CDriver_UART::SetByteSize(const LibMCDriver_UART::eUARTByteSize eByteSize)
{
	m_ByteSize = eByteSize;
}

LibMCDriver_UART::eUARTByteSize CDriver_UART::GetByteSize()
{
	return m_ByteSize;
}

void CDriver_UART::SetFlowControl(const LibMCDriver_UART::eUARTFlowControl eFlowControl)
{
	m_FlowControl = eFlowControl;
}

LibMCDriver_UART::eUARTFlowControl CDriver_UART::GetFlowControl()
{
	return m_FlowControl;
}