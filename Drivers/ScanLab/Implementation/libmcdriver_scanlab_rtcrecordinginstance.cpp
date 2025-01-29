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


Abstract: This is a stub class definition of CRTCRecording

*/

#include "libmcdriver_scanlab_rtcrecordinginstance.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

using namespace LibMCDriver_ScanLab::Impl;

#include <thread>

CRTCRecordingChunk::CRTCRecordingChunk(uint64_t nStartEntryIndex, size_t nChunkSize)
	: m_nStartEntryIndex (nStartEntryIndex), m_nWriteOffset (0)
{
	if (nChunkSize == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHUNKSIZE);

	m_Buffer.resize(nChunkSize);

}

CRTCRecordingChunk::~CRTCRecordingChunk()
{

}

size_t CRTCRecordingChunk::getChunkSize()
{
	return m_Buffer.size ();
}

uint64_t CRTCRecordingChunk::getStartEntryIndex()
{
	return m_nStartEntryIndex;
}

void CRTCRecordingChunk::clearValues()
{
	for (auto& value : m_Buffer)
		value = 0;
}

std::vector<int32_t>& CRTCRecordingChunk::getBuffer()
{
	return m_Buffer;
}

int32_t CRTCRecordingChunk::getRecordEntry(uint64_t nAbsoluteEntryIndex)
{
	if (nAbsoluteEntryIndex < m_nStartEntryIndex)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CHUNKENTRYINDEXOUTOFBOUNDS);

	uint64_t nLocalIndex = nAbsoluteEntryIndex - m_nStartEntryIndex;
	if (nLocalIndex >= m_Buffer.size ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CHUNKENTRYINDEXOUTOFBOUNDS);

	return m_Buffer.at (nAbsoluteEntryIndex);
}


bool CRTCRecordingChunk::isFull()
{
	return (m_nWriteOffset >= m_Buffer.size());
}

int32_t* CRTCRecordingChunk::reserveDataBuffer(uint32_t nCount, uint32_t& nEntriesToRead)
{
	nEntriesToRead = 0;

	if (isFull ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATABUFFERISFULL);

	if (nCount == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATABUFFERREADEMPTY);

	size_t nResultOffset = m_nWriteOffset;

	if ((nResultOffset + nCount) > m_Buffer.size()) {
		nEntriesToRead = (uint32_t) (m_Buffer.size() - nResultOffset);
	}
	else {
		nEntriesToRead = nCount;
	}

	m_nWriteOffset += nEntriesToRead;

	return &m_Buffer.at(nResultOffset);
}


CRTCRecordingChannel::CRTCRecordingChannel(const std::string& sChannelName, uint32_t nRTCChannelID, const LibMCDriver_ScanLab::eRTCChannelType eChannelType, size_t nChunkSize)
	: m_sChannelName (sChannelName), 
	m_nRTCChannelID (nRTCChannelID), 
	m_ChannelType (eChannelType), 
	m_nChunkSize (nChunkSize),
	m_nEntryCount (0)
{
	if (nChunkSize == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHUNKSIZE);
}

CRTCRecordingChannel::~CRTCRecordingChannel()
{

}

bool CRTCRecordingChannel::isActive()
{
	return (m_nRTCChannelID != 0);
}

std::string CRTCRecordingChannel::getName()
{
	return m_sChannelName;
}

uint32_t CRTCRecordingChannel::getRTCChannelID()
{
	return m_nRTCChannelID;
}

LibMCDriver_ScanLab::eRTCChannelType CRTCRecordingChannel::getChannelType()
{
	return m_ChannelType;
}

