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


Abstract: This is a stub class definition of CDriver_ScanLab_OIE

*/

#include "libmcdriver_scanlaboie_driver_scanlab_oie.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace LibMCDriver_ScanLabOIE::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_ScanLab_OIE 
**************************************************************************************************************************/

CDriver_ScanLab_OIE::CDriver_ScanLab_OIE(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pDriverEnvironment (pDriverEnvironment), 
	  m_sDriverName(sName), 
	  bSimulationMode (false),
	  m_pInstance (nullptr)
{
	if (sName.empty ())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDDRIVERNAME);
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDDRIVERENVIRONMENT);

	m_sLibSSLResourceName = "libssl-1_1";
	m_sLibCryptoResourceName = "libcrypto-1_1";
	m_sQT5CoreResourceName = "qt5core";
	m_sQT5NetworkResourceName = "qt5network";
}

CDriver_ScanLab_OIE::~CDriver_ScanLab_OIE()
{
	releaseInstance();
}

void CDriver_ScanLab_OIE::Configure(const std::string& sConfigurationString)
{	

	m_pDriverEnvironment->RegisterIntegerParameter("sdk_major_version", "Major OIE SDK Version", 0);
	m_pDriverEnvironment->RegisterIntegerParameter("sdk_minor_version", "Minor OIE SDK Version", 0);
	m_pDriverEnvironment->RegisterIntegerParameter("sdk_patch_version", "Patch OIE SDK Version", 0);

}

std::string CDriver_ScanLab_OIE::GetName()
{
	return m_sDriverName;
}

std::string CDriver_ScanLab_OIE::getTypeString()
{
	return "oie-1.0";
}

std::string CDriver_ScanLab_OIE::GetType()
{
	return getTypeString ();
}

void CDriver_ScanLab_OIE::GetVersion(LibMCDriver_ScanLabOIE_uint32& nMajor, LibMCDriver_ScanLabOIE_uint32& nMinor, LibMCDriver_ScanLabOIE_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_SCANLABOIE_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_SCANLABOIE_VERSION_MINOR;
	nMicro = LIBMCDRIVER_SCANLABOIE_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_ScanLab_OIE::GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName)
{
	sNameSpace = "LibMCDriver_ScanLabOIE";
	sBaseName = "libmcdriver_scanlaboie";
}


void CDriver_ScanLab_OIE::QueryParameters()
{
	
}



void CDriver_ScanLab_OIE::SetDependencyResourceNames(const std::string & sLibSSLResourceName, const std::string & sLibCryptoResourceName, const std::string & sQT5CoreResourceName, const std::string & sQT5NetworkResourceName)
{
	if (sLibSSLResourceName.empty ())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRESOURCENAME);
	if (sLibCryptoResourceName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRESOURCENAME);
	if (sQT5CoreResourceName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRESOURCENAME);
	if (sQT5NetworkResourceName.empty())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRESOURCENAME);

	m_sLibSSLResourceName = sLibSSLResourceName;
	m_sLibCryptoResourceName = sLibCryptoResourceName;
	m_sQT5CoreResourceName = sQT5CoreResourceName;
	m_sQT5NetworkResourceName = sQT5NetworkResourceName;

}


