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

#include <algorithm>
#include <thread>

#define MARLINDRIVER_MINSPEED 0.0001

namespace AMC {

	CSerialController_Marlin::CSerialController_Marlin(bool bDebug, bool bDoQueryFirmwareInfo, bool bDisableHoming)
		: m_sCOMPort("COM1"),
		m_nBaudRate(115200),
		m_bDoQueryFirmwareInfo(bDoQueryFirmwareInfo),
		m_bDisableHoming(bDisableHoming),
		m_dStatusUpdateTimerInterval(100),
		m_sAckSymbol("ok"),
		m_nLineNumber(1),
		m_bDebug(bDebug),
		m_sResendSymbol("Resend:"),
		m_nMinBufferSpace(3),
		m_nMaxBufferSpace(0),
		m_nCurrentBufferSpace(0),
		m_bIsHomed(false),
		m_iExtruderCount(0),
		m_dCurrentBedTemp(0),
		m_dTargetBedTemp(0),
		m_dTargetPosX(0),
		m_dTargetPosY(0),
		m_dTargetPosZ(0),
		m_dTargetPosE(0),
		m_dCurrentPosX(0),
		m_dCurrentPosY(0),
		m_dCurrentPosZ(0),
		m_dCurrentPosE(0),
		m_dAxisStepsPerUnitX(0),
		m_dAxisStepsPerUnitY(0),
		m_dAxisStepsPerUnitZ(0),
		m_dAxisStepsPerUnitE(0),
		m_dCurrentSpeedInMMperSecond(0),
		m_dPidValueP(0),
		m_dPidValueI(0),
		m_dPidValueD(0)

	{
		// If we disabled homing, then we expect to be at the home position at creation
		if (m_bDisableHoming)
			m_bIsHomed = true;
	}

