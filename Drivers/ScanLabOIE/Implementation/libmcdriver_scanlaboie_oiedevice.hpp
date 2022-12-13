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


namespace LibMCDriver_ScanLabOIE {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of COIEDevice 
**************************************************************************************************************************/

class COIEDevice : public virtual IOIEDevice, public virtual CBase {
private:

	/**
	* Put private members here.
	*/

protected:

	/**
	* Put protected members here.
	*/

public:

	/**
	* Put additional public members here. They will not be visible in the external API.
	*/


	/**
	* Public member functions to implement.
	*/

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
