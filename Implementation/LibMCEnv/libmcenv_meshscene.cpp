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


Abstract: This is a stub class definition of CMeshScene

*/

#include "libmcenv_meshscene.hpp"
#include "libmcenv_meshsceneitem.hpp"
#include "libmcenv_interfaceexception.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CMeshScene 
**************************************************************************************************************************/

CMeshScene::CMeshScene(AMC::PMeshScene pMeshScene, AMC::PMeshHandler pMeshHandler)
	: m_pMeshScene (pMeshScene), m_pMeshHandler (pMeshHandler)
{
	if (pMeshScene.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	if (pMeshHandler.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

CMeshScene::~CMeshScene()
{

}


std::string CMeshScene::GetSceneUUID()
{
	return m_pMeshScene->getSceneUUID();
}

bool CMeshScene::IsBoundToLoginSession()
{
	return m_pMeshScene->isBoundToLoginSession();
}

IMeshSceneItem * CMeshScene::AddSceneItem(IPersistentMeshObject* pMesh, const LibMCEnv::sModelDataTransform AbsoluteTransform)
{
	if (pMesh == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	auto pNewItem = m_pMeshScene->addItem(pMesh->GetUUID(), AbsoluteTransform);
	return new CMeshSceneItem(m_pMeshHandler, m_pMeshScene->getSceneUUID(), pNewItem->getUUID());
}

IMeshSceneItem * CMeshScene::AddModelDataMeshAsSceneItem(IModelDataMeshInstance* pModelDataMesh)
{
	if (pModelDataMesh == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

	std::unique_ptr<IPersistentMeshObject> pPersistentMesh (pModelDataMesh->CreatePersistentMesh(m_pMeshScene->isBoundToLoginSession()));

	auto pNewItem = m_pMeshScene->addItem(pPersistentMesh->GetUUID(), pModelDataMesh->GetAbsoluteTransform ());
	return new CMeshSceneItem(m_pMeshHandler, m_pMeshScene->getSceneUUID(), pNewItem->getUUID());
}

LibMCEnv_uint32 CMeshScene::GetSceneItemCount()
{
	return (uint32_t) m_pMeshScene->getItemCount();
}

IMeshSceneItem * CMeshScene::GetSceneItem(const LibMCEnv_uint32 nIndex)
{
	auto pMeshSceneItem = m_pMeshScene->getItem(nIndex);

	return new CMeshSceneItem(m_pMeshHandler, m_pMeshScene->getSceneUUID(), pMeshSceneItem->getUUID());
}

IMeshSceneItem * CMeshScene::FindSceneItem(const std::string & sUUID, const bool bMustExist)
{
	auto pMeshSceneItem = m_pMeshScene->findItem (sUUID, bMustExist);
	if (pMeshSceneItem.get () != nullptr)
		return new CMeshSceneItem(m_pMeshHandler, m_pMeshScene->getSceneUUID(), pMeshSceneItem->getUUID());

	return nullptr;
	
}

bool CMeshScene::HasSceneItem(const std::string & sUUID)
{
	auto pMeshSceneItem = m_pMeshScene->findItem(sUUID, false);
	return (pMeshSceneItem.get() != nullptr);
}

void CMeshScene::RemoveSceneItem(IMeshSceneItem* pSceneItem)
{
	if (pSceneItem == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
	m_pMeshScene->removeItem(pSceneItem->GetItemUUID());
}