	void CSerialController_Marlin::setStatusUpdateTimerInterval(const double dStatusUpdateTimerInterval)
	{
		m_dStatusUpdateTimerInterval = dStatusUpdateTimerInterval;
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

		// set timeout >1000 => 3000
		// 
		// if printer is booting (echoing start+ Date+Memory... after switch on printer or plug in cable) and
		// timeout would be 1000 the while loop below we be leaved "to early" (after timeout of 1sec) and
		// the next command is sent, but printer can't respond to this command with OK,
		// because he's not ready to receive commands => boot initialize hags up!!
		//
		// if printer is not booting (no echoing text... after software" reconnect)
		// "start" will no be received,
		// but timeout after 3000 will respond with an empty string and
		// printer is ready to receive commands
		m_pConnection.reset(new serial::Serial(m_sCOMPort, m_nBaudRate, serial::Timeout::simpleTimeout(2000)));

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

		m_nCurrentBufferSpace = 0;

		sendCommand("M400");
		sendCommand("G21"); // set mm units
		sendCommand("M149 C"); // set temperature unit to Celsius
		sendCommand("M113 S1"); // keep alive period 0-60 possible, no decimals!
		setPositioningAbolute();

		if (m_nCurrentBufferSpace > 0) {
			m_nMaxBufferSpace = m_nCurrentBufferSpace;
		}

		queryAxisStepsPerUnitStateAndPidValues();

		m_iExtruderCount = 1;

		if (m_bDoQueryFirmwareInfo)
			queryFirmwareInfo();

		// add an element to list of extruder for every detected extruder
		for (uint32_t i = 0; i < m_iExtruderCount; ++i)
		{
			m_dTargetExtruderTemp.push_back(0);
			m_dCurrentExtruderTemp.push_back(0);
		}

		checkIsHomed();


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



	void CSerialController_Marlin::checkIsHomed()
	{
		if (m_pConnection.get() == nullptr)
			throw std::runtime_error("Serial Port not initialized");

		if (!m_bDisableHoming) {

			// get endstops states
			auto sStream = sendCommand("M119");
			auto sLine = sStream.str();
			std::transform(sLine.begin(), sLine.end(), sLine.begin(), ::tolower);

			auto nPosition = sLine.find("_min: open\n");

			if (nPosition == std::string::npos) {
				// no axis is open => all are TRIGGERED => homed
				m_bIsHomed = true;
			}
			else {
				// at least on end stop of an axis is "open" => not homed
				m_bIsHomed = false;
			}

		}

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
				throw std::runtime_error("line number mismatch in firmware");
			}

			bAck = parseAckSymbol(sLineRead, nLineNumber);
				
		}
		sResultStream << std::endl;
		return sResultStream;
	}



	void CSerialController_Marlin::setHeatedBedTargetTemperature(double nTemperatureInDegreeCelcius)
	{
		sendCommand("M140 S" + std::to_string(nTemperatureInDegreeCelcius));
	}

	void CSerialController_Marlin::setExtruderTargetTemperature(uint32_t nExtruderIndex, double nTemperatureInDegreeCelcius)
	{
		if (nExtruderIndex < m_iExtruderCount)
		{
			sendCommand("M104 T" + std::to_string(nExtruderIndex) + " S" + std::to_string(nTemperatureInDegreeCelcius));
		}
	}

	void CSerialController_Marlin::setPidParameters(double dP, double dI, double dD)
	{
		sendCommand("M301 P" + std::to_string(dP) + " I" + std::to_string(dI) + " D" + std::to_string(dD));

		m_dPidValueP = dP;
		m_dPidValueI = dI;
		m_dPidValueD = dD;

	}

	void CSerialController_Marlin::queryTemperatureState(uint32_t nExtruderIndex)
	{
		if (nExtruderIndex < m_iExtruderCount)
		{
			auto sStream = sendCommand("M105 T" + std::to_string(nExtruderIndex));
			auto sLine = sStream.str();
			auto nPosition = sLine.find("T:");

			if (nPosition == std::string::npos)
				throw std::runtime_error("could not query extruder temperature state.");


			// get extruder temperature
			auto sExtruderTempPosString = sLine.substr(nPosition + 2);

			auto nCurrentExtruderTempPosition = sExtruderTempPosString.find('/');
			auto nTargetExtruderTempPosition = sExtruderTempPosString.find('B');

			if (nCurrentExtruderTempPosition == std::string::npos)
				throw std::runtime_error("could not find extruder current temperature position.");
			if (nTargetExtruderTempPosition == std::string::npos)
				throw std::runtime_error("could not find extruder target temperature position.");

			if (!(nCurrentExtruderTempPosition < nTargetExtruderTempPosition))
				throw std::runtime_error("invalid extruder temperature position order.");

			auto sCurrentExtTemp = sExtruderTempPosString.substr(0, nCurrentExtruderTempPosition);
			auto sTargetExtTemp = sExtruderTempPosString.substr(nCurrentExtruderTempPosition + 1, nTargetExtruderTempPosition - (nCurrentExtruderTempPosition + 1));

			m_dCurrentExtruderTemp[nExtruderIndex] = std::stod(sCurrentExtTemp);
			m_dTargetExtruderTemp[nExtruderIndex] = std::stod(sTargetExtTemp);


			// get bed temperature
			auto sBedTempPosString = sExtruderTempPosString.substr(nTargetExtruderTempPosition + 2);

			auto nCurrentBedTempPosition = sBedTempPosString.find('/');
			auto nTargetBedTempPosition = sBedTempPosString.find('@');

			if (nCurrentBedTempPosition == std::string::npos)
				throw std::runtime_error("could not find bed current temperature position.");
			if (nTargetBedTempPosition == std::string::npos)
				throw std::runtime_error("could not find bed target temperature position.");

			if (!(nCurrentBedTempPosition < nTargetBedTempPosition))
				throw std::runtime_error("invalid bed temperature position order.");

			auto sCurrentBedTemp = sBedTempPosString.substr(0, nCurrentBedTempPosition);
			auto sTargetBedTemp = sBedTempPosString.substr(nCurrentBedTempPosition + 1, nTargetBedTempPosition - (nCurrentBedTempPosition + 1));

			m_dCurrentBedTemp = std::stod(sCurrentBedTemp);
			m_dTargetBedTemp = std::stod(sTargetBedTemp);
		}
	}

	void CSerialController_Marlin::queryAxisStepsPerUnitStateAndPidValues()
	{
		auto sStream = sendCommand("M503 S0");
		auto sLine = sStream.str();

		// get AXIS_STEPS_PER_UNIT
		auto nPosition = sLine.find("M92");

		if (nPosition == std::string::npos)
			throw std::runtime_error("could not query axis_steps_per_unit state.");

		auto sTempString = sLine.substr(nPosition + 4);
		nPosition = sTempString.find("\n");
		auto sAxisStepsPerUnitString = sTempString.substr(0, nPosition);

		auto nAxisStepsPerUnitXPosition = sAxisStepsPerUnitString.find("X");
		auto nAxisStepsPerUnitYPosition = sAxisStepsPerUnitString.find("Y");
		auto nAxisStepsPerUnitZPosition = sAxisStepsPerUnitString.find("Z");
		auto nAxisStepsPerUnitEPosition = sAxisStepsPerUnitString.find("E");

		if (nAxisStepsPerUnitXPosition == std::string::npos)
			throw std::runtime_error("could not find axis_steps_per_unit X position.");
		if (nAxisStepsPerUnitYPosition == std::string::npos)
			throw std::runtime_error("could not find axis_steps_per_unit Y position.");
		if (nAxisStepsPerUnitZPosition == std::string::npos)
			throw std::runtime_error("could not find axis_steps_per_unit Z position.");
		if (nAxisStepsPerUnitEPosition == std::string::npos)
			throw std::runtime_error("could not find axis_steps_per_unit E position.");

		if (!((nAxisStepsPerUnitXPosition < nAxisStepsPerUnitYPosition) && (nAxisStepsPerUnitYPosition < nAxisStepsPerUnitZPosition) &&
			(nAxisStepsPerUnitZPosition < nAxisStepsPerUnitEPosition)))
			throw std::runtime_error("invalid axis_steps_per_unit position order.");

		auto sAxis_steps_per_unitXString = sAxisStepsPerUnitString.substr(nAxisStepsPerUnitXPosition + 1, nAxisStepsPerUnitYPosition - (nAxisStepsPerUnitXPosition + 1));
		auto sAxis_steps_per_unitYString = sAxisStepsPerUnitString.substr(nAxisStepsPerUnitYPosition + 1, nAxisStepsPerUnitZPosition - (nAxisStepsPerUnitYPosition + 1));
		auto sAxis_steps_per_unitZString = sAxisStepsPerUnitString.substr(nAxisStepsPerUnitZPosition + 1, nAxisStepsPerUnitEPosition - (nAxisStepsPerUnitZPosition + 1));
		auto sAxis_steps_per_unitEString = sAxisStepsPerUnitString.substr(nAxisStepsPerUnitEPosition + 1);

		m_dAxisStepsPerUnitX = std::stod(sAxis_steps_per_unitXString);
		m_dAxisStepsPerUnitY = std::stod(sAxis_steps_per_unitYString);
		m_dAxisStepsPerUnitZ = std::stod(sAxis_steps_per_unitZString);
		m_dAxisStepsPerUnitE = std::stod(sAxis_steps_per_unitEString);

		// get PID values
		nPosition = sLine.find("M301");

		if (nPosition == std::string::npos)
			throw std::runtime_error("could not query PID values.");

		sTempString = sLine.substr(nPosition + 5);
		nPosition = sTempString.find("\n");
		auto sPidValuesString = sTempString.substr(0, nPosition);

		auto nPidValuePPosition = sPidValuesString.find("P");
		auto nPidValueIPosition = sPidValuesString.find("I");
		auto nPidValueDPosition = sPidValuesString.find("D");

		if (nPidValuePPosition == std::string::npos)
			throw std::runtime_error("could not find PID value P position.");
		if (nPidValueIPosition == std::string::npos)
			throw std::runtime_error("could not find PID value I position.");
		if (nPidValueDPosition == std::string::npos)
			throw std::runtime_error("could not find PID value D position.");

		if (!((nPidValuePPosition < nPidValueIPosition) && (nPidValueIPosition < nPidValueDPosition)))
			throw std::runtime_error("invalid PID values position order.");

		auto sPidValuePString = sPidValuesString.substr(nPidValuePPosition + 1, nPidValueIPosition - (nPidValuePPosition + 1));
		auto sPidValueIString = sPidValuesString.substr(nPidValueIPosition + 1, nPidValueDPosition - (nPidValueIPosition + 1));
		auto sPidValueDString = sPidValuesString.substr(nPidValueDPosition + 1);

		m_dPidValueP = std::stod(sPidValuePString);
		m_dPidValueI = std::stod(sPidValueIString);
		m_dPidValueD = std::stod(sPidValueDString);

	}

	void CSerialController_Marlin::queryFirmwareInfo()
	{
		auto sStream = sendCommand("M115");
		auto sLine = sStream.str();


		const std::string sFirmwareName = "FIRMWARE_NAME:";
		const std::string sSourceCodeUrl = "SOURCE_CODE_URL:";
		const std::string sProtocolVersion = "PROTOCOL_VERSION:";
		const std::string sMachineType = "MACHINE_TYPE:";
		const std::string sExtruderCount = "EXTRUDER_COUNT:";
		const std::string sUuid = "UUID:";

		auto nPosFirmwareName = sLine.find(sFirmwareName);
		auto nPosSourceCodeUrl = sLine.find(sSourceCodeUrl);
		auto nPosProtocolVersion = sLine.find(sProtocolVersion);
		auto nPosMachineType = sLine.find(sMachineType);
		auto nPosExtruderCount = sLine.find(sExtruderCount);
		auto nPosUuid = sLine.find(sUuid);
		auto nPosEol = sLine.find("\n");

		if (nPosFirmwareName == std::string::npos)
			throw std::runtime_error("could not query FIRMWARE_NAME in firmware info.");
		if (nPosSourceCodeUrl == std::string::npos)
			throw std::runtime_error("could not query SOURCE_CODE_URL in firmware info.");
		if (nPosProtocolVersion == std::string::npos)
			throw std::runtime_error("could not query PROTOCOL_VERSION in firmware info.");
		if (nPosMachineType == std::string::npos)
			throw std::runtime_error("could not query MACHINE_TYPE in firmware info.");
		if (nPosExtruderCount == std::string::npos)
			throw std::runtime_error("could not query EXTRUDER_COUNT in firmware info.");
		if (nPosUuid == std::string::npos)
			throw std::runtime_error("could not query UUID in firmware info.");
		if (nPosEol == std::string::npos)
			throw std::runtime_error("could not query EOL in firmware info.");

		if (!((nPosFirmwareName < nPosSourceCodeUrl) && (nPosSourceCodeUrl < nPosProtocolVersion) &&
			(nPosProtocolVersion < nPosMachineType) && (nPosMachineType < nPosExtruderCount) &&
			(nPosExtruderCount < nPosUuid) && (nPosUuid < nPosEol)))
			throw std::runtime_error("invalid order of firmware info.");

		m_sFirmwareName = sLine.substr(sFirmwareName.length(), nPosSourceCodeUrl - sFirmwareName.length() - 1);
		m_sSourceCodeUrl = sLine.substr(nPosSourceCodeUrl + sSourceCodeUrl.length(), nPosProtocolVersion - (nPosSourceCodeUrl + sSourceCodeUrl.length() + 1));
		m_sProtocolVersion = sLine.substr(nPosProtocolVersion + sProtocolVersion.length(), nPosMachineType - (nPosProtocolVersion + sProtocolVersion.length() + 1));
		m_sMachineType = sLine.substr(nPosMachineType + sMachineType.length(), nPosExtruderCount - (nPosMachineType + sMachineType.length() + 1));
		m_iExtruderCount = std::stoi(sLine.substr(nPosExtruderCount + sExtruderCount.length(), nPosUuid - (nPosExtruderCount + sExtruderCount.length() + 1)));
		m_sUUID = sLine.substr(nPosUuid + sUuid.length(), nPosEol - (nPosUuid + sUuid.length())); 

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

	void CSerialController_Marlin::getHeatedBedTemperature(double& dTargetTemperature, double& dCurrentTemperature)
	{
		dTargetTemperature = m_dTargetBedTemp;
		dCurrentTemperature = m_dCurrentBedTemp;
	}

	void CSerialController_Marlin::getExtruderTemperature(uint32_t nExtruderIndex, double& dTargetTemperature, double& dCurrentTemperature)
	{
		if (nExtruderIndex < m_iExtruderCount) {
			dTargetTemperature = m_dTargetExtruderTemp[nExtruderIndex];
			dCurrentTemperature = m_dCurrentExtruderTemp[nExtruderIndex];
		}
	}


	void CSerialController_Marlin::getPidParameters(double& dP, double& dI, double& dD)
	{
		dP = m_dPidValueP;
		dI = m_dPidValueI;
		dD = m_dPidValueD;
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
		if (!m_bDisableHoming) {
			sendCommand("G28");
		}
		sendCommand("M400");
		checkIsHomed();
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

	void CSerialController_Marlin::moveToEx(bool bFastMove, bool bInX, const double dX, bool bInY, const double dY, bool bInZ, const double dZ, const double dSpeedInMMperSecond)
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
		if (dSpeedInMMperSecond > 0) {
			if (fabs(m_dCurrentSpeedInMMperSecond - dSpeedInMMperSecond) > MARLINDRIVER_MINSPEED) {
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
	}


	void CSerialController_Marlin::moveXY(const double dX, const double dY, const double dSpeedInMMperSecond)
	{
		moveToEx(false, true, dX, true, dY, false, 0.0, dSpeedInMMperSecond);
	}

	void CSerialController_Marlin::moveFastXY(const double dX, const double dY, const double dSpeedInMMperSecond)
	{
		moveToEx(true, true, dX, true, dY, false, 0.0, dSpeedInMMperSecond);
	}

	void CSerialController_Marlin::moveZ(const double dZ, const double dSpeedInMMperSecond)
	{
		moveToEx(false, false, 0.0, false, 0.0, true, dZ, dSpeedInMMperSecond);
	}

	void CSerialController_Marlin::moveFastZ(const double dZ, const double dSpeedInMMperSecond)
	{
		moveToEx(true, false, 0.0, false, 0.0, true, dZ, dSpeedInMMperSecond);
	}

	void CSerialController_Marlin::waitForMovement()
	{
		sendCommand("M400");		
		// Print time (M31) responses immediately with ok (+ADVANCED_OK) + print time
		// not added to the planning buffer
		//sendCommand("M31");
	}

	bool CSerialController_Marlin::canReceiveMovement()
	{
		return (m_nCurrentBufferSpace > m_nMinBufferSpace);
	}

	bool CSerialController_Marlin::isMoving()
	{
		return (m_nCurrentBufferSpace != m_nMaxBufferSpace);
	}

	bool CSerialController_Marlin::isHomed()
	{
		return (m_bIsHomed);
	}


}