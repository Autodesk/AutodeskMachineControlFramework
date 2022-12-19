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


Abstract: This is the class declaration of COIEDevice

*/


#ifndef __LIBMCDRIVER_SCANLABOIE_OIEDEVICE
#define __LIBMCDRIVER_SCANLABOIE_OIEDEVICE

#include "libmcdriver_scanlaboie_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlaboie_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlaboie_sdk.hpp"
#include <mutex>

namespace LibMCDriver_ScanLabOIE {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of COIEDevice 
**************************************************************************************************************************/
class COIEDeviceApp {
protected:
	std::string m_sName;
	uint32_t m_nMajorVersion;
	uint32_t m_nMinorVersion;
	uint32_t m_nPatchVersion;

public:
	COIEDeviceApp(const std::string & sName, uint32_t nMajorVersion, uint32_t nMinorVersion, uint32_t nPatchVersion);
	virtual ~COIEDeviceApp();

	std::string getName();
	uint32_t getMajorVersion();
	uint32_t getMinorVersion();
	uint32_t getPatchVersion();

};


typedef std::shared_ptr<COIEDeviceApp> POIEDeviceApp;

class COIEDeviceInstance {
protected:
	PScanLabOIESDK m_pOIESDK;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

	oie_instance m_pInstance;
	oie_device m_pDevice;

	std::string m_sHostName;
	uint32_t m_nPort;

	uint32_t m_nDeviceID;

	bool m_bIsConnected;

	std::vector<POIEDeviceApp> m_AppList;

	std::mutex m_PacketMutex;

	void ensureConnectivity();
	void removeDevice(bool bCheckForError);

	static std::string getNameFromAppInfo(const oie_appinfo& appInfo);

	void startAppEx(const std::string& sName, const int32_t nMajorVersion, const int32_t nMinorVersion, const std::string& sDeviceConfig);
	void uninstallAppEx(const std::string& sName, const int32_t nMajorVersion, const int32_t nMinorVersion);

public:

	COIEDeviceInstance(PScanLabOIESDK pOIESDK, oie_instance pInstance, const std::string& sHostName, const LibMCDriver_ScanLabOIE_uint32 nPort, uint32_t nResponseTimeOut, LibMCEnv::PWorkingDirectory pWorkingDirectory);

	virtual ~COIEDeviceInstance();

	void SetHostName(const std::string& sHostName);

	std::string GetHostName();

	void SetPort(const LibMCDriver_ScanLabOIE_uint32 nPort);

	LibMCDriver_ScanLabOIE_uint32 GetPort();

	bool IsConnected();

	LibMCDriver_ScanLabOIE_uint32 GetDeviceID();

	void Connect(const std::string& sUserName, const std::string& sPassword);

	void Disconnect();

	void RefreshAppList();

	LibMCDriver_ScanLabOIE_uint32 GetAppCount();

	std::string GetAppName(const LibMCDriver_ScanLabOIE_uint32 nIndex);

	void GetAppVersion(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nPatch);

	void GetAppInfo(const LibMCDriver_ScanLabOIE_uint32 nIndex, std::string& sName, LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nPatch);

	void StartAppByName(const std::string& sName, const std::string& sDeviceConfig);

	void StartAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex, const std::string& sDeviceConfig);

	void StartAppByMajorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const std::string& sDeviceConfig);

	void StartAppByMinorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion, const std::string& sDeviceConfig);

	void StopApp();

	bool AppIsRunning();

	void GetRunningApp(std::string& sName, LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nPatch);

	void InstallApp(const LibMCDriver_ScanLabOIE_uint64 nAppPackageBufferSize, const LibMCDriver_ScanLabOIE_uint8* pAppPackageBuffer);

	void UninstallAppByName(const std::string& sName);

	void UninstallAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex);

	void UninstallAppByMajorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion);

	void UninstallAppByMinorVersion(const std::string& sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion);

	void onPacketEvent (oie_device device, const oie_pkt* pkt);

	void onErrorEvent (oie_device device, oie_error error, int32_t value);

};

typedef std::shared_ptr<COIEDeviceInstance> POIEDeviceInstance;


class COIEDevice : public virtual IOIEDevice, public virtual CBase {
protected:

	std::weak_ptr<COIEDeviceInstance> m_pDeviceInstancePtr;
	POIEDeviceInstance lockInstance();

public:

	COIEDevice(POIEDeviceInstance pDeviceInstance);

	virtual ~COIEDevice();

	void SetHostName(const std::string & sHostName) override;

	std::string GetHostName() override;

	void SetPort(const LibMCDriver_ScanLabOIE_uint32 nPort) override;

	LibMCDriver_ScanLabOIE_uint32 GetPort() override;

	bool IsConnected() override;

	LibMCDriver_ScanLabOIE_uint32 GetDeviceID() override;

	void Connect(const std::string & sUserName, const std::string & sPassword) override;

	void Disconnect() override;

	LibMCDriver_ScanLabOIE_uint32 GetAppCount() override;

	std::string GetAppName(const LibMCDriver_ScanLabOIE_uint32 nIndex) override;

	void GetAppVersion(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch) override;

	void GetAppInfo(const LibMCDriver_ScanLabOIE_uint32 nIndex, std::string & sName, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch) override;

	void StartAppByName(const std::string & sName, const std::string & sDeviceConfig) override;

	void StartAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex, const std::string & sDeviceConfig) override;

	void StartAppByMajorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const std::string & sDeviceConfig) override;

	void StartAppByMinorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion, const std::string & sDeviceConfig) override;

	void StopApp() override;

	bool AppIsRunning() override;

	void GetRunningApp(std::string & sName, LibMCDriver_ScanLabOIE_uint32 & nMajor, LibMCDriver_ScanLabOIE_uint32 & nMinor, LibMCDriver_ScanLabOIE_uint32 & nPatch) override;

	void InstallApp(const LibMCDriver_ScanLabOIE_uint64 nAppPackageBufferSize, const LibMCDriver_ScanLabOIE_uint8 * pAppPackageBuffer) override;

	void UninstallAppByName(const std::string & sName) override;

	void UninstallAppByIndex(const LibMCDriver_ScanLabOIE_uint32 nIndex) override;

	void UninstallAppByMajorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion) override;

	void UninstallAppByMinorVersion(const std::string & sName, const LibMCDriver_ScanLabOIE_uint32 nMajorVersion, const LibMCDriver_ScanLabOIE_uint32 nMinorVersion) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLabOIE

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABOIE_OIEDEVICE