uint32_t CRTCRecordingChannel::getRTCTrigger8Parameter()
{
	switch (m_ChannelType) {

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelLaserOn:
			return 0; /** Laser On Value (RTC Channel 0, LASERON) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelCurrentXRaw:
			return 1; /** Raw X Value of the Scan Head. Fails if Scanhead feedback is not enabled. (RTC Channel 1, StatusAX) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelCurrentYRaw:
			return 2; /** Raw Y Value of the Scan Head Fails if Scanhead feedback is not enabled. (RTC Channel 2, StatusAY) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelCurrentZRaw: 
			return 4; /** Raw Z Value of the Scan Head Fails if Scanhead feedback is not enabled. (RTC Channel 4, StatusBX) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetX:
			return 7; /** Target X Value of the Scan Head. (RTC Channel 7, SampleX) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetY:
			return 8; /** Target Y Value of the Scan Head. (RTC Channel 8, SampleY) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetZ:
			return 9; /** Target Z Value of the Scan Head. (RTC Channel 9, SampleZ) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetXTransformed:
			return 20; /** Target X Value of the Scan Head (RTC Channel 7, SampleX) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelTargetYTransformed:
			return 21; /** Target Y Value of the Scan Head (RTC Channel 8, SampleY) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelAutoLaserControlMode:
			return 24; /** Control Parameter of AutoLaserControl (RTC Channel 24) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelVectorControlMode: 
			return 31; /** Control Parameter of VectorControl (RTC Channel 31) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelDefocus: 
			return 32; /** Defocus setting (RTC Channel 32) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelAnalogOut1:
			return 33; /** Analog Out 1 Signal (RTC Channel 33) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelAnalogOut2:
			return 34; /** Analog Out 2 Signal (RTC Channel 34) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelDigitalOut16Bit:
			return 35; /** 16 Bit Digital Out Signal (RTC Channel 35) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelDigitalOut8Bit:
			return 36; /** 8 Bit Digital Out Signal (RTC Channel 36) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelPulseLength:
			return 37; /** Pulse length of laser control (RTC Channel 37) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelHalfPeriod:
			return 38; /** Half period of laser control (RTC Channel 38) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable0:
			return 39; /** Free Variable 0 (RTC Channel 39) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable1:
			return 40; /** Free Variable 1 (RTC Channel 40) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable2: 
			return 41; /** Free Variable 2 (RTC Channel 41) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable3:
			return 42; /** Free Variable 3 (RTC Channel 42) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelEncoder0:
			return 43; /** Counter value of Encoder 0 (RTC Channel 43) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelEncoder1:
			return 44; /** Counter value of Encoder 1 (RTC Channel 44) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelMarkSpeed:
			return 45; /** Laser Mark Speed in Bits/ms (RTC Channel 45) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelDigitalIn16Bit:
			return 46; /** 16bit Digital In Channel of EXTENSION 1 (RTC Channel 46) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable4:
			return 48; /** Free Variable 4 (RTC Channel 48) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable5:
			return 49; /** Free Variable 5 (RTC Channel 49) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable6:
			return 50; /** Free Variable 6 (RTC Channel 50) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelFreeVariable7:
			return 51; /** Free Variable 7 (RTC Channel 51) */
		
		case LibMCDriver_ScanLab::eRTCChannelType::ChannelTimeStamp:
			return 52; /** RTC Time Stamp (RTC Channel 52) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelWobbleAmplitude:
			return 53; /** Wobble Amplitude (RTC Channel 53) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelAnalogIn:
			return 54; /** Analog Input Value (RTC Channel 54) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelScaledEncoderX:
			return 55; /** Scaled Encoder Value for X Axis (RTC Channel 55) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelScaledEncoderY: 
			return 56; /** Scaled Encoder Value for Y Axis (RTC Channel 56) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelScaledEncoderZ:
			return 57; /** Scaled Encoder Value for Z Axis (RTC Channel 57) */

		case LibMCDriver_ScanLab::eRTCChannelType::ChannelRS232: 
			return 58; /** RS232 Channel Value (RTC Channel 58) */
						
		default:
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHANNELTYPE, "invalid channel type: " + std::to_string ((uint32_t) m_ChannelType));
	}
}


uint64_t CRTCRecordingChannel::getRecordCount()
{
	return m_nEntryCount;
}

int32_t CRTCRecordingChannel::getRecordEntry(uint64_t nEntryIndex)
{
	if (nEntryIndex >= m_nEntryCount)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDRECORDINDEX);

	uint64_t nChunkIndex = nEntryIndex / m_nChunkSize;
	if (nChunkIndex >= m_Chunks.size ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHUNKINDEX);

	auto pChunk = m_Chunks.at(nChunkIndex);

	return pChunk->getRecordEntry(nEntryIndex);

}

