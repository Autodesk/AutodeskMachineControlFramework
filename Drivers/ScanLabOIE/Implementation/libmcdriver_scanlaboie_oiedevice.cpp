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


Abstract: This is a stub class definition of COIEDevice

*/

#include "libmcdriver_scanlaboie_oiedevice.hpp"
#include "libmcdriver_scanlaboie_datarecording.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <sstream>

// Include custom headers here.
using namespace LibMCDriver_ScanLabOIE::Impl;

/*************************************************************************************************************************
 Class definition of COIEDevice 
**************************************************************************************************************************/
#define OIE_MAXSUPPORTEDAPPSONDEVICE (128 * 1024)
#define OIE_MAXSUPPORTEDAPPVERSION (1024UL * 1024UL * 1024UL)
#define OIE_MINRESPONSETIMEOUT 1


COIEDeviceApp::COIEDeviceApp(const std::string& sName, uint32_t nMajorVersion, uint32_t nMinorVersion, uint32_t nPatchVersion)
	: m_sName (sName), m_nMajorVersion (nMajorVersion), m_nMinorVersion (nMinorVersion), m_nPatchVersion (nPatchVersion)
{

}

COIEDeviceApp::~COIEDeviceApp()
{

}

std::string COIEDeviceApp::getName()
{
	return m_sName;
}

uint32_t COIEDeviceApp::getMajorVersion()
{
	return m_nMajorVersion;
}

uint32_t COIEDeviceApp::getMinorVersion()
{
	return m_nMinorVersion;
}

uint32_t COIEDeviceApp::getPatchVersion()
{
	return m_nPatchVersion;
}

void oieDevicePacketListener(oie_device device, const oie_pkt* pkt, void* userData)
{
	if (userData != nullptr) {
		COIEDeviceInstance* pDeviceInstance = static_cast<COIEDeviceInstance*> (userData);
		pDeviceInstance->onPacketEvent (device, pkt);
	}
}

void oieErrorListener(oie_device device, oie_error error, int32_t value, void* userData)
{
	if (userData != nullptr) {
		COIEDeviceInstance* pDeviceInstance = static_cast<COIEDeviceInstance*> (userData);
		pDeviceInstance->onErrorEvent(device, error, value);
	}
}

COIEDeviceInstance::COIEDeviceInstance(PScanLabOIESDK pOIESDK, oie_instance pInstance, const std::string& sDeviceName, const std::string& sHostName, const LibMCDriver_ScanLabOIE_uint32 nPort, uint32_t nResponseTimeOut, IDeviceConfiguration* pDeviceConfiguration, std::vector<uint8_t>& correctionFileData, LibMCEnv::PWorkingDirectory pWorkingDirectory)
	: m_pOIESDK (pOIESDK), 
	  m_pInstance (pInstance), 
	  m_pDevice (nullptr), 
	  m_sHostName (sHostName),
	  m_sDeviceName (sDeviceName),
	  m_nPort (nPort),
	  m_bIsConnected (false),
	  m_pWorkingDirectory (pWorkingDirectory),
	  m_nDeviceID (0),
	  m_bHasCorrectionData (false),
	  m_nRTCSignalCount (0),
	  m_nSensorSignalCount (0)

