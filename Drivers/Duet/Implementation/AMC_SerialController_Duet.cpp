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

#include "AMC_SerialController_Duet.hpp"

#include <algorithm>
#include <thread>

#ifdef __linux__
#include <math.h>
#endif

#define DUETDRIVER_MINSPEED 0.0001

namespace AMC {

	CSerialController_Duet::CSerialController_Duet()
		: m_sCOMPort("COM1"),
		m_nBaudRate(115200),
		m_nConnectTimeout(2000),
		m_bIsConnected(false),
		m_dStatusUpdateTimerInterval(100),
		m_sAckSymbol("ok"),
		m_nLineNumber(1),
		m_bIsHomed(false),
		m_dTargetPosX(0),
		m_dTargetPosY(0),
		m_dTargetPosZ(0),
		m_dTargetPosE(0),
		m_dCurrentPosX(0),
		m_dCurrentPosY(0),
		m_dCurrentPosZ(0),
		m_dAxisStepsPerUnitX(0),
		m_dAxisStepsPerUnitY(0),
		m_dAxisStepsPerUnitZ(0),
		m_dAxisStepsPerUnitE(0),
		m_dCurrentSpeedInMMperSecond(0),
		m_sResendSymbol ("Resend:")
	{
	}

	void CSerialController_Duet::setStatusUpdateTimerInterval(const double dStatusUpdateTimerInterval)
	{
		m_dStatusUpdateTimerInterval = dStatusUpdateTimerInterval;
	}


		void CSerialController_Duet::setCOMPort(const std::string& sCOMPort)
	{
		m_sCOMPort = sCOMPort;
	}

	std::string CSerialController_Duet::getCOMPort()
	{
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		return m_sCOMPort;
	}
	
	void CSerialController_Duet::setBaudrate(const uint32_t nBaudrate)
	{
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		m_nBaudRate = nBaudrate;
	}

	uint32_t CSerialController_Duet::getBaudrate()
	{
		return m_nBaudRate;
	}

	bool CSerialController_Duet::isConnected()
	{
		return m_bIsConnected;
	}

	void CSerialController_Duet::setConnectTimeout(const uint32_t nConnectTimeout)
	{
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		m_nConnectTimeout = nConnectTimeout;
	}

	uint32_t CSerialController_Duet::getConnectTimeout()
	{
		return m_nConnectTimeout;
	}

	CSerialController_Duet::~CSerialController_Duet()
	{
	}

	void CSerialController_Duet::initializeController()
	{
		m_bIsConnected = false;
		if (m_pConnection.get() != nullptr)
			throw std::runtime_error("Serial Port already initialized");

		// set timeout >1000 => 2000 (default of m_nConnectTimeout)
		// 
		// if printer is booting (echoing start+ Date+Memory... after switch on printer or plug in cable) and
		// timeout would be 1000 the while loop below we be leaved "to early" (after timeout of 1sec) and
		// the next command is sent, but printer can't respond to this command with OK,
		// because he's not ready to receive commands => boot initialize hags up!!
		//
		// if printer is not booting (no echoing text... after software" reconnect)
		// "start" will no be received,
		// but timeout after 2000 will respond with an empty string and
		// printer is ready to receive commands
		m_pConnection.reset(new serial::Serial(m_sCOMPort, m_nBaudRate, serial::Timeout::simpleTimeout(m_nConnectTimeout)));

		if (!m_pConnection->isOpen()) {
			m_pConnection.reset();
			throw std::runtime_error("Could not connect to serial port");
		}

		// instead of waiting a fixed time wait until "start" or "" (after timeout) received from device??
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		bool bAck = false;
		while (!bAck) {
			std::string sLineRead = m_pConnection->readline();
			if (m_bDebug) {
				std::cout << "RETURN: " << sLineRead << std::endl;
			}

			bAck = sLineRead.empty() || (sLineRead.find("start") != std::string::npos);
		}


		// reset line number
		std::stringstream sCommand;
		sCommand << "M110 " << m_nLineNumber;
		sendCommand(sCommand.str());

		sendCommand("M400");
		sendCommand("G21"); // set mm units
		sendCommand("M149 C"); // set temperature unit to Celsius
		sendCommand("M113 S1"); // keep alive period 0-60 possible, no decimals!
		setPositioningAbolute();
		m_bIsConnected = true;
	}

	void CSerialController_Duet::disconnectController()
	{
		m_bIsConnected = false;
		m_pConnection.reset();
	}

	void CSerialController_Duet::resetController()
	{
		disconnectController();
		initializeController();
	}



	uint32_t CSerialController_Duet::calculateLineChecksum(const std::string& sCommand)
	{
		uint32_t nChecksum = 0;
		for (size_t idx = 0; idx < sCommand.length(); idx++) {
			unsigned char nCharacter = (unsigned char)sCommand.at(idx);
			nChecksum = nChecksum ^ (uint32_t) (nCharacter);
		}

		return nChecksum & 0x00ff;
	}


	bool CSerialController_Duet::parseAckSymbol(const std::string& sLine, const uint32_t nLineNumber) {

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
				//m_nCurrentBufferSpace = std::stoul(sPLine);
			}

