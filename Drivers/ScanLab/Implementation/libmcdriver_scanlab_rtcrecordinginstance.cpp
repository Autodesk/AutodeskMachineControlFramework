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
CRTCRecordingInstance::CRTCRecordingInstance(const std::string& sUUID, PScanLabSDK pSDK, uint32_t cardNo, double dCorrectionFactor, size_t nChunkSize)
	: m_pSDK (pSDK), 
	m_sUUID (sUUID),
	m_CardNo (cardNo),
	m_dCorrectionFactor (dCorrectionFactor),
	m_bScanHeadConnectionCheckEnabled (true),
	m_nChunkSize (nChunkSize)

{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	if ((nChunkSize < RTC_CHUNKSIZE_MIN) || (nChunkSize > RTC_CHUNKSIZE_MAX))
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDCHUNKSIZE, "Invalid chunk size: " + std::to_string (nChunkSize));

	m_AvailableChannelIDs.reserve(RTC_CHANNELCOUNT);
	for (int32_t nChannelID = RTC_CHANNELCOUNT; nChannelID > 0; nChannelID--)
		m_AvailableChannelIDs.push_back(nChannelID);
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

bool CRTCRecordingInstance::getScanheadConnectionCheckIsEnabled()
{
	return m_bScanHeadConnectionCheckEnabled;
}

void CRTCRecordingInstance::setScanheadConnectionCheckIsEnabled(bool bValue)
{
	m_bScanHeadConnectionCheckEnabled = bValue;
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

	auto iIter = m_ChannelMap.find(sNormalizedChannelName);
	if (iIter != m_ChannelMap.end())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DUPLICATECHANNELNAME, "duplicate channel name: " + sNormalizedChannelName);

	if (m_AvailableChannelIDs.empty ())
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_NORECORDINGCHANNELAVAILABLE, "no recording channel is available");

	uint32_t nChannelID = *m_AvailableChannelIDs.rbegin ();
	m_AvailableChannelIDs.pop_back();

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
	if (nCount > m_nChunkSize)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_DATARECORDINGOVERFLOW, "data recording interval overflow");

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

void CRTCRecordingInstance::enableRecording()
{

	std::array<uint32_t, 8> rtcChannels;
	uint32_t nPeriod = 1;
	rtcChannels.at(0) = 1;
	rtcChannels.at(1) = 1;
	rtcChannels.at(2) = 1;
	rtcChannels.at(3) = 1;
	rtcChannels.at(4) = 1;
	rtcChannels.at(5) = 1;
	rtcChannels.at(6) = 1;
	rtcChannels.at(7) = 0;

	m_pSDK->checkGlobalErrorOfCard(m_CardNo);
	//m_pSDK->n_set_trigger8(m_CardNo, nPeriod, rtcChannels[0], rtcChannels[1], rtcChannels[2], rtcChannels[3], rtcChannels[4], rtcChannels[5], rtcChannels[6], rtcChannels[7]);
	m_pSDK->n_set_trigger4(m_CardNo, 1, 7, 8, 52, 0);
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

			if (pChannel.get() > 0) {

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

	uint32_t nMaxMESPosition = (1UL << 23) - 1;

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
			readRecordedDataBlockFromRTC(LastPosition, nMaxMESPosition);
			LastPosition = 0;
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