{
	if ((pOIESDK.get() == nullptr) || (pInstance == nullptr) || (pWorkingDirectory.get () == nullptr) || (pDeviceConfiguration == nullptr))
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	if (sDeviceName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDDEVICENAME);

	if (nResponseTimeOut < OIE_MINRESPONSETIMEOUT)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRESPONSETIMEOUT);

	buildDeviceConfig(pDeviceConfiguration, correctionFileData);

	m_nRTCSignalCount = pDeviceConfiguration->GetRTCSignalCount (); 
	m_nSensorSignalCount = pDeviceConfiguration->GetSensorSignalCount();


	if (m_pConfigurationFile == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTCREATEDEVICECONFIGURATION);
	std::string sConfigurationFileName = m_pConfigurationFile->GetAbsoluteFileName();

	switch (m_pOIESDK->getDeviceDriverType()) {
	case LibMCDriver_ScanLabOIE::eOIEDeviceDriverType::OIEVersion2:
		m_pDevice = m_pOIESDK->oie_add_device_VERSION2(m_pInstance);
		break;

	case LibMCDriver_ScanLabOIE::eOIEDeviceDriverType::OIEVersion3Compatibility:
	case LibMCDriver_ScanLabOIE::eOIEDeviceDriverType::OIEVersion3:
		m_pDevice = m_pOIESDK->oie_add_device_VERSION3(m_pInstance, sConfigurationFileName.c_str());
		break;

	default:
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_UNSUPPORTEDOIESDKVERSION);
	}

	if (m_pDevice == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTADDDEVICE);

	m_nDeviceID = m_pOIESDK->oie_get_device_id (m_pDevice);

	m_pOIESDK->oie_set_reply_timeout(m_pDevice, nResponseTimeOut);

	m_pOIESDK->oie_set_packet_listener(m_pDevice, oieDevicePacketListener, this);

	m_pOIESDK->oie_set_runtime_error_listener(m_pDevice, oieErrorListener, this);

}


COIEDeviceInstance::~COIEDeviceInstance()
{
	removeDevice(false);

	m_pInstance = nullptr;
	m_pOIESDK = nullptr;
}

bool checkIniKey(const std::string& sLine, const std::string& sKey)
{
	std::string sLowerLine = sLine;
	std::transform(sLowerLine.begin(), sLowerLine.end(), sLowerLine.begin(),
		[](unsigned char c) { return std::tolower(c); });

	std::string sKeyWithEqual = sKey + "=";
	std::transform(sKeyWithEqual.begin(), sKeyWithEqual.end(), sKeyWithEqual.begin(),
		[](unsigned char c) { return std::tolower(c); });

	if (sLowerLine.length() >= sKeyWithEqual.length()) {
		if (sLowerLine.substr(0, sKeyWithEqual.length()) == sKeyWithEqual)
			return true;
	}

	return false;
}


void COIEDeviceInstance::buildDeviceConfig(IDeviceConfiguration* pDeviceConfiguration, std::vector<uint8_t>& correctionFileData)
{
	if (pDeviceConfiguration == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	std::string sCorrectionFileName;
	std::string sDeviceConfig = pDeviceConfiguration->GetDeviceConfigurationString();

	if (!correctionFileData.empty()) {
		m_pCorrectionFile = m_pWorkingDirectory->StoreCustomDataInTempFile("ct5", correctionFileData);
		sCorrectionFileName = m_pCorrectionFile->GetAbsoluteFileName();
		m_bHasCorrectionData = true;

		std::replace(sCorrectionFileName.begin(), sCorrectionFileName.end(), '\\', '/');
	}
	else {
		m_bHasCorrectionData = false;
	}

	std::stringstream sRawDeviceConfigStream(sDeviceConfig);
	std::stringstream sFilteredDeviceConfigStream;
	std::string sLine;
	while (std::getline(sRawDeviceConfigStream, sLine, '\n')) {

		if (checkIniKey(sLine, "BackwardTransformation\\CorrectionTable5")) {
			sLine = "BackwardTransformation\\CorrectionTable5=" + sCorrectionFileName;
		}

		if (checkIniKey(sLine, "BackwardTransformation\\enabled")) {
			if (sCorrectionFileName.empty())
				sLine = "BackwardTransformation\\enabled=false";
		}

		sFilteredDeviceConfigStream << sLine << std::endl;
	}



	m_pConfigurationFile = m_pWorkingDirectory->StoreCustomStringInTempFile("ini", sFilteredDeviceConfigStream.str());

}



std::string COIEDeviceInstance::GetDeviceName()
{
	return m_sDeviceName;
}

void COIEDeviceInstance::removeDevice(bool bCheckForError)
{
	if (m_pOIESDK.get() != nullptr) {
		if ((m_pDevice != nullptr) && (m_pInstance != nullptr)) {
			
			oie_error nErrorCode = m_pOIESDK->oie_remove_device(m_pInstance, m_pDevice);
			if (bCheckForError)
				m_pOIESDK->checkError(nErrorCode);

			m_pDevice = nullptr;
			m_nDeviceID = 0;
		}
	}
}

void COIEDeviceInstance::SetHostName(const std::string& sHostName)
{
	if (m_bIsConnected)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICEALREADYCONNECTED);

	m_sHostName = sHostName;
}

