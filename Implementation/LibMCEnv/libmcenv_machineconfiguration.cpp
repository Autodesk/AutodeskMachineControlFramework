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


Abstract: This is a stub class definition of CMachineConfiguration

*/

#include "libmcenv_machineconfiguration.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmcenv_xmldocument.hpp"
#include "libmcenv_xmldocumentnode.hpp"
#include "libmcenv_machineconfigurationversion.hpp"

#include <memory>

using namespace LibMCEnv::Impl;

// Configuration leaf values are stored in the "value" attribute by convention.
#define MACHINECONFIGURATION_VALUEATTRIBUTE "value"

/*************************************************************************************************************************
 Class definition of CMachineConfiguration 
**************************************************************************************************************************/

CMachineConfiguration::CMachineConfiguration(LibMCData::PMachineConfigurationType pDataType, LibMCData::PMachineConfigurationVersion pBaseVersion)
    : m_pDataType(pDataType), m_pBaseVersion(pBaseVersion), m_bDirty(false)
{
    if (pBaseVersion.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_pXMLDocument = std::make_shared<AMC::CXMLDocumentInstance>();
    m_pXMLDocument->parseXMLString(pBaseVersion->GetConfigurationXMLString());

    auto pDefaultNamespace = m_pXMLDocument->GetDefaultNamespace();
    if (pDefaultNamespace.get() != nullptr)
        m_sDefaultNamespace = pDefaultNamespace->getNameSpaceName();
}

CMachineConfiguration::~CMachineConfiguration()
{
}

AMC::PXMLDocumentNodeInstance CMachineConfiguration::resolvePathNode(const std::string& sPath, bool bCreate)
{
    if (sPath.empty())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "empty configuration parameter path");

    auto pNameSpace = m_pXMLDocument->GetDefaultNamespace();
    auto pNode = m_pXMLDocument->GetRootNode();

    size_t nStart = 0;
    while (nStart <= sPath.length()) {
        size_t nPos = sPath.find('/', nStart);
        std::string sSegment = (nPos == std::string::npos) ? sPath.substr(nStart) : sPath.substr(nStart, nPos - nStart);

        if (!sSegment.empty()) {
            auto pChild = pNode->FindChild(pNameSpace.get(), sSegment, false);
            if (pChild.get() == nullptr) {
                if (!bCreate)
                    return nullptr;
                pChild = pNode->AddChild(pNameSpace, sSegment);
            }
            pNode = pChild;
        }

        if (nPos == std::string::npos)
            break;
        nStart = nPos + 1;
    }

    return pNode;
}

std::string CMachineConfiguration::GetVersionUUID()
{
    return m_pBaseVersion->GetVersionUUID();
}

LibMCEnv_uint32 CMachineConfiguration::GetNumericVersion()
{
    return m_pBaseVersion->GetNumericVersion();
}

std::string CMachineConfiguration::GetXSDUUID()
{
    return m_pBaseVersion->GetXSDUUID();
}

LibMCEnv::Impl::IXMLDocument * CMachineConfiguration::GetXMLDocument()
{
    return new CXMLDocument(m_pXMLDocument);
}

bool CMachineConfiguration::HasParameter(const std::string & sPath)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        return false;

    return pLeaf->HasAttribute(m_pXMLDocument->GetDefaultNamespace().get(), MACHINECONFIGURATION_VALUEATTRIBUTE);
}

std::string CMachineConfiguration::GetStringParameter(const std::string & sPath)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "configuration parameter not found: " + sPath);

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE);
}

LibMCEnv_int64 CMachineConfiguration::GetIntegerParameter(const std::string & sPath, const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "configuration parameter not found: " + sPath);

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeIntegerValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, nMinValue, nMaxValue);
}

LibMCEnv_double CMachineConfiguration::GetDoubleParameter(const std::string & sPath, const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "configuration parameter not found: " + sPath);

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeDoubleValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, dMinValue, dMaxValue);
}

bool CMachineConfiguration::GetBoolParameter(const std::string & sPath)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "configuration parameter not found: " + sPath);

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeBoolValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE);
}

