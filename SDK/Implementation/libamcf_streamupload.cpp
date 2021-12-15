/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is a stub class definition of CStreamUpload

*/

#include "libamcf_streamupload.hpp"
#include "libamcf_resthandler.hpp"
#include "libamcf_interfaceexception.hpp"
#include "libamcf_operationresult.hpp"
#include "libamcf_datastream.hpp"

#include <sstream>
#include "../Implementation/Common/common_utils.hpp"

// Include custom headers here.
#define AMCF_MINUPLOADCHUNKSIZE (64 * 1024)
#define AMCF_MAXUPLOADCHUNKSIZE (64 * 1024 * 1024)

#define AMCF_HASHBLOCKSIZE 65536

using namespace LibAMCF::Impl;

/*************************************************************************************************************************
 Class definition of CStreamUpload 
**************************************************************************************************************************/

class CStreamUploadHandler
{
protected:
	std::string m_sBaseURL;
	std::string m_sAuthToken;
	uint32_t m_nTimeout;
	uint32_t m_nRetryCount;

public:

	CStreamUploadHandler(PConnectionState pConnectionState)
	{
		if (pConnectionState.get() == nullptr)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);

		m_sBaseURL = pConnectionState->getBaseURL();
		m_sAuthToken = pConnectionState->getAuthToken();
		m_nTimeout = pConnectionState->getTimeOut();
		m_nRetryCount = pConnectionState->getRetryCount();
	}

	void sendBeginChunking_Blocking(std::string& sStreamUUID, std::string& sContextUUID, const std::string& sName, uint64_t nTotalSize, const std::string& sMimeType, const std::string& sUsageContext)
	{
		CRestHandler_JSONPost sessionRequest("beginchunking", m_sBaseURL + "api/upload", m_sAuthToken, m_nTimeout, m_nRetryCount);
		sessionRequest.addValue("name", sName);
		sessionRequest.addIntegerValue("size", nTotalSize);
		sessionRequest.addValue("mimetype", sMimeType);
		sessionRequest.addValue("context", sUsageContext);
		sessionRequest.sendRequest();

		sStreamUUID = sessionRequest.getResultValue("streamuuid");
		sContextUUID = sessionRequest.getResultValue("contextuuid");

		if (sStreamUUID.empty())
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_COULDNOTBEGINSTREAMUPLOAD);
	}

	void sendUploadChunk_Blocking(std::string sStreamUUID, uint64_t nStartOffset, std::vector<uint8_t> & ChunkToUpload, uint64_t nHashBlockSize, std::map<uint64_t, std::string> & HashMap)
	{
		if (nHashBlockSize == 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);
		if (ChunkToUpload.size () > AMCF_MAXUPLOADCHUNKSIZE)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
		if (ChunkToUpload.size() == 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);

		// Calculate block hashes!
		if ((nStartOffset % nHashBlockSize) != 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CHUNKSTARTMUSTBEAMULTIPLEOFHASHBLOCKSIZE);
		uint64_t nBlockStart = nStartOffset / nHashBlockSize;
		uint64_t nBlockCount = (ChunkToUpload.size() + nHashBlockSize - 1) / nHashBlockSize;

		for (uint64_t nBlockIndex = 0; nBlockIndex < nBlockCount; nBlockIndex++) {

			auto nStartByte = nBlockIndex * nHashBlockSize;
			if (nStartByte >= ChunkToUpload.size())
				throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDHASHBLOCKSIZE);

			uint64_t nByteCount = (ChunkToUpload.size() - nStartByte);
			if (nByteCount > nHashBlockSize)
				nByteCount = nHashBlockSize;

			std::string sBlockHash = AMCCommon::CUtils::calculateSHA256FromData(&ChunkToUpload[nStartByte], nByteCount);
			HashMap.insert(std::make_pair (nBlockStart + nBlockIndex, sBlockHash));
		}
		

		// Assemble MultipartForm Stream
		std::string sBoundary = "-----------------------------------------------------------" + AMCCommon::CUtils::calculateRandomSHA256String(16);;

		std::stringstream MultiPartFormStream;
		MultiPartFormStream << "--" << sBoundary;
		MultiPartFormStream << "\r\nContent-Disposition: form-data; name=\"size\"";
		MultiPartFormStream << "\r\nContent-type: text/plain";
		MultiPartFormStream << "\r\n\r\n" << ChunkToUpload.size() << "\r\n";
		MultiPartFormStream << "--" << sBoundary;
		MultiPartFormStream << "\r\nContent-Disposition: form-data; name=\"offset\"";
		MultiPartFormStream << "\r\nContent-type: text/plain";
		MultiPartFormStream << "\r\n\r\n" << nStartOffset << "\r\n";
		MultiPartFormStream << "--" << sBoundary;
		MultiPartFormStream << "\r\nContent-Disposition: form-data; name=\"data\"; filename=\"upload.3mf\"";
		MultiPartFormStream << "\r\nContent-type: application/3mf";
		MultiPartFormStream << "\r\n\r\n";
		MultiPartFormStream.write((char*)ChunkToUpload.data(), ChunkToUpload.size());
		MultiPartFormStream << "\r\n";
		MultiPartFormStream << "--" << sBoundary << "--\r\n";
		std::string sStreamString = MultiPartFormStream.str();

		CRestHandler_RawPost chunkRequest("uploadchunk", m_sBaseURL + "api/upload/" + sStreamUUID, m_sAuthToken, m_nTimeout, m_nRetryCount);
		chunkRequest.sendRawRequest(sStreamString, "multipart/form-data; boundary=" + sBoundary);

	}

	void sendFinishChunking_Blocking(std::string& sStreamUUID, std::string& sBlockChecksum, std::string & sCalculatedSHA256, std::string& sTimestamp)
	{
		CRestHandler_JSONPost sessionRequest("finishchunking", m_sBaseURL + "api/upload/finish", m_sAuthToken, m_nTimeout, m_nRetryCount);
		sessionRequest.addValue("streamuuid", sStreamUUID);
		sessionRequest.addValue("sha256block64k", sBlockChecksum);
		sessionRequest.sendRequest();

		sCalculatedSHA256 = sessionRequest.getResultValue("sha256");
		sTimestamp = sessionRequest.getResultValue("timestamp");
		
	}
	

};