void CRTCRecordingChannel::getAllRecordEntries(uint64_t nValuesBufferSize, uint64_t* pValuesNeededCount, int32_t* pValuesBuffer)
{
	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nEntryCount;

	if (pValuesBuffer != nullptr) {
		if (m_nEntryCount > nValuesBufferSize)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BUFFERTOOSMALL);

		uint64_t nIndex = 0;
		int32_t* pTarget = pValuesBuffer;
		for (auto pChunk : m_Chunks) {
			if (nIndex < m_nEntryCount) {


				auto& buffer = pChunk->getBuffer();
				for (int32_t value : buffer) {
					*pTarget = value;
					nIndex++;
					pTarget++;

					if (nIndex >= m_nEntryCount)
						break;
				}
			} else {
				break;
			}
		}

	}


}

void CRTCRecordingChannel::getAllScaledRecordEntries(uint64_t nValuesBufferSize, uint64_t* pValuesNeededCount, double* pValuesBuffer, double dScaleFactor, double dOffset)
{
	if (pValuesNeededCount != nullptr)
		*pValuesNeededCount = m_nEntryCount;

	if (pValuesBuffer != nullptr) {
		if (m_nEntryCount > nValuesBufferSize)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BUFFERTOOSMALL);

		uint64_t nIndex = 0;
		double* pTarget = pValuesBuffer;
		for (auto pChunk : m_Chunks) {
			auto& buffer = pChunk->getBuffer();
			for (int32_t value : buffer) {
				*pTarget = ((double)value * dScaleFactor) + dOffset;
				nIndex++;
				pTarget++;
			}

		}

	}


}



/*************************************************************************************************************************
 Class definition of CRTCRecording 
**************************************************************************************************************************/
CRTCRecordingInstance::CRTCRecordingInstance(const std::string& sUUID, PScanLabSDK pSDK, uint32_t cardNo, double dXYCorrectionFactor, double dZCorrectionFactor, size_t nChunkSize, bool bEnableScanheadFeedback, bool bEnableBacktransformation)
	: m_pSDK (pSDK), 
	m_sUUID (sUUID),
	m_CardNo (cardNo),
	m_dXYCorrectionFactor (dXYCorrectionFactor),
	m_dZCorrectionFactor(dZCorrectionFactor),
	m_nChunkSize (nChunkSize),
	m_bEnableScanheadFeedback (bEnableScanheadFeedback),
	m_bEnableBacktransformation (bEnableBacktransformation)

{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if ((nChunkSize < RTC_CHUNKSIZE_MIN) || (nChunkSize > RTC_CHUNKSIZE_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHUNKSIZE, "Invalid chunk size: " + std::to_string (nChunkSize));

	m_AvailableChannelIDs.reserve(RTC_CHANNELCOUNT);
	for (int32_t nChannelID = RTC_CHANNELCOUNT; nChannelID > 0; nChannelID--)
		m_AvailableChannelIDs.push_back(nChannelID);

	uint32_t nHeadNoXY = 1;
	uint32_t nHeadNoZ = 2;
	uint32_t nAxisX = 1;
	uint32_t nAxisY = 2;
	uint32_t nAxisZ = 1;

	if (bEnableScanheadFeedback) {

		uint32_t ControlCommand = 0x0501; // activates actual position recording
		//activate porition recording for each axis induvidually, x-axis = 1, y-axis = 2
		m_pSDK->n_control_command(m_CardNo, nHeadNoXY, nAxisX, ControlCommand);
		m_pSDK->checkLastErrorOfCard(m_CardNo);

		m_pSDK->n_control_command(m_CardNo, nHeadNoXY, nAxisY, ControlCommand);
		m_pSDK->checkLastErrorOfCard(m_CardNo);

		m_pSDK->n_control_command(m_CardNo, nHeadNoZ, nAxisZ, ControlCommand);
		m_pSDK->checkLastErrorOfCard(m_CardNo);
		
		m_pSDK->n_control_command(m_CardNo, nHeadNoZ, 2, ControlCommand);
		m_pSDK->checkLastErrorOfCard(m_CardNo);

	}

	if (m_bEnableBacktransformation) {
		m_pSDK->checkGlobalErrorOfCard(m_CardNo);

		m_HeadTransform.resize(528520 * 4);

		m_pSDK->checkError(m_pSDK->n_upload_transform(m_CardNo, 1, m_HeadTransform.data()));

	}
}

CRTCRecordingInstance::~CRTCRecordingInstance()
{

}



std::string CRTCRecordingInstance::normalizeChannelName(const std::string& sChannelName)
{
	std::vector<char> normalizedBuffer;
	normalizedBuffer.reserve(sChannelName.length() + 1);


	for (auto ch : sChannelName) {
		char lowerChar = tolower(ch);
		if (isalnum(lowerChar) || (lowerChar == '-') || (lowerChar == '_')) {
			normalizedBuffer.push_back(lowerChar);
		}
		else {
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHANNELNAME, "invalid channel name: " + sChannelName);
		}
	}

	if (normalizedBuffer.empty ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_EMPTYCHANNELNAME);

	normalizedBuffer.push_back(0);

	return normalizedBuffer.data();
}

