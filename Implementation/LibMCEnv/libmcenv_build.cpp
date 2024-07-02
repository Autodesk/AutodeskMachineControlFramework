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


Abstract: This is a stub class definition of CBuild

*/

#include "libmcenv_build.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_toolpathaccessor.hpp"
#include "libmcenv_discretefielddata2d.hpp"
#include "libmcenv_buildexecution.hpp"
#include "libmcenv_buildexecutioniterator.hpp"
#include "libmcenv_imagedata.hpp"
#include "libmcenv_streamreader.hpp"
#include "libmcenv_tempstreamwriter.hpp"
#include "libmcenv_datatable.hpp"

// Include custom headers here.
#include "amc_systemstate.hpp"
#include "amc_toolpathhandler.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"

#include <iostream>

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CBuild 
**************************************************************************************************************************/

CBuild::CBuild(LibMCData::PDataModel pDataModel, const std::string& sBuildJobUUID, AMC::PToolpathHandler pToolpathHandler, AMCCommon::PChrono pGlobalChrono)
	: m_pDataModel(pDataModel),
	m_sBuildJobUUID(AMCCommon::CUtils::normalizeUUIDString(sBuildJobUUID)),
	m_pToolpathHandler(pToolpathHandler),
	m_pGlobalChrono (pGlobalChrono)
{
	if (pToolpathHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pDataModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pGlobalChrono.get () == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CBuild::~CBuild()
{
}

std::string CBuild::GetName()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->GetName();
}

std::string CBuild::GetBuildUUID()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->GetUUID();
}

std::string CBuild::GetStorageUUID()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->GetStorageStreamUUID();
}

std::string CBuild::GetStorageSHA256()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->GetStorageStream()->GetSHA2 ();

}


LibMCEnv_double CBuild::GetBuildHeightInMM()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(pBuildJob->GetStorageStreamUUID(), false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0.0;

	return pToolpathEntity->getLayerZInUnits(nLayerCount - 1) * pToolpathEntity->getUnits();
}

LibMCEnv_double CBuild::GetZValueInMM(const LibMCEnv_uint32 nLayerIndex)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(pBuildJob->GetStorageStreamUUID(), false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0.0;

	if (nLayerIndex >= nLayerCount)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDLAYERINDEX);

	return pToolpathEntity->getLayerZInUnits(nLayerIndex) * pToolpathEntity->getUnits();
}


LibMCEnv_uint32 CBuild::GetLayerCount()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	return pBuildJob->GetLayerCount();
}

IToolpathAccessor * CBuild::CreateToolpathAccessor()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto sStreamUUID = pBuildJob->GetStorageStreamUUID();
	return new CToolpathAccessor(sStreamUUID, pBuildJob->GetUUID(), m_pToolpathHandler);
}


void CBuild::LoadToolpath()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto sStreamUUID = pBuildJob->GetStorageStreamUUID();
	m_pToolpathHandler->loadToolpathEntity(sStreamUUID);
}


void CBuild::UnloadToolpath()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto sStreamUUID = pBuildJob->GetStorageStreamUUID();
	m_pToolpathHandler->unloadToolpathEntity(sStreamUUID);
}

bool CBuild::ToolpathIsLoaded()
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto sStreamUUID = pBuildJob->GetStorageStreamUUID();
	return (m_pToolpathHandler->findToolpathEntity(sStreamUUID, false) != nullptr);
}

bool CBuild::HasAttachment(const std::string& sDataUUID)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->HasJobDataUUID(sDataUUID);
}

bool CBuild::HasAttachmentIdentifier(const std::string& sIdentifier)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->HasJobDataIdentifier(sIdentifier);
}



