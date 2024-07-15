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


Abstract: This is a stub class definition of CMeshSceneItem

*/

#include "libmcenv_meshsceneitem.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_persistentmeshobject.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CMeshSceneItem 
**************************************************************************************************************************/

CMeshSceneItem::CMeshSceneItem(AMC::PMeshHandler pMeshHandler, const std::string& sSceneUUID, const std::string& sItemUUID)
    : m_pMeshHandler(pMeshHandler),
    m_sSceneUUID(AMCCommon::CUtils::normalizeUUIDString(sSceneUUID)),
    m_sItemUUID(AMCCommon::CUtils::normalizeUUIDString(sItemUUID))
{

}

CMeshSceneItem::~CMeshSceneItem()
{

}


std::string CMeshSceneItem::GetItemUUID()
{
    return m_sItemUUID;
}

std::string CMeshSceneItem::GetSceneUUID()
{
    return m_sSceneUUID;
}


LibMCEnv::sModelDataTransform CMeshSceneItem::GetTransform()
{
    auto pScene = m_pMeshHandler->findScene(m_sSceneUUID, true);
    auto pItem = pScene->findItem(m_sItemUUID, true);

    return pItem->getTransform();
}

void CMeshSceneItem::UpdateTransform(const LibMCEnv::sModelDataTransform AbsoluteTransform)
{
    auto pScene = m_pMeshHandler->findScene(m_sSceneUUID, true);
    auto pItem = pScene->findItem(m_sItemUUID, true);

    pItem->updateTransform(AbsoluteTransform);
}

IPersistentMeshObject * CMeshSceneItem::GetMeshObject()
{
    auto pScene = m_pMeshHandler->findScene(m_sSceneUUID, true);
    auto pItem = pScene->findItem(m_sItemUUID, true);

    std::string sMeshEntityUUID = pItem->getMeshEntityUUID();
    auto pMeshEntity = m_pMeshHandler->findMeshEntity(sMeshEntityUUID, true);

    return new CPersistentMeshObject(m_pMeshHandler, pMeshEntity->getUUID());
}

bool CMeshSceneItem::ReferenceIsValid()
{
    auto pScene = m_pMeshHandler->findScene(m_sSceneUUID, false);
    if (pScene == nullptr)
        return false;

    auto pItem = pScene->findItem(m_sItemUUID, true);
    if (pItem == nullptr)
        return false;

    std::string sMeshEntityUUID = pItem->getMeshEntityUUID();
    return m_pMeshHandler->hasMeshEntity(sMeshEntityUUID);
}

