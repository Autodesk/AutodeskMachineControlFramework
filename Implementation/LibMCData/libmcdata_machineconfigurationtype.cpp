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

#include "libmcdata_machineconfigurationtype.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"
#include "common_chrono.hpp"

#include "libmcdata_machineconfigurationxsd.hpp"
#include "libmcdata_machineconfigurationversion.hpp"
#include "libmcdata_machineconfigurationversioniterator.hpp"
#include "libmcdata_machineconfigurationtypeiterator.hpp"

#include <memory>

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CMachineConfigurationType 
**************************************************************************************************************************/

CMachineConfigurationType* CMachineConfigurationType::makeFrom(CMachineConfigurationType* pMachineConfigurationType)
{
	if (pMachineConfigurationType == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	return new CMachineConfigurationType(
		pMachineConfigurationType->GetSQLHandler(),
		pMachineConfigurationType->GetUUID(),
		pMachineConfigurationType->GetName(),
		pMachineConfigurationType->GetSchemaType(),
		pMachineConfigurationType->GetTimestamp());
}

CMachineConfigurationType* CMachineConfigurationType::makeBySchema(AMCData::PSQLHandler pSQLHandler, const std::string& sSchemaType)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	if (sSchemaType.empty())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "Machine configuration Schema Type is empty.");

	auto pStatement = pSQLHandler->prepareStatement(
		"SELECT uuid, name, schematype, timestamp FROM machineconfiguration_types WHERE schematype = ?");

	pStatement->setString(1, sSchemaType);
	if (pStatement->nextRow()) {
		std::string sDBUUID = pStatement->getColumnString(1);
		std::string sDBName = pStatement->getColumnString(2);
		std::string sDBSchemaType = pStatement->getColumnString(3);
		std::string sDBTimestamp = pStatement->getColumnString(4);

		return new CMachineConfigurationType(pSQLHandler, sDBUUID, sDBName, sDBSchemaType, sDBTimestamp);
	}
	else {
		return nullptr;
	}
}

CMachineConfigurationType* CMachineConfigurationType::makeByUUID(AMCData::PSQLHandler pSQLHandler, const std::string& sUUID)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	if (sUUID.empty())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM, "Machine configuration Type UUID is empty.");

	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

	auto pStatement = pSQLHandler->prepareStatement(
		"SELECT uuid, name, schematype, timestamp FROM machineconfiguration_types WHERE uuid = ?");

	pStatement->setString(1, sNormalizedUUID);

	if (!pStatement->nextRow())
		return nullptr; // Type not found

	std::string sDBUUID = pStatement->getColumnString(1);
	std::string sDBName = pStatement->getColumnString(2);
	std::string sDBSchemaType = pStatement->getColumnString(3);
	std::string sDBTimestamp = pStatement->getColumnString(4);

	return new CMachineConfigurationType(pSQLHandler, sDBUUID, sDBName, sDBSchemaType, sDBTimestamp);
}

CMachineConfigurationType* CMachineConfigurationType::createNewConfigurationType(AMCData::PSQLHandler pSQLHandler, const std::string& sSchemaType, const std::string& sName)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	auto sNewCfgUUID = AMCCommon::CUtils::createUUID();

	AMCCommon::CChrono chrono;
	auto sTimeStamp = chrono.getUTCTimeInISO8601();

	auto pStatement = pSQLHandler->prepareStatement(
		"INSERT INTO machineconfiguration_types (uuid, name, schematype, activeversionuuid, timestamp) "
		"VALUES (?, ?, ?, ?, ?)");
	pStatement->setString(1, sNewCfgUUID);
	pStatement->setString(2, sName);
	pStatement->setString(3, sSchemaType);
	pStatement->setString(4, "00000000-0000-0000-0000-000000000000");
	pStatement->setString(5, sTimeStamp);

	pStatement->execute();
	pStatement = nullptr;

	return makeBySchema(pSQLHandler, sSchemaType);
}

