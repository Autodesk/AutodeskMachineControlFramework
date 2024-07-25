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


Abstract: This is a stub class definition of CModelDataMeshInstance

*/

#include "libmcenv_modeldatameshinstance.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_meshobject.hpp"
#include "libmcenv_persistentmeshobject.hpp"

// Include custom headers here.
#include "common_utils.hpp"

#include "amc_meshutils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CModelDataMeshInstance 
**************************************************************************************************************************/

CModelDataMeshInstance::CModelDataMeshInstance(Lib3MF::PModel pModel, Lib3MF::PMeshObject p3MFObject, LibMCEnv::sModelDataTransform transform, AMC::PMeshHandler pMeshHandler)
    : m_pMeshHandler (pMeshHandler), m_pModel (pModel), m_LocalTransform (transform), m_ParentTransform (transform)
{
    if (pModel.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pMeshHandler.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (p3MFObject.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_sName = p3MFObject->GetName();

    bool bHasUUID = false;
    std::string sObjectUUID = p3MFObject->GetUUID(bHasUUID);
    if (bHasUUID) {
        m_sUUID = AMCCommon::CUtils::normalizeUUIDString(sObjectUUID);
    } else {
        m_sUUID = AMCCommon::CUtils::createUUID();
    }

    m_pMeshObject = p3MFObject;
}

CModelDataMeshInstance::~CModelDataMeshInstance()
{
    m_pMeshObject = nullptr;
    m_pModel = nullptr;
}



std::string CModelDataMeshInstance::GetName()
{
    return m_sName;
}

std::string CModelDataMeshInstance::GetUUID()
{
    return m_sUUID;
}

LibMCEnv::sModelDataTransform CModelDataMeshInstance::GetLocalTransform()
{
    return m_LocalTransform;
}

LibMCEnv::sModelDataTransform CModelDataMeshInstance::GetAbsoluteTransform()
{
    return AMC::CMeshUtils::multiplyTransforms(m_ParentTransform, m_LocalTransform);
}


IMeshObject * CModelDataMeshInstance::CreateCopiedMesh()
{
    std::string sUUID = AMCCommon::CUtils::createUUID();

    auto pMeshEntity = std::make_shared<AMC::CMeshEntity>(sUUID, m_sName);

    pMeshEntity->loadFrom3MF(m_pMeshObject.get());

    return new CMeshObject(m_pMeshHandler, pMeshEntity);
}

IPersistentMeshObject * CModelDataMeshInstance::CreatePersistentMesh(const bool bBoundToLoginSession)
{
    std::string sUUID = AMCCommon::CUtils::createUUID();

    auto pMeshEntity = std::make_shared<AMC::CMeshEntity>(sUUID, m_sName);

    pMeshEntity->loadFrom3MF(m_pMeshObject.get());

    m_pMeshHandler->registerEntity(pMeshEntity);

    return new CPersistentMeshObject(m_pMeshHandler, pMeshEntity->getUUID ());
    

}