std::string CBuild::AddBinaryData(const std::string& sIdentifier, const std::string& sName, const std::string& sMIMEType, const std::string& sUserUUID, const LibMCEnv_uint64 nContentBufferSize, const LibMCEnv_uint8* pContentBuffer)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	auto pStorage = m_pDataModel->CreateStorage();

	std::string sNormalizedUserUUID;
	if (sUserUUID.empty()) {
		sNormalizedUserUUID = AMCCommon::CUtils::createEmptyUUID();
	}
	else {
		sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	}
	

	auto sDataUUID = AMCCommon::CUtils::createUUID();
	uint64_t nTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

	pStorage->StoreNewStream(sDataUUID, sName, sMIMEType, LibMCData::CInputVector<uint8_t>(pContentBuffer, nContentBufferSize), sNormalizedUserUUID, nTimeStamp);

	auto pStorageStream = pStorage->RetrieveStream(sDataUUID);
	pBuildJob->AddJobData(sIdentifier, sName, pStorageStream, LibMCData::eCustomDataType::CustomBinaryData, sNormalizedUserUUID, nTimeStamp);

	return sDataUUID;

}

std::string CBuild::AttachTempStream(const std::string& sIdentifier, const std::string& sName, const std::string& sUserUUID, IBaseTempStreamWriter* pStreamWriterInstance)
{
	if (pStreamWriterInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (!pStreamWriterInstance->IsFinished())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_STREAMWRITERISNOTFINISHED);

	std::string sStreamUUID = pStreamWriterInstance->GetUUID();

	std::string sNormalizedUserUUID;
	if (sUserUUID.empty()) {
		sNormalizedUserUUID = AMCCommon::CUtils::createEmptyUUID();
	}
	else {
		sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	}

	auto nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

	auto pStorage = m_pDataModel->CreateStorage();
	auto pStorageStream = pStorage->RetrieveStream(sStreamUUID);

	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
	return pBuildJob->AddJobData(sIdentifier, sName, pStorageStream, LibMCData::eCustomDataType::CustomBinaryData, sNormalizedUserUUID, nAbsoluteTimeStamp);
}

IStreamReader* CBuild::LoadStreamByIdentifier(const std::string& sIdentifier)
{

	auto pStorage = m_pDataModel->CreateStorage();
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto pJobData = pBuildJob->RetrieveJobDataByIdentifier(sIdentifier);
	auto pStorageStream = pJobData->GetStorageStream();

	return new CStreamReader(pStorage, pStorageStream);
}

IStreamReader* CBuild::LoadStreamByUUID(const std::string& sDataUUID)
{
	auto pStorage = m_pDataModel->CreateStorage();
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sDataUUID);
	auto pJobData = pBuildJob->RetrieveJobData(sNormalizedUUID);
	auto pStorageStream = pJobData->GetStorageStream();

	return new CStreamReader(pStorage, pStorageStream);
}

IDataTable* CBuild::LoadDataTableByIdentifier(const std::string& sIdentifier)
{
	
	auto pStorage = m_pDataModel->CreateStorage();
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto pJobData = pBuildJob->RetrieveJobDataByIdentifier(sIdentifier);
	auto pStorageStream = pJobData->GetStorageStream();

	auto pStreamReader = std::make_unique<CStreamReader>(pStorage, pStorageStream);
	return CDataTable::makeFromStream(pStreamReader.get());
}

IDataTable* CBuild::LoadDataTableByUUID(const std::string& sDataUUID)
{
	auto pStorage = m_pDataModel->CreateStorage();
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sDataUUID);

	auto pJobData = pBuildJob->RetrieveJobData(sNormalizedUUID);
	auto pStorageStream = pJobData->GetStorageStream();

	auto pStreamReader = std::make_unique<CStreamReader> (pStorage, pStorageStream);
	return CDataTable::makeFromStream (pStreamReader.get ());
}

std::string CBuild::StoreDataTable(const std::string& sIdentifier, const std::string& sName, IDataTable* pFieldDataInstance, IDataTableWriteOptions* pStoreOptions, const std::string& sUserUUID)
{
	if (sName.empty ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYDATATABLENAME);
	if (sIdentifier.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYDATATABLEIDENTIFIER);
	if (AMCCommon::CUtils::stringIsValidAlphanumericNameString(sIdentifier))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDDATATABLEIDENTIFIER, "invalid datatable identifier: " + sIdentifier);

	if (pFieldDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);

	std::string sNormalizedUserUUID;
	if (!sUserUUID.empty())
		sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sNormalizedUserUUID);
	else
		sNormalizedUserUUID = AMCCommon::CUtils::createEmptyUUID();

	auto pStreamWriter = std::make_unique<CTempStreamWriter> (m_pDataModel, sName, "application/amcf-datatable", sNormalizedUserUUID, m_pGlobalChrono);
	pFieldDataInstance->WriteDataToStream(pStreamWriter.get(), pStoreOptions);
	pStreamWriter->Finish();

	return AttachTempStream(sIdentifier, sName, sUserUUID, pStreamWriter.get());
}