IMachineConfigurationTypeIterator* CMachineConfigurationType::listRegisteredConfigurationTypes(AMCData::PSQLHandler pSQLHandler)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	auto pStatement = pSQLHandler->prepareStatement(
		"SELECT uuid, name, schematype, timestamp FROM machineconfiguration_types "
		"ORDER BY name ASC");

	auto pIterator = std::make_unique<CMachineConfigurationTypeIterator>();

	while (pStatement->nextRow()) {
		std::string sUUID = pStatement->getColumnString(1);
		std::string sName = pStatement->getColumnString(2);
		std::string sSchemaType = pStatement->getColumnString(3);
		std::string sTimestamp = pStatement->getColumnString(4);

		auto pType = std::make_shared<CMachineConfigurationType>(pSQLHandler, sUUID, sName, sSchemaType, sTimestamp);

		pIterator->AddType(pType);
	}

	return pIterator.release();
}

CMachineConfigurationType::CMachineConfigurationType(AMCData::PSQLHandler pSQLHandler, const std::string& sUUID, const std::string& sName, const std::string& sSchemaType, const std::string& sTimestamp)
: m_pSQLHandler (pSQLHandler), 
	m_sUUID (AMCCommon::CUtils::normalizeUUIDString(sUUID)),
	m_sName (sName),
	m_sSchemaType (sSchemaType),
	m_sTimestamp(sTimestamp)
{
	if (pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);
}

CMachineConfigurationType::~CMachineConfigurationType()
{
}

