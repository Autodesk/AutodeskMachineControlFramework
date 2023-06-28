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


Abstract: This is the class declaration of CDriver_ScanLab_RTC6xN

*/


#ifndef __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6XN
#define __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6XN

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_managedptr.hpp"

// Parent classes
#include "libmcdriver_scanlab_driver_scanlab.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlab_rtccontext.hpp"
#include "libmcdriver_scanlab_rtcselector.hpp"
#include "libmcdriver_scanlab_configurationpreset.hpp"


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_ScanLab_RTC6xN 
**************************************************************************************************************************/

class CDriver_ScanLab_RTC6xN : public virtual IDriver_ScanLab_RTC6xN, public virtual CDriver_ScanLab {
private:

	uint32_t m_nScannerCount;

	std::string m_sName;
	std::string m_sType;

	bool m_SimulationMode;

	float m_fMaxLaserPowerInWatts;

	uint32_t m_nAttributeFilterID;
	int64_t m_nAttributeFilterValue;

	act_managed_ptr<IRTCSelector> m_pRTCSelector;
	
	std::map<uint32_t, act_managed_ptr<IRTCContext>> m_ScannerContexts;
	std::map<uint32_t, act_managed_ptr<IRTCContext>> m_LaserIndexMapping;

	std::map<std::string, PDriver_ScanLab_RTC6ConfigurationPreset> m_ConfigurationPresets;

	LibMCDriver_ScanLab::eOIERecordingMode m_OIERecordingMode;

	void updateCardStatus(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance);

	act_managed_ptr<IRTCContext> getRTCContextForScannerIndex(uint32_t nScannerIndex, bool bFailIfNotExisting);

	act_managed_ptr<IRTCContext> getRTCContextForLaserIndex(uint32_t nLaserIndex, bool bFailIfNotExisting);

public:

	CDriver_ScanLab_RTC6xN(const std::string& sName, const std::string& sType, uint32_t nLaserCount, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_ScanLab_RTC6xN();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;


	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	bool IsInitialized() override;

	bool ScannerIsInitialized(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;

	LibMCDriver_ScanLab_uint32 GetScannerCount() override;

	void InitialiseScanner(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sIP, const std::string & sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber, const LibMCDriver_ScanLab_uint32 nLaserIndex) override;

	void InitialiseScannerFromConfiguration(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sPresetName) override;

	std::string GetIPAddress(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;

	std::string GetNetmask(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;

	LibMCDriver_ScanLab_uint32 GetSerialNumber(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;

	LibMCDriver_ScanLab_uint32 GetLaserIndex(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;

	IRTCSelector* GetSelector() override;

	IRTCContext * GetContext(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;

	void LoadFirmware(const LibMCDriver_ScanLab_uint32 nScannerIndex, const std::string & sFirmwareResource, const std::string & sFPGAResource, const std::string & sAuxiliaryResource) override;

	void LoadCustomFirmware(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8 * pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8 * pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8 * pAuxiliaryDataBuffer) override;

	void SetCorrectionFile(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8 * pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) override;

	void ConfigureLaserMode(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive) override;

	void ConfigureDelays(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay) override;

	void SetOIERecordingMode(const LibMCDriver_ScanLab::eOIERecordingMode eRecordingMode) override;

	LibMCDriver_ScanLab::eOIERecordingMode GetOIERecordingMode() override;

	void SetAttributeFilter(const LibMCDriver_ScanLab_uint32 nAttributeID, const LibMCDriver_ScanLab_int64 nAttributeValue) override;

	void DrawLayer(const std::string & sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex, const bool bFailIfNonAssignedDataExists) override;

	void SetCommunicationTimeouts(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier) override;

	void GetCommunicationTimeouts(const LibMCDriver_ScanLab_uint32 nScannerIndex, LibMCDriver_ScanLab_double & dInitialTimeout, LibMCDriver_ScanLab_double & dMaxTimeout, LibMCDriver_ScanLab_double & dMultiplier) override;

	PDriver_ScanLab_RTC6ConfigurationPreset findPresetByName(const std::string& sPresetName, bool bMustExist);

	void EnableTimelagCompensation(const LibMCDriver_ScanLab_uint32 nScannerIndex, const LibMCDriver_ScanLab_uint32 nTimeLagXYInMicroseconds, const LibMCDriver_ScanLab_uint32 nTimeLagZInMicroseconds) override;

	void DisableTimelagCompensation(const LibMCDriver_ScanLab_uint32 nScannerIndex) override;


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6XN
