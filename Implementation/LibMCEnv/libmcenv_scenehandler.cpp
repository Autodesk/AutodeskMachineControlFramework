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


Abstract: This is a stub class definition of CSceneHandler

*/

#include "libmcenv_scenehandler.hpp"
#include "libmcenv_meshscene.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_persistentmeshobject.hpp"
#include "libmcenv_modeldatacomponentinstance.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CSceneHandler 
**************************************************************************************************************************/

CSceneHandler::CSceneHandler(AMC::PMeshHandler pMeshHandler, Lib3MF::PWrapper pLib3MFWrapper, AMC::PResourcePackage pCoreResourcePackage)
    : m_pMeshHandler (pMeshHandler), m_pLib3MFWrapper (pLib3MFWrapper), m_pCoreResourcePackage(pCoreResourcePackage)
{
    if (pMeshHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pLib3MFWrapper.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pCoreResourcePackage.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CSceneHandler::~CSceneHandler()
{

}

bool CSceneHandler::MeshIsPersistent(const std::string & sMeshUUID)
{
    return m_pMeshHandler->hasMeshEntity(sMeshUUID);
}

IPersistentMeshObject * CSceneHandler::FindPersistentMesh(const std::string & sMeshUUID)
{
    auto pMeshEntity = m_pMeshHandler->findMeshEntity(sMeshUUID, false);

    if (pMeshEntity.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MESHISNOTREGISTERED, "mesh is not registered: " + sMeshUUID);

    return new CPersistentMeshObject(m_pMeshHandler, pMeshEntity->getUUID());
}

IMeshScene * CSceneHandler::CreateEmptyMeshScene(const bool bBoundToLoginSession)
{
    auto pScene = m_pMeshHandler->createEmptyScene(bBoundToLoginSession);
    return new CMeshScene(pScene, m_pMeshHandler);
}

void CSceneHandler::ReleaseMeshScene(IMeshScene* pSceneInstance)
{
    if (pSceneInstance == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_pMeshHandler->releaseScene(pSceneInstance->GetSceneUUID());
}

IModelDataComponentInstance * CSceneHandler::Load3MFFromResource(const std::string & sResourceName)
{

	m_pCoreResourcePackage->findEntryByName(sResourceName, true);

	return new CModelDataComponentInstance(m_pLib3MFWrapper.get(), m_pCoreResourcePackage.get(), sResourceName, m_pMeshHandler);

}

IModelDataComponentInstance * CSceneHandler::Load3MFFromMemory(const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8 * pDataBuffer)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

IModelDataComponentInstance * CSceneHandler::Load3MFFromStream(IStreamReader* pReaderInstance)
{
	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}

