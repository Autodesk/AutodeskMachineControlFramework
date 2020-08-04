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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "AMC_SerialController_Marlin.hpp"

#include <thread>

namespace AMC {

	CSerialController_Marlin::CSerialController_Marlin(bool bDebug)
		: m_sCOMPort ("COM1"), 
		m_nBaudRate (115200), 
		m_sAckSymbol ("ok"), 
		m_nLineNumber (1), 
		m_bDebug (bDebug),
		m_sResendSymbol ("Resend:"), 
		 m_nMinBufferSpace (3), m_nMaxBufferSpace (0), m_nCurrentBufferSpace (0)
	{

	}

	void CSerialController_Marlin::setCOMPort(const std::string& sCOMPort)
	{
		m_sCOMPort = sCOMPort;
	}

	std::string CSerialController_Marlin::getCOMPort()
	{
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		return m_sCOMPort;
	}

	void CSerialController_Marlin::setBaudrate(const uint32_t nBaudrate)
	{
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		m_nBaudRate = nBaudrate;
	}

	uint32_t CSerialController_Marlin::getBaudrate()
	{
		return m_nBaudRate;
	}

	CSerialController_Marlin::~CSerialController_Marlin()
	{
	}

	void CSerialController_Marlin::initializeController()
	{
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		m_pConnection.reset(new serial::Serial(m_sCOMPort, m_nBaudRate, serial::Timeout::simpleTimeout(1000)));

		if (!m_pConnection->isOpen()) {
			m_pConnection.reset();
			throw std::runtime_error("Could not connect to serial port");
		}

		m_nLineNumber = 1;

		sendCommand("M110 N1");

		m_nCurrentBufferSpace = 0;
		sendCommand("M400");

		if (m_nCurrentBufferSpace > 0) {
			m_nMaxBufferSpace = m_nCurrentBufferSpace;
		}

		sendCommand("G21");

	}

	void CSerialController_Marlin::disconnectController()
	{
		m_pConnection.reset();
	}

	void CSerialController_Marlin::resetController()
	{
		disconnectController();
		initializeController();
	}

	uint32_t CSerialController_Marlin::calculateLineChecksum(const std::string& sCommand)
	{
		uint32_t nChecksum = 0;
		for (size_t idx = 0; idx < sCommand.length(); idx++) {
			unsigned char nCharacter = (unsigned char)sCommand.at(idx);
			nChecksum = nChecksum ^ (uint32_t) (nCharacter);
		}

		return nChecksum & 0x00ff;
	}


	bool CSerialController_Marlin::parseAckSymbol(const std::string& sLine, const uint32_t nLineNumber) {

		auto nOKPos = sLine.find(m_sAckSymbol);

		if (nOKPos != std::string::npos) {

			std::string sOKLine = sLine.substr(nOKPos);
			auto nNPos = sOKLine.find ("N");
			if (nNPos != std::string::npos) {
				auto sNLine = sOKLine.substr(nNPos + 1);
				auto nReturnedLineNumber = std::stoul(sNLine);
				if (nLineNumber != nReturnedLineNumber)
					throw std::runtime_error("invalid line number!");
			}

			auto nPPos = sOKLine.find("P");
			if (nPPos != std::string::npos) {
				auto sPLine = sOKLine.substr(nPPos + 1);
				m_nCurrentBufferSpace = std::stoul(sPLine);
			}

			return true;
		}

		return false;
	}


	std::stringstream CSerialController_Marlin::sendCommand(const std::string& sCommand)
	{
		if (m_pConnection.get() == nullptr)
			throw std::runtime_error("Serial Port not initialized");

		std::string sLineToSend;
		std::string sLineNumberCommand = "N" + std::to_string(m_nLineNumber) + " " + sCommand;
		auto nLineNumber = m_nLineNumber;
		m_nLineNumber++;

		uint32_t nCheckSum = calculateLineChecksum(sLineNumberCommand);
		   sLineToSend = sLineNumberCommand + "*" + std::to_string(nCheckSum) + "\n";

		m_pConnection->write(sLineToSend);

		if (m_bDebug) {
			std::cout << "SENT: " << sLineToSend;
		}

		std::stringstream sResultStream;

		bool bAck = false;
		while (!bAck) {
			std::string sLineRead = m_pConnection->readline();
			sResultStream << sLineRead;

			if (m_bDebug) {
				std::cout << "RETURN: " << sLineRead << std::endl;
			}

			bool bResend = (sLineRead.find(m_sResendSymbol) != std::string::npos);
			if (bResend) {
				// For now fail, because firmware was not in reset state...
				// TODO: Find out how to reset firmware
				throw std::runtime_error("line number mismatch in firmware");
			}

			bAck = parseAckSymbol(sLineRead, nLineNumber);
				
		}
		sResultStream << std::endl;
		return sResultStream;
	}



	void CSerialController_Marlin::setHeatedBedTargetTemperature(double nTemperatureInDegreeCelcius)
	{
	}

	void CSerialController_Marlin::setExtruderTargetTemperature(uint32_t nExtruderIndex, double nTemperatureInDegreeCelcius)
	{

	}

	void CSerialController_Marlin::queryTemperatureState(uint32_t nExtruderIndex)
	{
		auto sStream = sendCommand("M105 T" + std::to_string (nExtruderIndex));
	}

