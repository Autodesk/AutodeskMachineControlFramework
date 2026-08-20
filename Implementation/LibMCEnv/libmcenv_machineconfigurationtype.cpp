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


Abstract: This is a stub class definition of CMachineConfigurationType

*/

#include "libmcenv_machineconfigurationtype.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "libmcenv_machineconfigurationxsd.hpp"
#include "libmcenv_machineconfigurationxsditerator.hpp"
#include "libmcenv_machineconfigurationversion.hpp"
#include "libmcenv_machineconfigurationversioniterator.hpp"
#include "libmcenv_machineconfiguration.hpp"
#include "libmcenv_xmldocument.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CMachineConfigurationType 
**************************************************************************************************************************/
CMachineConfigurationType::CMachineConfigurationType(LibMCData::PMachineConfigurationType pMachineConfigurationType)
	: m_pMachineConfigurationType (pMachineConfigurationType)
{
	if (pMachineConfigurationType.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);	
}

CMachineConfigurationType::~CMachineConfigurationType()
{
}

std::string CMachineConfigurationType::GetUUID()
{
	return m_pMachineConfigurationType->GetUUID();
}

std::string CMachineConfigurationType::GetName()
{
	return m_pMachineConfigurationType->GetName();
}

std::string CMachineConfigurationType::GetSchemaType()
{
	return m_pMachineConfigurationType->GetSchemaType();
}

std::string CMachineConfigurationType::GetTimestamp()
{
	return m_pMachineConfigurationType->GetTimestamp();
}