class CAsyncBeginChunkData : public CAsyncSuccess {
private:
	std::string m_sStreamUUID;
	std::string m_sContextUUID;

public:
	CAsyncBeginChunkData(const std::string& sStreamUUID, const std::string& sContextUUID)
		: m_sStreamUUID(sStreamUUID), m_sContextUUID (sContextUUID)
	{

	}

	std::string getStreamUUID()
	{
		return m_sStreamUUID;
	}

	std::string getContextUUID()
	{
		return m_sContextUUID;
	}
};


class CAsyncBeginChunkingRequest : public CAsyncRequest, public CStreamUploadHandler
{

private:
	std::string m_sName;
	uint64_t m_nTotalSize;
	std::string m_sMimeType;
	std::string m_sUsageContext;

public:

	CAsyncBeginChunkingRequest(PConnectionState pConnectionState, const std::string& sName, uint64_t nTotalSize, const std::string& sMimeType, const std::string& sUsageContext)
		: CAsyncRequest(), CStreamUploadHandler (pConnectionState),
		    m_sName(sName), 
		    m_nTotalSize(nTotalSize), 
		    m_sMimeType(sMimeType), 
		    m_sUsageContext(sUsageContext)

	{
		
	}

	PAsyncResult onExecute() override {

		std::string sStreamUUID, sContextUUID;
		sendBeginChunking_Blocking(sStreamUUID, sContextUUID, m_sName, m_nTotalSize, m_sMimeType, m_sUsageContext);

		return std::make_shared<CAsyncBeginChunkData>(sStreamUUID, sContextUUID);
	}

};

