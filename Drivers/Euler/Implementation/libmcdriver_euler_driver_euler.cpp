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


Abstract: This is a stub class definition of CDriver_Euler

*/

#include "libmcdriver_euler_driver_euler.hpp"
#include "libmcdriver_euler_interfaceexception.hpp"
#include "libmcdriver_euler_eulerconnection.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_Euler::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Euler 
**************************************************************************************************************************/


CDriver_Euler::CDriver_Euler(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment, eulerDriverType driverType)
	: m_sName(sName), m_pDriverEnvironment(pDriverEnvironment), m_DriverType(driverType)
{
	m_sApplicationName = "AMCF_LibEuler";
	m_sApplicationVersion = __STRINGIZE_VALUE_OF(__GITHASH);
}

CDriver_Euler::~CDriver_Euler()
{
	m_pLibEulerSDK = nullptr;
	m_pLibEulerDLLFile = nullptr;
	m_pWorkingDirectory = nullptr;
}

void CDriver_Euler::Configure(const std::string& sConfigurationString)
{


}

std::string CDriver_Euler::GetName()
{
	return m_sName;
}

std::string CDriver_Euler::GetType()
{
	switch (m_DriverType) {
		case eulerDriverType::libEuler_0_1:
			return "libeuler-0.1";
		default:
			return "unknown";
	}
	
}

void CDriver_Euler::GetVersion(LibMCDriver_Euler_uint32& nMajor, LibMCDriver_Euler_uint32& nMinor, LibMCDriver_Euler_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_EULER_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_EULER_VERSION_MINOR;	
	nMicro = LIBMCDRIVER_EULER_VERSION_MICRO;	
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_Euler::QueryParameters()
{
}

void CDriver_Euler::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
}


void CDriver_Euler::SetCustomSDKResource(const std::string & sResourceName)
{
	m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, m_LibEulerDLLData);
}

void CDriver_Euler::SetApplicationDetails(const std::string & sApplicationName, const std::string & sApplicationVersion)
{
	m_sApplicationName = sApplicationName;
	m_sApplicationVersion = sApplicationVersion;
}

IEulerConnection * CDriver_Euler::Connect(const std::string & sIdentifier, const std::string & sBaseURL, const std::string & sAPIKey, const std::string & sDeviceID)
{
	if (sIdentifier.empty ())
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDIDENTIFIER);

	auto iIter = m_Connections.find(sIdentifier);
	if (iIter != m_Connections.end ())
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_CONNECTIONIDENTIFIERALREADYINUSE, "connection identifier already in use: " + sIdentifier);

	LoadSDK();

	auto pInstance = std::make_shared<CEulerConnectionInstance>(m_pLibEulerSDK, sIdentifier, m_sApplicationName, m_sApplicationVersion, sBaseURL, sAPIKey, sDeviceID);
	m_Connections.insert(std::make_pair (pInstance->getIdentifier (), pInstance));

	return new CEulerConnection(pInstance);

}

IEulerConnection* CDriver_Euler::ConnectWithLicenseData(const std::string& sIdentifier, const LibMCDriver_Euler_uint64 nLicenseDataBufferSize, const LibMCDriver_Euler_uint8* pLicenseDataBuffer, const std::string& sDeviceID)
{
	if (pLicenseDataBuffer == nullptr)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDPARAM);
	if (nLicenseDataBufferSize == 0)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDPARAM);

	LoadSDK();

	auto pLicenseTempFile = m_pWorkingDirectory->StoreCustomDataInTempFile("lic", LibMCEnv::CInputVector<uint8_t>(pLicenseDataBuffer, nLicenseDataBufferSize));
	std::string sLicenseFilePath = pLicenseTempFile->GetAbsoluteFileName();

	auto licenseInfo = m_pLibEulerSDK->euler_connect_read_license(sLicenseFilePath.c_str ());
	if (licenseInfo == nullptr)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTREADLICENSEDATA);

	std::string sBaseURL;
	if (licenseInfo->m_pszBaseURL != nullptr)
		sBaseURL = std::string(licenseInfo->m_pszBaseURL);

	std::string sAPIKey;
	if (licenseInfo->m_pszAPIKey != nullptr)
		sAPIKey = std::string(licenseInfo->m_pszAPIKey);

	std::string sErrorMessage;
	if (licenseInfo->m_pszErrorMessage != nullptr)
		sErrorMessage = std::string(licenseInfo->m_pszErrorMessage);

	bool bSuccess = (licenseInfo->m_pszBaseURL != nullptr) && (licenseInfo->m_pszAPIKey != nullptr);

	m_pLibEulerSDK->euler_connect_free_license_info(licenseInfo);

	if (!bSuccess) 
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDLICENSEDATA, "invalid euler license data:" + sErrorMessage);
	
	return Connect(sIdentifier, sBaseURL, sAPIKey, sDeviceID);
}

IEulerConnection* CDriver_Euler::ConnectWithLicenseResource(const std::string& sIdentifier, const std::string& sLicenseResourceName, const std::string& sDeviceID)
{
	std::vector<uint8_t> resourceData;
	m_pDriverEnvironment->RetrieveMachineResourceData(sLicenseResourceName, resourceData);

	if (resourceData.size() == 0)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDLICENSERESOURCE, "license resource is empty: " + sLicenseResourceName);

	return ConnectWithLicenseData(sIdentifier, resourceData.size(), resourceData.data(), sDeviceID);

}


IEulerConnection * CDriver_Euler::FindConnection(const std::string & sIdentifier)
{
	auto iIter = m_Connections.find(sIdentifier);
	if (iIter == m_Connections.end())
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_CONNECTIONIDENTIFIERNOTFOUND, "connection identifier not found: " + sIdentifier);

	return new CEulerConnection(iIter->second);
}

bool CDriver_Euler::ConnectionExists(const std::string & sIdentifier)
{
	auto iIter = m_Connections.find(sIdentifier);
	return (iIter != m_Connections.end());
}

void CDriver_Euler::CloseAllConnections()
{
	for (auto iIter : m_Connections)
		iIter.second->closeConnection();

	m_Connections.clear();
}

void CDriver_Euler::LoadSDK()
{
	if (m_LibEulerDLLData.empty())
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_LIBEULERDLLNOTSPECIFIED);

	if (m_pWorkingDirectory.get() == nullptr)
		m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

	if (m_pLibEulerSDK.get() == nullptr) {
		m_pLibEulerDLLFile = m_pWorkingDirectory->StoreCustomData("libeuler.dll", m_LibEulerDLLData);

		m_pLibEulerSDK = std::make_shared<CLibEulerSDK>(m_pLibEulerDLLFile->GetAbsoluteFileName());
	}
}


