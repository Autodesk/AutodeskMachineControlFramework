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
#include "common_utils.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <variant>
#include <vector>

using namespace LibMCEnv::Impl;

namespace {

	struct SConfigurationParameterMapping {
		std::string m_sStateMachine;
		std::string m_sGroup;
		std::string m_sParameter;
		std::string m_sPath;
		std::string m_sDescription;
		std::string m_sDefaultValue;
		AMC::eParameterDataType m_DataType;
		AMC::PParameterGroup m_pGroup;
	};

	std::vector<std::string> splitConfigurationPath(const std::string& sPath)
	{
		std::vector<std::string> segments;
		size_t nStart = 0;
		while (nStart <= sPath.length()) {
			auto nSeparator = sPath.find('/', nStart);
			auto sSegment = (nSeparator == std::string::npos) ? sPath.substr(nStart) : sPath.substr(nStart, nSeparator - nStart);
			if (sSegment.empty() || !AMC::CXMLDocumentNodeInstance::checkXMLNodeName(sSegment))
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "invalid machine configuration path: " + sPath);
			segments.push_back(sSegment);
			if (nSeparator == std::string::npos)
				break;
			nStart = nSeparator + 1;
		}
		return segments;
	}

	std::vector<SConfigurationParameterMapping> collectConfigurationParameterMappings(AMC::PStateMachineData pStateMachineData, const std::string& sSchema)
	{
		if (pStateMachineData.get() == nullptr)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "machine configuration parameter synchronization is not available in this environment");

		std::vector<SConfigurationParameterMapping> mappings;
		std::map<std::string, std::string> pathOwners;

		for (auto& handlerEntry : pStateMachineData->getParameterHandlers()) {
			auto pHandler = handlerEntry.second;
			for (uint32_t nGroupIndex = 0; nGroupIndex < pHandler->getGroupCount(); nGroupIndex++) {
				auto pGroup = pHandler->getGroup(nGroupIndex);
				for (uint32_t nParameterIndex = 0; nParameterIndex < pGroup->getParameterCount(); nParameterIndex++) {
					std::string sParameterName, sDescription, sDefaultValue;
					pGroup->getParameterInfo(nParameterIndex, sParameterName, sDescription, sDefaultValue);

					std::string sMappingSchema, sMappingPath;
					if (!pGroup->getParameterConfigurationMapping(sParameterName, sMappingSchema, sMappingPath) || (sMappingSchema != sSchema))
						continue;

					splitConfigurationPath(sMappingPath);

					std::string sOwner = handlerEntry.first + "." + pGroup->getName() + "." + sParameterName;
					auto iOwner = pathOwners.find(sMappingPath);
					if (iOwner != pathOwners.end())
						throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "duplicate machine configuration path " + sSchema + "/" + sMappingPath + " mapped by " + iOwner->second + " and " + sOwner);
					pathOwners.insert(std::make_pair(sMappingPath, sOwner));

					SConfigurationParameterMapping mapping;
					mapping.m_sStateMachine = handlerEntry.first;
					mapping.m_sGroup = pGroup->getName();
					mapping.m_sParameter = sParameterName;
					mapping.m_sPath = sMappingPath;
					mapping.m_sDescription = sDescription;
					mapping.m_sDefaultValue = sDefaultValue;
					mapping.m_DataType = pGroup->getParameterDataTypeByName(sParameterName);
					mapping.m_pGroup = pGroup;
					mappings.push_back(mapping);
				}
			}
		}

		if (mappings.empty())
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "no state-machine parameters are mapped to machine configuration schema " + sSchema);

		for (auto& left : mappings) {
			for (auto& right : mappings) {
				if ((left.m_sPath.length() < right.m_sPath.length()) &&
					(right.m_sPath.compare(0, left.m_sPath.length(), left.m_sPath) == 0) &&
					(right.m_sPath[left.m_sPath.length()] == '/'))
					throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "machine configuration path may not be both a value and a parent path: " + left.m_sPath);
			}
		}

		return mappings;
	}

	size_t getXSDElementOrder(const std::string& sXSDString, const std::string& sElementName)
	{
		auto nDoubleQuoted = sXSDString.find("name=\"" + sElementName + "\"");
		auto nSingleQuoted = sXSDString.find("name='" + sElementName + "'");
		return (std::min)(nDoubleQuoted, nSingleQuoted);
	}

	void sortMappingsByXSDOrder(std::vector<SConfigurationParameterMapping>& mappings, const std::string& sXSDString)
	{
		std::stable_sort(mappings.begin(), mappings.end(), [&sXSDString](const SConfigurationParameterMapping& left, const SConfigurationParameterMapping& right) {
			auto leftSegments = splitConfigurationPath(left.m_sPath);
			auto rightSegments = splitConfigurationPath(right.m_sPath);
			auto nCommonCount = (std::min)(leftSegments.size(), rightSegments.size());
			for (size_t nIndex = 0; nIndex < nCommonCount; nIndex++) {
				if (leftSegments[nIndex] == rightSegments[nIndex])
					continue;
				auto nLeftOrder = getXSDElementOrder(sXSDString, leftSegments[nIndex]);
				auto nRightOrder = getXSDElementOrder(sXSDString, rightSegments[nIndex]);
				if (nLeftOrder != nRightOrder)
					return nLeftOrder < nRightOrder;
				return leftSegments[nIndex] < rightSegments[nIndex];
			}
			return leftSegments.size() < rightSegments.size();
		});
	}

	std::string buildConfigurationFromParameterDefaults(AMC::PStateMachineData pStateMachineData, const std::string& sSchema, const std::string& sTypeName, const std::string& sXSDString, uint32_t nXSDVersion)
	{
		auto mappings = collectConfigurationParameterMappings(pStateMachineData, sSchema);
		sortMappingsByXSDOrder(mappings, sXSDString);

		auto pDocument = std::make_shared<AMC::CXMLDocumentInstance>();
		pDocument->createEmptyDocument("Configuration", sSchema);
		auto pNameSpace = pDocument->GetDefaultNamespace();
		auto pRoot = pDocument->GetRootNode();
		pRoot->AddAttribute(pNameSpace, "Version", std::to_string(nXSDVersion) + ".0");
		pRoot->AddAttribute(pNameSpace, "TypeName", sTypeName);

		for (auto& mapping : mappings) {
			auto pNode = pRoot;
			for (auto& sSegment : splitConfigurationPath(mapping.m_sPath)) {
				auto pChild = pNode->FindChild(pNameSpace.get(), sSegment, false);
				if (pChild.get() == nullptr)
					pChild = pNode->AddChild(pNameSpace, sSegment);
				pNode = pChild;
			}
			pNode->AddAttribute(pNameSpace, "value", mapping.m_sDefaultValue);
			if (!mapping.m_sDescription.empty())
				pNode->AddAttribute(pNameSpace, "description", mapping.m_sDescription);
		}

		return pDocument->SaveToString(true);
	}

	using TConfigurationValue = std::variant<std::string, int64_t, double, bool>;

	struct SValidatedConfigurationValue {
		SConfigurationParameterMapping m_Mapping;
		TConfigurationValue m_Value;
	};

}

