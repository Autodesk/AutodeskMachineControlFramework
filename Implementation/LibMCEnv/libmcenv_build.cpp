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

// Include custom headers here.
#include "amc_systemstate.hpp"
#include "amc_toolpathhandler.hpp"

#include "common_utils.hpp"

#include <iostream>

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CBuild 
**************************************************************************************************************************/

CBuild::CBuild(LibMCData::PBuildJob pBuildJob, AMC::PToolpathHandler pToolpathHandler, LibMCData::PStorage pStorage, const std::string& sSystemUserID)
	: m_pBuildJob(pBuildJob), m_pToolpathHandler(pToolpathHandler), m_pStorage (pStorage), m_sSystemUserID (sSystemUserID)
{
	if (pToolpathHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pBuildJob.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pStorage.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CBuild::~CBuild()
{
}

std::string CBuild::GetName()
{
	return m_pBuildJob->GetName();
}

std::string CBuild::GetBuildUUID()
{
	return m_pBuildJob->GetUUID();
}

std::string CBuild::GetStorageUUID()
{
	return m_pBuildJob->GetStorageStreamUUID();
}

std::string CBuild::GetStorageSHA256()
{
	return m_pBuildJob->GetStorageStream()->GetSHA2 ();

}


LibMCEnv_double CBuild::GetBuildHeightInMM()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_pBuildJob->GetStorageStreamUUID(), false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0.0;

	return pToolpathEntity->getLayerZInUnits(nLayerCount - 1) * pToolpathEntity->getUnits();
}

LibMCEnv_double CBuild::GetZValueInMM(const LibMCEnv_uint32 nLayerIndex)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_pBuildJob->GetStorageStreamUUID(), false);
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
	return m_pBuildJob->GetLayerCount();
}

IToolpathAccessor * CBuild::CreateToolpathAccessor()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	return new CToolpathAccessor(sStreamUUID, m_pBuildJob->GetUUID(), m_pToolpathHandler);
}


void CBuild::LoadToolpath()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	m_pToolpathHandler->loadToolpathEntity(sStreamUUID);
}


void CBuild::UnloadToolpath()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	m_pToolpathHandler->unloadToolpathEntity(sStreamUUID);
}

bool CBuild::ToolpathIsLoaded()
{
	auto sStreamUUID = m_pBuildJob->GetStorageStreamUUID();
	return (m_pToolpathHandler->findToolpathEntity(sStreamUUID, false) != nullptr);
}

std::string CBuild::AddBinaryData(const std::string& sIdentifier, const std::string& sName, const std::string& sMIMEType, const LibMCEnv_uint64 nContentBufferSize, const LibMCEnv_uint8* pContentBuffer)
{
	auto sDataUUID = AMCCommon::CUtils::createUUID();
	auto sSystemUserID = m_sSystemUserID;

	m_pStorage->StoreNewStream(sDataUUID, m_pBuildJob->GetUUID(), sIdentifier, sName, sMIMEType, LibMCData::CInputVector<uint8_t>(pContentBuffer, nContentBufferSize), sSystemUserID);

	auto pStorageStream = m_pStorage->RetrieveStream(sDataUUID);
	m_pBuildJob->AddJobData(sIdentifier, sName, pStorageStream, LibMCData::eBuildJobDataType::CustomBinaryData, sSystemUserID);

	return sDataUUID;

}

IDiscreteFieldData2D* CBuild::LoadDiscreteField2DByIdentifier(const std::string& sContextIdentifier)
{

	std::string sFoundUUID;

	auto pJobDataIterator = m_pBuildJob->ListJobData();
	while (pJobDataIterator->MoveNext()) {
		auto pJobData = pJobDataIterator->GetCurrentJobData();
		std::string sJobDataIdentifier = pJobData->GetContextIdentifier();
		if (sJobDataIdentifier == sContextIdentifier)
			sFoundUUID = pJobData->GetDataUUID();
	}
	
	if (sFoundUUID.empty())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DISCRETEFIELDDATANOTFOUND, "discrete field not found: " + sContextIdentifier);

	return LoadDiscreteField2DByUUID(sFoundUUID);
}

IDiscreteFieldData2D* CBuild::LoadDiscreteField2DByUUID(const std::string& sDataUUID)
{

	std::vector<uint8_t> Buffer;

	auto pJobData = m_pBuildJob->RetrieveJobData(sDataUUID);
	auto pStorageStream = pJobData->GetStorageStream ();
	pStorageStream->GetContent(Buffer);

	auto pFieldData2D = AMC::CDiscreteFieldData2DInstance::createFromBuffer (Buffer);
	return new CDiscreteFieldData2D (pFieldData2D);
	
}

std::string CBuild::StoreDiscreteField2D(const std::string& sContextIdentifier, const std::string & sName, IDiscreteFieldData2D* pFieldDataInstance, IDiscreteFieldData2DStoreOptions* pStoreOptions)
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

	return AddBinaryData(sContextIdentifier, sName, "application/amcf-discretefield2d", Buffer.size(), Buffer.data());



}

IImageData* CBuild::LoadPNGImageByIdentifier(const std::string& sContext)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IImageData* CBuild::LoadPNGImageByUUID(const std::string& sDataUUID)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

std::string CBuild::StorePNGImage(const std::string& sContextIdentifier, const std::string& sName, IImageData* pImageDataInstance, IPNGImageStoreOptions* pStoreOptions)
{
	if (pImageDataInstance == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	uint64_t nNeededCount = 0;
	/*std::unique_ptr<LibMCEnv::Impl::IPNGImageData> pPNGImage(pImageDataInstance->CreatePNGImage(pStoreOptions));
	pPNGImage->GetPNGDataStream(0, &nNeededCount, nullptr); */

	pImageDataInstance->EncodePNG();
	pImageDataInstance->GetEncodedPNGData(0, &nNeededCount, nullptr);

	if (nNeededCount == 0)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTCOMPRESSPNGIMAGE);

	std::vector<uint8_t> pngBuffer;
	pngBuffer.resize(nNeededCount);

	uint64_t nWrittenCount = 0;
	pImageDataInstance->GetEncodedPNGData(pngBuffer.size(), &nWrittenCount, pngBuffer.data());
	//pPNGImage->GetPNGDataStream(pngBuffer.size (), &nWrittenCount, pngBuffer.data ());

	if (nWrittenCount != pngBuffer.size())
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_COULDNOTRETRIEVEPNGSTREAM);

	return AddBinaryData(sContextIdentifier, sName, "image/png", pngBuffer.size(), pngBuffer.data());

}

