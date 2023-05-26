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


#include "libmcenv_toolpathaccessor.hpp"
#include "libmcenv_toolpathlayer.hpp"
#include "libmcenv_toolpathpart.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_xmldocument.hpp"
#include "libmcenv_xmldocumentnode.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CToolpathAccessor 
**************************************************************************************************************************/

CToolpathAccessor::CToolpathAccessor(const std::string& sStorageUUID, AMC::PToolpathHandler pToolpathHandler)
	: m_sStorageUUID(sStorageUUID), m_pToolpathHandler(pToolpathHandler)
{
	if (pToolpathHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CToolpathAccessor::~CToolpathAccessor()
{
}

std::string CToolpathAccessor::GetStorageUUID()
{
	return m_sStorageUUID;
}

LibMCEnv_uint32 CToolpathAccessor::GetLayerCount()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);
	return pToolpathEntity->getLayerCount();

}

IToolpathLayer * CToolpathAccessor::LoadLayer(const LibMCEnv_uint32 nLayerIndex)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);

	return new CToolpathLayer(pToolpathEntity->readLayer (nLayerIndex));
}

LibMCEnv_double CToolpathAccessor::GetUnits()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);
	return pToolpathEntity->getUnits();
}


LibMCEnv_uint32 CToolpathAccessor::GetPartCount()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);
	return pToolpathEntity->getPartCount();
}

IToolpathPart* CToolpathAccessor::GetPart(const LibMCEnv_uint32 nPartIndex)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);
	auto pPart = pToolpathEntity->getPart(nPartIndex);

	return new CToolpathPart(pPart);
}

IToolpathPart* CToolpathAccessor::FindPartByUUID(const std::string& sPartUUID)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);
	auto pPart = pToolpathEntity->findPartByUUID(sPartUUID);
	if (pPart.get() != nullptr)
		return new CToolpathPart(pPart);

	return nullptr;

}

LibMCEnv_int32 CToolpathAccessor::GetBuildHeightInUnits()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, true);
	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0;

	return pToolpathEntity->getLayerZInUnits(nLayerCount - 1);

}

LibMCEnv_int32 CToolpathAccessor::GetZValueInUnits(const LibMCEnv_uint32 nLayerIndex)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0;

	if (nLayerIndex >= nLayerCount)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDLAYERINDEX);

	return pToolpathEntity->getLayerZInUnits(nLayerIndex);

}

LibMCEnv_double CToolpathAccessor::GetBuildHeightInMM()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0.0;

	return pToolpathEntity->getLayerZInUnits(nLayerCount - 1) * pToolpathEntity->getUnits();

}

LibMCEnv_double CToolpathAccessor::GetZValueInMM(const LibMCEnv_uint32 nLayerIndex)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	uint32_t nLayerCount = pToolpathEntity->getLayerCount();
	if (nLayerCount == 0)
		return 0.0;

	if (nLayerIndex >= nLayerCount)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDLAYERINDEX);

	return pToolpathEntity->getLayerZInUnits(nLayerIndex) * pToolpathEntity->getUnits();

}

LibMCEnv_uint32 CToolpathAccessor::GetMetaDataCount()
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	return pToolpathEntity->getMetaDataCount();
}

void CToolpathAccessor::GetMetaDataInfo(const LibMCEnv_uint32 nMetaDataIndex, std::string& sNamespace, std::string& sName)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	pToolpathEntity->getMetaDataInfo(nMetaDataIndex, sNamespace, sName);
}

IXMLDocumentNode* CToolpathAccessor::GetMetaDataContent(const LibMCEnv_uint32 nMetaDataIndex)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	auto pXMLDocumentInstance = pToolpathEntity->getMetaData(nMetaDataIndex);
	return new CXMLDocumentNode (pXMLDocumentInstance, pXMLDocumentInstance->GetRootNode ());
}

bool CToolpathAccessor::HasUniqueMetaData(const std::string& sNamespace, const std::string& sName)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	return pToolpathEntity->hasUniqueMetaData(sNamespace, sName);
}

IXMLDocumentNode* CToolpathAccessor::FindUniqueMetaData(const std::string& sNamespace, const std::string& sName)
{
	auto pToolpathEntity = m_pToolpathHandler->findToolpathEntity(m_sStorageUUID, false);
	if (pToolpathEntity == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_TOOLPATHNOTLOADED);

	auto pXMLDocumentInstance = pToolpathEntity->findUniqueMetaData(sNamespace, sName);
	return new CXMLDocumentNode(pXMLDocumentInstance, pXMLDocumentInstance->GetRootNode());
}

