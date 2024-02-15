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
#include "thread"
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

	auto tOut = serial::Timeout(5, 5, 5, 5, 5);

	m_pConnection.reset(new serial::Serial(sCOMPort, 1000000, tOut));

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
	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}

	m_pConnection->write("O");

}


void CDriverContextInstance::SetPrintheadMode(const LibMCDriver_ASL::eBoardMode eMode)
{
	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}

	if (eMode == LibMCDriver_ASL::eBoardMode::DW_INT)
		m_pConnection->write("M 1\n");
	if (eMode == LibMCDriver_ASL::eBoardMode::DW_EXT)
		m_pConnection->write("M 2\n");
	if (eMode == LibMCDriver_ASL::eBoardMode::IMG_ENC)
		m_pConnection->write("M 4\n");
	if (eMode == LibMCDriver_ASL::eBoardMode::IMG_PD)
		m_pConnection->write("M 5\n");

}

void CDriverContextInstance::SetFrequency(const LibMCDriver_ASL_uint32 nFrequency)
{
	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}

	m_pConnection->write("p" + std::to_string(nFrequency) + '\n');

}

void CDriverContextInstance::SetTemperature(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_double dTemperature)
{
	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}

	m_pConnection->write("T" + std::to_string(nIndex) + " " + std::to_string(dTemperature) + '\n');

}

void CDriverContextInstance::HomeLocation()
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::SetPrintStart(const LibMCDriver_ASL_uint32 nStartLocation)
{
	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}

	m_pConnection->write("," + std::to_string(nStartLocation) + '\n');
}

void CDriverContextInstance::SendImage(LibMCEnv::PImageData pImageObject)
{
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

void CDriverContextInstance::Poll()
{

	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}
	//flush buffer

	m_pConnection->flushInput();

	auto toRead = m_pConnection->available(); 
	m_pConnection->read(toRead);

	m_pConnection->write("b");

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	//m_pConnection->waitReadable();
	toRead = m_pConnection->available();
	//bad code
	while (toRead != m_pConnection->available())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		toRead = m_pConnection->available();
	}

	auto msg = m_pConnection->read(toRead);

	/*
	double m_dSetTemperatures[4];
	double m_dCurrentTemperatures[4];
	double m_dPrintCounts[4];
	double m_dImageLength[4];
	double m_dStates[4];
	*/

	for (int idx = 0; idx < 4; idx++) {

		auto nHeadStartPosition = msg.find("\"head\": " + std::to_string(idx + 1));
		auto nHeadStopPosition = msg.find("\"head\": " + std::to_string(idx + 2)); //wont find 5, but that should be OK

		auto sHeadString = msg.substr(nHeadStartPosition, nHeadStopPosition - nHeadStartPosition);

		auto nHeadStatus = sHeadString.find("status");
		auto nHeadCurrentTemp = sHeadString.find("curTemperature");
		auto nHeadSetTemp = sHeadString.find("setTemperature");
		auto nHeadPrintCounts = sHeadString.find("printCounts");


		auto nHeadStatusEnd = sHeadString.find(',', nHeadStatus);
		auto nHeadCurrentTempEnd = sHeadString.find(',', nHeadCurrentTemp);
		auto nHeadSetTempEnd = sHeadString.find(',', nHeadSetTemp);
		auto nHeadPrintCountsEnd = sHeadString.find(',', nHeadPrintCounts);


		auto sHeadStatus = sHeadString.substr(nHeadStatus+8, nHeadStatusEnd - (nHeadStatus+8));
		auto sHeadCurrentTemp = sHeadString.substr(nHeadCurrentTemp+16, nHeadCurrentTempEnd - (nHeadCurrentTemp+16));
		auto sHeadSetTemp = sHeadString.substr(nHeadSetTemp + 16, nHeadSetTempEnd - (nHeadSetTemp+16));
		auto sHeadPrintCounts = sHeadString.substr(nHeadPrintCounts+13, nHeadPrintCountsEnd - (nHeadPrintCounts+13));

		m_dStates[idx] = std::stod(sHeadStatus);
		m_dCurrentTemperatures[idx] = std::stod(sHeadCurrentTemp);
		m_dSetTemperatures[idx] = std::stod(sHeadSetTemp);
		m_dPrintCounts[idx] = std::stod(sHeadPrintCounts);

	}
}

LibMCDriver_ASL_double CDriverContextInstance::GetTemperature(const LibMCDriver_ASL_uint8 nIndex)
{
	if(true)
		CDriverContextInstance::Poll();

	return m_dSetTemperatures[nIndex-1];

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