class CAsyncUploadChunkData : public CAsyncSuccess {
private:
	std::map <uint64_t, std::string> m_HashMap;

public:

	CAsyncUploadChunkData()
	{

	}

	void storeBlockChecksums(std::vector<std::string>& HashSums)
	{
		for (auto iIter : m_HashMap) {
			if (iIter.first >= HashSums.size())
				ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDHASHBLOCKINDEX);

			HashSums[iIter.first] = iIter.second;
		}
	}

	std::map <uint64_t, std::string>& getHashMap()
	{
		return m_HashMap;
	}

};


class CAsyncUploadChunkRequest : public CAsyncRequest, public CStreamUploadHandler
{

private:
	std::string m_sStreamUUID;
	uint64_t m_nStartOffset;
	std::vector<uint8_t> m_Data;
	uint64_t m_nHashBlockSize;

public:

	CAsyncUploadChunkRequest(PConnectionState pConnectionState, std::string sStreamUUID, uint64_t nStartOffset, uint64_t nChunkSize, const uint8_t * pData, uint64_t nHashBlockSize)
		: CAsyncRequest(), CStreamUploadHandler(pConnectionState),
		m_sStreamUUID (sStreamUUID), m_nStartOffset (nStartOffset), m_nHashBlockSize (nHashBlockSize)
	{
		if (pData == nullptr)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);
		if (nChunkSize > AMCF_MAXUPLOADCHUNKSIZE)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
		if (nChunkSize == 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);

		m_Data.resize(nChunkSize);
		for (uint64_t nIndex = 0; nIndex < nChunkSize; nIndex++)
			m_Data[nIndex] = pData[nIndex];

	}

	PAsyncResult onExecute() override {
		auto pUploadChunkData = std::make_shared<CAsyncUploadChunkData>();

		sendUploadChunk_Blocking(m_sStreamUUID, m_nStartOffset, m_Data, m_nHashBlockSize, pUploadChunkData->getHashMap ());

		return pUploadChunkData;
	}

};


class CAsyncFinishChunkingData : public CAsyncSuccess {
private:
	std::string m_sCalculatedSHA256;
	std::string m_sTimestamp;

public:
	CAsyncFinishChunkingData(const std::string& sCalculatedSHA256, const std::string & sTimestamp)
		: m_sCalculatedSHA256(sCalculatedSHA256), m_sTimestamp (sTimestamp)
	{

	}

	std::string getSHA256() 
	{
		return m_sCalculatedSHA256;
	}

	std::string getTimestamp()
	{
		return m_sTimestamp;
	}
};


class CAsyncFinishChunkingRequest : public CAsyncRequest, public CStreamUploadHandler
{

private:
	std::string m_sStreamUUID;
	std::string m_sBlockChecksum;

public:

	CAsyncFinishChunkingRequest(PConnectionState pConnectionState, const std::string & sStreamUUID, const std::string & sBlockChecksum)
		: CAsyncRequest(), CStreamUploadHandler(pConnectionState),
		m_sStreamUUID(sStreamUUID), m_sBlockChecksum(sBlockChecksum)
	{
	}

	PAsyncResult onExecute() override {
		std::string sCalculatedSHA256;
		std::string sTimeStamp;
		sendFinishChunking_Blocking(m_sStreamUUID, m_sBlockChecksum, sCalculatedSHA256, sTimeStamp);

		return std::make_shared<CAsyncFinishChunkingData>(sCalculatedSHA256, sTimeStamp);
	}

};

