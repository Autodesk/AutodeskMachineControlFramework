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


Abstract: This is a stub class definition of CBuildExecution

*/

#include "libmcenv_buildexecution.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmcenv_build.hpp"
#include "libmcenv_discretefielddata2d.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_streamreader.hpp"
#include "libmcenv_datatable.hpp"
#include "libmcenv_tempstreamwriter.hpp"
#include "libmcenv_journalhandler_historic.hpp"
#include "libmcenv_journalhandler_current.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

using namespace LibMCEnv::Impl;

#define CACHEMEMORYQUOTA_MINMEGABYTES 16
#define CACHEMEMORYQUOTA_MAXMEGABYTES 4096

/*************************************************************************************************************************
 Class definition of CBuildExecution 
**************************************************************************************************************************/
CBuildExecution::CBuildExecution(LibMCData::PBuildJobExecution pExecution, LibMCData::PDataModel pDataModel, AMC::PToolpathHandler pToolpathHandler, AMC::PMeshHandler pMeshHandler, AMCCommon::PChrono pGlobalChrono, AMC::PStateJournal pStateJournal)
	: m_pExecution (pExecution), 
	m_pDataModel (pDataModel), 
	m_pToolpathHandler (pToolpathHandler), 
	m_pMeshHandler (pMeshHandler),
	m_pGlobalChrono (pGlobalChrono),
	m_pStateJournal (pStateJournal)
	
{
	if (pExecution.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pDataModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pToolpathHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pMeshHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pStateJournal.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	// Cache Execution UUID
	m_sExecutionUUID = m_pExecution->GetExecutionUUID();
}

CBuildExecution::~CBuildExecution()
{
	m_pStateJournal = nullptr;
	m_pExecution = nullptr;
	m_pDataModel = nullptr;
	m_pToolpathHandler = nullptr;
}

CBuildExecution* CBuildExecution::makeFrom(CBuildExecution* pBuildExecution)
{
	if (pBuildExecution == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	return new CBuildExecution(pBuildExecution->m_pExecution, pBuildExecution->m_pDataModel, pBuildExecution->m_pToolpathHandler, pBuildExecution->m_pMeshHandler, pBuildExecution->m_pGlobalChrono, pBuildExecution->m_pStateJournal);
}

std::shared_ptr<CBuildExecution> CBuildExecution::makeSharedFrom(CBuildExecution* pBuildExecution)
{
	return std::shared_ptr<CBuildExecution>(makeFrom (pBuildExecution));
}


std::string CBuildExecution::GetUUID()
{
	return m_sExecutionUUID;
}

std::string CBuildExecution::GetBuildUUID()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);

	return m_pExecution->GetJobUUID();
}

IBuild * CBuildExecution::GetBuild()
{
	std::string sBuildUUID = GetBuildUUID();
	return new CBuild(m_pDataModel, sBuildUUID, m_pToolpathHandler, m_pMeshHandler, m_pGlobalChrono, m_pStateJournal);
}

LibMCEnv::eBuildExecutionStatus CBuildExecution::GetExecutionStatus()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	LibMCData::eBuildJobExecutionStatus eStatus = m_pExecution->GetStatus();

	switch (eStatus) {
	case LibMCData::eBuildJobExecutionStatus::InProcess: return LibMCEnv::eBuildExecutionStatus::InProcess;
	case LibMCData::eBuildJobExecutionStatus::Finished: return LibMCEnv::eBuildExecutionStatus::Finished;
	case LibMCData::eBuildJobExecutionStatus::Failed: return LibMCEnv::eBuildExecutionStatus::Failed;
	default:
		return LibMCEnv::eBuildExecutionStatus::Unknown;
	}
}

bool CBuildExecution::IsInProcess()
{
	return (GetExecutionStatus() == LibMCEnv::eBuildExecutionStatus::InProcess);
}

bool CBuildExecution::IsFinished()
{
	return (GetExecutionStatus() == LibMCEnv::eBuildExecutionStatus::Finished);
}

bool CBuildExecution::IsFailed()
{
	return (GetExecutionStatus() == LibMCEnv::eBuildExecutionStatus::Failed);
}

void CBuildExecution::SetStatusToFinished()
{
	if (GetExecutionStatus() != LibMCEnv::eBuildExecutionStatus::InProcess)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_CANNOTCHANGESTATUSOFBUILDEXECUTION, "can not change status of build execution" + m_sExecutionUUID);

	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->ChangeStatus (LibMCData::eBuildJobExecutionStatus::Finished, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());
}