	void CSerialController_Marlin::queryPositionState()
	{
		auto sStream = sendCommand("M114");
		auto sLine = sStream.str();

		auto nPosition = sLine.find("Count");

		if (nPosition == std::string::npos) 
			throw std::runtime_error("could not query position state.");

		auto sTargetPosString = sLine.substr(0, nPosition);
		auto sCurrentPosString = sLine.substr(nPosition + 5);

		auto nTargetXPosition = sTargetPosString.find("X:");
		auto nTargetYPosition = sTargetPosString.find("Y:");
		auto nTargetZPosition = sTargetPosString.find("Z:");
		auto nTargetEPosition = sTargetPosString.find("E:");

		if (nTargetXPosition == std::string::npos)
			throw std::runtime_error("could not find target X position.");
		if (nTargetYPosition == std::string::npos)
			throw std::runtime_error("could not find target Y position.");
		if (nTargetZPosition == std::string::npos)
			throw std::runtime_error("could not find target Z position.");
		if (nTargetEPosition == std::string::npos)
			throw std::runtime_error("could not find target E position.");

		if (!((nTargetXPosition < nTargetYPosition) && (nTargetYPosition < nTargetZPosition) &&
			(nTargetZPosition < nTargetEPosition)))
				throw std::runtime_error("invalid target position order.");

		auto sTargetXString = sTargetPosString.substr(nTargetXPosition + 2, nTargetYPosition - (nTargetXPosition + 2));
		auto sTargetYString = sTargetPosString.substr(nTargetYPosition + 2, nTargetZPosition - (nTargetYPosition + 2));
		auto sTargetZString = sTargetPosString.substr(nTargetZPosition + 2, nTargetEPosition - (nTargetZPosition + 2));
		auto sTargetEString = sTargetPosString.substr(nTargetEPosition + 2);

		m_dTargetPosX = std::stod(sTargetXString);
		m_dTargetPosY = std::stod(sTargetYString);
		m_dTargetPosZ = std::stod(sTargetZString);
		m_dTargetPosE = std::stod(sTargetEString);

		auto nCurrentXPosition = sCurrentPosString.find("X:");
		auto nCurrentYPosition = sCurrentPosString.find("Y:");
		auto nCurrentZPosition = sCurrentPosString.find("Z:");

		if (nCurrentXPosition == std::string::npos)
			throw std::runtime_error("could not find current X position.");
		if (nCurrentYPosition == std::string::npos)
			throw std::runtime_error("could not find current Y position.");
		if (nCurrentZPosition == std::string::npos)
			throw std::runtime_error("could not find current Z position.");

		if (!((nCurrentXPosition < nCurrentYPosition) && (nCurrentYPosition < nCurrentZPosition)))
			throw std::runtime_error("invalid target current order.");

		auto sCurrentXString = sCurrentPosString.substr(nCurrentXPosition + 2, nCurrentYPosition - (nCurrentXPosition + 2));
		auto sCurrentYString = sCurrentPosString.substr(nCurrentYPosition + 2, nCurrentZPosition - (nCurrentYPosition + 2));
		auto sCurrentZString = sCurrentPosString.substr(nCurrentZPosition + 2);

		m_dCurrentPosX = std::stod(sCurrentXString);
		m_dCurrentPosY = std::stod(sCurrentYString);
		m_dCurrentPosZ = std::stod(sCurrentZString);

	}

	void CSerialController_Marlin::getHeatedBedTemperature(double& dTargetTemperature, double& dCurrentTemperature)
	{

	}

	void CSerialController_Marlin::getExtruderTemperature(uint32_t nExtruderIndex, double& dTargetTemperature, double& dCurrentTemperature)
	{
		dTargetTemperature = 0.0;
		dCurrentTemperature = 0.0;
	}


	void CSerialController_Marlin::getTargetPosition(double& dX, double& dY, double& dZ)
	{
		dX = m_dTargetPosX;
		dY = m_dTargetPosY;
		dZ = m_dTargetPosZ;
	}

	void CSerialController_Marlin::getCurrentPosition(double& dX, double& dY, double& dZ)
	{
		dX = m_dCurrentPosX;
		dY = m_dCurrentPosY;
		dZ = m_dCurrentPosZ;
	}

	void CSerialController_Marlin::getExtruderPosition(double& dE)
	{
		dE = m_dCurrentPosE;
	}

	void CSerialController_Marlin::startHoming()
	{
		sendCommand("G28");
		sendCommand("M400");
	}

	void CSerialController_Marlin::setLcdMsg(const std::string& sLcdMsg)
	{
		std::string sCommand = "M117 " + sLcdMsg;
		sendCommand(sCommand);
	}

	void CSerialController_Marlin::setPositioningAbolute()
	{
		sendCommand("G90");
	}

	void CSerialController_Marlin::setPositioningRelative()
	{
		sendCommand("G91");
	}

	void CSerialController_Marlin::move(const double dX, const double dY, const double dZ, const double dSpeedInMMperSecond)
	{
		std::stringstream sCommand;
		sCommand << "G1 X" << dX << " Y" << dY << " Z" << dZ << " F" << dSpeedInMMperSecond * 60.0;
		sendCommand(sCommand.str());

		m_dTargetPosX = dX;
		m_dTargetPosY = dY;
		m_dTargetPosZ = dZ;
	}

	void CSerialController_Marlin::moveFast(const double dX, const double dY, const double dZ, const double dSpeedInMMperSecond)
	{
		std::stringstream sCommand;
		sCommand << "G0 X" << dX << " Y" << dY << " Z" << dZ << " F" << dSpeedInMMperSecond * 60.0;

		sendCommand(sCommand.str());

		m_dTargetPosX = dX;
		m_dTargetPosY = dY;
		m_dTargetPosZ = dZ;
	}

	void CSerialController_Marlin::waitForMovement()
	{
		sendCommand("M400");		
	}

	bool CSerialController_Marlin::canReceiveMovement()
	{
		return (m_nCurrentBufferSpace > m_nMinBufferSpace);
	}

	bool CSerialController_Marlin::isMoving()
	{
		return (m_nCurrentBufferSpace != m_nMaxBufferSpace);
	}


}