IDiscreteFieldData2D* CBuild::LoadDiscreteField2DByIdentifier(const std::string& sContextIdentifier)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::string sFoundUUID;

	auto pJobDataIterator = pBuildJob->ListJobData();
	while (pJobDataIterator->MoveNext()) {
		auto pJobData = pJobDataIterator->GetCurrentJobData();
		std::string sJobDataIdentifier = pJobData->GetIdentifier();
		if (sJobDataIdentifier == sContextIdentifier)
			sFoundUUID = pJobData->GetDataUUID();
	}
	
	if (sFoundUUID.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DISCRETEFIELDDATANOTFOUND, "discrete field not found: " + sContextIdentifier);

	return LoadDiscreteField2DByUUID(sFoundUUID);
}

IDiscreteFieldData2D* CBuild::LoadDiscreteField2DByUUID(const std::string& sDataUUID)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::vector<uint8_t> Buffer;

	auto pJobData = pBuildJob->RetrieveJobData(sDataUUID);
	auto pStorageStream = pJobData->GetStorageStream ();
	pStorageStream->GetContent(Buffer);

	auto pFieldData2D = AMC::CDiscreteFieldData2DInstance::createFromBuffer (Buffer);
	return new CDiscreteFieldData2D (pFieldData2D);
	
}

std::string CBuild::StoreDiscreteField2D(const std::string& sContextIdentifier, const std::string & sName, IDiscreteFieldData2D* pFieldDataInstance, IDiscreteFieldData2DStoreOptions* pStoreOptions, const std::string& sUserUUID)
{
	if (pFieldDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto pFieldDataImplInstance = dynamic_cast<CDiscreteFieldData2D*> (pFieldDataInstance);
	if (pFieldDataImplInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

	auto pFieldData2D = pFieldDataImplInstance->getInstance();

	std::vector<uint8_t> Buffer;
	pFieldData2D->saveToBuffer(Buffer);

	if (Buffer.size () == 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTSTOREFIELDDATA);

	return AddBinaryData(sContextIdentifier, sName, "application/amcf-discretefield2d", sUserUUID, Buffer.size(), Buffer.data());



}



IImageData* CBuild::LoadPNGImageByIdentifier(const std::string& sContextIdentifier, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) 
{

	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::vector<uint8_t> Buffer;

	auto pJobData = pBuildJob->RetrieveJobDataByIdentifier(sContextIdentifier);
	auto pStorageStream = pJobData->GetStorageStream();
	pStorageStream->GetContent(Buffer);

	if (Buffer.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGBUFFER);

	return CImageData::createFromPNG(Buffer.data(), Buffer.size(), dDPIValueX, dDPIValueY, ePixelFormat);
}

IImageData* CBuild::LoadPNGImageByUUID(const std::string& sDataUUID, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) 
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::vector<uint8_t> Buffer;

	auto pJobData = pBuildJob->RetrieveJobData(AMCCommon::CUtils::normalizeUUIDString(sDataUUID));
	auto pStorageStream = pJobData->GetStorageStream();
	pStorageStream->GetContent(Buffer);

	if (Buffer.empty ())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYPNGBUFFER);

	return CImageData::createFromPNG (Buffer.data (), Buffer.size (), dDPIValueX, dDPIValueY, ePixelFormat);
}

std::string CBuild::StorePNGImage(const std::string& sContextIdentifier, const std::string& sName, IImageData* pImageDataInstance, IPNGImageStoreOptions* pStoreOptions, const std::string& sUserUUID)
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


IBuildExecution* CBuild::StartExecution(const std::string& sDescription, const std::string& sUserUUID)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::string sNormalizedUserUUID;

	if (!sUserUUID.empty()) {
		sNormalizedUserUUID = AMCCommon::CUtils::normalizeUUIDString(sUserUUID);
	}
	else {
		sNormalizedUserUUID = AMCCommon::CUtils::createEmptyUUID();
	}

	auto pExecutionData = pBuildJob->CreateBuildJobExecution(sDescription, sNormalizedUserUUID, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());

	return new CBuildExecution (pExecutionData, m_pDataModel, m_pToolpathHandler, m_pGlobalChrono);

}