void CBuildExecution::SetStatusToFailed()
{
	if (GetExecutionStatus() != LibMCEnv::eBuildExecutionStatus::InProcess)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_CANNOTCHANGESTATUSOFBUILDEXECUTION, "can not change status of build execution" + m_sExecutionUUID);

	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->ChangeStatus(LibMCData::eBuildJobExecutionStatus::Failed, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());
}

std::string CBuildExecution::GetDescription()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetDescription();
}

void CBuildExecution::SetDescription(const std::string & sDescription)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->SetDescription(sDescription);
}

std::string CBuildExecution::GetJournalUUID()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetJournalUUID();
}

bool CBuildExecution::HasAttachedUser()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	std::string sUserUUID = m_pExecution->GetUserUUID();

	if (sUserUUID.empty())
		return false;

	std::string sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);

	return (sNormalizedUserUUID != AMCCommon::CUtils::createEmptyUUID());
}

std::string CBuildExecution::GetUserUUID()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	std::string sUserUUID = m_pExecution->GetUserUUID();

	if (sUserUUID.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUILDEXECUTIONHASNOATTACHEDUSER, "build execution has no attached user: " + m_sExecutionUUID);

	std::string sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	if (sNormalizedUserUUID == AMCCommon::CUtils::createEmptyUUID())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUILDEXECUTIONHASNOATTACHEDUSER, "build execution has no attached user: " + m_sExecutionUUID);

	return sNormalizedUserUUID;
}

LibMCEnv_uint64 CBuildExecution::GetStartTimeStampInMilliseconds()
{
	return GetStartTimeStampInMicroseconds() / 1000;
}

LibMCEnv_uint64 CBuildExecution::GetStartTimeStampInMicroseconds()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetStartTimeStampInMicroseconds();
}

LibMCEnv_uint64 CBuildExecution::GetEndTimeStampInMilliseconds()
{
	return GetEndTimeStampInMicroseconds() / 1000;
}

LibMCEnv_uint64 CBuildExecution::GetEndTimeStampInMicroseconds()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetEndTimeStampInMicroseconds();
}

LibMCEnv_uint64 CBuildExecution::GetElapsedTimeInMilliseconds()
{
	return GetElapsedTimeInMicroseconds() / 1000;
}

LibMCEnv_uint64 CBuildExecution::GetElapsedTimeInMicroseconds()
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->ComputeElapsedTimeInMicroseconds(m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970(), true);
}

bool CBuildExecution::HasAttachment(const std::string& sDataUUID)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->HasJobExecutionDataUUID(sDataUUID);
}

bool CBuildExecution::HasAttachmentIdentifier(const std::string& sIdentifier)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->HasJobExecutionDataIdentifier(sIdentifier);
}


std::string CBuildExecution::AddBinaryData(const std::string & sIdentifier, const std::string & sName, const std::string & sMIMEType, const std::string& sUserUUID, const LibMCEnv_uint64 nContentBufferSize, const LibMCEnv_uint8 * pContentBuffer)
{
	auto pStorage = m_pDataModel->CreateStorage();

	auto sDataUUID = AMCCommon::CUtils::createUUID();

	std::string sNormalizedUserUUID;
	if (sUserUUID.empty()) {
		sNormalizedUserUUID = AMCCommon::CUtils::createEmptyUUID();
	}
	else {
		sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	}

	pStorage->StoreNewStream(sDataUUID, sName, sMIMEType, LibMCData::CInputVector<uint8_t>(pContentBuffer, nContentBufferSize), sNormalizedUserUUID, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970());

	auto nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

	auto pStorageStream = pStorage->RetrieveStream(sDataUUID);
	m_pExecution->AddJobExecutionData(sIdentifier, sName, pStorageStream, LibMCData::eCustomDataType::CustomBinaryData, sNormalizedUserUUID, nAbsoluteTimeStamp);

	return sDataUUID;
}

std::string CBuildExecution::AttachTempStream(const std::string& sIdentifier, const std::string& sName, const std::string& sUserUUID, IBaseTempStreamWriter* pStreamWriterInstance)
{
	if (pStreamWriterInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (!pStreamWriterInstance->IsFinished ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_STREAMWRITERISNOTFINISHED);

	std::string sStreamUUID = pStreamWriterInstance->GetUUID();

	std::string sNormalizedUUID;
	if (sUserUUID.empty()) {
		sNormalizedUUID = AMCCommon::CUtils::createEmptyUUID();
	}
	else {
		sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	}


	auto nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

	auto pStorage = m_pDataModel->CreateStorage();
	auto pStorageStream = pStorage->RetrieveStream(sStreamUUID);

	return m_pExecution->AddJobExecutionData(sIdentifier, sName, pStorageStream, LibMCData::eCustomDataType::CustomBinaryData, sNormalizedUUID, nAbsoluteTimeStamp);
}

IStreamReader* CBuildExecution::LoadStreamByIdentifier(const std::string& sIdentifier)
{
	auto pJobExecutionData = m_pExecution->RetrieveJobExecutionDataByIdentifier(sIdentifier);
	auto pStorageStream = pJobExecutionData->GetStorageStream();

	auto pStorage = m_pDataModel->CreateStorage();

	return new CStreamReader(pStorage, pStorageStream);
}

IStreamReader* CBuildExecution::LoadStreamByUUID(const std::string& sDataUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sDataUUID);
	auto pJobExecutionData = m_pExecution->RetrieveJobExecutionData(sNormalizedUUID);
	auto pStorageStream = pJobExecutionData->GetStorageStream();

	auto pStorage = m_pDataModel->CreateStorage();

	return new CStreamReader(pStorage, pStorageStream);
}