std::string CRTCRecordingInstance::getUUID()
{
	return m_sUUID;
}


void CRTCRecordingInstance::clear()
{
	m_ChannelMap.clear();
	for (auto& channel : m_Channels)
		channel = nullptr;
}


PRTCRecordingChannel CRTCRecordingInstance::addChannel(const std::string& sChannelName, const LibMCDriver_ScanLab::eRTCChannelType eChannelType)
{

	std::string sNormalizedChannelName = normalizeChannelName(sChannelName);

	if (!m_bEnableScanheadFeedback) {
		if ((eChannelType == LibMCDriver_ScanLab::eRTCChannelType::ChannelCurrentXRaw) ||
			(eChannelType == LibMCDriver_ScanLab::eRTCChannelType::ChannelCurrentYRaw) ||
			(eChannelType == LibMCDriver_ScanLab::eRTCChannelType::ChannelCurrentZRaw)) {
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_SCANHEADCHANNELNEEDSFEEDBACKENABLED, "scan head channel needs feedback enabled: " + sNormalizedChannelName);
		}
	}

	auto iIter = m_ChannelMap.find(sNormalizedChannelName);
	if (iIter != m_ChannelMap.end())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DUPLICATECHANNELNAME, "duplicate channel name: " + sNormalizedChannelName);

	if (m_AvailableChannelIDs.empty ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NORECORDINGCHANNELAVAILABLE, "no recording channel is available");

	uint32_t nChannelID = *m_AvailableChannelIDs.rbegin ();
	m_AvailableChannelIDs.pop_back();

	for (auto iIter : m_ChannelMap) {
		if (iIter.second->getChannelType () == eChannelType)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CHANNELTYPEISALREADYRECORDED, "channel type is already recorded: " + sNormalizedChannelName);
	}

	if ((nChannelID < 1) || (nChannelID > RTC_CHANNELCOUNT))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHANNELID, "invalid channel ID");

	if (eChannelType == LibMCDriver_ScanLab::eRTCChannelType::ChannelUndefined)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CHANNELTYPECANNOTBEUNDEFINED);

	auto pChannel = std::make_shared<CRTCRecordingChannel>(sChannelName, nChannelID, eChannelType, m_nChunkSize);

	m_Channels.at(nChannelID - 1) = pChannel;
	m_ChannelMap.insert(std::make_pair (sChannelName, pChannel));

	return pChannel;
}


CRTCRecordingChannel* CRTCRecordingInstance::findChannel(const std::string& sChannelName, bool bMustExist)
{
	std::string sNormalizedChannelName = normalizeChannelName(sChannelName);
	auto iIter = m_ChannelMap.find(sNormalizedChannelName);
	if (iIter != m_ChannelMap.end())
		return iIter->second.get();

	if (bMustExist)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_CHANNELNOTFOUND, "Channel not found: " + sNormalizedChannelName);

	return nullptr;
}




int32_t * CRTCRecordingChannel::reserveDataBuffer(uint32_t nCount, uint32_t& nEntriesToRead)
{
	nEntriesToRead = 0;

	if (nCount == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATARECORDINGUNDERFLOW, "data recording interval underflow");
	if (nCount > m_nChunkSize) {
		nCount = m_nChunkSize;		
	}

	if (m_pCurrentChunk.get() != nullptr) {
		if (m_pCurrentChunk->isFull())
			m_pCurrentChunk = nullptr;
	}

	if (m_pCurrentChunk.get() == nullptr) {
		m_pCurrentChunk = std::make_shared<CRTCRecordingChunk> (m_nEntryCount, m_nChunkSize);
		m_Chunks.push_back(m_pCurrentChunk);
	}

	auto pBuffer =  m_pCurrentChunk->reserveDataBuffer(nCount, nEntriesToRead);
	m_nEntryCount += nEntriesToRead;

	return pBuffer;

}

