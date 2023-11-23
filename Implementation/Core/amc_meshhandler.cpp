/*++

Copyright (C) 2023 Autodesk Inc.

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


#include "amc_meshhandler.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

namespace AMC {


	CMeshHandler::CMeshHandler()
	{
	}
	
	CMeshHandler::~CMeshHandler()
	{
	}

	bool CMeshHandler::hasMeshEntity(const std::string& sEntityUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sEntityUUID);
		auto iIter = m_Entities.find(sNormalizedUUID);
		return (iIter != m_Entities.end ());
	}

	PMeshEntity CMeshHandler::findMeshEntity(const std::string & sEntityUUID, bool bFailIfNotExistent)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sEntityUUID);
		auto iIter = m_Entities.find(sNormalizedUUID);
		if (iIter != m_Entities.end()) {
			return iIter->second;
		}

		if (bFailIfNotExistent)
			throw ELibMCCustomException(LIBMC_ERROR_MESHENTITYNOTLOADED, sNormalizedUUID);

		return nullptr;


	}	
		
	void CMeshHandler::unloadMeshEntity (const std::string& sEntityUUID)
	{
		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sEntityUUID);
		auto pToolpathEntity = findMeshEntity(sNormalizedUUID, true);
		if (pToolpathEntity->DecRef())
			m_Entities.erase(sNormalizedUUID);
	}
	
	void CMeshHandler::unloadAllEntities()
	{
		m_Entities.clear();
	}

	PMeshEntity CMeshHandler::register3MFResource(Lib3MF::CLib3MFWrapper* pWrapper, AMC::CResourcePackage* pResourcePackage, const std::string& sResourceName)
	{
		if (pWrapper == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pResourcePackage == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::string sMeshUUID = AMCCommon::CUtils::createUUID();
		auto pMeshEntity = std::make_shared<CMeshEntity>(sMeshUUID, sResourceName);
		pMeshEntity->loadFrom3MFResource(pWrapper, pResourcePackage, sResourceName);

		pMeshEntity->IncRef();
		m_Entities.insert(std::make_pair(sMeshUUID, pMeshEntity));

		return pMeshEntity;
	}


	PMeshEntity CMeshHandler::register3MFMesh(const std::string& sEntityUUID, Lib3MF::CLib3MFMeshObject* pMeshObject)
	{
		if (pMeshObject == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sEntityUUID);

		auto iIter = m_Entities.find(sEntityUUID);
		if (iIter != m_Entities.end())
			throw ELibMCCustomException(LIBMC_ERROR_MESHENTITYALREADYLOADED, sNormalizedUUID);

		auto pMeshEntity = std::make_shared<CMeshEntity>(sNormalizedUUID, pMeshObject->GetName ());
		pMeshEntity->loadFrom3MF(pMeshObject);

		pMeshEntity->IncRef();
		m_Entities.insert(std::make_pair (sNormalizedUUID, pMeshEntity));

		return pMeshEntity;
	}

}