IDiscreteFieldData2D * CBuildExecution::LoadDiscreteField2DByIdentifier(const std::string & sContextIdentifier)
{
	std::vector<uint8_t> Buffer;

	auto pJobData = m_pExecution->RetrieveJobExecutionDataByIdentifier(sContextIdentifier);
	auto pStorageStream = pJobData->GetStorageStream();
	pStorageStream->GetContent(Buffer);

	auto pFieldData2D = AMC::CDiscreteFieldData2DInstance::createFromBuffer(Buffer);
	return new CDiscreteFieldData2D(pFieldData2D);
}

IDiscreteFieldData2D * CBuildExecution::LoadDiscreteField2DByUUID(const std::string & sDataUUID)
{

	std::vector<uint8_t> Buffer;

	auto pJobData = m_pExecution->RetrieveJobExecutionData(AMCCommon::CUtils::normalizeUUIDString (sDataUUID));
	auto pStorageStream = pJobData->GetStorageStream();
	pStorageStream->GetContent(Buffer);

	auto pFieldData2D = AMC::CDiscreteFieldData2DInstance::createFromBuffer(Buffer);
	return new CDiscreteFieldData2D(pFieldData2D);

}

std::string CBuildExecution::StoreDiscreteField2D(const std::string & sContextIdentifier, const std::string & sName, IDiscreteFieldData2D* pFieldDataInstance, IDiscreteFieldData2DStoreOptions* pStoreOptions, const std::string& sUserUUID)
{

	if (pFieldDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto pFieldDataImplInstance = dynamic_cast<CDiscreteFieldData2D*> (pFieldDataInstance);
	if (pFieldDataImplInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

	auto pFieldData2D = pFieldDataImplInstance->getInstance();

	std::vector<uint8_t> Buffer;
	pFieldData2D->saveToBuffer(Buffer);

	if (Buffer.size() == 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSTOREFIELDDATA);

	return AddBinaryData(sContextIdentifier, sName, "application/amcf-discretefield2d", sUserUUID, Buffer.size(), Buffer.data());

}

IDataTable* CBuildExecution::LoadDataTableByIdentifier(const std::string& sIdentifier)
{
	auto pExecutionData = m_pExecution->RetrieveJobExecutionDataByIdentifier(sIdentifier);
	auto pStorageStream = pExecutionData->GetStorageStream();

	auto pStorage = m_pDataModel->CreateStorage();

	auto pStreamReader = std::make_unique<CStreamReader>(pStorage, pStorageStream);
	return CDataTable::makeFromStream(pStreamReader.get(), m_pToolpathHandler);
}

IDataTable* CBuildExecution::LoadDataTableByUUID(const std::string& sDataUUID)
{
	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sDataUUID);

	auto pExecutionData = m_pExecution->RetrieveJobExecutionData(sNormalizedUUID);
	auto pStorageStream = pExecutionData->GetStorageStream();

	auto pStorage = m_pDataModel->CreateStorage();

	auto pStreamReader = std::make_unique<CStreamReader>(pStorage, pStorageStream);
	return CDataTable::makeFromStream(pStreamReader.get(), m_pToolpathHandler);
}

std::string CBuildExecution::StoreDataTable(const std::string& sIdentifier, const std::string& sName, IDataTable* pFieldDataInstance, IDataTableWriteOptions* pStoreOptions, const std::string& sUserUUID)
{
	if (sName.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYDATATABLENAME);
	if (sIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYDATATABLEIDENTIFIER);
	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDATATABLEIDENTIFIER, "invalid datatable identifier: " + sIdentifier);

	if (pFieldDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);

	std::string sNormalizedUserUUID;
	if (!sUserUUID.empty())
		sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sNormalizedUserUUID);
	else
		sNormalizedUserUUID = AMCCommon::CUtils::createEmptyUUID();

	auto pStreamWriter = std::make_unique<CTempStreamWriter>(m_pDataModel, sName, "application/amcf-datatable", sNormalizedUserUUID, m_pGlobalChrono);
	pFieldDataInstance->WriteDataToStream(pStreamWriter.get(), pStoreOptions);
	pStreamWriter->Finish();

	return AttachTempStream(sIdentifier, sName, sUserUUID, pStreamWriter.get());
}