void CRTCRecordingInstance::removeChannel(const std::string& sChannelName)
{
	std::string sNormalizedChannelName = normalizeChannelName(sChannelName);

	auto iIter = m_ChannelMap.find(sNormalizedChannelName);
	if (iIter != m_ChannelMap.end()) {
		auto pChannel = iIter->second;
		m_ChannelMap.erase(iIter);

		uint32_t nChannelID = pChannel->getRTCChannelID();
		if ((nChannelID >= 1) && (nChannelID <= RTC_CHANNELCOUNT)) {
			m_AvailableChannelIDs.push_back (nChannelID);
			m_Channels.at (nChannelID - 1) = nullptr;
		}
	}

}

bool CRTCRecordingInstance::hasChannel(const std::string& sChannelName)
{
	auto pChannel = findChannel(sChannelName, false);
	return (pChannel != nullptr);

}

LibMCDriver_ScanLab::eRTCChannelType CRTCRecordingInstance::getChannelType(const std::string& sChannelName)
{
	auto pChannel = findChannel(sChannelName, false);
	if (pChannel != nullptr)
	{
		return pChannel->getChannelType();
	}
	else
		return LibMCDriver_ScanLab::eRTCChannelType::ChannelUndefined;

}

LibMCDriver_ScanLab_uint64 CRTCRecordingInstance::getRecordCount(const std::string& sChannelName)
{
	auto pChannel = findChannel(sChannelName, true);
	return pChannel->getRecordCount();
}

LibMCDriver_ScanLab_int32 CRTCRecordingInstance::getRecordEntry(const std::string& sChannelName, const LibMCDriver_ScanLab_uint64 nRecordIndex)
{
	auto pChannel = findChannel(sChannelName, true);
	return pChannel->getRecordEntry(nRecordIndex);
}

void CRTCRecordingInstance::getAllRecordEntries(const std::string& sChannelName, LibMCDriver_ScanLab_uint64 nValuesBufferSize, LibMCDriver_ScanLab_uint64* pValuesNeededCount, LibMCDriver_ScanLab_int32* pValuesBuffer)
{
	auto pChannel = findChannel(sChannelName, true);
	pChannel->getAllRecordEntries(nValuesBufferSize, pValuesNeededCount, pValuesBuffer);

}

void CRTCRecordingInstance::enableRecording(uint32_t nPeriod)
{

	std::array<uint32_t, RTC_CHANNELCOUNT> rtcChannels;
	for (uint32_t nIndex = 0; nIndex < RTC_CHANNELCOUNT; nIndex++) {
		auto pChannel = m_Channels.at(nIndex);
		if (pChannel.get() != nullptr)
			rtcChannels.at(nIndex) = pChannel->getRTCTrigger8Parameter();
		else
			rtcChannels.at(nIndex) = 0;
	}

	m_pSDK->checkGlobalErrorOfCard(m_CardNo);
	m_pSDK->n_set_trigger8(m_CardNo, nPeriod | RTC6_BITFLAG_SETTRIGGER_ROUNDTRIP, rtcChannels.at (0), rtcChannels.at (1), rtcChannels.at (2), rtcChannels.at(3), rtcChannels.at(4), rtcChannels.at(5), rtcChannels.at (6), rtcChannels.at(7));
	m_pSDK->checkLastErrorOfCard(m_CardNo);

}

