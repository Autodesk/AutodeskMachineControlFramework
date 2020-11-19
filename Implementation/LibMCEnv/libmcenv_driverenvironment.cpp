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


Abstract: This is a stub class definition of CDriverEnvironment

*/

#include "libmcenv_driverenvironment.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_workingdirectory.hpp"

// Include custom headers here.
#include "common_utils.hpp"


// Include custom headers here.


using namespace LibMCEnv::Impl;
/*************************************************************************************************************************
 Class definition of CDriverEnvironment
**************************************************************************************************************************/

CDriverEnvironment::CDriverEnvironment(AMC::PParameterGroup pParameterGroup, AMC::PResourcePackage pResourcePackage, const std::string& sBasePath)
    : m_bIsInitializing(false), m_pParameterGroup(pParameterGroup), m_pResourcePackage (pResourcePackage), m_sBasePath(sBasePath)
{
    if (pParameterGroup.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (pParameterGroup.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
    if (sBasePath.empty ())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

IWorkingDirectory* CDriverEnvironment::CreateWorkingDirectory()
{
    return new CWorkingDirectory(m_sBasePath, m_pResourcePackage);
}

void CDriverEnvironment::RetrieveDriverData(const std::string& sIdentifier, LibMCEnv_uint64 nDataBufferBufferSize, LibMCEnv_uint64* pDataBufferNeededCount, LibMCEnv_uint8* pDataBufferBuffer)
{
    auto pEntry = m_pResourcePackage->findEntryByName(sIdentifier, false);
    if (pEntry.get () == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_RESOURCEENTRYNOTFOUND);

    size_t nSize = pEntry->getSize();
    if (pDataBufferNeededCount != nullptr)
        *pDataBufferNeededCount = nSize;

    if (pDataBufferBuffer != nullptr) {
        if (nDataBufferBufferSize < nSize)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

        std::vector <uint8_t> Buffer;
        if (Buffer.size () != nSize)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INTERNALERROR);

        m_pResourcePackage->readEntry(sIdentifier, Buffer);
        const uint8_t* pSrc = Buffer.data();
        uint8_t* pDst = pDataBufferBuffer;

        for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
            *pDst = *pSrc;
            pDst++;
            pSrc++;
        }

    }

}

void CDriverEnvironment::RegisterStringParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewStringParameter(sParameterName, sDescription, sDefaultValue);
}

void CDriverEnvironment::RegisterUUIDParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewStringParameter(sParameterName, sDescription, AMCCommon::CUtils::normalizeUUIDString(sDefaultValue));
}

void CDriverEnvironment::RegisterDoubleParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_double dDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewDoubleParameter(sParameterName, sDescription, dDefaultValue, 1.0);
}

void CDriverEnvironment::RegisterIntegerParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCEnv_int64 nDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewIntParameter(sParameterName, sDescription, nDefaultValue);
}

void CDriverEnvironment::RegisterBoolParameter(const std::string& sParameterName, const std::string& sDescription, const bool bDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewBoolParameter(sParameterName, sDescription, bDefaultValue);
}

void CDriverEnvironment::SetStringParameter(const std::string& sParameterName, const std::string& sValue)
{
    m_pParameterGroup->setParameterValueByName(sParameterName, sValue);
}

void CDriverEnvironment::SetUUIDParameter(const std::string& sParameterName, const std::string& sValue)
{
    m_pParameterGroup->setParameterValueByName(sParameterName, AMCCommon::CUtils::normalizeUUIDString(sValue));
}

void CDriverEnvironment::SetDoubleParameter(const std::string& sParameterName, const LibMCEnv_double dValue)
{
    m_pParameterGroup->setDoubleParameterValueByName(sParameterName, dValue);
}

void CDriverEnvironment::SetIntegerParameter(const std::string& sParameterName, const LibMCEnv_int64 nValue)
{
    m_pParameterGroup->setIntParameterValueByName(sParameterName, nValue);
}

void CDriverEnvironment::SetBoolParameter(const std::string& sParameterName, const bool bValue)
{
    m_pParameterGroup->setBoolParameterValueByName(sParameterName, bValue);
}


void CDriverEnvironment::setIsInitializing(bool bIsInitializing)
{
    m_bIsInitializing = bIsInitializing;
}
