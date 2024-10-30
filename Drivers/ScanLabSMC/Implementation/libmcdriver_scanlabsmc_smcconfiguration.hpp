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


Abstract: This is the class declaration of CSMCConfiguration

*/


#ifndef __LIBMCDRIVER_SCANLABSMC_SMCCONFIGURATION
#define __LIBMCDRIVER_SCANLABSMC_SMCCONFIGURATION

#include "libmcdriver_scanlabsmc_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlabsmc_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_ScanLabSMC {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSMCConfiguration 
**************************************************************************************************************************/

class CSMCConfiguration : public virtual ISMCConfiguration, public virtual CBase {
private:

    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    LibMCDriver_ScanLabSMC::eDynamicViolationReaction m_DynamicViolationReaction;
    LibMCDriver_ScanLabSMC::eWarnLevel m_WarnLevel;
    uint32_t m_nSerialNumber;
    std::vector<uint8_t> m_CorrectionFileData;
    std::vector<uint8_t> m_FirmwareData;
    std::vector<uint8_t> m_FPGAData;
    std::vector<uint8_t> m_AuxiliaryData;
    std::string m_sIPAddress;
    std::string m_sConfigurationTemplateXML;

public:

    CSMCConfiguration(LibMCEnv::PDriverEnvironment pDriverEnvironment);

    virtual ~CSMCConfiguration();

	void SetDynamicViolationReaction(const LibMCDriver_ScanLabSMC::eDynamicViolationReaction eValue) override;

	LibMCDriver_ScanLabSMC::eDynamicViolationReaction GetDynamicViolationReaction() override;

	void SetWarnLevel(const LibMCDriver_ScanLabSMC::eWarnLevel eValue) override;

	LibMCDriver_ScanLabSMC::eWarnLevel GetWarnLevel() override;

	void SetSerialNumber(const LibMCDriver_ScanLabSMC_uint32 nValue) override;

	LibMCDriver_ScanLabSMC_uint32 GetSerialNumber() override;

	void SetIPAddress(const std::string& sValue) override;

	std::string GetIPAddress() override;

	void SetCorrectionFile(const LibMCDriver_ScanLabSMC_uint64 nCorrectionFileDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pCorrectionFileDataBuffer) override;

	void SetCorrectionFileResource(const std::string& sResourceName) override;

    void SetConfigurationTemplateResource(const std::string& sResourceName) override;
    
    void SetConfigurationTemplate(const std::string& sTemplateXML) override;

    std::string GetConfigurationTemplate() override;

    void SetFirmware(const LibMCDriver_ScanLabSMC_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLabSMC_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pFPGADataBuffer, const LibMCDriver_ScanLabSMC_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLabSMC_uint8* pAuxiliaryDataBuffer) override;

    void SetFirmwareResources(const std::string& sFirmwareDataResource, const std::string& sFPGADataResource, const std::string& sAuxiliaryDataResource) override;

    std::string buildConfigurationXML(LibMCEnv::CWorkingDirectory * pWorkingDirectory, LibMCEnv::PWorkingFile & newCorrectionFile);

};

} // namespace Impl
} // namespace LibMCDriver_ScanLabSMC

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABSMC_SMCCONFIGURATION