/*************************************************************************************************************************
 Class definition of CMachineConfigurationType 
**************************************************************************************************************************/
CMachineConfigurationType::CMachineConfigurationType(LibMCData::PMachineConfigurationType pMachineConfigurationType, AMC::PStateMachineData pStateMachineData)
	: m_pMachineConfigurationType (pMachineConfigurationType), m_pStateMachineData (pStateMachineData)
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

IMachineConfigurationXSD* CMachineConfigurationType::EnsureXSDVersionFromParameterDefaults(const std::string& sXSDString, const LibMCEnv_uint32 nXSDVersion)
{
	auto nLatestXSDVersion = m_pMachineConfigurationType->GetLatestXSDNumericVersion();
	if (nLatestXSDVersion >= nXSDVersion)
		return nullptr;

	auto sDefaultXML = buildConfigurationFromParameterDefaults(m_pStateMachineData, GetSchemaType(), GetName(), sXSDString, nXSDVersion);
	auto pXSD = m_pMachineConfigurationType->RegisterNewXSD(sXSDString, nXSDVersion);
	if (pXSD.get() == nullptr)
		return nullptr;

	m_pMachineConfigurationType->CreateDefaultConfiguration(pXSD->GetUUID(), sDefaultXML, pXSD->GetTimestamp());
	return new CMachineConfigurationXSD(pXSD);
}