void CRTCRecordingInstance::disableRecording()
{
	m_pSDK->checkGlobalErrorOfCard(m_CardNo);
	
	// add long delay ~ TODO: change timing to proper tracking error or preview time, depending on RTC configuration
	m_pSDK->n_long_delay(m_CardNo, 1200); 
	m_pSDK->n_set_trigger8(m_CardNo, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	m_pSDK->checkLastErrorOfCard(m_CardNo);

}


void CRTCRecordingInstance::readRecordedDataBlockFromRTC(uint32_t DataStart, uint32_t DataEnd)
{
	if (DataEnd < DataStart)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDDATARECORDINGINTERVAL, "invalid data recording interval: " + std::to_string (DataStart) + " - " + std::to_string (DataEnd));

	uint32_t Error = 0;
	uint32_t nDataLength = DataEnd - DataStart;

	//std::cout << "Saving RTC Data Block FROM " << DataStart << " TO " << DataEnd << std::endl;

	if (nDataLength > 0) {
		
		for (auto pChannel : m_Channels) {

			if (pChannel.get() != nullptr) {

				uint32_t nEntriesLeft = nDataLength;
				uint32_t nDataAddress = DataStart;
				while (nEntriesLeft > 0) {
					uint32_t nEntriesToRead = 0;

					int32_t* pBuffer = pChannel->reserveDataBuffer(nEntriesLeft, nEntriesToRead);
					if (nEntriesToRead > nEntriesLeft)
						throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATARECORDINGOVERFLOW, "data recording interval overflow: " + std::to_string(nEntriesToRead) + " of " + std::to_string(nEntriesLeft) + " bytes");
					if ((nEntriesToRead == 0) || (pBuffer == nullptr))
						throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATARECORDINGUNDERFLOW, "data recording interval underflow: " + std::to_string(nEntriesToRead) + " of " + std::to_string(nEntriesLeft) + " bytes");

					m_pSDK->n_get_waveform_offset(m_CardNo, pChannel->getRTCChannelID(), nDataAddress, nEntriesToRead, pBuffer);
					m_pSDK->checkLastErrorOfCard(m_CardNo);

					nDataAddress += nEntriesToRead;
					nEntriesLeft -= nEntriesToRead;

				}
			}

		}
			
	}

}


void CRTCRecordingInstance::executeListWithRecording()
{
	m_pSDK->n_execute_list_pos(m_CardNo, 1, 0);
	m_pSDK->checkError(m_pSDK->n_get_last_error(m_CardNo));

	uint32_t Busy, Position, MesBusy, MesPosition;
	uint32_t LastPosition = 0;
	uint32_t Increment = 100000;

	uint32_t nMaxMESPosition = (1UL << 22) - 1;

	do // Wait for measurement to start
	{
		m_pSDK->n_measurement_status(m_CardNo, &MesBusy, &MesPosition);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while ((MesBusy == 0) && (MesPosition == (uint32_t)-1));

	do   //blockwise data polling
	{

		m_pSDK->n_get_status(m_CardNo, &Busy, &Position);
		m_pSDK->n_measurement_status(m_CardNo, &MesBusy, &MesPosition);
		//std::cout << "RTC Status busy: " << Busy << " Position: " << Position << " Measure Busy: " << MesBusy << " Measure Position: " << MesPosition << std::endl;

		if (MesPosition > LastPosition + Increment)
		{
			readRecordedDataBlockFromRTC(LastPosition, LastPosition + Increment);
			LastPosition += Increment;
		}
		else if (MesPosition < LastPosition)
		{
			//std::cout << "Roundtrip reached: MesPosition: " << MesPosition << " / LastPosition: " << LastPosition << std::endl;

			if (MesPosition > nMaxMESPosition)
				throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATARECORDINGOVERFLOW, "data recording interval overflow: MesPosition exceeds Max MES Position");

			uint32_t nBytesToRead = nMaxMESPosition - LastPosition;
			if (nBytesToRead > Increment) {
				readRecordedDataBlockFromRTC(LastPosition, LastPosition + Increment);
				LastPosition += Increment;
			}
			else {
				readRecordedDataBlockFromRTC(LastPosition, nMaxMESPosition);
				LastPosition = 0;

			}

		}
		else {
			if (!MesBusy) {
				readRecordedDataBlockFromRTC(LastPosition, MesPosition);
				LastPosition = MesPosition;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		m_pSDK->n_measurement_status(m_CardNo, &MesBusy, &MesPosition);

	} while (MesBusy || (MesPosition != LastPosition));           // Wait for the job to be finished executing

}

void CRTCRecordingInstance::addRecordsToDataTable(const std::string& sChannelName, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get () == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	auto pChannel = findChannel(sChannelName, true);
	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::Int32Column);

	uint64_t nRecordCount = pChannel->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<int32_t> buffer;
		uint64_t nNeededEntries = 0;

		buffer.resize(nRecordCount);
		pChannel->getAllRecordEntries(nRecordCount, &nNeededEntries, buffer.data());

		pDataTable->SetInt32ColumnValues(sColumnIdentifier, buffer);
	}


}

