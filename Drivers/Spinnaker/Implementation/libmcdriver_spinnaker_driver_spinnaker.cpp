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


Abstract: This is a stub class definition of CDriver_Spinnaker

*/

#include "libmcdriver_spinnaker_driver_spinnaker.hpp"
#include "libmcdriver_spinnaker_interfaceexception.hpp"
#include "libmcdriver_spinnaker_spinnakerdevice.hpp"

#include <cctype>

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace LibMCDriver_Spinnaker::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Spinnaker 
**************************************************************************************************************************/

CDriver_Spinnaker::CDriver_Spinnaker(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName(sName), m_pDriverEnvironment(pDriverEnvironment), m_pSystemHandle(nullptr)
{
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);
}

CDriver_Spinnaker::~CDriver_Spinnaker()
{
	unloadSpinnakerSDK();
}

void CDriver_Spinnaker::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_Spinnaker::GetName()
{
	return m_sName;
}

std::string CDriver_Spinnaker::GetType()
{
	return "spinnaker-1.0";
}

void CDriver_Spinnaker::GetVersion(LibMCDriver_Spinnaker_uint32& nMajor, LibMCDriver_Spinnaker_uint32& nMinor, LibMCDriver_Spinnaker_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_SPINNAKER_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_SPINNAKER_VERSION_MINOR;
	nMicro = LIBMCDRIVER_SPINNAKER_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_Spinnaker::QueryParameters()
{
}

void CDriver_Spinnaker::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
}

void CDriver_Spinnaker::SetCustomSDKResource(const std::string& sResourceName)
{
	if (m_pSpinnakerSDK.get() != nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_SDKALREADYLOADED);

	m_pDriverEnvironment->RetrieveMachineResourceData(sResourceName, m_SpinnakerCDLLData);
}

void CDriver_Spinnaker::unloadSpinnakerSDK()
{
	if (m_pSpinnakerSDK.get() != nullptr) {
		if (m_pSystemHandle != nullptr) {
			m_pSpinnakerSDK->SpinSystemReleaseInstance(m_pSystemHandle);
		}
	}
	m_pSystemHandle = nullptr;
	m_pSpinnakerSDK = nullptr;
	m_pSpinnakerCDLLFile = nullptr;
	m_pWorkingDirectory = nullptr;
}

void CDriver_Spinnaker::loadSpinnakerSDK()
{
	if (m_pSpinnakerSDK.get() == nullptr) {
		unloadSpinnakerSDK();

		try {
			if (m_SpinnakerCDLLData.empty())
				throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_SDKRESOURCENOTSET);

			m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

#ifdef _WIN32
			const std::string sLibraryName = "SpinnakerC.dll";
#else
			const std::string sLibraryName = "libSpinnaker_C.so";
#endif

			m_pSpinnakerCDLLFile = m_pWorkingDirectory->StoreCustomData(sLibraryName, m_SpinnakerCDLLData);
			m_pSpinnakerSDK = std::make_shared<CLibSpinnakerSDK>(m_pSpinnakerCDLLFile->GetAbsoluteFileName());

			m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinSystemGetInstance(&m_pSystemHandle));
		}
		catch (...) {
			unloadSpinnakerSDK();
			throw;
		}
	}
}

void CDriver_Spinnaker::checkIdentifier(const std::string& sIdentifier)
{
	// Check if identifier is alphanumeric, non-empty and max 64 chars
	bool bValidIdentifier = (!sIdentifier.empty() && sIdentifier.length() <= 64);
	for (char c : sIdentifier) {
		if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {
			bValidIdentifier = false;
			break;
		}
	}

	if (!bValidIdentifier)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDIDENTIFIER, "connection identifier is not alphanumeric or too long: " + sIdentifier);
}

ISpinnakerDevice* CDriver_Spinnaker::connectToUniqueDevice(const std::string& sIdentifier)
{
	auto iIter = m_ConnectedDevices.find(sIdentifier);
	if (iIter != m_ConnectedDevices.end())
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_CONNECTIONIDENTIFIERALREADYINUSE, "connection identifier already in use: " + sIdentifier);

	checkIdentifier(sIdentifier);
	loadSpinnakerSDK();

	libSpinnakerCameraListHandle hCameraList = nullptr;
	libSpinnakerCameraHandle hCamera = nullptr;
	try {
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraListCreateEmpty(&hCameraList));
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinSystemGetCameras(m_pSystemHandle, hCameraList));

		size_t nNumCameras = 0;
		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraListGetSize(hCameraList, &nNumCameras));
		if (nNumCameras != 1)
			throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_DEVICENOTUNIQUE, "number of devices is not unique: " + std::to_string(nNumCameras));

		m_pSpinnakerSDK->checkError(m_pSpinnakerSDK->SpinCameraListGet(hCameraList, 0, &hCamera));
		m_pSpinnakerSDK->SpinCameraListDestroy(hCameraList);
		hCameraList = nullptr;
	}
	catch (...) {
		if (hCameraList != nullptr)
			m_pSpinnakerSDK->SpinCameraListDestroy(hCameraList);
		throw;
	}

	if (hCamera == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_COULDNOTCREATEGENERICDEVICE);

	auto pDeviceInstance = std::make_shared<CSpinnakerDeviceInstance>(sIdentifier, m_pSpinnakerSDK, hCamera);
	m_ConnectedDevices.insert(std::make_pair(sIdentifier, pDeviceInstance));

	return new CSpinnakerDevice(pDeviceInstance);
}

ISpinnakerDevice* CDriver_Spinnaker::ConnectToUniqueGenericDevice(const std::string& sIdentifier)
{
	return connectToUniqueDevice(sIdentifier);
}

ISpinnakerDevice* CDriver_Spinnaker::ConnectToUniqueGigEDevice(const std::string& sIdentifier)
{
	return connectToUniqueDevice(sIdentifier);
}

bool CDriver_Spinnaker::ConnectionExists(const std::string& sIdentifier)
{
	auto iIter = m_ConnectedDevices.find(sIdentifier);
	return (iIter != m_ConnectedDevices.end());
}

void CDriver_Spinnaker::CloseAllConnections()
{
	for (auto iIter : m_ConnectedDevices)
		iIter.second->close();
	m_ConnectedDevices.clear();
}

ISpinnakerDevice* CDriver_Spinnaker::FindDeviceConnection(const std::string& sIdentifier)
{
	auto iIter = m_ConnectedDevices.find(sIdentifier);
	if (iIter == m_ConnectedDevices.end())
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_CONNECTIONIDENTIFIERNOTFOUND, "connection identifier not found: " + sIdentifier);

	return new CSpinnakerDevice(iIter->second);
}
