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
#include <set>

namespace LibMCDriver_ScanLab {
namespace Impl {


typedef struct _sMeasurementTagInfo {
	uint32_t m_nCurrentPartID;
	uint32_t m_nCurrentProfileID;
	uint32_t m_nCurrentSegmentID;
	uint32_t m_nCurrentVectorID;
} sMeasurementTagInfo;

/*************************************************************************************************************************
 Class declaration of CRTCContext 
**************************************************************************************************************************/
class CRTCContextOwnerData {
private:
	PScanLabSDK m_pScanlabSDK;
	std::string m_sAttributeFilterNameSpace;
	std::string m_sAttributeFilterName;
	int64_t m_nAttributeFilterValue;
	double m_dMaxLaserPowerInWatts;
	eOIERecordingMode m_OIERecordingMode;


public:
	CRTCContextOwnerData();
	virtual ~CRTCContextOwnerData();

	void getAttributeFilters(std::string& sAttributeFilterNameSpace, std::string& sAttributeFilterName, int64_t& nAttributeFilterValue);
	void setAttributeFilters(const std::string& sAttributeFilterNameSpace, const std::string& sAttributeFilterName, const int64_t sAttributeFilterValue);
	void setMaxLaserPower(double dMaxLaserPowerInWatts);
	double getMaxLaserPower();
	void setOIERecordingMode(eOIERecordingMode oieRecordingMode);
	eOIERecordingMode getOIERecordingMode();
	PScanLabSDK getScanLabSDK();
	void setScanLabSDK(PScanLabSDK pScanLabSDK);
};

typedef std::shared_ptr<CRTCContextOwnerData> PRTCContextOwnerData;

class CRTCContext : public virtual IRTCContext, public virtual CBase {

protected:
	PRTCContextOwnerData m_pOwnerData;

	PScanLabSDK m_pScanLabSDK;
	uint32_t m_CardNo;
	double m_dCorrectionFactor;
	double m_dZCorrectionFactor;
	eLaserPort m_LaserPort;
	bool m_bIsNetwork;
	std::vector<uint32_t> m_MCBSPSignalChannels;

	bool m_b2DMarkOnTheFlyEnabled;
	double m_dScaleXInBitsPerEncoderStep;
	double m_dScaleYInBitsPerEncoderStep;


	std::string m_sIPAddress;
	std::string m_sNetmask;
	uint32_t m_nLaserIndex;

	sMeasurementTagInfo m_CurrentMeasurementTagInfo;
	std::vector<sMeasurementTagInfo> m_MeasurementTags;

	bool m_bEnableOIEPIDControl;
	uint32_t m_nCurrentFreeVariable0;
	bool m_bMeasurementTagging;

	double m_dLaserOriginX;
	double m_dLaserOriginY;
	double m_dLaserFieldMinX;
	double m_dLaserFieldMinY;
	double m_dLaserFieldMaxX;
	double m_dLaserFieldMaxY;
	bool m_bHasLaserField;

	SpatialPowerModulationCallback m_pModulationCallback;
	void* m_pModulationCallbackUserData;
	bool m_bEnableLineSubdivision;
	double m_dLineSubdivisionThreshold;

	std::vector<uint8_t> m_HeadTransform;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	LibMCDriver_ScanLab::eOIEOperationMode m_OIEOperationMode;

	double m_dLaserPowerCalibrationUnits;
	std::vector<sLaserCalibrationPoint> m_LaserPowerCalibrationList;
	
	void writeJumpSpeed (float jumpSpeed);

	void writeMarkSpeed(float markSpeed);

	void writePower(double dPowerInPercent, bool bOIEPIDControlFlag);

	void writeSpeeds(const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, bool bOIEPIDControlFlag);

	uint32_t getCurrentFreeVariable0 ();

	void sendFreeVariable0 (uint32_t nValue);

	void sendOIEMeasurementTag (uint32_t nCurrentVectorID);

	uint32_t saveRecordedDataBlock(std::ofstream& MyFile, uint32_t DataStart, uint32_t DataEnd, double CalibrationFactorXY);

	void addLayerToListEx(LibMCEnv::PToolpathLayer pLayer, eOIERecordingMode oieRecordingMode, uint32_t nAttributeFilterID, int64_t nAttributeFilterValue, float fMaxLaserPowerInWatts, bool bFailIfNonAssignedDataExists);

	void updateLaserField(double dMinXInMM, double dMaxXInMM, double dMinYInMM, double dMaxYInMM);
	
	void clearLaserField();

	double adjustLaserPowerCalibration(double dLaserPowerInPercent, double dPowerOffsetInPercent, double ddPowerOutputScaling);