bool CBuild::HasExecution(const std::string& sExecutionUUID)
{
	std::string sNormalizedExecutionUUID = AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID);

	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto pExecutionData = pBuildJob->RetrieveBuildJobExecution (sNormalizedExecutionUUID);

	return pExecutionData.get() != nullptr;

}

IBuildExecution* CBuild::FindExecution(const std::string& sExecutionUUID)
{
	std::string sNormalizedExecutionUUID = AMCCommon::CUtils::normalizeUUIDString(sExecutionUUID);

	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	auto pExecutionData = pBuildJob->RetrieveBuildJobExecution(sNormalizedExecutionUUID);

	return new CBuildExecution(pExecutionData, m_pDataModel, m_pToolpathHandler, m_pGlobalChrono);
}

IBuildExecutionIterator* CBuild::ListExecutions(const bool bOnlyCurrentJournalSession)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::string sJournalUUIDFilter;
	if (bOnlyCurrentJournalSession) {
		auto pJournalSession = m_pDataModel->CreateJournalSession();
		sJournalUUIDFilter = pJournalSession->GetSessionUUID();
	}

	std::unique_ptr<CBuildExecutionIterator> pResult(new CBuildExecutionIterator ());
		
	auto pIterator = pBuildJob->RetrieveBuildJobExecutions (sJournalUUIDFilter);
	while (pIterator->MoveNext()) {
		auto pExecutionData = pIterator->GetCurrentJobExecution();
		pResult->AddBuildExecution (std::make_shared<CBuildExecution>(pExecutionData, m_pDataModel, m_pToolpathHandler, m_pGlobalChrono));
	}

	return pResult.release();
}

IBuildExecutionIterator* CBuild::ListExecutionsByStatus(const LibMCEnv::eBuildExecutionStatus eExecutionStatus, const bool bOnlyCurrentJournalSession)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	std::string sJournalUUIDFilter;
	if (bOnlyCurrentJournalSession) {
		auto pJournalSession = m_pDataModel->CreateJournalSession();
		sJournalUUIDFilter = pJournalSession->GetSessionUUID();
	}

	LibMCData::eBuildJobExecutionStatus eLibMCDataStatus = LibMCData::eBuildJobExecutionStatus::Unknown;

	switch (eExecutionStatus) {
	case LibMCEnv::eBuildExecutionStatus::InProcess: eLibMCDataStatus = LibMCData::eBuildJobExecutionStatus::InProcess; break;
	case LibMCEnv::eBuildExecutionStatus::Finished: eLibMCDataStatus = LibMCData::eBuildJobExecutionStatus::Finished; break;
	case LibMCEnv::eBuildExecutionStatus::Failed: eLibMCDataStatus = LibMCData::eBuildJobExecutionStatus::Failed; break;
	}


	std::unique_ptr<CBuildExecutionIterator> pResult(new CBuildExecutionIterator());

	auto pIterator = pBuildJob->RetrieveBuildJobExecutionsByStatus(eLibMCDataStatus, sJournalUUIDFilter);
	while (pIterator->MoveNext()) {
		auto pExecutionData = pIterator->GetCurrentJobExecution();
		pResult->AddBuildExecution(std::make_shared<CBuildExecution>(pExecutionData, m_pDataModel, m_pToolpathHandler, m_pGlobalChrono));
	}

	return pResult.release();
}

void CBuild::StoreMetaDataString(const std::string& sKey, const std::string& sValue)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sKey))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMETADATAKEY);

	pBuildJob->StoreMetaDataString(sKey, sValue, m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970 ());

}

bool CBuild::HasMetaDataString(const std::string& sKey)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sKey))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMETADATAKEY);

	return pBuildJob->HasMetaDataString(sKey);

}

std::string CBuild::GetMetaDataString(const std::string& sKey)
{
	auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
	auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);

	if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sKey))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMETADATAKEY);

	return pBuildJob->GetMetaDataString(sKey);


}

