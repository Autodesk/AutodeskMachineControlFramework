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


Abstract: This is the class declaration of CDriver_ScanLab_OIE

*/


#ifndef __LIBMCDRIVER_SCANLABOIE_DRIVER_SCANLAB_OIE
#define __LIBMCDRIVER_SCANLABOIE_DRIVER_SCANLAB_OIE

#include "libmcdriver_scanlaboie_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlaboie_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlaboie_sdk.hpp"
#include "libmcdriver_scanlaboie_oiedevice.hpp"
#include <map>

namespace LibMCDriver_ScanLabOIE {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_ScanLab_OIE 
**************************************************************************************************************************/

class CDriver_ScanLab_OIE : public virtual IDriver_ScanLab_OIE, public virtual CDriver {
protected:

	PScanLabOIESDK m_pOIESDK;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pSDKLibraryFile;

	std::string m_sDriverName;
		
	std::string m_sLibSSLResourceName;
	std::string m_sLibCryptoResourceName;
	std::string m_sQT5CoreResourceName;
	std::string m_sQT5NetworkResourceName;
	std::string m_sOIECalibrationLibraryResourceName;
	std::string m_sRTCStreamParserResourceName;

	LibMCEnv::PWorkingFile m_pLibSSLResourceFile;
	LibMCEnv::PWorkingFile m_pLibCryptoResourceFile;
	LibMCEnv::PWorkingFile m_pQT5CoreResourceFile;
	LibMCEnv::PWorkingFile m_pQT5NetworkResourceFile;
	LibMCEnv::PWorkingFile m_pOIECalibrationLibraryResourceFile;
	LibMCEnv::PWorkingFile m_pRTCStreamParserResourceFile;

	bool bSimulationMode;
	LibMCDriver_ScanLabOIE::eOIEDeviceDriverType m_DeviceDriverType;

	oie_instance m_pInstance;

	std::map<std::string, POIEDeviceInstance> m_Devices;

	void initializeSDKEx (const std::vector<uint8_t> & SDKDLLBuffer, bool bForVersion3);

	void releaseInstance();

public:

	CDriver_ScanLab_OIE (const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment, LibMCDriver_ScanLabOIE::eOIEDeviceDriverType eDeviceDriverType);

	virtual ~CDriver_ScanLab_OIE ();

	LibMCDriver_ScanLabOIE::eOIEDeviceDriverType GetDriverType() override;

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	void GetVersion(LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetDependencyResourceNames(const std::string & sLibSSLResourceName, const std::string & sLibCryptoResourceName, const std::string & sQT5CoreResourceName, const std::string & sQT5NetworkResourceName) override;

	void SetOIE3ResourceNames(const std::string& sOIECalibrationLibraryResourceName, const std::string& sRTCStreamParserResourceName) override;

	void InitializeSDK(const std::string & sOIEResourceName) override;

	void InitializeCustomSDK(const LibMCDriver_ScanLabOIE_uint64 nOIEDLLBufferSize, const LibMCDriver_ScanLabOIE_uint8 * pOIEDLLBuffer) override;

	IOIEDevice* AddDevice(const std::string& sName, const std::string& sHostName, const LibMCDriver_ScanLabOIE_uint32 nPort, IDeviceConfiguration* pDeviceConfig, const LibMCDriver_ScanLabOIE_uint64 nCorrectionDataBufferSize, const LibMCDriver_ScanLabOIE_uint8* pCorrectionDataBuffer, const LibMCDriver_ScanLabOIE_uint32 nResponseTimeOut) override;

	bool HasDevice(const std::string& sName) override;

	virtual IOIEDevice* FindDevice(const std::string& sName) override;

	void RemoveDevice(IOIEDevice* pDeviceInstance) override;

	void RemoveDeviceByName(const std::string& sName) override;

	IDeviceConfiguration* ParseDeviceConfiguration(const std::string& sDeviceConfigString) override;


};


class CDriver_ScanLab_OIE2 : public virtual CDriver_ScanLab_OIE
{
private:
public:

	CDriver_ScanLab_OIE2(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_ScanLab_OIE2();

	std::string GetType() override;

	static std::string getTypeString();

};


class CDriver_ScanLab_OIE3Compat : public virtual CDriver_ScanLab_OIE
{
private:
public:

	CDriver_ScanLab_OIE3Compat(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_ScanLab_OIE3Compat();

	std::string GetType() override;

	static std::string getTypeString();

};


class CDriver_ScanLab_OIE3 : public virtual CDriver_ScanLab_OIE
{
private:
public:

	CDriver_ScanLab_OIE3(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_ScanLab_OIE3();

	std::string GetType() override;

	static std::string getTypeString();
};


} // namespace Impl
} // namespace LibMCDriver_ScanLabOIE

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABOIE_DRIVER_SCANLAB_OIE