	// Performs a marking with the settings given by modulation and subdivision
	void jumpAbsoluteEx (double dTargetXInMM, double dTargetYInMM);
	void markAbsoluteEx (double dStartXInMM, double dStartYInMM, double dTargetXInMM, double dTargetYInMM, double dLaserPowerInPercent, bool bOIEControlFlag);

public:

	CRTCContext(PRTCContextOwnerData pOwnerData, uint32_t nCardNo, bool bIsNetwork, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	~CRTCContext();

	std::string GetIPAddress();

	std::string GetNetmask();

	void setIPAddress(const std::string & sIPAddress, const std::string & sNetmask);

	// setLaserIndex should not be exposed via API from the Context.
	// Laser Index Management should be implemented in the Driver.
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

	void SetLaserDelaysInBits(const LibMCDriver_ScanLab_int32 nLaserOnDelay, const LibMCDriver_ScanLab_int32 nLaserOffDelay) override;

	void DrawPolyline(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D * pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue) override;

	void DrawPolylineOIE(const LibMCDriver_ScanLab_uint64 nPointsBufferSize, const LibMCDriver_ScanLab::sPoint2D* pPointsBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEControlIndex) override;

	void DrawHatches(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D * pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue) override;
	
	void DrawHatchesOIE(const LibMCDriver_ScanLab_uint64 nHatchesBufferSize, const LibMCDriver_ScanLab::sHatch2D* pHatchesBuffer, const LibMCDriver_ScanLab_single fMarkSpeed, const LibMCDriver_ScanLab_single fJumpSpeed, const LibMCDriver_ScanLab_single fPower, const LibMCDriver_ScanLab_single fZValue, const uint32_t nOIEControlIndex) override;

	void AddLayerToList(LibMCEnv::PToolpathLayer pLayer, bool bFailIfNonAssignedDataExists) override;

	void WaitForEncoderX(const LibMCDriver_ScanLab_double dPosition, const bool bInPositiveHalfPlane) override;

	void WaitForEncoderY(const LibMCDriver_ScanLab_double dPosition, const bool bInPositiveHalfPlane) override;

	void WaitForEncoderXSteps(const LibMCDriver_ScanLab_int32 nPositionInSteps, const bool bInPositiveHalfPlane) override;

	void WaitForEncoderYSteps(const LibMCDriver_ScanLab_int32 nPositionInSteps, const bool bInPositiveHalfPlane) override;

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

	void StartOIEMeasurementEx(bool bTriggerOnFlag);

	void StopOIEMeasurement() override;

	void SetOIEPIDMode(const LibMCDriver_ScanLab_uint32 nOIEPIDIndex) override;

	void ClearOIEMeasurementTags() override;

	void EnableOIEMeasurementTagging() override;

	void DisableOIEMeasurementTagging() override;

	void MapOIEMeasurementTag(const LibMCDriver_ScanLab_uint32 nMeasurementTag, LibMCDriver_ScanLab_uint32& nPartID, LibMCDriver_ScanLab_uint32& nProfileID, LibMCDriver_ScanLab_uint32& nSegmentID, LibMCDriver_ScanLab_uint32& nVectorID) override;

	void EnableOIEPIDControl() override;

	void DisableOIEPIDControl() override;

	void DisableSkyWriting() override;

	void EnableSkyWritingMode1(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost) override;

	void EnableSkyWritingMode2(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost) override;

	void EnableSkyWritingMode3(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost, const LibMCDriver_ScanLab_double dLimit) override;

	void EnableSkyWritingMode4(const LibMCDriver_ScanLab_double dTimelag, const LibMCDriver_ScanLab_int64 nLaserOnShift, const LibMCDriver_ScanLab_int64 nNPrev, const LibMCDriver_ScanLab_int64 nNPost, const LibMCDriver_ScanLab_double dLimit) override;

	void SetTransformationAngle(const LibMCDriver_ScanLab_double dAngleInDegrees) override;

	void SetTransformationScale(const LibMCDriver_ScanLab_double dScaleFactor) override;

	void SetTransformationOffset(const LibMCDriver_ScanLab_int32 nOffsetX, const LibMCDriver_ScanLab_int32 nOffsetY) override;
	
	void SetTransformationMatrix(const LibMCDriver_ScanLab_double dM11, const LibMCDriver_ScanLab_double dM12, const LibMCDriver_ScanLab_double dM21, const LibMCDriver_ScanLab_double dM22) override;

	void PrepareRecording() override;

	void EnableRecording() override;

	void DisableRecording() override;

	void ExecuteListWithRecording(const LibMCDriver_ScanLab_uint32 nListIndex, const LibMCDriver_ScanLab_uint32 nPosition) override;

	void EnableTimelagCompensation(const LibMCDriver_ScanLab_uint32 nTimeLagXYInMicroseconds, const LibMCDriver_ScanLab_uint32 nTimeLagZInMicroseconds) override;

	void DisableTimelagCompensation() override;

	void EnableMarkOnTheFly2D(const LibMCDriver_ScanLab_double dScaleXInMMperEncoderSteps, const LibMCDriver_ScanLab_double dScaleYInMMperEncoderSteps) override;

	void DisableMarkOnTheFly2D() override;
	
	bool MarkOnTheFly2DIsEnabled() override; 	

	void Get2DMarkOnTheFlyPosition(LibMCDriver_ScanLab_int32& nPositionX, LibMCDriver_ScanLab_int32& nPositionY) override;

	LibMCDriver_ScanLab_uint32 CheckOnTheFlyError(const bool bFailIfError) override;

	void SetLaserOrigin(const LibMCDriver_ScanLab_double dOriginX, const LibMCDriver_ScanLab_double dOriginY) override;

	void GetLaserOrigin(LibMCDriver_ScanLab_double& dOriginX, LibMCDriver_ScanLab_double& dOriginY) override;

	void SetLaserField(const LibMCDriver_ScanLab_double dMinX, const LibMCDriver_ScanLab_double dMinY, const LibMCDriver_ScanLab_double dMaxX, const LibMCDriver_ScanLab_double dMaxY) override;

	bool GetLaserField(LibMCDriver_ScanLab_double& dMinX, LibMCDriver_ScanLab_double& dMinY, LibMCDriver_ScanLab_double& dMaxX, LibMCDriver_ScanLab_double& dMaxY) override;

	void ResetLaserField() override;

	void EnableRangeChecking() override;

	void DisableRangeChecking() override;

	void AddJumpMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY) override;