std::string COIEDeviceInstance::GetHostName()
{
	return m_sHostName;
}

void COIEDeviceInstance::SetPort(const LibMCDriver_ScanLabOIE_uint32 nPort)
{
	if (m_bIsConnected)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICEALREADYCONNECTED);

	m_nPort = nPort;
}

LibMCDriver_ScanLabOIE_uint32 COIEDeviceInstance::GetPort()
{
	return m_nPort;
}

bool COIEDeviceInstance::IsConnected()
{
	return m_bIsConnected;
}

LibMCDriver_ScanLabOIE_uint32 COIEDeviceInstance::GetDeviceID()
{
	if (m_pDevice == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICEALREADYREMOVED);

	return m_nDeviceID;
}

void COIEDeviceInstance::Connect(const std::string& sUserName, const std::string& sPassword)
{
	if (m_pDevice == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICEALREADYREMOVED);

	Disconnect();

	auto pDLLCache = m_pOIESDK->cacheDllDirectory();

	m_pOIESDK->checkError (m_pOIESDK->oie_connect(m_pDevice, m_sHostName.c_str(), m_nPort));
	
	m_pOIESDK->checkError (m_pOIESDK->oie_device_login(m_pDevice, sUserName.c_str (), sPassword.c_str ()));

	m_bIsConnected = true;

	RefreshAppList();

	pDLLCache = nullptr;

}

void COIEDeviceInstance::Disconnect()
{
	if (m_pDevice == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICEALREADYREMOVED);

	if (m_bIsConnected) {
		m_bIsConnected = false;
		m_pOIESDK->oie_disconnect(m_pDevice);
	}

}


void COIEDeviceInstance::ensureConnectivity()
{
	if (m_pDevice == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICEALREADYREMOVED);
	if (m_bIsConnected == false)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICENOTCONNECTED);

}

std::string COIEDeviceInstance::getNameFromAppInfo(const oie_appinfo& appInfo)
{
	std::array<char, sizeof (appInfo.m_Name) + 1> buffer;
	for (uint32_t nIndex = 0; nIndex < sizeof(appInfo.m_Name); nIndex++)
		buffer[nIndex] = appInfo.m_Name[nIndex];
	buffer[sizeof(appInfo.m_Name)] = 0;

	return std::string(&appInfo.m_Name[0]);
}

void COIEDeviceInstance::RefreshAppList()
{
	ensureConnectivity();

	m_AppList.clear();

	std::vector<oie_appinfo> appDataBuffer;
	appDataBuffer.resize(OIE_MAXSUPPORTEDAPPSONDEVICE);

	int32_t nAppsOnDevice = 0;
	m_pOIESDK->checkError(m_pOIESDK->oie_device_list_apps (m_pDevice, appDataBuffer.data(), (int32_t)appDataBuffer.size (), &nAppsOnDevice));

	if ((nAppsOnDevice < 0) || (nAppsOnDevice > OIE_MAXSUPPORTEDAPPSONDEVICE))
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPCOUNTONDEVICE);

	m_AppList.reserve(nAppsOnDevice);

	for (int32_t nIndex = 0; nIndex < nAppsOnDevice; nIndex++) {
		auto appData = appDataBuffer[nIndex];
		std::string sName = getNameFromAppInfo(appData);

		m_AppList.push_back(std::make_shared<COIEDeviceApp> (sName, appData.m_nMajorVersion, appData.m_nMinorVersion, appData.m_nPatchVersion));
	}
}

