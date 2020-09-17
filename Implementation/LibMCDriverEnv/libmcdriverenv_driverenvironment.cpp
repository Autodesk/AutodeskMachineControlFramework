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

#include "libmcdriverenv_driverenvironment.hpp"
#include "libmcdriverenv_interfaceexception.hpp"
#include "libmcdriverenv_workingdirectory.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCDriverEnv::Impl;

/*************************************************************************************************************************
 Class definition of CDriverEnvironment 
**************************************************************************************************************************/

CDriverEnvironment::CDriverEnvironment(AMC::PParameterGroup pParameterGroup)
    : m_bIsInitializing (false), m_pParameterGroup (pParameterGroup)
{
    if (pParameterGroup.get() == nullptr)
        throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_INVALIDPARAM);

}

IWorkingDirectory * CDriverEnvironment::CreateWorkingDirectory()
{
    return new CWorkingDirectory();
}

void CDriverEnvironment::RetrieveDriverData(const std::string & sIdentifier, LibMCDriverEnv_uint64 nDataBufferBufferSize, LibMCDriverEnv_uint64* pDataBufferNeededCount, LibMCDriverEnv_uint8 * pDataBufferBuffer)
{
	
}

void CDriverEnvironment::RegisterStringParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewStringParameter(sParameterName, sDescription, sDefaultValue);
}

void CDriverEnvironment::RegisterUUIDParameter(const std::string& sParameterName, const std::string& sDescription, const std::string& sDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewStringParameter(sParameterName, sDescription, AMCCommon::CUtils::normalizeUUIDString (sDefaultValue));
}

void CDriverEnvironment::RegisterDoubleParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCDriverEnv_double dDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewDoubleParameter(sParameterName, sDescription, dDefaultValue);
}

void CDriverEnvironment::RegisterIntegerParameter(const std::string& sParameterName, const std::string& sDescription, const LibMCDriverEnv_int64 nDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewIntParameter(sParameterName, sDescription, nDefaultValue);
}

void CDriverEnvironment::RegisterBoolParameter(const std::string& sParameterName, const std::string& sDescription, const bool bDefaultValue)
{
    if (!m_bIsInitializing)
        throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_DRIVERISNOTINITIALISING);

    m_pParameterGroup->addNewBoolParameter(sParameterName, sDescription, bDefaultValue);
}

void CDriverEnvironment::SetStringParameter(const std::string& sParameterName, const std::string& sValue)
{
    m_pParameterGroup->setParameterValueByName(sParameterName, sValue);
}

void CDriverEnvironment::SetUUIDParameter(const std::string& sParameterName, const std::string& sValue) 
{
    m_pParameterGroup->setParameterValueByName(sParameterName, AMCCommon::CUtils::normalizeUUIDString (sValue));
}

void CDriverEnvironment::SetDoubleParameter(const std::string& sParameterName, const LibMCDriverEnv_double dValue) 
{
    m_pParameterGroup->setDoubleParameterValueByName(sParameterName, dValue);
}

void CDriverEnvironment::SetIntegerParameter(const std::string& sParameterName, const LibMCDriverEnv_int64 nValue) 
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
