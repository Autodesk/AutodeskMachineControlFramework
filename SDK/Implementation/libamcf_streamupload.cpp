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

#include <sstream>
#include "../Implementation/Common/common_utils.hpp"

// Include custom headers here.
#define AMCF_MINUPLOADCHUNKSIZE (64 * 1024)
#define AMCF_MAXUPLOADCHUNKSIZE (64 * 1024 * 1024)

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

	void sendUploadChunk_Blocking(std::string sStreamUUID, uint64_t nStartOffset, std::vector<uint8_t> & ChunkToUpload)
	{
		if (ChunkToUpload.size () > AMCF_MAXUPLOADCHUNKSIZE)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
		if (ChunkToUpload.size() == 0)
			throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);


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


class CAsyncUploadChunkRequest : public CAsyncRequest, public CStreamUploadHandler
{

private:
	std::string m_sStreamUUID;
	uint64_t m_nStartOffset;
	std::vector<uint8_t> m_Data;

public:

	CAsyncUploadChunkRequest(PConnectionState pConnectionState, std::string sStreamUUID, uint64_t nStartOffset, uint64_t nChunkSize, const uint8_t * pData)
		: CAsyncRequest(), CStreamUploadHandler(pConnectionState),
		m_sStreamUUID (sStreamUUID), m_nStartOffset (nStartOffset)
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
		sendUploadChunk_Blocking(m_sStreamUUID, m_nStartOffset, m_Data);

		return std::make_shared<CAsyncSuccess>();
	}

};


CStreamUpload::CStreamUpload(PConnectionState pConnectionState, const std::string& sName, const std::string& sMimeType, const std::string& sUsageContext)
    : m_pConnectionState (pConnectionState), m_sName (sName), m_sMimeType (sMimeType), m_sUsageContext (sUsageContext), m_nTotalUploadSize (0), m_nCurrentUploadSize (0)
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

std::string CStreamUpload::GetUsageContext()
{
    return m_sUsageContext;
}


IOperationResult * CStreamUpload::UploadData(const LibAMCF_uint64 nDataBufferSize, const LibAMCF_uint8 * pDataBuffer, const LibAMCF_uint32 nChunkSize)
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

IOperationResult * CStreamUpload::UploadFile(const std::string & sFileName, const LibAMCF_uint32 nChunkSize)
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

    m_nTotalUploadSize = nDataSize;
	m_nCurrentUploadSize = 0;

	auto pRequest = std::make_shared<CAsyncBeginChunkingRequest>(m_pConnectionState, m_sName, m_nTotalUploadSize, m_sMimeType, m_sUsageContext);
	auto pRequestHandler = m_pConnectionState->getRequestHandler();
	pRequestHandler->executeRequest(pRequest, [this](CAsyncResult* pResult) {

		auto pBeginChunkingData = dynamic_cast<CAsyncBeginChunkData*> (pResult);
		if (pBeginChunkingData != nullptr) {
			m_sStreamUUID = pBeginChunkingData->getStreamUUID();
			m_sContextUUID = pBeginChunkingData->getContextUUID();
		} 
	});

	return new COperationResult(pRequest->getFuture());
}

IOperationResult * CStreamUpload::UploadChunk(const LibAMCF_uint64 nDataBufferSize, const LibAMCF_uint8 * pDataBuffer)
{
    if (nDataBufferSize == 0)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_CANNOTUPLOADEMPTYDATA);
    if (nDataBufferSize > AMCF_MAXUPLOADCHUNKSIZE)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDUPLOADCHUNKSIZE);
    if (pDataBuffer == nullptr)
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDPARAM);

	if (m_nTotalUploadSize == 0)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGNOTCALLED);

	if (m_nCurrentUploadSize + nDataBufferSize > m_nTotalUploadSize)
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_UPLOADDATAEXCEEDSTOTALSIZE);

	if (m_sStreamUUID.empty ())
		throw ELibAMCFInterfaceException(LIBAMCF_ERROR_BEGINCHUNKINGFAILED);


	auto pRequest = std::make_shared<CAsyncUploadChunkRequest>(m_pConnectionState, m_sStreamUUID, m_nCurrentUploadSize, nDataBufferSize, pDataBuffer);
	auto pRequestHandler = m_pConnectionState->getRequestHandler();
	pRequestHandler->executeRequest(pRequest, [this](CAsyncResult* pResult) {

		/*auto pBeginChunkingData = dynamic_cast<CAsyncBeginChunkData*> (pResult);
		if (pBeginChunkingData != nullptr) {
			m_sStreamUUID = pBeginChunkingData->getStreamUUID();
			m_sContextUUID = pBeginChunkingData->getContextUUID();
		} */
	});

	return new COperationResult(pRequest->getFuture());

}

IOperationResult * CStreamUpload::FinishChunking()
{
	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);
}

void CStreamUpload::GetStatus(LibAMCF_uint64 & nUploadSize, LibAMCF_uint64 & nUploadedBytes, bool & bFinished)
{
	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);
}

IDataStream * CStreamUpload::GetDataStream()
{
	throw ELibAMCFInterfaceException(LIBAMCF_ERROR_NOTIMPLEMENTED);
}

