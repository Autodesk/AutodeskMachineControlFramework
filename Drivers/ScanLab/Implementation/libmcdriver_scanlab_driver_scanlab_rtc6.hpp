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

// Parent classes
#include "libmcdriver_scanlab_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


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

	std::shared_ptr<IRTCSelector> m_pRTCSelector;
	std::shared_ptr<IRTCContext> m_pRTCContext;

	void internalBegin();
	void internalExecute();

	void updateCardStatus();

protected:

public:

	CDriver_ScanLab_RTC6(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void QueryParameters() override;

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void Initialise(const std::string& sIP, const std::string& sNetmask, const LibMCDriver_ScanLab_uint32 nTimeout, const LibMCDriver_ScanLab_uint32 nSerialNumber) override;

	void LoadFirmware(const std::string& sFirmwareResource, const std::string& sFPGAResource, const std::string& sAuxiliaryResource) override;

	void SetCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension, const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) override;

	void ConfigureLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort, const LibMCDriver_ScanLab_double dMaxLaserPower, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive) override;

	void ConfigureDelays(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay, const LibMCDriver_ScanLab_double dMarkDelay, const LibMCDriver_ScanLab_double dJumpDelay, const LibMCDriver_ScanLab_double dPolygonDelay) override;

	void DrawLayer(const std::string& sStreamUUID, const LibMCDriver_ScanLab_uint32 nLayerIndex) override;

	void SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier) override;

	void GetCommunicationTimeouts(LibMCDriver_ScanLab_double& dInitialTimeout, LibMCDriver_ScanLab_double& dMaxTimeout, LibMCDriver_ScanLab_double& dMultiplier) override;


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_DRIVER_SCANLAB_RTC6