LibMCDriver_ScanLabOIE_uint32 COIEDeviceInstance::GetAppCount()
{
	ensureConnectivity();

	return (uint32_t) m_AppList.size();

}

std::string COIEDeviceInstance::GetAppName(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	ensureConnectivity();

	if (nIndex >= m_AppList.size())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPINDEX);

	return m_AppList.at(nIndex)->getName();
}

void COIEDeviceInstance::GetAppVersion(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nPatch)
{
	ensureConnectivity();

	if (nIndex >= m_AppList.size())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPINDEX);

	auto pApp = m_AppList.at(nIndex);
	nMajor = pApp->getMajorVersion();
	nMinor = pApp->getMinorVersion();
	nPatch = pApp->getPatchVersion();

}

void COIEDeviceInstance::GetAppInfo(const LibMCDriver_ScanLabOIE_uint32 nIndex, std::string& sName, LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nPatch)
{
	ensureConnectivity();

	if (nIndex >= m_AppList.size())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPINDEX);

	auto pApp = m_AppList.at(nIndex);
	sName = pApp->getName();
	nMajor = pApp->getMajorVersion();
	nMinor = pApp->getMinorVersion();
	nPatch = pApp->getPatchVersion();

}


void COIEDeviceInstance::startAppEx(const std::string& sName, const int32_t nMajorVersion, const int32_t nMinorVersion)
{
	if (m_nRTCSignalCount == 0)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRTCSIGNALCOUNT);
	if (m_nSensorSignalCount == 0)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSENSORSIGNALCOUNT);
	if (m_pConfigurationFile.get () == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTCREATEDEVICECONFIGURATION);

	std::string sDeviceConfigFileName = m_pConfigurationFile->GetAbsoluteFileName();

	ensureConnectivity();

	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);


	try {
		//std::cout << "Starting app call..." << std::endl;
		m_pOIESDK->checkError(m_pOIESDK->oie_device_start_app_ver(m_pDevice, sName.c_str(), nMajorVersion, nMinorVersion, sDeviceConfigFileName.c_str()));
		//std::cout << "Starting app call finished..." << std::endl;
	}
	catch (std::runtime_error & E) {
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTSTARTAPP, "could not start app: " + std::string (E.what ()));
	}
	catch (...) {

		throw ELibMCDriver_ScanLabOIEInterfaceException (LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTSTARTAPP);
	}

	{
		std::lock_guard<std::mutex> lockGuard(m_RecordingMutex);
		m_pCurrentDataRecording = std::make_shared<CDataRecordingInstance>(m_nRTCSignalCount + m_nSensorSignalCount, m_nRTCSignalCount, 1024);
	}

}

void COIEDeviceInstance::StartAppByName(const std::string& sName)
{
	startAppEx(sName, -1, -1);
}

void COIEDeviceInstance::StartAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	ensureConnectivity();

	if (nIndex >= m_AppList.size())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPINDEX);

	auto pApp = m_AppList.at(nIndex);

	startAppEx(pApp->getName(), pApp->getMajorVersion(), pApp->getMinorVersion());
}

void COIEDeviceInstance::StartAppByMajorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion)
{
	ensureConnectivity();

	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);

	if (nMajorVersion >= OIE_MAXSUPPORTEDAPPVERSION)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPVERSION);

	startAppEx(sName, (int32_t) nMajorVersion, -1);
}

void COIEDeviceInstance::StartAppByMinorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion)
{
	ensureConnectivity();

	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);

	if ((nMajorVersion >= OIE_MAXSUPPORTEDAPPVERSION) || (nMinorVersion >= OIE_MAXSUPPORTEDAPPVERSION))
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPVERSION);

	startAppEx(sName, (int32_t)nMajorVersion, (int32_t)nMinorVersion);
}

void COIEDeviceInstance::StopApp()
{
	ensureConnectivity();

	m_pOIESDK->checkError(m_pOIESDK->oie_device_stop_app (m_pDevice));
}