IImageData* CBuildExecution::LoadPNGImageByIdentifier(const std::string& sContextIdentifier, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat)
{
	std::vector<uint8_t> Buffer;

	auto pJobData = m_pExecution->RetrieveJobExecutionDataByIdentifier(sContextIdentifier);
	auto pStorageStream = pJobData->GetStorageStream();
	pStorageStream->GetContent(Buffer);

	if (Buffer.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGBUFFER);

	return CImageData::createFromPNG(Buffer.data(), Buffer.size(), dDPIValueX, dDPIValueY, ePixelFormat);
}


IImageData* CBuildExecution::LoadPNGImageByUUID(const std::string& sDataUUID, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) 
{
	std::vector<uint8_t> Buffer;

	auto pJobData = m_pExecution->RetrieveJobExecutionData(AMCCommon::CUtils::normalizeUUIDString(sDataUUID));
	auto pStorageStream = pJobData->GetStorageStream();
	pStorageStream->GetContent(Buffer);

	if (Buffer.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGBUFFER);

	return CImageData::createFromPNG(Buffer.data(), Buffer.size(), dDPIValueX, dDPIValueY, ePixelFormat);
}

std::string CBuildExecution::StorePNGImage(const std::string & sContextIdentifier, const std::string & sName, IImageData* pImageDataInstance, IPNGImageStoreOptions* pStoreOptions, const std::string& sUserUUID)
{
	if (pImageDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	uint64_t nNeededCount = 0;
	std::unique_ptr<LibMCEnv::Impl::IPNGImageData> pPNGImage(pImageDataInstance->CreatePNGImage(pStoreOptions));
	pPNGImage->GetPNGDataStream(0, &nNeededCount, nullptr); 

	if (nNeededCount == 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTCOMPRESSPNGIMAGE);

	std::vector<uint8_t> pngBuffer;
	pngBuffer.resize(nNeededCount);

	uint64_t nWrittenCount = 0;
	pPNGImage->GetPNGDataStream(pngBuffer.size (), &nWrittenCount, pngBuffer.data ());

	if (nWrittenCount != pngBuffer.size())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTRETRIEVEPNGSTREAM);

	return AddBinaryData(sContextIdentifier, sName, "image/png", sUserUUID, pngBuffer.size(), pngBuffer.data());

}

void CBuildExecution::StoreMetaDataString(const std::string & sKey, const std::string & sValue)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	m_pExecution->StoreMetaDataString(sKey, sValue, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());
}

bool CBuildExecution::HasMetaDataString(const std::string & sKey)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->HasMetaDataString(sKey);
}

std::string CBuildExecution::GetMetaDataString(const std::string & sKey)
{
	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	return m_pExecution->GetMetaDataString(sKey);
}

IJournalHandler* CBuildExecution::LoadAttachedJournal(const LibMCEnv_uint32 nCacheMemoryQuotaInMegabytes)
{

	std::lock_guard <std::mutex> lockGuard(m_Mutex);
	std::string sJournalUUIDToLoad = AMCCommon::CUtils::normalizeUUIDString (m_pExecution->GetJournalUUID());

	auto pActiveJournalSession = m_pDataModel->CreateJournalSession();
	std::string sActiveJournalUUID = AMCCommon::CUtils::normalizeUUIDString(pActiveJournalSession->GetSessionUUID());


	if (sJournalUUIDToLoad == sActiveJournalUUID) {
		return new CJournalHandler_Current(m_pStateJournal);
	}
	else {

		auto pDataReader = m_pDataModel->CreateJournalReader(sJournalUUIDToLoad);

		if ((nCacheMemoryQuotaInMegabytes < CACHEMEMORYQUOTA_MINMEGABYTES) ||
			(nCacheMemoryQuotaInMegabytes > CACHEMEMORYQUOTA_MAXMEGABYTES))
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMEMORYCACHEQUOTA);

		uint64_t nMemoryQuotaInBytes = ((uint64_t)nCacheMemoryQuotaInMegabytes) * 1024ULL;

		return new CJournalHandler_Historic(std::make_shared<AMC::CStateJournalReader>(pDataReader, nMemoryQuotaInBytes, nullptr));

	}

}
