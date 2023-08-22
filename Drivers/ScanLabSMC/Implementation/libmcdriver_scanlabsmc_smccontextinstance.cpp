/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is a stub class definition of CSMCContext

*/

#include "libmcdriver_scanlabsmc_smccontextinstance.hpp"
#include "libmcdriver_scanlabsmc_smcconfiguration.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"
#include "libmcdriver_scanlabsmc_smcjob.hpp"

// Include custom headers here.


using namespace LibMCDriver_ScanLabSMC::Impl;

/*************************************************************************************************************************
 Class definition of CSMCContext 
**************************************************************************************************************************/

CSMCContextInstance::CSMCContextInstance(const std::string& sContextName, ISMCConfiguration* pSMCConfiguration, PScanLabSMCSDK pSDK, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_pSDK (pSDK), m_pDriverEnvironment (pDriverEnvironment), m_sContextName (sContextName)
{
	if (pSDK.get() == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
	if (pDriverEnvironment.get() == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);
	if (sContextName.empty ())
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDCONTEXTNAME);
	if (pSMCConfiguration == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_EMPTYCONFIGURATIONXML);

	auto pCastedConfiguration = dynamic_cast<CSMCConfiguration*> (pSMCConfiguration);
	if (pCastedConfiguration == nullptr)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDCAST);

	m_pWorkingDirectory = m_pDriverEnvironment->CreateWorkingDirectory ();

	std::string sConfigurationXML = pCastedConfiguration->buildConfigurationXML(m_pWorkingDirectory.get ());

	std::vector<uint8_t> Buffer (sConfigurationXML.begin (), sConfigurationXML.end ());

	auto pConfigurationFile = m_pWorkingDirectory->StoreCustomDataInTempFile("xml", Buffer);
	std::string sConfigurationFilePath = pConfigurationFile->GetAbsoluteFileName ();

	slscHandle newHandle = 0;
	m_pSDK->checkError(m_pSDK->slsc_cfg_initialize_from_file(&newHandle, sConfigurationFilePath.c_str()));

	m_pContextHandle = std::make_shared<CSMCContextHandle>(m_pSDK, newHandle);

	pConfigurationFile = nullptr;

}

CSMCContextInstance::~CSMCContextInstance()
{
	m_pContextHandle = nullptr;
	m_pSDK = nullptr;

	m_pWorkingDirectory = nullptr;

}


void CSMCContextInstance::SetToSimulationMode()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

bool CSMCContextInstance::IsSimulationMode()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

void CSMCContextInstance::SetFirmware(const LibMCDriver_ScanLabSMC_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLabSMC_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pFPGADataBuffer, const LibMCDriver_ScanLabSMC_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pAuxiliaryDataBuffer)
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

void CSMCContextInstance::SetCorrectionFile(const LibMCDriver_ScanLabSMC_uint64 nCorrectionFileDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pCorrectionFileDataBuffer)
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

void CSMCContextInstance::ReinitializeInstance()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

std::string CSMCContextInstance::GetIPAddress()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

std::string CSMCContextInstance::GetNetmask()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ScanLabSMC_uint32 CSMCContextInstance::GetSerialNumber()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}

LibMCDriver_ScanLabSMC_uint32 CSMCContextInstance::GetLaserIndex()
{
	throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_NOTIMPLEMENTED);
}