bool COIEDeviceInstance::AppIsRunning()
{
	ensureConnectivity();

	uint32_t nState = 0;
	m_pOIESDK->checkError(m_pOIESDK->oie_device_is_app_running(m_pDevice, &nState));

	return nState != 0;
}

void COIEDeviceInstance::GetRunningApp(std::string& sName, LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nPatch)
{
	ensureConnectivity();

	oie_appinfo appData;
	memset((void*)&appData, 0, sizeof(appData));
	m_pOIESDK->checkError(m_pOIESDK->oie_device_get_appinfo(m_pDevice, &appData));
	sName = getNameFromAppInfo(appData);
	nMajor = appData.m_nMajorVersion;
	nMinor = appData.m_nMinorVersion;
	nPatch = appData.m_nPatchVersion;

}

void COIEDeviceInstance::InstallApp(const LibMCDriver_ScanLabOIE_uint64 nAppPackageBufferSize, const LibMCDriver_ScanLabOIE_uint8* pAppPackageBuffer)
{
	if ((nAppPackageBufferSize == 0) || (pAppPackageBuffer == nullptr))
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPPACKAGEBUFFER);

	ensureConnectivity();

	auto pAppDataFile = m_pWorkingDirectory->StoreCustomDataInTempFile("efw", LibMCEnv::CInputVector<uint8_t> (pAppPackageBuffer, nAppPackageBufferSize));
	std::string sAppDataFileName = pAppDataFile->GetAbsoluteFileName();

	try {
		m_pOIESDK->checkError(m_pOIESDK->oie_device_install_app (m_pDevice, sAppDataFileName.c_str ()));

		pAppDataFile->DeleteFromDisk();
		pAppDataFile = nullptr;
	}
	catch (...) {
		if (pAppDataFile != nullptr) {
			pAppDataFile->DeleteFromDisk();
			pAppDataFile = nullptr;
		}

		throw;
	}


}


void COIEDeviceInstance::uninstallAppEx(const std::string& sName, const int32_t nMajorVersion, const int32_t nMinorVersion)
{
	ensureConnectivity();

	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);

	m_pOIESDK->checkError(m_pOIESDK->oie_device_uninstall_app_ver(m_pDevice, sName.c_str(), nMajorVersion, nMinorVersion));

}

void COIEDeviceInstance::UninstallAppByName(const std::string& sName)
{
	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);

	uninstallAppEx(sName, -1, -1);
}

void COIEDeviceInstance::UninstallAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	ensureConnectivity();

	if (nIndex >= m_AppList.size())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPINDEX);

	auto pApp = m_AppList.at(nIndex);

	uninstallAppEx(pApp->getName(), pApp->getMajorVersion(), pApp->getMinorVersion());

}

void COIEDeviceInstance::UninstallAppByMajorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion)
{
	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);

	if (nMajorVersion >= OIE_MAXSUPPORTEDAPPVERSION)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPVERSION);

	uninstallAppEx(sName, (int32_t) nMajorVersion, -1);
}

void COIEDeviceInstance::UninstallAppByMinorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion)
{
	if (sName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPNAME);

	if ((nMajorVersion >= OIE_MAXSUPPORTEDAPPVERSION) || (nMinorVersion >= OIE_MAXSUPPORTEDAPPVERSION))
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDAPPVERSION);

	uninstallAppEx(sName, (int32_t)nMajorVersion, (int32_t)nMinorVersion);
}


