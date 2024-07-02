/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CRTCRecording

*/


#ifndef __LIBMCDRIVER_SCANLAB_RTCRECORDINGINSTANCE
#define __LIBMCDRIVER_SCANLAB_RTCRECORDINGINSTANCE

#include "libmcdriver_scanlab_interfaces.hpp"

// Include custom headers here.
#include "libmcdriver_scanlab_sdk.hpp"
#include <map>
#include <array>


#define RTC_CHANNELCOUNT 8
#define RTC_CHUNKSIZE_MIN 1024
#define RTC_CHUNKSIZE_MAX (1024 * 1024)
#define RTC_CHUNKSIZE_DEFAULT 100000


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CRTCRecording 
**************************************************************************************************************************/

class CRTCRecordingChannel;
typedef std::shared_ptr<CRTCRecordingChannel> PRTCRecordingChannel;

class CRTCRecordingInstance;
typedef std::shared_ptr<CRTCRecordingInstance> PRTCRecordingInstance;


class CRTCRecordingChunk
{
private:
	uint64_t m_nStartEntryIndex;
	std::vector<int32_t> m_Buffer;

	uint32_t m_nWriteOffset;

public:
	CRTCRecordingChunk(uint64_t nStartEntryIndex, size_t nChunkSize);

	virtual ~CRTCRecordingChunk();

	size_t getChunkSize();

	uint64_t getStartEntryIndex();

	void clearValues();

	std::vector<int32_t> & getBuffer ();

	int32_t getRecordEntry(uint64_t nAbsoluteEntryIndex);

	bool isFull ();

	int32_t* reserveDataBuffer(uint32_t nCount, uint32_t& nEntriesToRead);


};

typedef std::shared_ptr<CRTCRecordingChunk> PRTCRecordingChunk;


class CRTCRecordingChannel
{
private:
	std::string m_sChannelName;
	uint32_t m_nRTCChannelID;
	LibMCDriver_ScanLab::eRTCChannelType m_ChannelType;

	PRTCRecordingChunk m_pCurrentChunk;

	uint32_t m_nEntryCount;
	size_t m_nChunkSize;
	std::vector<PRTCRecordingChunk> m_Chunks;

public:
	CRTCRecordingChannel(const std::string& sChannelName, uint32_t nRTCChannelID, const LibMCDriver_ScanLab::eRTCChannelType eChannelType, size_t nChunkSize);

	virtual ~CRTCRecordingChannel();

	bool isActive();

	std::string getName();

	uint32_t getRTCChannelID();

	uint32_t getRTCTrigger8Parameter();

	LibMCDriver_ScanLab::eRTCChannelType getChannelType ();

	uint64_t getRecordCount();

	int32_t getRecordEntry(uint64_t nEntryIndex);

	void getAllRecordEntries (uint64_t nValuesBufferSize, uint64_t* pValuesNeededCount, int32_t* pValuesBuffer);

	void getAllScaledRecordEntries(uint64_t nValuesBufferSize, uint64_t* pValuesNeededCount, double* pValuesBuffer, double dScaleFactor, double dOffset);

	int32_t* reserveDataBuffer(uint32_t nCount, uint32_t& nEntriesToRead);
};



class CRTCRecordingInstance {
private:

	uint32_t m_CardNo;
	std::string m_sUUID;
	PScanLabSDK m_pSDK;

	size_t m_nChunkSize;
	double m_dCorrectionFactor;
	bool m_bScanHeadConnectionCheckEnabled;

	std::map<std::string, PRTCRecordingChannel> m_ChannelMap;

	std::vector<uint32_t> m_AvailableChannelIDs;

	std::array<PRTCRecordingChannel, RTC_CHANNELCOUNT> m_Channels;

	void readRecordedDataBlockFromRTC(uint32_t DataStart, uint32_t DataEnd);

	static std::string normalizeChannelName(const std::string& sChannelName);

	CRTCRecordingChannel* findChannel (const std::string & sChannelName, bool bMustExist);

public:

	CRTCRecordingInstance(const std::string & sUUID, PScanLabSDK pSDK, uint32_t cardNo, double dCorrectionFactor, size_t nChunkSize);

	virtual ~CRTCRecordingInstance();
	
	std::string getUUID ();

	bool getScanheadConnectionCheckIsEnabled();

	void setScanheadConnectionCheckIsEnabled(bool bValue);

	void clear();

	PRTCRecordingChannel addChannel(const std::string & sChannelName, const LibMCDriver_ScanLab::eRTCChannelType eChannelType);

	void removeChannel(const std::string & sChannelName);

	bool hasChannel(const std::string & sChannelName);

	LibMCDriver_ScanLab::eRTCChannelType getChannelType(const std::string & sChannelName);

	LibMCDriver_ScanLab_uint64 getRecordCount(const std::string& sChannelName);

	LibMCDriver_ScanLab_int32 getRecordEntry(const std::string & sChannelName, const LibMCDriver_ScanLab_uint64 nRecordIndex);

	void getAllRecordEntries(const std::string & sChannelName, LibMCDriver_ScanLab_uint64 nValuesBufferSize, LibMCDriver_ScanLab_uint64* pValuesNeededCount, LibMCDriver_ScanLab_int32 * pValuesBuffer);

	void enableRecording(uint32_t nPeriod);

	void disableRecording();

	void executeListWithRecording();

	void addRecordsToDataTable(const std::string & sChannelName, LibMCEnv::PDataTable pDataTable, const std::string & sColumnIdentifier, const std::string & sColumnDescription);

	void addScaledRecordsToDataTable(const std::string & sChannelName, LibMCEnv::PDataTable pDataTable, const std::string & sColumnIdentifier, const std::string & sColumnDescription, const LibMCDriver_ScanLab_double dScaleFactor, const LibMCDriver_ScanLab_double dOffset);

};


} // namespace Impl
} // namespace LibMCDriver_ScanLab

#endif // __LIBMCDRIVER_SCANLAB_RTCRECORDINGINSTANCE
