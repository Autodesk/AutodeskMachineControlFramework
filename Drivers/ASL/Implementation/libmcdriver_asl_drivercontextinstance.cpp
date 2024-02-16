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
#include <chrono>
// Include custom headers here.

#define MAX_TIME_POLL 500

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

	for (uint8_t idx = 0; idx < 4; idx++) {
		m_nImageVerifyDL_INT[idx] = 0;
		m_nImageVerifyRC_INT[idx] = 0;
		m_nImageVerifyLV_INT[idx] = 0;
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

	CDriverContextInstance::VerifyIndex(nIndex);
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

void CDriverContextInstance::SendImage(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_uint32 nPadding, LibMCEnv::PImageData pImageObject)
{

	CDriverContextInstance::VerifyIndex(nIndex);
	uint32_t imageHeight, imageWidth;
	pImageObject->GetSizeInPixels(imageWidth, imageHeight);

	if(imageWidth != 128)
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_IMAGEWIDTHINCORRECT, "incorrect image size sent.");

	if(imageHeight == 0)
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_IMAGEWIDTHINCORRECT, "no length image height.");

	//auto pFormat = pImageObject->GetPixelFormat();
	//LibMCEnv::eImagePixelFormat::Unknown;

	std::vector<uint8_t> imageData;
	imageData.push_back(87); // 87 = W
	imageData.push_back(100 + nIndex); // head index 1-4 so 101 - 104

	uint64_t sumofval = 0;
	int count = 0;
	uint8_t curByte = 0;
	// Pre image white space
	for (int i = 0; i < nPadding; ++i) {
		for (int byt = 0; byt < 16; ++byt) {
			imageData.push_back(0);
			sumofval += 0; // last value is 0 in this case
			count++;
		}
	}

	// Convert image data to binary string, note reversal inside the byte
	for (int i = 0; i < 128; ++i) {
		for (int byt = 0; byt < 16; ++byt) {
			curByte = 0;
			for (int bit = 0; bit < 8; ++bit) {
				int j = byt * 8 + bit;
				if (j < imageHeight && pImageObject->GetPixel(j, i) > 0) {
					curByte |= 1 << (7 - bit);
				}
			}
			imageData.push_back(curByte);
			sumofval += curByte;
			count++;
		}
	}

	auto nBytesWritten = m_pConnection->write(imageData);
	if(nBytesWritten != imageData.size())
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "incorrect data length sent.");

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	m_nImageVerifyDL_INT[nIndex - 1] = count;
	m_nImageVerifyLV_INT[nIndex - 1] = curByte;
	m_nImageVerifyRC_INT[nIndex - 1] = sumofval;

	auto toRead = m_pConnection->available();
	auto msg = m_pConnection->read(toRead);

}

void CDriverContextInstance::Poll()
{

	if (!m_pConnection->isOpen()) {
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_COULDNOTCONNECTTOCOMPORT, "port was not opened before call.");
	}
	//flush buffer

	//m_pConnection->flushInput();

	auto toClear = m_pConnection->available(); 
	m_pConnection->read(toClear);

	m_pConnection->write("b");


	//m_pConnection->waitReadable();

	int nTimeMillisToWait = 500;

	auto toRead = m_pConnection->available();
	while (toRead < 1300) //json should be over 1300
	{
		if (nTimeMillisToWait > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			nTimeMillisToWait--;
			toRead = m_pConnection->available();
		}
		else {
			throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_BOARDPOLLFAILED, "max retries reached for polling.");
		}
	}



	auto msg = m_pConnection->read(toRead);


	m_cLastPollTime = std::chrono::system_clock::now();

	m_nTimeOn = std::stod(CDriverContextInstance::FindAndExtract(msg, "timeOn", ","));
	m_nTimeOn = nTimeMillisToWait;

	for (int idx = 0; idx < 4; idx++) {

		auto sHeadString = CDriverContextInstance::FindAndExtract(msg, "\"head\": " + std::to_string(idx + 1), "\"head\": " + std::to_string(idx + 2));

		m_dStates[idx] = std::stod(CDriverContextInstance::FindAndExtract(sHeadString, "status", ","));
		m_dCurrentTemperatures[idx] = std::stod(CDriverContextInstance::FindAndExtract(sHeadString, "curTemperature", ","));
		m_dSetTemperatures[idx] = std::stod(CDriverContextInstance::FindAndExtract(sHeadString, "setTemperature", ","));
		m_dPrintCounts[idx] = std::stod(CDriverContextInstance::FindAndExtract(sHeadString, "printCounts", ","));

		auto sImageString = CDriverContextInstance::FindAndExtract(msg, "\"image\": " + std::to_string(idx + 1), "\"image\": " + std::to_string(idx + 2));

		m_dImageLength[idx] = std::stod(CDriverContextInstance::FindAndExtract(sImageString, "length", ","));
		m_bHasData[idx] = std::stod(CDriverContextInstance::FindAndExtract(sImageString, "hasData", ",")) == 1;
		m_dProgress[idx] = std::stod(CDriverContextInstance::FindAndExtract(sImageString, "progress", ","));
		m_nImageVerifyLV[idx] = std::stod(CDriverContextInstance::FindAndExtract(sImageString, "image_stored_lv", ","));
		m_nImageVerifyDL[idx] = std::stod(CDriverContextInstance::FindAndExtract(sImageString, "image_stored_dl", ","));
		m_nImageVerifyRC[idx] = std::stod(CDriverContextInstance::FindAndExtract(sImageString, "image_stored_rc", ","));

	}

}