	void AddMarkMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY) override;

	void AddFreeVariable(const LibMCDriver_ScanLab_uint32 nVariableNo, const LibMCDriver_ScanLab_uint32 nValue) override;

	void AddSetPower(const LibMCDriver_ScanLab_single fPower) override;

	void AddSetPowerForPIDControl(const LibMCDriver_ScanLab_single fPowerInPercent) override;


	void AddSetJumpSpeed(const LibMCDriver_ScanLab_single fJumpSpeed) override;

	void AddSetMarkSpeed(const LibMCDriver_ScanLab_single fMarkSpeed) override;

	void AddTimedMarkMovement(const LibMCDriver_ScanLab_double dTargetX, const LibMCDriver_ScanLab_double dTargetY, const LibMCDriver_ScanLab_double dDuration) override;

	LibMCDriver_ScanLab_uint32 GetCurrentFreeVariable(const LibMCDriver_ScanLab_uint32 nVariableNo) override;

	LibMCDriver_ScanLab_uint32 GetTimeStamp() override;

	void StopExecution() override;

	bool LaserPowerCalibrationIsEnabled() override;

	bool LaserPowerCalibrationIsLinear() override;

	void ClearLaserPowerCalibration() override;

	void GetLaserPowerCalibration(LibMCDriver_ScanLab_uint64 nCalibrationPointsBufferSize, LibMCDriver_ScanLab_uint64* pCalibrationPointsNeededCount, LibMCDriver_ScanLab::sLaserCalibrationPoint* pCalibrationPointsBuffer) override;

	void SetLinearLaserPowerCalibration(const LibMCDriver_ScanLab_double dPowerOffsetInPercent, const LibMCDriver_ScanLab_double dPowerOutputScaling) override;

	void SetPiecewiseLinearLaserPowerCalibration(const LibMCDriver_ScanLab_uint64 nCalibrationPointsBufferSize, const LibMCDriver_ScanLab::sLaserCalibrationPoint* pCalibrationPointsBuffer) override;

	void EnableSpatialLaserPowerModulation(const LibMCDriver_ScanLab::SpatialPowerModulationCallback pModulationCallback, const LibMCDriver_ScanLab_pvoid pUserData) override;

	void DisablePowerModulation() override;

	void EnableLineSubdivision(const LibMCDriver_ScanLab_double dLengthThreshold) override;

	void DisableLineSubdivision() override;

	LibMCDriver_ScanLab_int32 ReadMultiMCBSP(const LibMCDriver_ScanLab_uint32 nRegisterNo) override;

};


} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_RTCCONTEXT
