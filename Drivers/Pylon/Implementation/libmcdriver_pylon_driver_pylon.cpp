/*++

Copyright (C) 2025 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_Pylon

*/

#include "libmcdriver_pylon_driver_pylon.hpp"
#include "libmcdriver_pylon_interfaceexception.hpp"
#include "libmcdriver_pylon_pylondevice.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace LibMCDriver_Pylon::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Pylon 
**************************************************************************************************************************/

CDriver_Pylon::CDriver_Pylon(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName(sName), m_pDriverEnvironment(pDriverEnvironment)
{
	if (pDriverEnvironment.get () == nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDPARAM);

}

CDriver_Pylon::~CDriver_Pylon()
{
	unloadPylonSDK();
}


void CDriver_Pylon::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_Pylon::GetName()
{
	return m_sName;
}

std::string CDriver_Pylon::GetType()
{
	return "pylon-1.0";
}

void CDriver_Pylon::GetVersion(LibMCDriver_Pylon_uint32& nMajor, LibMCDriver_Pylon_uint32& nMinor, LibMCDriver_Pylon_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_PYLON_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_PYLON_VERSION_MINOR;
	nMicro = LIBMCDRIVER_PYLON_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_Pylon::QueryParameters()
{
}

void CDriver_Pylon::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
}


void CDriver_Pylon::SetCustomSDKResource(const std::string & sResourceName)
{
	if (m_pPylonSDK.get() != nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_SDKALREADYLOADED);

	m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, m_PylonCDLLData);
}


void CDriver_Pylon::unloadPylonSDK()
{
	m_pPylonSDK = nullptr;
	m_pPylonCDLLFile = nullptr;
	m_pWorkingDirectory = nullptr;
}


void CDriver_Pylon::loadPylonSDK()
{
	if (m_pPylonSDK.get() == nullptr) {
		unloadPylonSDK();

		try {

			if (m_PylonCDLLData.empty())
				throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_SDKRESOURCENOTSET);

			m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();
			m_pPylonCDLLFile = m_pWorkingDirectory->StoreCustomData("pylonC.dll", m_PylonCDLLData);

			m_pPylonSDK = std::make_shared<CLibPylonSDK>(m_pPylonCDLLFile->GetAbsoluteFileName());

			m_pPylonSDK->checkError(m_pPylonSDK->PylonInitialize());

		}
		catch (...) {
			unloadPylonSDK();
			throw;
		}
	}
}

void CDriver_Pylon::checkIdentifier(const std::string& sIdentifier)
{
	// Check if identifier is alphanumeric, non-empty and max 64 chars
	bool bValidIdentifier = (!sIdentifier.empty() && sIdentifier.length() <= 64);
	for (char c : sIdentifier) {
		if (!isalnum(c) && c != '_' && c != '-') {
			bValidIdentifier = false;
			break;
		}
	}

	if (!bValidIdentifier)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDIDENTIFIER, "connection identifier is not alphanumeric or too long: " + sIdentifier);
}

IPylonDevice * CDriver_Pylon::ConnectToUniqueGenericDevice(const std::string & sIdentifier)
{
	auto iIter = m_ConnectedDevices.find(sIdentifier);
	if (iIter != m_ConnectedDevices.end())
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_CONNECTIONIDENTIFIERALREADYINUSE, "connection identifier already in use: " + sIdentifier);

	checkIdentifier(sIdentifier);

	loadPylonSDK();

	size_t numDevices = 0;
	m_pPylonSDK->checkError(m_pPylonSDK->PylonEnumerateDevices(&numDevices));

	if (numDevices != 1)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_DEVICENOTUNIQUE, "number of devices is not unique: " + std::to_string(numDevices));

	libPylonDeviceHandle pDeviceHandle = nullptr;
	m_pPylonSDK->checkError(m_pPylonSDK->PylonCreateDeviceByIndex(0, &pDeviceHandle));

	if (pDeviceHandle == nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTCREATEGENERICDEVICE);

	auto pDeviceInstance = std::make_shared<CPylonDeviceInstance>(sIdentifier, m_pPylonSDK, pDeviceHandle);
	m_ConnectedDevices.insert(std::make_pair(sIdentifier, pDeviceInstance));

	return new CPylonDevice(pDeviceInstance);
}

IPylonDevice * CDriver_Pylon::ConnectToUniqueGigEDevice(const std::string & sIdentifier)
{

	auto iIter = m_ConnectedDevices.find(sIdentifier);
	if (iIter != m_ConnectedDevices.end())
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_CONNECTIONIDENTIFIERALREADYINUSE, "connection identifier already in use: " + sIdentifier);

	loadPylonSDK();

	size_t numDevices = 0;
	m_pPylonSDK->checkError(m_pPylonSDK->PylonGigEEnumerateAllDevices(&numDevices));

	if (numDevices != 1)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_DEVICENOTUNIQUE, "number of devices is not unique: " + std::to_string(numDevices));

	libPylonDeviceHandle pDeviceHandle = nullptr;
	m_pPylonSDK->checkError(m_pPylonSDK->PylonCreateDeviceByIndex(0, &pDeviceHandle));

	if (pDeviceHandle == nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTCREATEGIGEDEVICE);

	auto pDeviceInstance = std::make_shared<CPylonDeviceInstance>(sIdentifier, m_pPylonSDK, pDeviceHandle);
	m_ConnectedDevices.insert(std::make_pair(sIdentifier, pDeviceInstance));

	return new CPylonDevice(pDeviceInstance);

}

bool CDriver_Pylon::ConnectionExists(const std::string & sIdentifier)
{
	auto iIter = m_ConnectedDevices.find(sIdentifier);
	return (iIter != m_ConnectedDevices.end());
}

void CDriver_Pylon::CloseAllConnections()
{
	for (auto iIter : m_ConnectedDevices)
		iIter.second->close();
	m_ConnectedDevices.clear();
}

IPylonDevice* CDriver_Pylon::FindDeviceConnection(const std::string& sIdentifier)
{
	auto iIter = m_ConnectedDevices.find(sIdentifier);
	if (iIter == m_ConnectedDevices.end())
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_CONNECTIONIDENTIFIERNOTFOUND, "connection identifier not found: " + sIdentifier);

	return new CPylonDevice(iIter->second);
}