void CRTCRecordingInstance::addScaledRecordsToDataTable(const std::string& sChannelName, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription, const LibMCDriver_ScanLab_double dScaleFactor, const LibMCDriver_ScanLab_double dOffset)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	auto pChannel = findChannel(sChannelName, true);
	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::DoubleColumn);

	uint64_t nRecordCount = pChannel->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<double> buffer;
		uint64_t nNeededEntries = 0;

		buffer.resize(nRecordCount);
		pChannel->getAllScaledRecordEntries(nRecordCount, &nNeededEntries, buffer.data(), dScaleFactor, dOffset);

		pDataTable->SetDoubleColumnValues(sColumnIdentifier, buffer);
	}

}

void CRTCRecordingInstance::addBacktransformedXYPositionsToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifierX, const std::string& sColumnDescriptionX, const std::string& sColumnIdentifierY, const std::string& sColumnDescriptionY)
{
	if (!m_bEnableBacktransformation)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);
	if (m_HeadTransform.size () == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);

	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	PRTCRecordingChannel pChannelX = nullptr;
	PRTCRecordingChannel pChannelY = nullptr;

	for (auto iIter : m_ChannelMap) {
		auto channelType = iIter.second->getChannelType();
		if (channelType == eRTCChannelType::ChannelCurrentXRaw)
			pChannelX = iIter.second;
		if (channelType == eRTCChannelType::ChannelCurrentYRaw)
			pChannelY = iIter.second;
	}

	if (pChannelX.get () == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELXNOTRECORDED);
	if (pChannelY.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELYNOTRECORDED);

	pDataTable->AddColumn(sColumnIdentifierX, sColumnDescriptionX, LibMCEnv::eDataTableColumnType::DoubleColumn);
	pDataTable->AddColumn(sColumnIdentifierY, sColumnDescriptionY, LibMCEnv::eDataTableColumnType::DoubleColumn);

	uint64_t nRecordCount = pChannelX->getRecordCount();
	if (nRecordCount != pChannelY->getRecordCount())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELXANDYRECORDCOUNTMISMATCH);

	if (nRecordCount > 0) {
		std::vector<int32_t> bufferX;
		std::vector<int32_t> bufferY;
		uint64_t nNeededEntries = 0;

		bufferX.resize(nRecordCount);
		pChannelX->getAllRecordEntries(nRecordCount, &nNeededEntries, bufferX.data());
		
		bufferY.resize(nRecordCount);
		pChannelY->getAllRecordEntries(nRecordCount, &nNeededEntries, bufferY.data());

		std::vector<double> backtransformedX;
		std::vector<double> backtransformedY;

		backtransformedX.resize(nRecordCount);
		backtransformedY.resize(nRecordCount);

		for (uint64_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
			int32_t nX = bufferX.at(nIndex);
			int32_t nY = bufferY.at(nIndex);
			m_pSDK->checkError(m_pSDK->transform(&nX, &nY, m_HeadTransform.data(), 0));

			backtransformedX.at(nIndex) = (double)nX / m_dXYCorrectionFactor;
			backtransformedY.at(nIndex) = (double)nY / m_dXYCorrectionFactor;
		}

		pDataTable->SetDoubleColumnValues(sColumnIdentifierX, backtransformedX);
		pDataTable->SetDoubleColumnValues(sColumnIdentifierY, backtransformedY);
	}


}

void CRTCRecordingInstance::backtransformRawXYCoordinates(const int32_t nRawCoordinateX, const int32_t nRawCoordinateY, double& dBacktransformedX, double& dBacktransformedY)
{
	if (!m_bEnableBacktransformation)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);
	if (m_HeadTransform.size() == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);

	// Apply BackTransformation XY
	int32_t nCoordinateX = nRawCoordinateX;
	int32_t nCoordinateY = nRawCoordinateY;
	m_pSDK->checkError(m_pSDK->transform(&nCoordinateX, &nCoordinateY, m_HeadTransform.data(), 0));

	dBacktransformedX = (double)nCoordinateX / m_dXYCorrectionFactor;
	dBacktransformedY = (double)nCoordinateY / m_dXYCorrectionFactor;

}

