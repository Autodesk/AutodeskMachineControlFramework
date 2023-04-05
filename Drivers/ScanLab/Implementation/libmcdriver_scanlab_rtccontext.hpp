/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CRTCContext

*/


#ifndef __LIBMCDRIVER_SCANLAB_RTCCONTEXT
#define __LIBMCDRIVER_SCANLAB_RTCCONTEXT

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_sdk.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CRTCContext 
**************************************************************************************************************************/

class CRTCContext : public virtual IRTCContext, public virtual CBase {

protected:
	PScanLabSDK m_pScanLabSDK;
	uint32_t m_CardNo;
	double m_dCorrectionFactor;
	double m_dZCorrectionFactor;
	eLaserPort m_LaserPort;
	bool m_bIsNetwork;
	std::vector<uint32_t> m_MCBSPSignalChannels;

	std::string m_sIPAddress;
	std::string m_sNetmask;
	uint32_t m_nLaserIndex;

	uint32_t m_nCurrentFreeVariable0;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	LibMCDriver_ScanLab::eOIEOperationMode m_OIEOperationMode;

	void writeSpeeds(const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, bool bOIEPIDControlFlag);

	uint32_t getCurrentFreeVariable0 ();

	void sendFreeVariable0 (uint32_t nValue);

public:

	CRTCContext(PScanLabSDK pScanLabSDK, uint32_t nCardNo, bool bIsNetwork, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	~CRTCContext();

	std::string GetIPAddress();

	std::string GetNetmask();

	void setIPAddress(const std::string & sIPAddress, const std::string & sNetmask);

	void setLaserIndex (const uint32_t nLaserIndex);

	void LoadFirmware(const LibMCDriver_ScanLab_uint64 nFirmwareDataBufferSize, const LibMCDriver_ScanLab_uint8* pFirmwareDataBuffer, const LibMCDriver_ScanLab_uint64 nFPGADataBufferSize, const LibMCDriver_ScanLab_uint8* pFPGADataBuffer, const LibMCDriver_ScanLab_uint64 nAuxiliaryDataBufferSize, const LibMCDriver_ScanLab_uint8* pAuxiliaryDataBuffer);

	void LoadCorrectionFile(const LibMCDriver_ScanLab_uint64 nCorrectionFileBufferSize, const LibMCDriver_ScanLab_uint8* pCorrectionFileBuffer, const LibMCDriver_ScanLab_uint32 nTableNumber, const LibMCDriver_ScanLab_uint32 nDimension);

	void SelectCorrectionTable(const LibMCDriver_ScanLab_uint32 nTableNumberHeadA, const LibMCDriver_ScanLab_uint32 nTableNumberHeadB) override;

	void ConfigureLists(const LibMCDriver_ScanLab_uint32 nSizeListA, const LibMCDriver_ScanLab_uint32 nSizeListB) override;

	void SetLaserMode(const LibMCDriver_ScanLab::eLaserMode eLaserMode, const LibMCDriver_ScanLab::eLaserPort eLaserPort) override;

	void DisableAutoLaserControl() override;

	void SetLaserControlParameters(const bool DisableLaser, const bool bFinishLaserPulseAfterOn, const bool bPhaseShiftOfLaserSignal, const bool bLaserOnSignalLowActive, const bool bLaserHalfSignalsLowActive, const bool bSetDigitalInOneHighActive, const bool bOutputSynchronizationActive) override;

	void SetLaserPulsesInBits(const LibMCDriver_ScanLab_uint32 nHalfPeriod, const LibMCDriver_ScanLab_uint32 nPulseLength) override;

	void SetLaserPulsesInMicroSeconds(const LibMCDriver_ScanLab_double dHalfPeriod, const LibMCDriver_ScanLab_double dPulseLength) override;

	void SetStandbyInBits(const LibMCDriver_ScanLab_uint32 nHalfPeriod, const LibMCDriver_ScanLab_uint32 nPulseLength) override;

	void SetStandbyInMicroSeconds(const LibMCDriver_ScanLab_double dHalfPeriod, const LibMCDriver_ScanLab_double dPulseLength) override;

	LibMCDriver_ScanLab_uint32 GetSerialNumber() override;

	LibMCDriver_ScanLab_uint32 GetLaserIndex() override;


	void SetStartList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition) override;