AMCData::PSQLHandler CMachineConfigurationType::GetSQLHandler()
{
	if (m_pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	return m_pSQLHandler;
}

std::string CMachineConfigurationType::GetUUID()
{
	return m_sUUID;
}

std::string CMachineConfigurationType::GetName()
{
	return m_sName;
}

std::string CMachineConfigurationType::GetSchemaType()
{
	return m_sSchemaType;
}

std::string CMachineConfigurationType::GetTimestamp()
{
	return m_sTimestamp;
}

IMachineConfigurationXSD* CMachineConfigurationType::GetLatestXSD()
{
	return CMachineConfigurationXSD::getLatestXSDForType(m_pSQLHandler, m_sUUID);
}

IMachineConfigurationXSDIterator* CMachineConfigurationType::ListXSDVersions()
{
	return CMachineConfigurationXSD::listConfigurationXSDVersions(m_pSQLHandler, m_sUUID);
}

LibMCData_uint32 CMachineConfigurationType::GetLatestXSDNumericVersion()
{	
	return CMachineConfigurationXSD::getLatestXSDNumericVersionForType(m_pSQLHandler, m_sUUID);
}

IMachineConfigurationXSD* CMachineConfigurationType::FindXSDByUUID(const std::string & sXSDUUID)
{
	return CMachineConfigurationXSD::findXSDByUUID(m_pSQLHandler, sXSDUUID);
}

IMachineConfigurationXSD* CMachineConfigurationType::FindXSDByNumericVersion(const LibMCData_uint32 nXSDNumericVersion)
{
	return CMachineConfigurationXSD::findXSDByNumericVersion(m_pSQLHandler, nXSDNumericVersion);
}

IMachineConfigurationXSD* CMachineConfigurationType::RegisterNewXSD(const std::string & sXSDString, const LibMCData_uint32 nXSDVersion)
{
	return CMachineConfigurationXSD::createNewXSDForType(m_pSQLHandler, m_sUUID, sXSDString, nXSDVersion);
}

IMachineConfigurationVersion* CMachineConfigurationType::CreateDefaultConfiguration(const std::string& sXSDUUID, const std::string& sDefaultXML, const std::string& sTimeStampUTC)
{
	return CMachineConfigurationVersion::createDefaultConfigurationForXSD(m_pSQLHandler, sXSDUUID, sDefaultXML, sTimeStampUTC);
}

IMachineConfigurationVersionIterator* CMachineConfigurationType::ListAllConfigurationVersions()
{	
	auto pReturnIterator = std::make_unique<CMachineConfigurationVersionIterator>();

	auto pXSDIterator = ListXSDVersions();
	while (pXSDIterator->MoveNext()) {
		auto pXSDVersion = pXSDIterator->GetCurrent();
		if (pXSDVersion != nullptr) {
			auto pVersionIterator = ListConfigurationVersionsForXSD(pXSDVersion->GetUUID());
			while (pVersionIterator->MoveNext()) {
				auto pConfigurationVersion = dynamic_cast<CMachineConfigurationVersion*>(pVersionIterator->GetCurrent());
				if (pConfigurationVersion != nullptr)
					pReturnIterator->AddVersion(CMachineConfigurationVersion::makeSharedFrom(pConfigurationVersion));
			}
		}
	}

	return pReturnIterator.release();
}

IMachineConfigurationVersionIterator* CMachineConfigurationType::ListConfigurationVersionsForXSD(const std::string& sXSDUUID)
{
	return CMachineConfigurationVersion::listConfigurationVersionsForXSD(m_pSQLHandler, sXSDUUID);
}

IMachineConfigurationVersion* CMachineConfigurationType::FindConfigurationVersionByUUID(const std::string & sVersionUUID)
{
	return CMachineConfigurationVersion::findConfigurationVersionByUUID(m_pSQLHandler, sVersionUUID);
}

IMachineConfigurationVersion* CMachineConfigurationType::GetActiveConfigurationVersion()
{
	if (m_pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	auto pStatement = m_pSQLHandler->prepareStatement(
		"SELECT activeversionuuid FROM machineconfiguration_types WHERE uuid = ?"
	);
	pStatement->setString(1, m_sUUID);

	if (!pStatement->nextRow())
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDMACHINECONFIGURATIONTYPE, m_sUUID);

	std::string sActiveVersionUUID = pStatement->getColumnString(1);
	sActiveVersionUUID = AMCCommon::CUtils::normalizeUUIDString(sActiveVersionUUID);

	if (sActiveVersionUUID.empty() || sActiveVersionUUID == "00000000-0000-0000-0000-000000000000")
		return nullptr;

	// Create instance of MachineConfigurationVersion from UUID
	return CMachineConfigurationVersion::findConfigurationVersionByUUID(m_pSQLHandler, sActiveVersionUUID);
}

IMachineConfigurationVersion* CMachineConfigurationType::GetLatestConfigurationVersion()
{
	// Wrap the returned raw pointer to avoid leaking it and to guard against a
	// type that has been registered but has no XSD version yet (returns null).
	std::unique_ptr<IMachineConfigurationXSD> pLatestXSD(CMachineConfigurationXSD::getLatestXSDForType(m_pSQLHandler, m_sUUID));
	if (pLatestXSD.get() == nullptr)
		return nullptr;

	return CMachineConfigurationVersion::getLatestConfigurationVersionForXSD(m_pSQLHandler, pLatestXSD->GetUUID());
}

void CMachineConfigurationType::SetActiveConfigurationVersion(const std::string& sVersionUUID)
{
	if (m_pSQLHandler.get() == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sVersionUUID);

	// Validate if the configuration version exists and matches this type
	auto pVersion = CMachineConfigurationVersion::findConfigurationVersionByUUID(m_pSQLHandler, sNormalizedUUID);
	if (pVersion == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDMACHINECONFIGURATIONVERSIONBYUUID, "Configuration version with UUID =  " + sVersionUUID + " is not found");

	// Ensure this version belongs to the current configuration type
	std::string sVersionXSDUUID = pVersion->GetXSDUUID();
	auto pXSD = CMachineConfigurationXSD::findXSDByUUID(m_pSQLHandler, sVersionXSDUUID);
	if (pXSD == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_COULDNOTFINDMACHINECONFIGURATIONXSDBYUUID, "Configuration XSD with UUID =  " + sVersionXSDUUID + " is not found");

	std::string sXSDTypeUUID = pXSD->GetTypeUUID();
	if (sXSDTypeUUID != m_sUUID)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_MACHINECONFIGURATIONTYPEMISMATCH, "Configuration XSD for requested version has type " + pXSD->GetTypeUUID() + " but current type is " + m_sUUID);

	// Perform the update
	auto pStatement = m_pSQLHandler->prepareStatement(
		"UPDATE machineconfiguration_types SET activeversionuuid = ? WHERE uuid = ?"
	);

	pStatement->setString(1, sNormalizedUUID);
	pStatement->setString(2, m_sUUID);
	pStatement->execute();
}