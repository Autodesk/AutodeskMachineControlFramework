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
#include <iostream>

#include "RapidJSON/document.h"
#include "RapidJSON/error/error.h"
#include "RapidJSON/error/en.h"

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
		m_bWaitForAck(false),
		m_dTargetPosX(0),
		m_dTargetPosY(0),
		m_dTargetPosZ(0),
		m_dTargetPosA(0),
		m_dTargetPosB(0),
		m_dCurrentPosX(0),
		m_dCurrentPosY(0),
		m_dCurrentPosZ(0),
		m_dCurrentPosA(0),
		m_dCurrentPosB(0),
		m_dCurrentSpeedInMMperSecond(0),
		m_sResendSymbol ("Resend:"),
		m_bDebug(false)
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
		sendCommandBlocking(sCommand.str());

		sendCommandBlocking("M400");
		setPositioningAbsolute();
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


	bool CSerialController_Duet::checkForAck(const int nLineNumber) {
		bool bAck = false;
		while (m_pConnection->available()) {
			std::string sLineRead = m_pConnection->readline();

			if (m_bDebug) {
				std::cout << "RETURN: " << sLineRead << std::endl;
			}

			bool bResend = (sLineRead.find(m_sResendSymbol) != std::string::npos);
			if (bResend) {
				// For now fail, because firmware was not in reset state...
				throw std::runtime_error("line number mismatch in firmware");
			}

			bAck = bAck || parseAckSymbol(sLineRead, nLineNumber);
		}
		return bAck;
	}


	std::stringstream CSerialController_Duet::readResponseBlocking(const int nLineNumber) {
		std::stringstream sResultStream;

		bool bAck = false;
		while (!bAck) {
			std::string sLineRead = m_pConnection->readline();

			if (m_bDebug) {
				std::cout << "RETURN: " << sLineRead << std::endl;
			}

			bool bResend = (sLineRead.find(m_sResendSymbol) != std::string::npos);
			if (bResend) {
				// For now fail, because firmware was not in reset state...
				throw std::runtime_error("line number mismatch in firmware");
			}

			bAck = parseAckSymbol(sLineRead, nLineNumber);

			if (!bAck) {
				// Do not include ok etc. in result
				sResultStream << sLineRead;
			}

		}

		m_bWaitForAck = false;
		return sResultStream;
	}

	int CSerialController_Duet::sendCommand(const std::string& sCommand)
	{
		if (m_bWaitForAck)
			throw std::runtime_error("Cannot send command asynchronously when waiting for an other one");
		if (m_pConnection.get() == nullptr)
			throw std::runtime_error("Serial Port not initialized");

		m_bWaitForAck = true;

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

		return nLineNumber;
	}

	std::stringstream CSerialController_Duet::sendCommandBlocking(const std::string& sCommand)
	{
		if (m_bWaitForAck) {
			readResponseBlocking(m_nLineNumber);
		}

		auto nLineNumber = sendCommand(sCommand);
		return readResponseBlocking(nLineNumber);
	}


	void CSerialController_Duet::queryPrinterState()
	{
		// Response should look like
		// {"status":"I","heaters":[0.0],"active":[0.0],"standby":[0.0],"hstat":[0],"pos":[20.000,0.000,0.000,0.000,0.000],
		//	"machine":[20.000,0.000,0.000,0.000,0.000],"sfactor":100.0,"efactor":[],"babystep":0.000,"tool":-1,"probe":"1000",
		//  "fanPercent":[0.0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1],"fanRPM":[-1,-1],"homed":[0,0,0,0,0],"msgBox.mode":-1}
		std::string sJsonResponse = sendCommandBlocking("M408 S0").str();
		auto cJsonResponse = sJsonResponse.c_str();
		rapidjson::Document jsonResponse;
		rapidjson::ParseResult parseResult = jsonResponse.Parse(cJsonResponse);

		if (!parseResult) {
			throw std::runtime_error("could not parse M408 S0 response json.");
		}
		assert(jsonResponse.IsObject());

		assert(jsonResponse["status"].IsString());
		auto sStatus = jsonResponse["status"].GetString();

		assert(jsonResponse["pos"].IsArray());
		auto targetPositions = jsonResponse["pos"].GetArray();
		assert(targetPositions.Size() == 5);

		assert(jsonResponse["machine"].IsArray());
		auto currentPositions = jsonResponse["machine"].GetArray();
		assert(currentPositions.Size() == 5);

		assert(jsonResponse["homed"].IsArray());
		auto homedFlags = jsonResponse["homed"].GetArray();
		assert(homedFlags.Size() == 5);

		assert(homedFlags[0].IsInt());
		auto xHomed = homedFlags[0].GetInt();
		assert(homedFlags[1].IsInt());
		auto yHomed = homedFlags[1].GetInt();
		assert(homedFlags[3].IsInt());
		auto aHomed = homedFlags[3].GetInt();

		auto homed = xHomed && yHomed && aHomed;

		assert(targetPositions[0].IsDouble());
		m_dTargetPosX = targetPositions[0].GetDouble();
		assert(targetPositions[1].IsDouble());
		m_dTargetPosY = targetPositions[1].GetDouble();
		assert(targetPositions[2].IsDouble());
		m_dTargetPosZ = targetPositions[2].GetDouble();
		assert(targetPositions[3].IsDouble());
		m_dTargetPosA = targetPositions[3].GetDouble();
		assert(targetPositions[4].IsDouble());
		m_dTargetPosB = targetPositions[4].GetDouble();


		assert(currentPositions[0].IsDouble());
		m_dCurrentPosX = currentPositions[0].GetDouble();
		assert(currentPositions[1].IsDouble());
		m_dCurrentPosY = currentPositions[1].GetDouble();
		assert(currentPositions[2].IsDouble());
		m_dCurrentPosZ = currentPositions[2].GetDouble();
		assert(currentPositions[3].IsDouble());
		m_dCurrentPosA = currentPositions[3].GetDouble();
		assert(currentPositions[4].IsDouble());
		m_dCurrentPosB = currentPositions[4].GetDouble();

		// std::cout << "Homed: " << homed << std::endl;
		// std::cout << "Current: [" << m_dCurrentPosX << ", " << m_dCurrentPosY << ", " << m_dCurrentPosZ << ", " << m_dCurrentPosA << ", " << m_dCurrentPosB << "]" << std::endl;
		// std::cout << "Target: [" << m_dTargetPosX << ", " << m_dTargetPosY << ", " << m_dTargetPosZ << ", " << m_dTargetPosA << ", " << m_dTargetPosB << "]" << std::endl;
	}



	void CSerialController_Duet::getTargetPosition(double& dX, double& dY, double& dZ, double& dA, double& dB)
	{
		dX = m_dTargetPosX;
		dY = m_dTargetPosY;
		dZ = m_dTargetPosZ;
		dA = m_dTargetPosA;
		dB = m_dTargetPosB;
	}

	void CSerialController_Duet::getCurrentPosition(double& dX, double& dY, double& dZ, double& dA, double& dB)
	{
		dX = m_dCurrentPosX;
		dY = m_dCurrentPosY;
		dZ = m_dCurrentPosZ;
		dA = m_dCurrentPosA;
		dB = m_dCurrentPosB;
	}

	void CSerialController_Duet::setPositioningAbsolute()
	{
		sendCommandBlocking("G90");
	}

	void CSerialController_Duet::setPositioningRelative()
	{
		sendCommandBlocking("G91");
	}

	void CSerialController_Duet::moveToEx(
		const bool bInX,
		const double dX,
		const bool bInY,
		const double dY,
		const bool bInZ,
		const double dZ,
		const bool bInA,
		const double dA,
		const bool bInB,
		const double dB,
		const double dLaserPower,
		const double dSpeedInMMperSecond)
	{
		std::stringstream sCommand;

		sCommand << "G1";

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
		if (bInA) {
			// A given => add A+value to command str
			sCommand << " A" << dA;
		}
		if (bInB) {
			// B given => add B+value to command str
			sCommand << " B" << dB;
		}

		// Add S+laserpower to command str
		sCommand << " S" << dLaserPower;

		if (dSpeedInMMperSecond > 0) {
			if (fabs(m_dCurrentSpeedInMMperSecond - dSpeedInMMperSecond) > DUETDRIVER_MINSPEED) {
				m_dCurrentSpeedInMMperSecond = dSpeedInMMperSecond;
				sCommand << " F" << (int)(dSpeedInMMperSecond * 60.0);
			}
		}
		sendCommand(sCommand.str());
	}


	void CSerialController_Duet::moveXY(const double dX, const double dY, const double dLaserPower, const double dSpeedInMMperSecond)
	{
		moveToEx(true, dX, true, dY, false, 0, false, 0, false, 0, dLaserPower, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveFastXY(const double dX, const double dY, const double dSpeedInMMperSecond)
	{
		moveToEx(true, dX, true, dY, false, 0, false, 0, false, 0, 0, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveZ(const double dZ, const double dSpeedInMMperSecond)
	{
		moveToEx(false, 0, false, 0, true, dZ, false, 0, false, 0, 0, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveA(const double dA, const double dSpeedInMMperSecond)
	{
		moveToEx(false, 0, false, 0, false, 0, true, dA, false, 0, 0, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::moveB(const double dB, const double dSpeedInMMperSecond)
	{
		moveToEx(false, 0, false, 0, false, 0, false, 0, true, dB, 0, dSpeedInMMperSecond);
	}

	void CSerialController_Duet::waitForMovement()
	{
		sendCommandBlocking("M400");
		// Print time (M31) responses immediately with ok (+ADVANCED_OK) + print time
		// not added to the planning buffer
		//sendCommandBlocking("M31");
	}

	bool CSerialController_Duet::canReceiveMovement()
	{
		if (m_bWaitForAck) {
			m_bWaitForAck = !checkForAck(m_nLineNumber);
		}
		return m_bIsConnected && !m_bWaitForAck;
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
		sendCommandBlocking("M112");
	}


	void CSerialController_Duet::powerOff()
	{
		sendCommandBlocking("M81");
	}

	void CSerialController_Duet::setAxisPosition(const std::string& sAxis, double dValue)
	{
		sendCommandBlocking("G92 " + sAxis + std::to_string(dValue));
	}

	void CSerialController_Duet::startHoming()
	{

	}


}