void COIEDeviceInstance::onPacketEvent(oie_device device, const oie_pkt* pkt)
{
	try {
		if ((device == m_pDevice) && (pkt != nullptr)) {

			std::lock_guard<std::mutex> lockGuard(m_RecordingMutex);

			if (m_pCurrentDataRecording.get() != nullptr) {


				double dX = 0.0;
				double dY = 0.0;

				if (m_bHasCorrectionData) {
					m_pOIESDK->checkError(m_pOIESDK->oie_pkt_get_xy(pkt, &dX, &dY));
				}				

				//std::cout << "Packet: " << " X: " << dX << " Y: " << dY << std::endl;

				// First uint32 of packet is packed number
				uint32_t* pPacketNumber = (uint32_t*)pkt;

				m_pCurrentDataRecording->startRecord(*pPacketNumber, dX, dY);

				uint32_t rtcSignalCount = m_pOIESDK->oie_pkt_get_rtc_signal_count(pkt);
				//std::cout << "RTC signal count" << rtcSignalCount << std::endl;
				for (uint32_t rtcSignalIndex = 0; rtcSignalIndex < rtcSignalCount; rtcSignalIndex++)
				{
					int32_t nValue = 0;
					m_pOIESDK->checkError(m_pOIESDK->oie_pkt_get_rtc_signal(pkt, rtcSignalIndex, &nValue));

					m_pCurrentDataRecording->recordValue(nValue);
				}

				uint32_t sensorSignalCount = m_pOIESDK->oie_pkt_get_sensor_signal_count(pkt);
				//std::cout << "Sensor signal count" << sensorSignalCount << std::endl;

				for (uint32_t sensorSignalIndex = 0; sensorSignalIndex < sensorSignalCount; sensorSignalIndex++)
				{
					int32_t nValue = 0;
					m_pOIESDK->checkError(m_pOIESDK->oie_pkt_get_sensor_signal(pkt, sensorSignalIndex, &nValue));
					m_pCurrentDataRecording->recordValue(nValue);
				}

				m_pCurrentDataRecording->finishRecord();

			}
		}
		else {
			std::cout << "Packet event: with null" << std::endl;
		}

	}
	catch (std::exception & E)
	{
		std::cout << "error getting data:" << E.what () << std::endl;

	}
	catch (...) {
		std::cout << "error getting data" << std::endl;
	}
}

void COIEDeviceInstance::onErrorEvent(oie_device device, oie_error error, int32_t value)
{
	try {
		if (device == m_pDevice) {
			std::cout << "Error event: " << error << " value: " << value << std::endl;
		}
	}
	catch (...)
	{

	}
}


PDataRecordingInstance COIEDeviceInstance::RetrieveCurrentRecording()
{
	std::lock_guard<std::mutex> lockGuard(m_RecordingMutex);

	if (m_pCurrentDataRecording.get() != nullptr) {

		auto pOldRecording = m_pCurrentDataRecording;
		m_pCurrentDataRecording = nullptr;
		m_pCurrentDataRecording = pOldRecording->createEmptyDuplicate();

		return pOldRecording;
	}
	else {
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NODATARECORDINGAVAILABLE);
	}


}



void COIEDeviceInstance::ClearCurrentRecording()
{
	std::lock_guard<std::mutex> lockGuard(m_RecordingMutex);
	if (m_pCurrentDataRecording.get() != nullptr) {
		auto pOldRecording = m_pCurrentDataRecording;
		m_pCurrentDataRecording = nullptr;
		m_pCurrentDataRecording = pOldRecording->createEmptyDuplicate();

	}
}

PDataRecordingInstance COIEDeviceInstance::LoadRecordingFromBuild(LibMCEnv::PBuild pBuild, const std::string& sDataUUID)
{
	throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTIMPLEMENTED);
}


COIEDevice::COIEDevice(POIEDeviceInstance pDeviceInstance)
{
	if (pDeviceInstance.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	m_pDeviceInstancePtr = pDeviceInstance;
}

COIEDevice::~COIEDevice()
{
	m_pDeviceInstancePtr.reset ();
}

POIEDeviceInstance COIEDevice::lockInstance()
{
	if (m_pDeviceInstancePtr.expired ())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_DEVICENOTAVAILABLEANYMORE);

	POIEDeviceInstance pLockedInstance = m_pDeviceInstancePtr.lock();

	return pLockedInstance;
}

std::string COIEDevice::GetDeviceName()
{
	return lockInstance()->GetDeviceName();
}

void COIEDevice::SetHostName(const std::string & sHostName)
{
	lockInstance()->SetHostName(sHostName);
}