void CDriver_ScanLab_OIE::initializeSDKEx(const std::vector<uint8_t>& SDKDLLBuffer)
{
	if (m_pOIESDK.get() != nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_SDKALREADYINITIALIZED);

	releaseInstance();

	if (!bSimulationMode) {

		if (SDKDLLBuffer.size() == 0)
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSCANLABOIESDK);

		std::string sFileName;
#ifdef _WIN32
		sFileName = "liboie.dll";
#else
		sFileName = "liboie.so";
#endif

		m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory();

		m_pSDKLibraryFile = m_pWorkingDirectory->StoreCustomData(sFileName, LibMCEnv::CInputVector<uint8_t>(SDKDLLBuffer.data(), SDKDLLBuffer.size()));

		if (m_pSDKLibraryFile->GetSize() == 0)
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSCANLABOIESDK);

		if (m_pDriverEnvironment->MachineHasResourceData(m_sLibSSLResourceName)) {
			std::vector<uint8_t> DLLBuffer;
			m_pDriverEnvironment->RetrieveMachineResourceData(m_sLibSSLResourceName, DLLBuffer);
			m_pLibSSLResourceFile = m_pWorkingDirectory->StoreCustomData("libssl-1_1.dll", DLLBuffer);
		}
		else {
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTSTORELIBRESSLRESOURCE);
		}

		if (m_pDriverEnvironment->MachineHasResourceData(m_sLibCryptoResourceName)) {
			std::vector<uint8_t> DLLBuffer;
			m_pDriverEnvironment->RetrieveMachineResourceData(m_sLibCryptoResourceName, DLLBuffer);
			m_pLibSSLResourceFile = m_pWorkingDirectory->StoreCustomData("libcrypto-1_1.dll", DLLBuffer);
		}
		else {
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTSTORELIBCRYPTORESOURCE);
		}
		
		if (m_pDriverEnvironment->MachineHasResourceData(m_sQT5CoreResourceName)) {
			std::vector<uint8_t> DLLBuffer;
			m_pDriverEnvironment->RetrieveMachineResourceData(m_sQT5CoreResourceName, DLLBuffer);
			m_pQT5CoreResourceFile = m_pWorkingDirectory->StoreCustomData("Qt5Core.dll", DLLBuffer);
		}
		else {
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTSTOREQT5CORERESOURCE);
		}

		if (m_pDriverEnvironment->MachineHasResourceData(m_sQT5NetworkResourceName)) {
			std::vector<uint8_t> DLLBuffer;
			m_pDriverEnvironment->RetrieveMachineResourceData(m_sQT5NetworkResourceName, DLLBuffer);
			m_pQT5CoreResourceFile = m_pWorkingDirectory->StoreCustomData("Qt5Network.dll", DLLBuffer);
		}
		else {
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTSTOREQT5NETWORKRESOURCE);
		}
		m_pOIESDK = std::make_shared<CScanLabOIESDK>(m_pSDKLibraryFile->GetAbsoluteFileName());

		m_pOIESDK->initDLL();

		uint32_t majorVersion = 0;
		uint32_t minorVersion = 0;
		uint32_t patchVersion = 0;
		m_pOIESDK->oie_get_version(&majorVersion, &minorVersion, &patchVersion);

		m_pDriverEnvironment->SetIntegerParameter("sdk_major_version", majorVersion);
		m_pDriverEnvironment->SetIntegerParameter("sdk_minor_version", minorVersion);
		m_pDriverEnvironment->SetIntegerParameter("sdk_patch_version", patchVersion);

		m_pInstance = m_pOIESDK->oie_create();
	}
	else {

		m_pDriverEnvironment->SetIntegerParameter("sdk_major_version", 1);
		m_pDriverEnvironment->SetIntegerParameter("sdk_minor_version", 5);
		m_pDriverEnvironment->SetIntegerParameter("sdk_patch_version", 1);

	}

}

void CDriver_ScanLab_OIE::InitializeSDK(const std::string & sOIEResourceName)
{

	std::vector<uint8_t> SDKBuffer;

	if (m_pDriverEnvironment->MachineHasResourceData(sOIEResourceName)) {
		m_pDriverEnvironment->RetrieveMachineResourceData(sOIEResourceName, SDKBuffer);		
	}
	else {
		m_pDriverEnvironment->RetrieveDriverResourceData(sOIEResourceName, SDKBuffer);			
	}

	initializeSDKEx (SDKBuffer);

}

void CDriver_ScanLab_OIE::InitializeCustomSDK(const LibMCDriver_ScanLabOIE_uint64 nOIEDLLBufferSize, const LibMCDriver_ScanLabOIE_uint8 * pOIEDLLBuffer)
{
	if ((nOIEDLLBufferSize == 0) || (pOIEDLLBuffer == nullptr))
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSCANLABOIESDK);

	std::vector<uint8_t> SDKBuffer;
	SDKBuffer.resize(nOIEDLLBufferSize);

	for (size_t nIndex = 0; nIndex < nOIEDLLBufferSize; nIndex++)
		SDKBuffer[nIndex] = pOIEDLLBuffer[nIndex];

	initializeSDKEx(SDKBuffer);

}

IOIEDevice * CDriver_ScanLab_OIE::AddDevice(const std::string & sHostName, const LibMCDriver_ScanLabOIE_uint32 nPort, const LibMCDriver_ScanLabOIE_uint32 nResponseTimeOut)
{
	return nullptr;
}

void CDriver_ScanLab_OIE::RemoveDevice(IOIEDevice* pDeviceInstance)
{
	
}

void CDriver_ScanLab_OIE::releaseInstance()
{
	if ((m_pInstance != nullptr) && (m_pOIESDK.get() != nullptr)) {
		m_pOIESDK->oie_destroy(m_pInstance);

	}

	m_pInstance = nullptr;
	m_pOIESDK = nullptr;
	m_pSDKLibraryFile = nullptr;
	m_pWorkingDirectory = nullptr;
	m_pLibSSLResourceFile = nullptr;
	m_pLibCryptoResourceFile = nullptr;
	m_pQT5CoreResourceFile = nullptr;
	m_pQT5NetworkResourceFile = nullptr;
}

