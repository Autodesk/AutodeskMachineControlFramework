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

#include "amc_toolpathentity.hpp"
#include "libmc_exceptiontypes.hpp"
#include "libmcenv_dynamic.hpp"
#include "libmcenv_interfaceexception.hpp"

#include <common_utils.hpp>

namespace AMC {

	CToolpathEntity::CToolpathEntity(LibMCData::PDataModel pDataModel, const std::string& sStorageStreamUUID, Lib3MF::PWrapper p3MFWrapper, const std::string& sDebugName, bool bAllowEmptyToolpath, const std::set<std::string>& attachmentRelationsToRead)
		: m_ReferenceCount (0), m_sDebugName (sDebugName)
	{
		LibMCAssertNotNull(pDataModel.get());
		LibMCAssertNotNull(p3MFWrapper.get());

		auto pStorage = pDataModel->CreateStorage();
		m_pStorageStream = pStorage->RetrieveStream(sStorageStreamUUID);

		void* pReadCallback = nullptr;
		void* pSeekCallback = nullptr;
		void* pUserData = nullptr;

		// It is important to keep the storage stream in the same context as the 3MF Model - as the callbacks would be invalidated otherwise!
		m_pStorageStream->GetCallbacks(pReadCallback, pSeekCallback, pUserData);
		uint64_t nStreamSize = m_pStorageStream->GetSize();

		m_p3MFWrapper = p3MFWrapper;

		m_p3MFModel = p3MFWrapper->CreateModel();
		m_pPersistentSource = m_p3MFModel->CreatePersistentSourceFromCallback((Lib3MF::ReadCallback)pReadCallback, nStreamSize, (Lib3MF::SeekCallback)pSeekCallback, pUserData);

		m_p3MFReader = m_p3MFModel->QueryReader("3mf");
		for (std::string sRelationToRead : attachmentRelationsToRead)
			m_p3MFReader->AddRelationToRead(sRelationToRead);

		m_p3MFReader->ReadFromPersistentSource(m_pPersistentSource.get ());

		auto pToolpathIterator = m_p3MFModel->GetToolpaths();
		if (pToolpathIterator->MoveNext()) {
			m_pToolpath = pToolpathIterator->GetCurrentToolpath();
		}
		else {

			if (!bAllowEmptyToolpath)
				throw ELibMCCustomException(LIBMC_ERROR_TOOLPATHENTITYINVALIDFILE, m_sDebugName);

		}

		auto pBuildItems = m_p3MFModel->GetBuildItems();
		while (pBuildItems->MoveNext()) {
			auto pPart = std::make_shared<CToolpathPart>(m_p3MFModel, pBuildItems->GetCurrent(), m_p3MFWrapper);
			m_PartList.push_back(pPart);
			m_PartMap.insert(std::make_pair (pPart->getUUID (), pPart));
		}

		uint32_t nAttachmentCount = m_p3MFModel->GetAttachmentCount();
		for (uint32_t nAttachmentIndex = 0; nAttachmentIndex < nAttachmentCount; nAttachmentIndex++) {
			auto pAttachment = m_p3MFModel->GetAttachment(nAttachmentIndex);
			std::string sPathWithoutLeadingSlash = AMCCommon::CUtils::removeLeadingPathDelimiter (pAttachment->GetPath());
			m_Attachments.insert(std::make_pair(AMCCommon::CUtils::toLowerString (sPathWithoutLeadingSlash), pAttachment));
		}

	}

	CToolpathEntity::~CToolpathEntity()
	{
		m_Attachments.clear();

		m_pToolpath = nullptr;
		m_p3MFReader = nullptr;
		m_pPersistentSource = nullptr;
		m_p3MFModel = nullptr;
		m_pStorageStream = nullptr;
		m_p3MFWrapper = nullptr;
	}