CStreamUpload::CStreamUpload(PConnectionState pConnectionState, const std::string& sName, const std::string& sMimeType, const LibAMCF::eStreamContextType StreamContext)
    : m_pConnectionState (pConnectionState), 
	  m_sName (sName), 
	  m_sMimeType (sMimeType), 
	  m_StreamContext(StreamContext),
	  m_nTotalUploadSize (0), 
	  m_nCurrentUploadSize (0),
	  m_nFinishedBytes (0),
	  m_bUploadFinished (false),
	  m_nHashBlockSize (AMCF_HASHBLOCKSIZE)
{
    if (pConnectionState.get() == nullptr)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);

}

std::string CStreamUpload::GetName()
{
    return m_sName;
}

std::string CStreamUpload::GetMimeType()
{
    return m_sMimeType;
}

LibAMCF::eStreamContextType CStreamUpload::GetContextType() 
{
    return m_StreamContext;
}

IOperationResult* CStreamUpload::UploadData(const LibAMCF_uint64 nDataBufferSize, const LibAMCF_uint8* pDataBuffer, const LibAMCF_uint32 nChunkSize, const LibAMCF_uint32 nThreadCount)
{
	if (nChunkSize < AMCF_MINUPLOADCHUNKSIZE)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
	if (nChunkSize > AMCF_MAXUPLOADCHUNKSIZE)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
	if (nDataBufferSize == 0)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);
	if (pDataBuffer == nullptr)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);

	uint64_t nChunksNeeded = (nDataBufferSize + (nChunkSize - 1)) / nChunkSize;

	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);

}

IOperationResult* CStreamUpload::UploadFile(const std::string& sFileName, const LibAMCF_uint32 nChunkSize, const LibAMCF_uint32 nThreadCount)
{
	if (nChunkSize < AMCF_MINUPLOADCHUNKSIZE)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
	if (nChunkSize > AMCF_MAXUPLOADCHUNKSIZE)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);

	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);

}


IOperationResult* CStreamUpload::BeginChunking(const LibAMCF_uint64 nDataSize)
{
    if (nDataSize == 0)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);
    if (m_nTotalUploadSize > 0)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGALREADYCALLED);

	{
		std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
		m_nTotalUploadSize = nDataSize;
		m_nCurrentUploadSize = 0;
		m_nFinishedBytes = 0;
		m_bUploadFinished = false;

		uint64_t nHashBlockCount = (m_nTotalUploadSize + m_nHashBlockSize - 1) / m_nHashBlockSize;
		m_HashBlockSHA256Sums.resize(nHashBlockCount);
	}

	std::string sStreamContextString;
	switch (m_StreamContext) {
		case eStreamContextType::NewBuildJob:
			sStreamContextString = "build";
			break;

		default:
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDSTREAMCONTEXTTYPE);

	}


	auto pRequest = std::make_shared<CAsyncBeginChunkingRequest>(m_pConnectionState, m_sName, m_nTotalUploadSize, m_sMimeType, sStreamContextString);
	auto pRequestHandler = m_pConnectionState->getRequestHandler();
	pRequestHandler->executeRequest(pRequest, [this](CAsyncResult* pResult) {

		auto pBeginChunkingData = dynamic_cast<CAsyncBeginChunkData*> (pResult);
		if (pBeginChunkingData != nullptr) {
			std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
			m_sStreamUUID = pBeginChunkingData->getStreamUUID();
			m_sContextUUID = pBeginChunkingData->getContextUUID();
		} 
	});

	return new COperationResult(pRequest->getFuture());
}

