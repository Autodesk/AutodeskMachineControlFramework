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


Abstract: This is a stub class definition of CMeshObject

*/

#include "libmcenv_meshobject.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_persistentmeshobject.hpp"

using namespace LibMCEnv::Impl;

#include "common_utils.hpp"

/*************************************************************************************************************************
 Class definition of CMeshObject 
**************************************************************************************************************************/


CMeshObject::CMeshObject(AMC::PMeshHandler pMeshHandler, const std::string& sMeshUUID)
    : m_pMeshHandler (pMeshHandler), m_sMeshUUID (AMCCommon::CUtils::normalizeUUIDString (sMeshUUID))
{
    if (pMeshHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    if (!pMeshHandler->hasMeshEntity (sMeshUUID))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MESHISNOTREGISTERED, "mesh is not registered: " + sMeshUUID);

}


CMeshObject::CMeshObject(AMC::PMeshHandler pMeshHandler, AMC::PMeshEntity pNonPersistentMeshEntity)
    : m_pMeshHandler(pMeshHandler), m_pNonPersistentMeshEntity (pNonPersistentMeshEntity)
{
    if (pMeshHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pNonPersistentMeshEntity.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_sMeshUUID = pNonPersistentMeshEntity->getUUID();
}


CMeshObject::~CMeshObject()
{

}


std::string CMeshObject::GetName()
{
    return getMeshEntity()->getName();
}

std::string CMeshObject::GetUUID()
{
    return m_sMeshUUID;
}

LibMCEnv_uint32 CMeshObject::GetTriangleCount()
{
    return (uint32_t)getMeshEntity()->getFaceCount();
}

LibMCEnv_uint32 CMeshObject::GetVertexCount()
{
    return (uint32_t)getMeshEntity()->getNodeCount();
}


AMC::PMeshEntity CMeshObject::getMeshEntity()
{
    if (m_pNonPersistentMeshEntity.get() != nullptr)
        return m_pNonPersistentMeshEntity;

    auto pMeshEntity = m_pMeshHandler->findMeshEntity(m_sMeshUUID, false);
    if (pMeshEntity.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MESHISNOTREGISTERED, "mesh is not registered: " + m_sMeshUUID);

    return pMeshEntity;
}


bool CMeshObject::IsManifold()
{
    return getMeshEntity()->isManifold ();
}

bool CMeshObject::IsOriented()
{
    return getMeshEntity()->isOriented();
}

bool CMeshObject::IsWatertight()
{
    auto pMeshEntity = getMeshEntity();
    return (pMeshEntity->isOriented() && pMeshEntity->isManifold());
}

LibMCEnv_uint32 CMeshObject::GetMaxVertexID()
{
    return getMeshEntity()->getMaxVertexID();
}

bool CMeshObject::VertexExists(const LibMCEnv_uint32 nVertexID)
{
    return getMeshEntity()->vertexExists(nVertexID);
}

bool CMeshObject::GetVertex(const LibMCEnv_uint32 nVertexID, LibMCEnv_double& dX, LibMCEnv_double& dY, LibMCEnv_double& dZ)
{
    return getMeshEntity()->getVertex(nVertexID, dX, dY, dZ);
}

void CMeshObject::GetVertexIDs(LibMCEnv_uint64 nVertexIDsBufferSize, LibMCEnv_uint64* pVertexIDsNeededCount, LibMCEnv_uint32* pVertexIDsBuffer)
{
    getMeshEntity()->getVertexIDs(nVertexIDsBufferSize, pVertexIDsNeededCount, pVertexIDsBuffer);
}

void CMeshObject::GetAllVertices(LibMCEnv_uint64 nVerticesBufferSize, LibMCEnv_uint64* pVerticesNeededCount, LibMCEnv::sMeshVertex3D* pVerticesBuffer)
{
    getMeshEntity()->getAllVertices(nVerticesBufferSize, pVerticesNeededCount, pVerticesBuffer);
}


LibMCEnv_uint32 CMeshObject::GetMaxTriangleID()
{
    return getMeshEntity()->getMaxTriangleID();
}

bool CMeshObject::TriangeExists(const LibMCEnv_uint32 nTriangleID)
{
    return getMeshEntity()->triangeExists(nTriangleID);
}

bool CMeshObject::GetTriangle(const LibMCEnv_uint32 nTriangleID, LibMCEnv_uint32& nVertex1ID, LibMCEnv_uint32& nVertex2ID, LibMCEnv_uint32& nVertex3ID)
{
    return getMeshEntity()->getTriangle(nTriangleID, nVertex1ID, nVertex2ID, nVertex3ID);
}

void CMeshObject::GetTriangleIDs(LibMCEnv_uint64 nTriangleIDsBufferSize, LibMCEnv_uint64* pTriangleIDsNeededCount, LibMCEnv_uint32* pTriangleIDsBuffer)
{
    getMeshEntity()->getTriangleIDs(nTriangleIDsBufferSize, pTriangleIDsNeededCount, pTriangleIDsBuffer);
}

void CMeshObject::GetAllTriangles(LibMCEnv_uint64 nTrianglesBufferSize, LibMCEnv_uint64* pTrianglesNeededCount, LibMCEnv::sMeshTriangle3D* pTrianglesBuffer)
{
    getMeshEntity()->getAllTriangles(nTrianglesBufferSize, pTrianglesNeededCount, pTrianglesBuffer);
}

bool CMeshObject::IsPersistent()
{
    return (m_pNonPersistentMeshEntity.get() == nullptr);
}

IPersistentMeshObject* CMeshObject::MakePersistent(const bool bBoundToLoginSession)
{
    if (m_pNonPersistentMeshEntity.get() != nullptr) {
        auto pMeshEntityToRegister = m_pNonPersistentMeshEntity;
        m_pNonPersistentMeshEntity = nullptr;

        m_pMeshHandler->registerEntity(pMeshEntityToRegister);

        return new CPersistentMeshObject(m_pMeshHandler, m_sMeshUUID);
    }
    else {
        return new CPersistentMeshObject(m_pMeshHandler, m_sMeshUUID);
    }
}