			return true;
		}

		return false;
	}


	std::stringstream CSerialController_Duet::sendCommand(const std::string& sCommand)
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
				throw std::runtime_error("line number mismatch in firmware");
			}

			bAck = parseAckSymbol(sLineRead, nLineNumber);
				
		}
		sResultStream << std::endl;
		return sResultStream;
	}



	void CSerialController_Duet::queryPositionState()
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
		auto nCurrentEolPosition = sCurrentPosString.find("\n");


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
		auto sCurrentZString = sCurrentPosString.substr(nCurrentZPosition + 2, nCurrentEolPosition - (nCurrentZPosition + 2));

		m_dCurrentPosX = std::stod(sCurrentXString) / m_dAxisStepsPerUnitX;;
		m_dCurrentPosY = std::stod(sCurrentYString) / m_dAxisStepsPerUnitY;;
		m_dCurrentPosZ = std::stod(sCurrentZString) / m_dAxisStepsPerUnitZ;;

	}



	void CSerialController_Duet::getTargetPosition(double& dX, double& dY, double& dZ)
	{
		dX = m_dTargetPosX;
		dY = m_dTargetPosY;
		dZ = m_dTargetPosZ;
	}

	void CSerialController_Duet::getCurrentPosition(double& dX, double& dY, double& dZ)
	{
		dX = m_dCurrentPosX;
		dY = m_dCurrentPosY;
		dZ = m_dCurrentPosZ;
	}

	void CSerialController_Duet::setPositioningAbolute()
	{
		sendCommand("G90");
	}

	void CSerialController_Duet::setPositioningRelative()
	{
		sendCommand("G91");
	}

	void CSerialController_Duet::moveToEx(bool bFastMove,
		bool bInX, const double dX, 
		bool bInY, const double dY, 
		bool bInZ, const double dZ,
		bool bInE, const double dE,
		const double dSpeedInMMperSecond)
	{
		std::stringstream sCommand;

		if (bFastMove) {
			sCommand << "G0";
		}
		else {
			sCommand << "G1";
		}

		if (bInX) {
			// X given => add X+value to command str
			sCommand << " X" << dX;
		}
		if (bInY) {
			// Y given => add Y+value to command str
			sCommand << " Y" << dY;
		}
		if (bInZ) {
			// Z given => add Z+value to command str
			sCommand << " Z" << dZ;
		}
		if (bInE && !bFastMove) {
			// E given => add E+value to command str
			// TODO XXXXXXXXXXXXXXXX remove to activate Extrusion
			sCommand << " E" << dE;
			//std::cout << "CALCULATED E = " << dE << std::endl;
		}
		if (dSpeedInMMperSecond > 0) {
			if (fabs(m_dCurrentSpeedInMMperSecond - dSpeedInMMperSecond) > DUETDRIVER_MINSPEED) {
				m_dCurrentSpeedInMMperSecond = dSpeedInMMperSecond;
				sCommand << " F" << (int)(dSpeedInMMperSecond * 60.0);
			}
		}
		sendCommand(sCommand.str());

		if (bInX) {
			m_dTargetPosX = dX;
		}
		if (bInY) {
			m_dTargetPosY = dY;
		}
		if (bInZ) {
			m_dTargetPosZ = dZ;
		}
		if (bInE) {
			m_dTargetPosE = dE;
		}
	}


	void CSerialController_Duet::moveXY(const double dX, const double dY, const double dE, const double dSpeedInMMperSecond)
	{
		bool dInE = false;
		if (fabs(dE) > DUETDRIVER_MINSPEED) {
			dInE = true;
		}
		moveToEx(false, true, dX, true, dY, false, 0.0, dInE, dE, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveFastXY(const double dX, const double dY, const double dSpeedInMMperSecond)
	{
		moveToEx(true, true, dX, true, dY, false, 0.0, false, 0.0, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveZ(const double dZ, const double dE, const double dSpeedInMMperSecond)
	{
		bool dInE = false;
		if (fabs(dE) > DUETDRIVER_MINSPEED) {
			dInE = true;
		}
		moveToEx(false, false, 0.0, false, 0.0, true, dZ, dInE, dE, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveFastZ(const double dZ, const double dSpeedInMMperSecond)
	{
		moveToEx(true, false, 0.0, false, 0.0, true, dZ, false, 0.0, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::waitForMovement()
	{
		sendCommand("M400");		
		// Print time (M31) responses immediately with ok (+ADVANCED_OK) + print time
		// not added to the planning buffer
		//sendCommand("M31");
	}

	bool CSerialController_Duet::canReceiveMovement()
	{
		return true;
		//return (m_bIsConnected && (m_nCurrentBufferSpace > m_nMinBufferSpace));
	}

	bool CSerialController_Duet::isMoving()
	{
		return false;
	}

	bool CSerialController_Duet::isHomed()
	{
		return (m_bIsConnected && m_bIsHomed);
	}

	void CSerialController_Duet::emergencyStop()
	{
		sendCommand("M112");
	}


	void CSerialController_Duet::powerOff()
	{
		sendCommand("M81");
	}

	void CSerialController_Duet::setAxisPosition(const std::string& sAxis, double dValue)
	{
		sendCommand("G92 " + sAxis + std::to_string(dValue));		
	}

	void CSerialController_Duet::startHoming()
	{

	}


}