std::string CDriverContextInstance::FindAndExtract(std::string sTotalMessage, std::string sRangeStart, std::string sRangeEnd) {

	auto nStart = sTotalMessage.find(sRangeStart);
	auto nEnd = sTotalMessage.find(sRangeEnd, nStart);

	nStart += sRangeStart.length() + 2;

	return sTotalMessage.substr(nStart, nEnd - nStart);
}

void CDriverContextInstance::SoftPoll() {

	auto now = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - m_cLastPollTime);
	if(elapsed > std::chrono::milliseconds(MAX_TIME_POLL))
		CDriverContextInstance::Poll();
}

void CDriverContextInstance::VerifyIndex(const LibMCDriver_ASL_uint8 nIndex) {

	if (nIndex > 4 | nIndex < 1)
		throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_INCORRECTHEADINDEX);

}

LibMCDriver_ASL_double CDriverContextInstance::GetTemperature(const LibMCDriver_ASL_uint8 nIndex, const bool bSet)
{

	CDriverContextInstance::SoftPoll();
	
	CDriverContextInstance::VerifyIndex(nIndex);

	if(bSet)
		return m_dSetTemperatures[nIndex-1];
	else
		return m_dCurrentTemperatures[nIndex - 1];
}

LibMCDriver_ASL_double CDriverContextInstance::GetPrintCounts(const LibMCDriver_ASL_uint8 nIndex)
{
	CDriverContextInstance::SoftPoll();

	CDriverContextInstance::VerifyIndex(nIndex);
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_double CDriverContextInstance::GetImageLength(const LibMCDriver_ASL_uint8 nIndex)
{
	CDriverContextInstance::SoftPoll();

	CDriverContextInstance::VerifyIndex(nIndex);
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_double CDriverContextInstance::GetHeadState(const LibMCDriver_ASL_uint8 nIndex)
{
	CDriverContextInstance::SoftPoll();

	CDriverContextInstance::VerifyIndex(nIndex);
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

bool CDriverContextInstance::IsHeating(const LibMCDriver_ASL_uint8 nIndex)
{
	CDriverContextInstance::SoftPoll();

	CDriverContextInstance::VerifyIndex(nIndex);
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

bool CDriverContextInstance::GetPower()
{
	CDriverContextInstance::SoftPoll();
	throw ELibMCDriver_ASLInterfaceException(LIBMCDRIVER_ASL_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ASL_uint32 CDriverContextInstance::GetHeadTimeOn()
{
	CDriverContextInstance::SoftPoll();
	return m_nTimeOn;
}

bool CDriverContextInstance::VerifyImages()
{
	CDriverContextInstance::Poll();

	for (uint8_t idx = 0; idx < 4; idx++) {

		if (m_nImageVerifyLV[idx] != m_nImageVerifyLV_INT[idx])
			return false;
		if (m_nImageVerifyRC[idx] != m_nImageVerifyRC_INT[idx])
			return false;
		if (m_nImageVerifyDL[idx] != m_nImageVerifyDL_INT[idx])
			return false;
	}

	return true;
}