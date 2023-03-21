/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CDriver_ScanLab_RTC6

*/


#ifndef __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6
#define __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_driver_scanlab.hpp"
#include "libmcdriver_scanlab_rtccontext.hpp"
#include "libmcdriver_scanlab_rtcselector.hpp"
#include "libmcdriver_scanlab_configurationpreset.hpp"
#include "libmcdriver_scanlab_managedptr.hpp"

// Parent classes
#include "libmcdriver_scanlab_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_ScanLab_RTC6 
**************************************************************************************************************************/

class CDriver_ScanLab_RTC6 : public virtual IDriver_ScanLab_RTC6, public virtual CDriver_ScanLab {
private:

	std::string m_sName; 
	std::string m_sType;

	bool m_SimulationMode;

	float m_fMaxLaserPowerInWatts;

	act_managed_ptr<IRTCSelector> m_pRTCSelector;
	act_managed_ptr<IRTCContext> m_pRTCContext;

	LibMCDriver_ScanLab::eOIERecordingMode m_OIERecordingMode;

	std::map<std::string, PDriver_ScanLab_RTC6ConfigurationPreset> m_ConfigurationPresets;

	void updateCardStatus(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance);

	

protected:

public:

	CDriver_ScanLab_RTC6(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_ScanLab_RTC6();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetToSimulationMode() override;

	bool IsInitialized() override;

	bool IsSimulationMode() override;

	void Initialise(const std::string& sIP, const std::string& sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber) override;

	void InitialiseFromConfiguration(const std::string& sPresetName) override;

	std::string GetIPAddress() override;

	std::string GetNetmask() override;

	LibMCDriver_ScanLab_uint32 GetSerialNumber() override;

	IRTCContext* GetContext() override;

	IRTCSelector* GetSelector() override;

	void LoadFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource) override;

	void LoadCustomFirmware(const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer) override;

	void SetCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) override;

	void ConfigureLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive) override;

	void ConfigureDelays(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay) override;

	void SetOIERecordingMode(const LibMCDriver_ScanLab::eOIERecordingMode eRecordingMode) override;

	LibMCDriver_ScanLab::eOIERecordingMode GetOIERecordingMode() override;

	void DrawLayer(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex) override;

	void SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier) override;

	void GetCommunicationTimeouts(LibMCDriver_ScanLab_double& dInitialTimeout, LibMCDriver_ScanLab_double& dMaxTimeout, LibMCDriver_ScanLab_double& dMultiplier) override;

	PDriver_ScanLab_RTC6ConfigurationPreset findPresetByName(const std::string & sPresetName, bool bMustExist);


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6