	void SetEndOfList() override;

	void ExecuteList(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition) override;

	void SetAutoChangePos(const LibMCDriver_ScanLab_uint32 nPosition) override;

	void SetDelays(const LibMCDriver_ScanLab_uint32 nMarkDelay, const LibMCDriver_ScanLab_uint32 nJumpDelay, const LibMCDriver_ScanLab_uint32 nPolygonDelay) override;

	void SetLaserDelaysInMicroseconds(const LibMCDriver_ScanLab_double dLaserOnDelay, const LibMCDriver_ScanLab_double dLaserOffDelay) override;

	void SetLaserDelaysInBits(const LibMCDriver_ScanLab_uint32 nLaserOnDelay, const LibMCDriver_ScanLab_uint32 nLaserOffDelay) override;

	void DrawPolyline(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue) override;

	void DrawPolylineOIE(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEControlIndex) override;

	void DrawHatches(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue) override;
	
	void DrawHatchesOIE(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEControlIndex) override;

	void AddCustomDelay(const LibMCDriver_ScanLab_uint32 nDelay) override;

	LibMCDriver_ScanLab_double GetCorrectionFactor() override;

	void GetStatus(bool & bBusy, LibMCDriver_ScanLab_uint32 & nPosition) override;

	void GetHeadStatus(const LibMCDriver_ScanLab_uint32 nHeadNo, bool& bPositionXisOK, bool& bPositionYisOK, bool& bTemperatureisOK, bool& bPowerisOK) override;

	LibMCDriver_ScanLab_uint32 GetInputPointer() override;

	void GetRTCVersion(LibMCDriver_ScanLab_uint32& nRTCVersion, LibMCDriver_ScanLab_uint32& nRTCType, LibMCDriver_ScanLab_uint32& nDLLVersion, LibMCDriver_ScanLab_uint32& nHEXVersion, LibMCDriver_ScanLab_uint32& nBIOSVersion) override;

	void GetStateValues(bool& bLaserIsOn, LibMCDriver_ScanLab_int32& nPositionX, LibMCDriver_ScanLab_int32& nPositionY, LibMCDriver_ScanLab_int32& nPositionZ, LibMCDriver_ScanLab_int32& nCorrectedPositionX, LibMCDriver_ScanLab_int32& nCorrectedPositionY, LibMCDriver_ScanLab_int32& nCorrectedPositionZ, LibMCDriver_ScanLab_int32& nFocusShift, LibMCDriver_ScanLab_int32& nMarkSpeed) override;

	void SetCommunicationTimeouts(const LibMCDriver_ScanLab_double dInitialTimeout, const LibMCDriver_ScanLab_double dMaxTimeout, const LibMCDriver_ScanLab_double dMultiplier) override;

	void GetCommunicationTimeouts(LibMCDriver_ScanLab_double& dInitialTimeout, LibMCDriver_ScanLab_double& dMaxTimeout, LibMCDriver_ScanLab_double& dMultiplier) override;

	void InitializeForOIE(const LibMCDriver_ScanLab_uint64 nSignalChannelsBufferSize, const LibMCDriver_ScanLab_uint32* pSignalChannelsBuffer, const LibMCDriver_ScanLab::eOIEOperationMode eOperationMode) override;

	void EnableOIE() override;

	void DisableOIE() override;

	void StartOIEMeasurement() override;

	void StopOIEMeasurement() override;

	void SetOIEPIDMode(const LibMCDriver_ScanLab_uint32 nOIEPIDIndex) override;

	void DisableSkyWriting() override;

	void EnableSkyWritingMode1(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost) override;

	void EnableSkyWritingMode2(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost) override;

	void EnableSkyWritingMode3(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost, const LibMCDriver_ScanLab_double dLimit) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_RTCCONTEXT