std::string CMachineConfiguration::GetStringParameterDef(const std::string & sPath, const std::string & sDefaultValue)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        return sDefaultValue;

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeValueDef(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, sDefaultValue);
}

LibMCEnv_int64 CMachineConfiguration::GetIntegerParameterDef(const std::string & sPath, const LibMCEnv_int64 nMinValue, const LibMCEnv_int64 nMaxValue, const LibMCEnv_int64 nDefaultValue)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        return nDefaultValue;

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeIntegerValueDef(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, nMinValue, nMaxValue, nDefaultValue);
}

LibMCEnv_double CMachineConfiguration::GetDoubleParameterDef(const std::string & sPath, const LibMCEnv_double dMinValue, const LibMCEnv_double dMaxValue, const LibMCEnv_double dDefaultValue)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        return dDefaultValue;

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeDoubleValueDef(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, dMinValue, dMaxValue, dDefaultValue);
}

bool CMachineConfiguration::GetBoolParameterDef(const std::string & sPath, const bool bDefaultValue)
{
    auto pLeaf = resolvePathNode(sPath, false);
    if (pLeaf.get() == nullptr)
        return bDefaultValue;

    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    return leaf.GetAttributeBoolValueDef(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, bDefaultValue);
}

void CMachineConfiguration::SetStringParameter(const std::string & sPath, const std::string & sValue)
{
    auto pLeaf = resolvePathNode(sPath, true);
    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    if (leaf.HasAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE))
        leaf.SetAttributeValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, sValue);
    else
        leaf.AddAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, sValue);
    m_bDirty = true;
}

void CMachineConfiguration::SetIntegerParameter(const std::string & sPath, const LibMCEnv_int64 nValue)
{
    auto pLeaf = resolvePathNode(sPath, true);
    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    if (leaf.HasAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE))
        leaf.SetAttributeIntegerValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, nValue);
    else
        leaf.AddIntegerAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, nValue);
    m_bDirty = true;
}

void CMachineConfiguration::SetDoubleParameter(const std::string & sPath, const LibMCEnv_double dValue)
{
    auto pLeaf = resolvePathNode(sPath, true);
    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    if (leaf.HasAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE))
        leaf.SetAttributeDoubleValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, dValue);
    else
        leaf.AddDoubleAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, dValue);
    m_bDirty = true;
}

void CMachineConfiguration::SetBoolParameter(const std::string & sPath, const bool bValue)
{
    auto pLeaf = resolvePathNode(sPath, true);
    CXMLDocumentNode leaf(m_pXMLDocument, pLeaf);
    if (leaf.HasAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE))
        leaf.SetAttributeValue(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, bValue ? "true" : "false");
    else
        leaf.AddBoolAttribute(m_sDefaultNamespace, MACHINECONFIGURATION_VALUEATTRIBUTE, bValue);
    m_bDirty = true;
}

bool CMachineConfiguration::HasChanges()
{
    return m_bDirty;
}

IMachineConfigurationVersion * CMachineConfiguration::Commit(const std::string & sUserUUID)
{
    std::string sXMLString = m_pXMLDocument->SaveToString(false);

    auto pNewDataVersion = m_pBaseVersion->CreateNewVersion(sXMLString, sUserUUID);
    if (pNewDataVersion.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "could not create new configuration version");

    // Rebase the working copy on the newly committed version so subsequent commits chain correctly.
    m_pBaseVersion = pNewDataVersion;
    m_bDirty = false;

    return new CMachineConfigurationVersion(pNewDataVersion);
}

IMachineConfigurationVersion * CMachineConfiguration::CommitAndActivate(const std::string & sUserUUID)
{
    std::string sXMLString = m_pXMLDocument->SaveToString(false);

    auto pNewDataVersion = m_pBaseVersion->CreateNewVersion(sXMLString, sUserUUID);
    if (pNewDataVersion.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "could not create new configuration version");

    if (m_pDataType.get() != nullptr)
        m_pDataType->SetActiveConfigurationVersion(pNewDataVersion->GetVersionUUID());

    m_pBaseVersion = pNewDataVersion;
    m_bDirty = false;

    return new CMachineConfigurationVersion(pNewDataVersion);
}