IOperationResult * CStreamUpload::UploadChunk(const LibAMCF_uint64 nDataBufferSize, const LibAMCF_uint8 * pDataBuffer)
{
	std::string sStreamUUID;
	uint64_t nStartOffset = 0;

    if (nDataBufferSize == 0)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);
    if (nDataBufferSize > AMCF_MAXUPLOADCHUNKSIZE)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
    if (pDataBuffer == nullptr)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);

	{
		std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);

		if (m_nTotalUploadSize == 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGNOTCALLED);
		if (m_sStreamUUID.empty())
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGFAILED);
		if (m_nCurrentUploadSize + nDataBufferSize > m_nTotalUploadSize)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_UPLOADDATAEXCEEDSTOTALSIZE);

		nStartOffset = m_nCurrentUploadSize;

		bool bIsLastChunk = (nStartOffset + nDataBufferSize == m_nTotalUploadSize);
		if ((!bIsLastChunk) && ((nDataBufferSize % m_nHashBlockSize) != 0) )
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CHUNKSIZEMUSTBEAMULTIPLEOFHASHBLOCKSIZE);

		m_nCurrentUploadSize += nDataBufferSize;
		sStreamUUID = m_sStreamUUID;
	}


	auto pRequest = std::make_shared<CAsyncUploadChunkRequest>(m_pConnectionState, sStreamUUID, nStartOffset, nDataBufferSize, pDataBuffer, m_nHashBlockSize);
	auto pRequestHandler = m_pConnectionState->getRequestHandler();
	pRequestHandler->executeRequest(pRequest, [this, nDataBufferSize](CAsyncResult* pResult) {

		auto pUploadChunkData = dynamic_cast<CAsyncUploadChunkData*> (pResult);
		if (pUploadChunkData != nullptr) {
			std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
			m_nFinishedBytes += nDataBufferSize;

			pUploadChunkData->storeBlockChecksums(m_HashBlockSHA256Sums);
		}
	});

	return new COperationResult(pRequest->getFuture());

}

IOperationResult * CStreamUpload::FinishChunking()
{

	std::string sStreamUUID;
	std::string sBlockChecksum;
	{
		std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
		if (m_nTotalUploadSize == 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGNOTCALLED);

		if (m_nCurrentUploadSize != m_nTotalUploadSize)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_DIDNOTUPLOADFULLDATA);

		if (m_bUploadFinished)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_UPLOADISALREADYFINISHED);

		if (m_sStreamUUID.empty())
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGFAILED);

		sStreamUUID = m_sStreamUUID;

		std::stringstream sConcatenatedChecksums;
		for (auto sSubChecksum : m_HashBlockSHA256Sums) {
			if (sSubChecksum.empty ())
				throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CHECKSUMOFBLOCKMISSING);

			sConcatenatedChecksums << sSubChecksum;
		}


		sBlockChecksum = AMCCommon::CUtils::calculateSHA256FromString(sConcatenatedChecksums.str ());
	}


	auto pRequest = std::make_shared<CAsyncFinishChunkingRequest>(m_pConnectionState, sStreamUUID, sBlockChecksum);
	auto pRequestHandler = m_pConnectionState->getRequestHandler();
	pRequestHandler->executeRequest(pRequest, [this](CAsyncResult* pResult) {

		auto pFinishChunkingData = dynamic_cast<CAsyncFinishChunkingData*> (pResult);
		if (pFinishChunkingData != nullptr) {
			std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
			m_bUploadFinished = true;
			m_sCalculatedSHA256 = pFinishChunkingData->getSHA256();
			m_sUploadTimestamp = pFinishChunkingData->getTimestamp();
		}
	});

	return new COperationResult(pRequest->getFuture());



}

void CStreamUpload::GetStatus(LibAMCF_uint64& nUploadSize, LibAMCF_uint64& nFinishedSize, LibAMCF_uint64& nInProgressSize, bool& bFinished) 
{
	std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
	nUploadSize = m_nTotalUploadSize;
	nFinishedSize = m_nFinishedBytes;
	nInProgressSize = m_nCurrentUploadSize;
	bFinished = m_bUploadFinished;

}

IDataStream * CStreamUpload::GetDataStream()
{
	std::lock_guard<std::mutex> lockGuard(m_UploadResultMutex);
	if (!m_bUploadFinished)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_UPLOADDIDNOTFINISH);

	return new CDataStream (m_sStreamUUID, m_StreamContext, m_sContextUUID, m_sName, m_sMimeType, m_sCalculatedSHA256, m_nTotalUploadSize, m_sUploadTimestamp);
}