	void CToolpathEntity::IncRef()
	{
		if (m_ReferenceCount >= AMC_TOOLPATH_MAXREFCOUNT)
			throw ELibMCCustomException(LIBMC_ERROR_TOOLPATHENTITYREFERENCEERROR, m_sDebugName);

		m_ReferenceCount++;

	}

	bool CToolpathEntity::DecRef()
	{
		if (m_ReferenceCount == 0)
			throw ELibMCCustomException(LIBMC_ERROR_TOOLPATHENTITYREFERENCEERROR, m_sDebugName);

		m_ReferenceCount--;
		return (m_ReferenceCount == 0);
	}

	uint32_t CToolpathEntity::getLayerCount()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);		
		if (m_pToolpath.get () != nullptr)
			return m_pToolpath->GetLayerCount ();

		return 0;
	}

	uint32_t CToolpathEntity::getLayerZInUnits(uint32_t nLayerIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (m_pToolpath.get() != nullptr) {
			auto nZValue = m_pToolpath->GetLayerZ(nLayerIndex);

			return nZValue;
		}

		return 0;
	}

	PToolpathLayerData CToolpathEntity::readLayer(uint32_t nLayerIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		double dUnits = m_pToolpath->GetUnits();

		auto p3MFLayerData = m_pToolpath->ReadLayerData(nLayerIndex);
		auto nZValue = m_pToolpath->GetLayerZ(nLayerIndex);
		return std::make_shared<CToolpathLayerData> (m_pToolpath, p3MFLayerData, dUnits, nZValue, m_sDebugName, m_CustomSegmentAttributes);
	}


	double CToolpathEntity::getUnits()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		return m_pToolpath->GetUnits();

	}

	uint32_t CToolpathEntity::getMetaDataCount()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		return m_pToolpath->GetCustomDataCount();

	}
	void CToolpathEntity::getMetaDataInfo(uint32_t nMetaDataIndex, std::string& sNameSpace, std::string& sName)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		
		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		uint32_t nDataCount = m_pToolpath->GetCustomDataCount();
		if (nMetaDataIndex >= nDataCount)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMETADATAINDEX);

		m_pToolpath->GetCustomDataName(nMetaDataIndex, sNameSpace, sName);
	}


	PXMLDocumentInstance CToolpathEntity::getMetaData(uint32_t nMetaDataIndex)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		uint32_t nDataCount = m_pToolpath->GetCustomDataCount();
		if (nMetaDataIndex >= nDataCount)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMETADATAINDEX);

		auto pMetaData = m_pToolpath->GetCustomData(nMetaDataIndex);
		auto pMetaDataRootNode = pMetaData->GetRootNode();

		PXMLDocumentInstance pXMLDocument = std::make_shared<CXMLDocumentInstance>();
		pXMLDocument->createEmptyDocument (pMetaDataRootNode->GetName (), pMetaData->GetNameSpace ());

		copyMetaDataNode(pXMLDocument->GetRootNode(), pMetaDataRootNode);

		return pXMLDocument;

	}

	void CToolpathEntity::copyMetaDataNode(AMC::PXMLDocumentNodeInstance pTargetNodeInstance, Lib3MF::PCustomXMLNode pSourceNodeInstance)
	{
		if (pTargetNodeInstance.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pSourceNodeInstance.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		size_t nAttributeCount = pSourceNodeInstance->GetAttributeCount();
		for (size_t nAttributeIndex = 0; nAttributeIndex < nAttributeCount; nAttributeIndex++) {
			auto pAttribute = pSourceNodeInstance->GetAttribute(nAttributeIndex);
			pTargetNodeInstance->AddAttribute(pTargetNodeInstance->GetNameSpace(), pAttribute->GetName(), pAttribute->GetValue());
		}

		auto pSourceChildren = pSourceNodeInstance->GetChildren ();
		size_t nChildCount = pSourceChildren->GetNodeCount();
		for (size_t nChildIndex = 0; nChildIndex < nChildCount; nChildIndex++) {
			auto pSourceChild = pSourceChildren->GetNode(nChildIndex);
			auto pTargetChild = pTargetNodeInstance->AddChild(pTargetNodeInstance->GetNameSpace(), pSourceChild->GetName());

			copyMetaDataNode(pTargetChild, pSourceChild);
		}
	}


	bool CToolpathEntity::hasUniqueMetaData(const std::string& sNameSpace, const std::string& sName)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		uint32_t nFoundData = 0;
		uint32_t nDataCount = m_pToolpath->GetCustomDataCount();

		for (uint32_t nMetaDataIndex = 0; nMetaDataIndex < nDataCount; nMetaDataIndex++) {
			std::string sMetaDataNameSpace, sMetaDataName;
			m_pToolpath->GetCustomDataName(nMetaDataIndex, sMetaDataNameSpace, sMetaDataName);
			if ((sMetaDataNameSpace == sNameSpace) && (sMetaDataName == sName))
				nFoundData++;
		}

		return (nFoundData == 1);
	}

	PXMLDocumentInstance CToolpathEntity::findUniqueMetaData(const std::string& sNameSpace, const std::string& sName)
	{
		int64_t nFoundIndex = -1;

		{
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			if (m_pToolpath.get() == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

			uint32_t nDataCount = m_pToolpath->GetCustomDataCount();

			for (uint32_t nMetaDataIndex = 0; nMetaDataIndex < nDataCount; nMetaDataIndex++) {
				std::string sMetaDataNameSpace, sMetaDataName;
				m_pToolpath->GetCustomDataName(nMetaDataIndex, sMetaDataNameSpace, sMetaDataName);
				if ((sMetaDataNameSpace == sNameSpace) && (sMetaDataName == sName)) {
					if (nFoundIndex != -1)
						throw ELibMCCustomException(LIBMC_ERROR_METADATAISNOTUNIQUE, sNameSpace + "/" + sName);

					nFoundIndex = nMetaDataIndex;
				}
			}
		}

		if (nFoundIndex < 0)
			throw ELibMCCustomException(LIBMC_ERROR_METADATANOTFOUND, sNameSpace + "/" + sName);

		return getMetaData ((uint32_t) nFoundIndex);
	}


	std::string CToolpathEntity::getDebugName()
	{
		return m_sDebugName;
	}


	uint32_t CToolpathEntity::getPartCount()
	{
		return (uint32_t) m_PartList.size();
	}

	PToolpathPart CToolpathEntity::getPart(uint32_t nIndex)
	{
		return m_PartList[nIndex];
	}

	PToolpathPart CToolpathEntity::findPartByUUID(const std::string& sUUID)
	{
		auto iIter = m_PartMap.find(sUUID);
		if (iIter == m_PartMap.end())
			return nullptr;

		return iIter->second;
	}

	void CToolpathEntity::registerCustomSegmentAttribute(const std::string& sNameSpace, const std::string& sAttributeName, const LibMCEnv::eToolpathAttributeType eAttributeType)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUILDHASNOTOOLPATH);

		auto key = std::make_pair(sNameSpace, sAttributeName);

		auto iIter = m_CustomSegmentAttributeMap.find(key);
		if (iIter != m_CustomSegmentAttributeMap.end()) {
			if (iIter->second->getAttributeType () != eAttributeType)
				throw ELibMCInterfaceException(LIBMC_ERROR_AMBIGUOUSSEGMENTATTRIBUTETYPE, "ambiguous segment attribute type of " + sNameSpace + "/" + sAttributeName);

			return;
		}


		switch (eAttributeType) {
		case LibMCEnv::eToolpathAttributeType::Integer: 
			m_pToolpath->RegisterCustomIntegerAttribute (sNameSpace, sAttributeName);
			break;
		case LibMCEnv::eToolpathAttributeType::Double:
			m_pToolpath->RegisterCustomDoubleAttribute (sNameSpace, sAttributeName);
			break;
		default: 
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSEGMENTATTRIBUTETYPE, "invalid segment attribute type of " + sNameSpace + "/" + sAttributeName);
		}
		
		auto pSegmentAttribute = std::make_shared<CToolpathCustomSegmentAttribute> (sNameSpace, sAttributeName, eAttributeType);
		m_CustomSegmentAttributes.push_back(pSegmentAttribute);

		m_CustomSegmentAttributeMap.insert(std::make_pair (key, pSegmentAttribute));
		
	}

	Lib3MF::PAttachment CToolpathEntity::findBinaryMetaData(const std::string& sPath, bool bMustExist)
	{
		std::string sPathWithoutLeadingSlash = AMCCommon::CUtils::removeLeadingPathDelimiter(sPath);
		std::string sLowerPath = AMCCommon::CUtils::toLowerString(sPathWithoutLeadingSlash);
		if (sLowerPath.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDBINARYMETADATAPATH);

		auto iIter = m_Attachments.find(sLowerPath);
		if (iIter != m_Attachments.end())
			return iIter->second;

		if (bMustExist)
			throw ELibMCCustomException(LIBMC_ERROR_BINARYMETADATAPATHNOTFOUND, sPath);

		return nullptr;
	}

	bool CToolpathEntity::hasBinaryMetaData(const std::string& sPath)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return (findBinaryMetaData(sPath, false) != nullptr);
	}

	void CToolpathEntity::getBinaryMetaData(const std::string& sPath, uint64_t nMetaDataBufferSize, uint64_t* pMetaDataNeededCount, uint8_t* pMetaDataBuffer)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto pMetaData = findBinaryMetaData(sPath, true);

		uint64_t nStreamSize = pMetaData->GetStreamSize();

		if (pMetaDataNeededCount != nullptr)
			*pMetaDataNeededCount = nStreamSize;

		if (pMetaDataBuffer != nullptr) {
			if (nMetaDataBufferSize < nStreamSize)
				throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

			// TODO: Improve efficiency...
			std::vector<uint8_t> tempBuffer;
			pMetaData->WriteToBuffer(tempBuffer);

			if (tempBuffer.size () != nStreamSize)
				throw ELibMCInterfaceException(LIBMC_ERROR_INTERNALMETADATAERROR);

			if (nStreamSize > 0) {

				uint8_t *pSource = tempBuffer.data();
				uint8_t* pTarget = pMetaDataBuffer;
				for (uint64_t nIndex = 0; nIndex < nStreamSize; nIndex++) {
					*pTarget = *pSource;
					pTarget++; pSource++;
				}
			}
		}

	}

	std::string CToolpathEntity::getBinaryMetaDataRelationship(const std::string& sPath)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		auto pMetaData = findBinaryMetaData(sPath, true);
		return pMetaData->GetRelationShipType();
	}


	bool CToolpathEntity::readThumbnail(std::vector<uint8_t>& thumbnailBuffer, std::string& sMimeType)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (m_p3MFModel.get() != nullptr) {
			auto pThumbnailAttachment = m_p3MFModel->GetPackageThumbnailAttachment();
			if (pThumbnailAttachment.get() != nullptr) {

				std::string sPath = AMCCommon::CUtils::toLowerString (pThumbnailAttachment->GetPath());

				// TODO: Move this function into proper Lib3MF code...
				size_t nLength = sPath.length();
				if (nLength >= 4) {
					std::string sSubstr = sPath.substr(nLength - 4, 4);
					if (sSubstr == ".png")
						sMimeType = "image/png";
					if (sSubstr == ".jpg")
						sMimeType = "image/jpeg";
				}

				if (nLength >= 5) {
					std::string sSubstr = sPath.substr(nLength - 5, 5);
					if (sSubstr == ".jpeg")
						sMimeType = "image/jpeg";
				}

				pThumbnailAttachment->WriteToBuffer(thumbnailBuffer);


				return true;
			}

		}


		return false;
	}


}