std::string COIEDevice::GetHostName()
{
	return lockInstance()->GetHostName();
}

void COIEDevice::SetPort(const LibMCDriver_ScanLabOIE_uint32 nPort)
{
	lockInstance()->SetPort(nPort);
}

LibMCDriver_ScanLabOIE_uint32 COIEDevice::GetPort()
{
	return lockInstance()->GetPort();
}

bool COIEDevice::IsConnected()
{
	return lockInstance()->IsConnected();
}

LibMCDriver_ScanLabOIE_uint32 COIEDevice::GetDeviceID()
{
	return lockInstance()->GetDeviceID();
}

void COIEDevice::Connect(const std::string & sUserName, const std::string & sPassword)
{
	lockInstance()->Connect(sUserName, sPassword);
}



void COIEDevice::Disconnect()
{
	lockInstance()->Disconnect();
}

LibMCDriver_ScanLabOIE_uint32 COIEDevice::GetAppCount()
{
	return lockInstance()->GetAppCount();
}

std::string COIEDevice::GetAppName(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	return lockInstance()->GetAppName(nIndex);
}

void COIEDevice::GetAppVersion(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch)
{
	return lockInstance()->GetAppVersion(nIndex, nMajor, nMinor, nPatch);
}

void COIEDevice::GetAppInfo(const LibMCDriver_ScanLabOIE_uint32 nIndex, std::string & sName, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch)
{
	return lockInstance()->GetAppInfo(nIndex, sName, nMajor, nMinor, nPatch);
}

void COIEDevice::StartAppByName(const std::string& sName)
{
	lockInstance()->StartAppByName(sName);
}

void COIEDevice::StartAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	lockInstance()->StartAppByIndex(nIndex);
}

void COIEDevice::StartAppByMajorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion)
{
	lockInstance()->StartAppByMajorVersion(sName, nMajorVersion);
}

void COIEDevice::StartAppByMinorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion)
{

	lockInstance()->StartAppByMinorVersion(sName, nMajorVersion, nMinorVersion);
}


void COIEDevice::StopApp()
{
	lockInstance()->StopApp();
}

bool COIEDevice::AppIsRunning()
{
	return lockInstance()->AppIsRunning();
}

void COIEDevice::GetRunningApp(std::string & sName, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch)
{
	lockInstance()->GetRunningApp(sName, nMajor, nMinor, nPatch);
}

void COIEDevice::InstallApp(const LibMCDriver_ScanLabOIE_uint64 nAppPackageBufferSize, const LibMCDriver_ScanLabOIE_uint8 * pAppPackageBuffer)
{
	lockInstance()->InstallApp(nAppPackageBufferSize, pAppPackageBuffer);
}

void COIEDevice::UninstallAppByName(const std::string & sName)
{
	lockInstance()->UninstallAppByName(sName);
}

void COIEDevice::UninstallAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	lockInstance()->UninstallAppByIndex(nIndex);
}

void COIEDevice::UninstallAppByMajorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion)
{
	lockInstance()->UninstallAppByMajorVersion(sName, nMajorVersion);
}

void COIEDevice::UninstallAppByMinorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion)
{
	lockInstance()->UninstallAppByMinorVersion(sName, nMajorVersion, nMinorVersion);
}


void COIEDevice::RefreshAppList()
{
	lockInstance()->RefreshAppList();
}

IDataRecording* COIEDevice::RetrieveCurrentRecording()
{
	auto pRecordingInstance = lockInstance()->RetrieveCurrentRecording();
	return new CDataRecording (pRecordingInstance);
}

void COIEDevice::ClearCurrentRecording()
{
	lockInstance()->ClearCurrentRecording();
}

IDataRecording* COIEDevice::LoadRecordingFromBuild(LibMCEnv::PBuild pBuild, const std::string& sDataUUID)
{
	auto pRecordingInstance = lockInstance()->LoadRecordingFromBuild(pBuild, sDataUUID);
	return new CDataRecording(pRecordingInstance);
}
