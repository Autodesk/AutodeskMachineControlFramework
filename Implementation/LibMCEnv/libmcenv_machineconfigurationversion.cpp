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


Abstract: This is a stub class definition of CMachineConfigurationVersion

*/

#include "libmcenv_machineconfigurationversion.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_xmldocument.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CMachineConfigurationVersion 
**************************************************************************************************************************/

CMachineConfigurationVersion::CMachineConfigurationVersion(LibMCData::PMachineConfigurationVersion pMachineConfigurationVersion)
    : m_pMachineConfigurationVersion(pMachineConfigurationVersion)
{
    if (pMachineConfigurationVersion.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);
}

CMachineConfigurationVersion::~CMachineConfigurationVersion()
{
}

std::string CMachineConfigurationVersion::GetVersionUUID()
{
    return m_pMachineConfigurationVersion->GetVersionUUID();
}

std::string CMachineConfigurationVersion::GetXSDUUID()
{
    return m_pMachineConfigurationVersion->GetXSDUUID();
}

LibMCEnv_uint32 CMachineConfigurationVersion::GetNumericVersion()
{
    return m_pMachineConfigurationVersion->GetNumericVersion();
}

std::string CMachineConfigurationVersion::GetParentUUID()
{
    return m_pMachineConfigurationVersion->GetParentUUID();
}

std::string CMachineConfigurationVersion::GetConfigurationXMLString()
{
    return m_pMachineConfigurationVersion->GetConfigurationXMLString();
}

LibMCEnv::Impl::IXMLDocument* CMachineConfigurationVersion::GetConfigurationXML()
{
    auto pXMLDocumentInstance = std::make_shared<AMC::CXMLDocumentInstance>();
    pXMLDocumentInstance->parseXMLString(m_pMachineConfigurationVersion->GetConfigurationXMLString());

	return new CXMLDocument(pXMLDocumentInstance);
}


std::string CMachineConfigurationVersion::GetUserUUID()
{
    return m_pMachineConfigurationVersion->GetUserUUID();
}

std::string CMachineConfigurationVersion::GetTimestamp()
{
    return m_pMachineConfigurationVersion->GetTimestamp();
}

IMachineConfigurationVersion* CMachineConfigurationVersion::CreateNewVersion(const std::string& sXMLString, const std::string& sUserUUID)
{
    auto pNewConfigurationVersion = m_pMachineConfigurationVersion->CreateNewVersion(sXMLString, sUserUUID);

    if (pNewConfigurationVersion.get() == nullptr)
        return nullptr;

    return new CMachineConfigurationVersion(pNewConfigurationVersion);
}

IMachineConfigurationVersion* CMachineConfigurationVersion::MigrateToNewXSD(IMachineConfigurationXSD* pNewXSD, const std::string& sXMLString, const std::string& sUserUUID)
{
    throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED);
}