IMachineConfigurationXSD* CMachineConfigurationType::GetLatestXSD()
{
	auto pConfigurationXSD = m_pMachineConfigurationType->GetLatestXSD();

	if (pConfigurationXSD.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationXSD(pConfigurationXSD);
}

IMachineConfigurationXSDIterator* CMachineConfigurationType::ListXSDVersions()
{
	auto pIterator = m_pMachineConfigurationType->ListXSDVersions();

	if (pIterator.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationXSDIterator(pIterator);	
}

LibMCEnv_uint32 CMachineConfigurationType::GetLatestXSDNumericVersion()
{
	return m_pMachineConfigurationType->GetLatestXSDNumericVersion();
}

IMachineConfigurationXSD* CMachineConfigurationType::RegisterNewXSD(const std::string& sXSDString, const LibMCEnv_uint32 nXSDVersion)
{
	auto pConfigurationXSD = m_pMachineConfigurationType->RegisterNewXSD(sXSDString, nXSDVersion);

	if (pConfigurationXSD.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationXSD(pConfigurationXSD);
}

IMachineConfigurationXSD* CMachineConfigurationType::RegisterXSDFromResource(const std::string& sXSDResourceName, const std::string& sDefaultXMLResourceName, const LibMCEnv_uint32 nXSDVersion, const bool bFailIfExisting)
{

	throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOTIMPLEMENTED, "RegisterXSDFromResource not implemented yet");

}

IMachineConfigurationVersion* CMachineConfigurationType::CreateDefaultConfiguration(const std::string & sXSDUUID, const std::string & sDefaultXML, const std::string & sTimeStampUTC)
{
	auto pDefaultConfiguration = m_pMachineConfigurationType->CreateDefaultConfiguration(sXSDUUID, sDefaultXML, sTimeStampUTC);

	if (pDefaultConfiguration.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationVersion(pDefaultConfiguration);
}

IMachineConfigurationXSD* CMachineConfigurationType::FindXSDByNumericVersion(const LibMCEnv_uint32 nXSDVersion)
{
	auto pXSD = m_pMachineConfigurationType->FindXSDByNumericVersion(nXSDVersion);

	if (pXSD.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationXSD(pXSD);	
}

IMachineConfigurationXSD* CMachineConfigurationType::FindXSDByUUID(const std::string& sXSDUUID)
{
	auto pXSD = m_pMachineConfigurationType->FindXSDByUUID(sXSDUUID);

	if (pXSD.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationXSD(pXSD);
}

IMachineConfigurationVersionIterator* CMachineConfigurationType::ListAllConfigurationVersions()
{
	auto pIterator = m_pMachineConfigurationType->ListAllConfigurationVersions();

	if (pIterator.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationVersionIterator(pIterator);
}

IMachineConfigurationVersionIterator* CMachineConfigurationType::ListConfigurationVersionsForXSD(const std::string& sXSDUUID)
{
	auto pIterator = m_pMachineConfigurationType->ListConfigurationVersionsForXSD(sXSDUUID);
	
	if (pIterator.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationVersionIterator(pIterator);
}

IMachineConfigurationVersion* CMachineConfigurationType::FindConfigurationVersionByUUID(const std::string& sVersionUUID)
{
	auto pConfiguration = m_pMachineConfigurationType->FindConfigurationVersionByUUID(sVersionUUID);

	if (pConfiguration.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationVersion(pConfiguration);
}

IMachineConfigurationVersion* CMachineConfigurationType::GetActiveConfigurationVersion()
{
	auto pMachineConfigurationVersion = m_pMachineConfigurationType->GetActiveConfigurationVersion();

	if (pMachineConfigurationVersion.get() == nullptr)
		return nullptr;

	return new CMachineConfigurationVersion(pMachineConfigurationVersion);
}

IMachineConfigurationVersion* CMachineConfigurationType::GetLatestConfigurationVersion()
{
	auto pLatestConfiguration = m_pMachineConfigurationType->GetLatestConfigurationVersion();

	if (pLatestConfiguration == nullptr)
		return nullptr;

	return new CMachineConfigurationVersion(pLatestConfiguration);
}

void CMachineConfigurationType::SetActiveConfigurationVersion(const std::string& sVersionUUID)
{
	m_pMachineConfigurationType->SetActiveConfigurationVersion(sVersionUUID);
}

LibMCEnv::Impl::IXMLDocument* CMachineConfigurationType::GetActiveConfigurationXML()
{
	auto pMachineConfigurationVersion = m_pMachineConfigurationType->GetActiveConfigurationVersion();

	if (pMachineConfigurationVersion.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOCONFIGURATIONVERSIONACTIVE, "no active configuration version for configuration type: " + GetName());

	std::string sXMLString = pMachineConfigurationVersion->GetConfigurationXMLString();

	auto pXMLDocumentInstance = std::make_shared<AMC::CXMLDocumentInstance>();
	pXMLDocumentInstance->parseXMLString(sXMLString);

	return new CXMLDocument(pXMLDocumentInstance);

}

LibMCEnv::Impl::IXMLDocument* CMachineConfigurationType::GetLatestConfigurationXML()
{
	auto pMachineConfigurationVersion = m_pMachineConfigurationType->GetLatestConfigurationVersion();

	if (pMachineConfigurationVersion.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOCONFIGURATIONVERSIONFOUND, "no configuration version for configuration type: " + GetName());

	std::string sXMLString = pMachineConfigurationVersion->GetConfigurationXMLString();

	auto pXMLDocumentInstance = std::make_shared<AMC::CXMLDocumentInstance>();
	pXMLDocumentInstance->parseXMLString(sXMLString);

	return new CXMLDocument(pXMLDocumentInstance);

}

IMachineConfiguration* CMachineConfigurationType::GetActiveConfiguration()
{
	auto pActiveVersion = m_pMachineConfigurationType->GetActiveConfigurationVersion();

	if (pActiveVersion.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOCONFIGURATIONVERSIONACTIVE, "no active configuration version for configuration type: " + GetName());

	return new CMachineConfiguration(m_pMachineConfigurationType, pActiveVersion);
}

IMachineConfiguration* CMachineConfigurationType::EnsureActiveConfiguration()
{
	auto pActiveVersion = m_pMachineConfigurationType->GetActiveConfigurationVersion();

	if (pActiveVersion.get() == nullptr) {
		// No active version yet: activate the latest one if any exists.
		auto pLatestVersion = m_pMachineConfigurationType->GetLatestConfigurationVersion();
		if (pLatestVersion.get() == nullptr)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOCONFIGURATIONVERSIONFOUND, "no configuration version for configuration type: " + GetName());

		m_pMachineConfigurationType->SetActiveConfigurationVersion(pLatestVersion->GetVersionUUID());
		pActiveVersion = pLatestVersion;
	}

	return new CMachineConfiguration(m_pMachineConfigurationType, pActiveVersion);
}

IMachineConfigurationXSD* CMachineConfigurationType::EnsureXSDVersion(const std::string& sXSDString, const LibMCEnv_uint32 nXSDVersion, const std::string& sDefaultXML)
{
	auto nLatestXSDVersion = m_pMachineConfigurationType->GetLatestXSDNumericVersion();

	// Only register if the given version is newer than the latest registered XSD.
	if (nLatestXSDVersion >= nXSDVersion)
		return nullptr;

	auto pXSD = m_pMachineConfigurationType->RegisterNewXSD(sXSDString, nXSDVersion);
	if (pXSD.get() == nullptr)
		return nullptr;

	m_pMachineConfigurationType->CreateDefaultConfiguration(pXSD->GetUUID(), sDefaultXML, pXSD->GetTimestamp());

	return new CMachineConfigurationXSD(pXSD);
}