LibMCEnv_uint32 CMachineConfigurationType::SynchronizeParameters()
{
	auto mappings = collectConfigurationParameterMappings(m_pStateMachineData, GetSchemaType());
	auto pActiveVersion = m_pMachineConfigurationType->GetActiveConfigurationVersion();
	if (pActiveVersion.get() == nullptr) {
		pActiveVersion = m_pMachineConfigurationType->GetLatestConfigurationVersion();
		if (pActiveVersion.get() == nullptr)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOCONFIGURATIONVERSIONFOUND, "no configuration version for configuration type: " + GetName());
		m_pMachineConfigurationType->SetActiveConfigurationVersion(pActiveVersion->GetVersionUUID());
	}

	CMachineConfiguration configuration(m_pMachineConfigurationType, pActiveVersion);
	std::vector<SValidatedConfigurationValue> values;
	values.reserve(mappings.size());

	for (auto& mapping : mappings) {
		SValidatedConfigurationValue value;
		value.m_Mapping = mapping;
		try {
			switch (mapping.m_DataType) {
			case AMC::eParameterDataType::String:
				value.m_Value = configuration.GetStringParameter(mapping.m_sPath);
				break;
			case AMC::eParameterDataType::UUID:
				value.m_Value = AMCCommon::CUtils::normalizeUUIDString(configuration.GetStringParameter(mapping.m_sPath));
				break;
			case AMC::eParameterDataType::Integer:
				value.m_Value = configuration.GetIntegerParameter(mapping.m_sPath, (std::numeric_limits<int64_t>::min)(), (std::numeric_limits<int64_t>::max)());
				break;
			case AMC::eParameterDataType::Double:
				value.m_Value = configuration.GetDoubleParameter(mapping.m_sPath, -(std::numeric_limits<double>::max)(), (std::numeric_limits<double>::max)());
				break;
			case AMC::eParameterDataType::Bool:
				value.m_Value = configuration.GetBoolParameter(mapping.m_sPath);
				break;
			default:
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "unsupported runtime parameter type");
			}
		}
		catch (std::exception& e) {
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "could not synchronize " + GetSchemaType() + "/" + mapping.m_sPath + " to " + mapping.m_sStateMachine + "." + mapping.m_sGroup + "." + mapping.m_sParameter + ": " + e.what());
		}
		values.push_back(value);
	}

	for (auto& value : values) {
		auto& mapping = value.m_Mapping;
		switch (mapping.m_DataType) {
		case AMC::eParameterDataType::String:
		case AMC::eParameterDataType::UUID:
			mapping.m_pGroup->setParameterValueByName(mapping.m_sParameter, std::get<std::string>(value.m_Value));
			break;
		case AMC::eParameterDataType::Integer:
			mapping.m_pGroup->setIntParameterValueByName(mapping.m_sParameter, std::get<int64_t>(value.m_Value));
			break;
		case AMC::eParameterDataType::Double:
			mapping.m_pGroup->setDoubleParameterValueByName(mapping.m_sParameter, std::get<double>(value.m_Value));
			break;
		case AMC::eParameterDataType::Bool:
			mapping.m_pGroup->setBoolParameterValueByName(mapping.m_sParameter, std::get<bool>(value.m_Value));
			break;
		default:
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "unsupported runtime parameter type");
		}
	}

	return (LibMCEnv_uint32)values.size();
}

IMachineConfigurationVersion* CMachineConfigurationType::ResetToParameterDefaults(const std::string& sUserUUID, const std::string& sName)
{
	auto sCheckedName = CMachineConfigurationVersion::checkVersionName(sName);
	auto pLatestXSD = m_pMachineConfigurationType->GetLatestXSD();
	if (pLatestXSD.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "cannot reset a configuration type without an XSD: " + GetName());

	auto sFactoryXML = buildConfigurationFromParameterDefaults(m_pStateMachineData, GetSchemaType(), GetName(), pLatestXSD->GetXSDString(), pLatestXSD->GetXSDVersion());

	LibMCData::PMachineConfigurationVersion pBaseVersion;
	auto pIterator = m_pMachineConfigurationType->ListConfigurationVersionsForXSD(pLatestXSD->GetUUID());
	while (pIterator->MoveNext()) {
		auto pCandidate = pIterator->GetCurrent();
		if ((pBaseVersion.get() == nullptr) || (pCandidate->GetNumericVersion() > pBaseVersion->GetNumericVersion()))
			pBaseVersion = pCandidate;
	}
	if (pBaseVersion.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_NOCONFIGURATIONVERSIONFOUND, "no configuration version exists for the latest XSD of " + GetName());

	auto pNewVersion = pBaseVersion->CreateNewVersion(sFactoryXML, sUserUUID, sCheckedName);
	if (pNewVersion.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM, "could not create factory-reset configuration version for " + GetName());

	m_pMachineConfigurationType->SetActiveConfigurationVersion(pNewVersion->GetVersionUUID());
	SynchronizeParameters();

	return new CMachineConfigurationVersion(pNewVersion);
}