void CRTCRecordingInstance::addBacktransformedZPositionToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifierZ, const std::string& sColumnDescriptionZ)
{
	if (!m_bEnableBacktransformation)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);
	if (m_HeadTransform.size() == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);

	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	PRTCRecordingChannel pChannelZ = nullptr;

	for (auto iIter : m_ChannelMap) {
		auto channelType = iIter.second->getChannelType();
		if (channelType == eRTCChannelType::ChannelCurrentZRaw)
			pChannelZ = iIter.second;
	}

	if (pChannelZ.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELZNOTRECORDED);

	pDataTable->AddColumn(sColumnIdentifierZ, sColumnDescriptionZ, LibMCEnv::eDataTableColumnType::DoubleColumn);

	uint64_t nRecordCount = pChannelZ->getRecordCount();

	if (nRecordCount > 0) {
		std::vector<int32_t> bufferZ;
		uint64_t nNeededEntries = 0;

		bufferZ.resize(nRecordCount);
		pChannelZ->getAllRecordEntries(nRecordCount, &nNeededEntries, bufferZ.data());

		std::vector<double> backtransformedZ;

		backtransformedZ.resize(nRecordCount);

		for (uint64_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
			int32_t nZ = bufferZ.at(nIndex);
			int32_t nDummy = 0;
			m_pSDK->checkError(m_pSDK->transform(&nZ, &nDummy, m_HeadTransform.data(), 1));

			backtransformedZ.at(nIndex) = (double)nZ / m_dZCorrectionFactor;
		}

		pDataTable->SetDoubleColumnValues(sColumnIdentifierZ, backtransformedZ);
	}


}

double CRTCRecordingInstance::backtransformRawZCoordinate(const int32_t nRawCoordinateZ)
{
	if (!m_bEnableBacktransformation)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);
	if (m_HeadTransform.size() == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_BACKTRANSFORMATIONISNOTENABLED);

	// Apply BackTransformation Z
	int32_t nCoordinateZ = nRawCoordinateZ;
	int32_t nDummy = 0;
	m_pSDK->checkError(m_pSDK->transform(&nCoordinateZ, &nDummy, m_HeadTransform.data(), 1));

	return (double)nCoordinateZ / m_dZCorrectionFactor;

}

void CRTCRecordingInstance::addTargetPositionsToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifierX, const std::string& sColumnDescriptionX, const std::string& sColumnIdentifierY, const std::string& sColumnDescriptionY)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	PRTCRecordingChannel pChannelX = nullptr;
	PRTCRecordingChannel pChannelY = nullptr;

	for (auto iIter : m_ChannelMap) {
		auto channelType = iIter.second->getChannelType();
		if (channelType == eRTCChannelType::ChannelTargetX)
			pChannelX = iIter.second;
		if (channelType == eRTCChannelType::ChannelTargetY)
			pChannelY = iIter.second;
	}

	if (pChannelX.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELXNOTRECORDED);
	if (pChannelY.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELYNOTRECORDED);

	pDataTable->AddColumn(sColumnIdentifierX, sColumnDescriptionX, LibMCEnv::eDataTableColumnType::DoubleColumn);
	pDataTable->AddColumn(sColumnIdentifierY, sColumnDescriptionY, LibMCEnv::eDataTableColumnType::DoubleColumn);

	uint64_t nRecordCount = pChannelX->getRecordCount();
	if (nRecordCount != pChannelY->getRecordCount())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_RTCCHANNELXANDYRECORDCOUNTMISMATCH);

	if (nRecordCount > 0) {
		std::vector<int32_t> bufferX;
		std::vector<int32_t> bufferY;
		uint64_t nNeededEntries = 0;

		bufferX.resize(nRecordCount);
		pChannelX->getAllRecordEntries(nRecordCount, &nNeededEntries, bufferX.data());

		bufferY.resize(nRecordCount);
		pChannelY->getAllRecordEntries(nRecordCount, &nNeededEntries, bufferY.data());

		std::vector<double> coordXinmm;
		std::vector<double> coordYinmm;

		coordXinmm.resize(nRecordCount);
		coordYinmm.resize(nRecordCount);

		for (uint64_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
			int32_t nX = bufferX.at(nIndex);
			int32_t nY = bufferY.at(nIndex);

			coordXinmm.at(nIndex) = (double)nX / m_dXYCorrectionFactor;
			coordYinmm.at(nIndex) = (double)nY / m_dXYCorrectionFactor;
		}

		pDataTable->SetDoubleColumnValues(sColumnIdentifierX, coordXinmm);
		pDataTable->SetDoubleColumnValues(sColumnIdentifierY, coordYinmm);
	}

}


