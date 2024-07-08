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


Abstract: This is a stub class definition of CModelDataComponentInstance

*/

#include "libmcenv_modeldatacomponentinstance.hpp"
#include "libmcenv_modeldatameshinstance.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CModelDataComponentInstance 
**************************************************************************************************************************/

CModelDataComponentInstance::CModelDataComponentInstance(Lib3MF::PModel pModel, Lib3MF::PObject p3MFObject, LibMCEnv::sModelDataTransform transform, AMC::PMeshHandler pMeshHandler)
	: m_Transform (transform), m_pModel (pModel), m_pMeshHandler (pMeshHandler)
{
	if (pMeshHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (p3MFObject.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sName = p3MFObject->GetName();

	bool bHasUUID = false;
	std::string sObjectUUID = p3MFObject->GetUUID(bHasUUID);
	if (bHasUUID) {
		m_sUUID = AMCCommon::CUtils::normalizeUUIDString(sObjectUUID);
	}
	else {
		m_sUUID = AMCCommon::CUtils::createUUID();
	}

	if (p3MFObject->IsComponentsObject())
	{
		auto pComponentsObject = pModel->GetComponentsObjectByID (p3MFObject->GetUniqueResourceID ());
		uint32_t nCount = pComponentsObject->GetComponentCount();

		for (uint32_t nIndex = 0; nIndex < nCount; nIndex++) {

			auto pComponent = pComponentsObject->GetComponent(nIndex);
			auto pSubObject = pComponent->GetObjectResource();

			sModelDataTransform transform = map3MFTransform (pComponent->GetTransform());
			addSubObjectWithTransform(pSubObject, transform);

		}
	}


	if (p3MFObject->IsMeshObject())
	{
		sModelDataTransform transform = createIdentityTransform ();
		addSubObjectWithTransform(p3MFObject, transform);
	}
}

CModelDataComponentInstance::CModelDataComponentInstance(Lib3MF::PModel pModel, Lib3MF::PBuildItem p3MFBuildItem, AMC::PMeshHandler pMeshHandler)
	: m_pModel(pModel), m_pMeshHandler(pMeshHandler)
{

	if (pMeshHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (p3MFBuildItem.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sName = p3MFBuildItem->GetPartNumber ();

	bool bHasUUID = false;
	std::string sBuildItemUUID = p3MFBuildItem->GetUUID(bHasUUID);
	if (bHasUUID) {
		m_sUUID = AMCCommon::CUtils::normalizeUUIDString(sBuildItemUUID);
	}
	else {
		m_sUUID = AMCCommon::CUtils::createUUID();
	}

	m_Transform = createIdentityTransform();

	addBuildItem(p3MFBuildItem);
}

// Creates a Component instance for the full model
CModelDataComponentInstance::CModelDataComponentInstance(Lib3MF::PModel pModel, AMC::PMeshHandler pMeshHandler)
	: m_pModel(pModel), m_pMeshHandler(pMeshHandler)
{
	if (pMeshHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pModel.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sName = "";
	m_sUUID = AMCCommon::CUtils::createUUID();

	m_Transform = createIdentityTransform();

	auto pBuildItemIterator = m_pModel->GetBuildItems();
	while (pBuildItemIterator->MoveNext()) {
		addBuildItem(pBuildItemIterator->GetCurrent());
	}

}


CModelDataComponentInstance::CModelDataComponentInstance(Lib3MF::CWrapper* p3MFWrapper, AMC::CResourcePackage* pResourcePackage, const std::string sResourceName, AMC::PMeshHandler pMeshHandler)
	: m_pMeshHandler(pMeshHandler)
{
	if (p3MFWrapper == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pMeshHandler == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pResourcePackage == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	m_sName = "";
	m_sUUID = AMCCommon::CUtils::createUUID();

	m_Transform = createIdentityTransform();
	m_pModel = p3MFWrapper->CreateModel();

	std::vector<uint8_t> Buffer;
	pResourcePackage->readEntry(sResourceName, Buffer);

	auto pModel = p3MFWrapper->CreateModel();
	auto pReader = pModel->QueryReader("3mf");

	pReader->ReadFromBuffer(Buffer);

	auto pBuildItemIterator = m_pModel->GetBuildItems();
	while (pBuildItemIterator->MoveNext()) {
		addBuildItem(pBuildItemIterator->GetCurrent());
	}
}



CModelDataComponentInstance::~CModelDataComponentInstance()
{

}

void CModelDataComponentInstance::addBuildItem(Lib3MF::PBuildItem pBuildItem)
{
	if (pBuildItem.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto transform = map3MFTransform(pBuildItem->GetObjectTransform());
	addSubObjectWithTransform(pBuildItem->GetObjectResource(), transform);
}

void CModelDataComponentInstance::addSubObjectWithTransform(Lib3MF::PObject pSubObject, LibMCEnv::sModelDataTransform transform)
{
	if (pSubObject.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	if (pSubObject->IsComponentsObject()) {
		// In case we have a subcomponent, add to subcomponent list
		m_SubComponents.push_back(std::make_pair(pSubObject, transform));
	}

	if (pSubObject->IsMeshObject()) {
		// In case we have a mesh, return mesh object
		auto pMeshObject = m_pModel->GetMeshObjectByID(pSubObject->GetUniqueResourceID());

		auto objectType = pMeshObject->GetType();
		switch (objectType) {
		case Lib3MF::eObjectType::Model:
			m_Solids.push_back(std::make_pair(pMeshObject, transform));
			break;

		case Lib3MF::eObjectType::Support:
			m_Supports.push_back(std::make_pair(std::make_pair(pMeshObject, false), transform));
			break;

		case Lib3MF::eObjectType::SolidSupport:
			m_Supports.push_back(std::make_pair(std::make_pair(pMeshObject, true), transform));
			break;
		}
	}

}


LibMCEnv::sModelDataTransform CModelDataComponentInstance::map3MFTransform(const Lib3MF::sTransform transform3MF)
{
	LibMCEnv::sModelDataTransform transform;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			transform.m_Matrix[j][i] = transform3MF.m_Fields[j][i];
		}

		transform.m_Translation[i] = transform3MF.m_Fields[3][i];
	}

	return transform;

}

LibMCEnv::sModelDataTransform CModelDataComponentInstance::createIdentityTransform()
{
	// Create identity transform
	sModelDataTransform transform;
	memset((void*)&transform, 0, sizeof(transform));
	for (uint32_t i = 0; i < 3; i++)
		transform.m_Matrix[i][i] = 1.0;

	return transform;
}


std::string CModelDataComponentInstance::GetName()
{
	return m_sName;
}

std::string CModelDataComponentInstance::GetUUID()
{
	return m_sUUID;
}

LibMCEnv::sModelDataTransform CModelDataComponentInstance::GetTransform()
{
	return m_Transform;
}

LibMCEnv_uint32 CModelDataComponentInstance::GetSolidCount()
{
	return (uint32_t)m_Solids.size();
}

IModelDataMeshInstance * CModelDataComponentInstance::GetSolidMesh(const LibMCEnv_uint32 nIndex)
{
	if (nIndex < m_Solids.size()) {
		auto& iter = m_Solids.at(nIndex);
		return new CModelDataMeshInstance(m_pModel, iter.first, iter.second, m_pMeshHandler);
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSOLIDINDEX);

	}
}

LibMCEnv_uint32 CModelDataComponentInstance::GetSupportCount()
{
	return (uint32_t)m_Supports.size();
}

IModelDataMeshInstance * CModelDataComponentInstance::GetSupportMesh(const LibMCEnv_uint32 nIndex)
{
	if (nIndex < m_Supports.size()) {
		auto& iter = m_Supports.at(nIndex);
		return new CModelDataMeshInstance (m_pModel, iter.first.first, iter.second, m_pMeshHandler);
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDSUPPORTINDEX);

	}
}

LibMCEnv_uint32 CModelDataComponentInstance::GetSubComponentCount()
{
	return (uint32_t)m_SubComponents.size();
}

IModelDataComponentInstance * CModelDataComponentInstance::GetSubComponent(const LibMCEnv_uint32 nIndex)
{
	if (nIndex < m_SubComponents.size()) {
		auto& iter = m_SubComponents.at(nIndex);
		return new CModelDataComponentInstance(m_pModel, iter.first, iter.second, m_pMeshHandler);
	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCOMPONENTINDEX);

	}

}

