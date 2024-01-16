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
    auto pMeshEntity = m_pMeshHandler->findMeshEntity(m_sMeshUUID, false);
    if (pMeshEntity.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MESHISNOTREGISTERED, "mesh is not registered: " + m_sMeshUUID);

    return pMeshEntity;
}

