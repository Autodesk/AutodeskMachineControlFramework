/*++

Copyright (C) 2024 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __LIBMCDRIVER_RAYLASE_SDK
#define __LIBMCDRIVER_RAYLASE_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define RAYLASE_CALLINGCONVENTION __stdcall

#else

#define RAYLASE_CALLINGCONVENTION 

#endif



namespace LibMCDriver_Raylase {
	namespace Impl {

		enum class eRLLaserStatus : uint32_t {
			lsNone = 0,
			lsPilot = 1,
			lsArmed = 2,
			lsAlarm = 4
		};

		enum class eRLIOPort : uint32_t {
			ioPortA = 0,
			ioPortB = 1,
			ioPortC = 2,
			ioPortD = 3,
			ioPortE = 4,
			ioLaserIn = 11,
			ioLaserOut = 12,
		};

		enum class eRLIODirection : uint32_t {
			ioInput = 0,
			ioOutput = 1,
		};


		enum class eRLIOVoltage : uint32_t {
			ioNone = 0,
			ioZero = 1,
			ioThreePointThree = 2,
			ioFive = 3
		};

		enum class eRLPolarity : uint32_t {
			plActiveHigh = 0,
			plActiveLow = 1
		};

		enum class eRLPinAction : uint32_t {
			paClear = 0,
			paToggle = 1,
			paSet = 2,
			paWrite = 3
		};

		enum class eRLOperatingState : uint32_t {
			opsNormal = 0,
			opsConfigurationFailed = 1,
			opsInitializationFailed = 2,
			opsScannerError = 4
		};

		enum class eRLPowerChannels : uint32_t {
			ptcNone = 0,
			ptcChannel0 = 1,
			ptcChannel1 = 2,
			ptcAllChannels = 0xFF
		};
		
		enum class eRLScanHeadFlags : uint32_t
		{
			XY2 = 0 << 0,
			SL2 = 1 << 0,
			RL3 = 2 << 0,
			Defocus = 1 << 2,
			SingleHead = 0 << 3,
			DualHead = 1 << 3,
			Auxiliary = 1 << 4,
			Extended = 1 << 5,
			FourD = 1 << 6
		};

		enum class eRLScanHeadFormat : uint32_t
		{
			shfXY2_Single3D = (uint32_t) eRLScanHeadFlags::XY2 | (uint32_t) eRLScanHeadFlags::SingleHead,
			shfXY2_Dual3D = (uint32_t) eRLScanHeadFlags::XY2 | (uint32_t) eRLScanHeadFlags::DualHead,

			shfSL2_Single3D = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead,
			shfSL2_Single3DX = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::Extended,
			shfSL2_Single3DA = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::Auxiliary,

			shfSL2_Single4DF = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus,
			shfSL2_Single4DFX = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus | (uint32_t)eRLScanHeadFlags::Extended,
			shfSL2_Single4DFA = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus | (uint32_t)eRLScanHeadFlags::Auxiliary,

			shfSL2_Single4D = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD,

			shfSL2_Dual2D = (uint32_t)eRLScanHeadFlags::SL2 | (uint32_t)eRLScanHeadFlags::DualHead,

			shfRL3_Single3D = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead,
			shfRL3_Single3DX = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::Extended,
			shfRL3_Single3DA = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::Auxiliary,
			shfRL3_Single3DXA = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::Extended | (uint32_t)eRLScanHeadFlags::Auxiliary,

			shfRL3_Single4DF = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus,
			shfRL3_Single4DFX = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus | (uint32_t)eRLScanHeadFlags::Extended,
			shfRL3_Single4DFA = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus | (uint32_t)eRLScanHeadFlags::Auxiliary,
			shfRL3_Single4DFXA = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Defocus | (uint32_t)eRLScanHeadFlags::Extended | (uint32_t) eRLScanHeadFlags::Auxiliary,

			shfRL3_Single4D = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD,
			shfRL3_Single4DX = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Extended,
			shfRL3_Single4DA = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Auxiliary,
			shfRL3_Single4DXA = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::SingleHead | (uint32_t)eRLScanHeadFlags::FourD | (uint32_t)eRLScanHeadFlags::Extended | (uint32_t) eRLScanHeadFlags::Auxiliary,

			shfRL3_Dual3D = (uint32_t)eRLScanHeadFlags::RL3 | (uint32_t)eRLScanHeadFlags::DualHead
		};

		enum class eRLAxis : uint32_t
		{
			aiX = 0,
			aiY = 1,
			aiZ = 2,
			aiZoomZ = 3,
			aiAuxiliary = 4,
			aiSensorZ = 5
		};

		enum class eRLAxes : uint32_t
		{
			axNone = 0x00,
			axX = 0x01,
			axY = 0x02,
			axZ = 0x04,
			axZoomZ = 0x08,
			axAuxiliary = 0x10,
			axSensorZ = 0x20,
			axXY = axX | axY,
			axXYZ = axX | axY | axZ
		};

		enum class eRLSfRegister
		{
			Sfr0 = 0,
			Sfr1 = 1,
		};


		typedef struct
		{
			double m_dX;
			double m_dY;
		} rlPoint2D;

		typedef struct
		{
			double m_dX;
			double m_dY;
			double m_dZ;
		} rlPoint3D;

		typedef struct
		{
			double m_Transform[3][3];
			double m_Translation[3];
		} rlTransform3D;

		typedef void rlSystemConfig;
		typedef void rlGpioConfig;
		typedef void rlSerialPortConfig;
		typedef void rlLaserConfig;
		typedef void rlScannerConfig;
		typedef void rlAdapterBoardConfig;
		typedef void rlProcessVariables;
		typedef void rlOpticalParameters;
		typedef void rlQuadratureDecoderConfig;
		typedef void rlMotfConfig;
		typedef void rlSpiConfig;

		typedef int32_t rlHandle;
		typedef int32_t rlResult;
		typedef int64_t rlListHandle;

		typedef rlHandle(RAYLASE_CALLINGCONVENTION* PrlConnect) (const char * pszAddress, int32_t nPort);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlIsConnected) (rlHandle handle, bool * pbIsConnected);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlDisconnect) (rlHandle handle);
		
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemResetToDefaults) (rlHandle handle);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserLaserOn) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserLaserOff) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserIsPilotEnabled) (rlHandle handle, bool & enabled);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserEnablePilot) (rlHandle handle, bool enable);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserIsLaserArmed) (rlHandle handle, bool & armed);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserArmLaser) (rlHandle handle, bool arm);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserReadLaserStatus) (rlHandle handle, uint32_t & laserStatus);

		typedef rlListHandle(RAYLASE_CALLINGCONVENTION* PrlListAllocate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListReleaseHandle) (rlListHandle handle);		
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendLaserOn) (rlListHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendLaserOff) (rlListHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendPower) (rlListHandle handle, uint16_t nPower, eRLPowerChannels powerTargetChannel);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendJumpSpeed) (rlListHandle handle, double jumpSpeed);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendMarkSpeed) (rlListHandle handle, double markSpeed);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendJumpDelay) (rlListHandle handle, double jumpDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendMinJumpDelay) (rlListHandle handle, double minJumpDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendMarkDelay) (rlListHandle handle, double markDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendPolyDelay) (rlListHandle handle, double polyDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendLaserOnDelay) (rlListHandle handle, double laserOnDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendLaserOffDelay) (rlListHandle handle, double laserOffDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendJumpAbs2D) (rlListHandle handle, double x, double y);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendMarkAbs2D) (rlListHandle handle, double x, double y);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListSet) (rlHandle handle, int32_t listID, rlListHandle listHandle, bool append, int32_t executionLimit);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListExecute) (rlHandle handle, int32_t listID);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListWaitForListIdle) (rlHandle handle, int32_t timeOutInMilliseconds, bool & successful, int32_t & listID);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListWaitForListDone) (rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListDelete) (rlHandle handle, int32_t listID, bool bFromDisk);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListResetExecution) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAbortExecution) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListIsExecutionInProgress) (rlHandle handle, bool * pbInProgress);

		typedef void(RAYLASE_CALLINGCONVENTION* PrlGetLastError) (char * pBuffer, int32_t nBufferSize);
		typedef int32_t(RAYLASE_CALLINGCONVENTION* PrlGetLastErrorLen) ();

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlEnableCommandLogging) (rlHandle handle, const char* pszPath, int32_t truncateArguments);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlDisableCommandLogging) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSetTimeout) (rlHandle handle, uint32_t nTimeOut);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGetTimeout) (rlHandle handle, uint32_t* pnTimeOut);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationBeginUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationEndUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationAbortUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationIsUpdateActive) (rlHandle handle, uint32_t * pbIsActive);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioInitConfig) (rlGpioConfig * pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetConfig) (rlHandle handle, rlGpioConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioSetConfig) (rlHandle handle, rlGpioConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetPinCount) (rlHandle handle, eRLIOPort port, int32_t & pPinCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetGroupCount) (rlHandle handle, eRLIOPort port, int32_t& pGroupCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetSupportedIOLevels) (rlHandle handle, eRLIOPort port, eRLIOVoltage * pIOLevels, uint32_t nIOLevelSize, uint32_t & nLevelsCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetSupportedDirections) (rlHandle handle, eRLIOPort port, eRLIODirection* pIODirections, uint32_t nIODirectionsSize, uint32_t& nDirectionsCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetIOLevel) (rlHandle handle, eRLIOPort port, eRLIOVoltage & ioLevel);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioSetIOLevel) (rlHandle handle, eRLIOPort port, eRLIOVoltage ioLevel);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetDirections) (rlHandle handle, eRLIOPort port, eRLIODirection* pIODirections, uint32_t nIODirectionsSize, uint32_t& nDirectionsCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioSetDirections) (rlHandle handle, eRLIOPort port, eRLIODirection* pIODirections, uint32_t nDirectionsCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetPolarities) (rlHandle handle, eRLIOPort port, eRLPolarity * pPolarities, uint32_t nPolaritiesSize, uint32_t& nPolaritiesCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioSetPolarities) (rlHandle handle, eRLIOPort port, eRLPolarity* pPolarities, uint32_t nPolaritiesCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetWriteDelay) (rlHandle handle, eRLIOPort port, double& dDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioSetWriteDelay) (rlHandle handle, eRLIOPort port, double dDelay);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetWriteWidth) (rlHandle handle, eRLIOPort port, double& dWidth);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioSetWriteWidth) (rlHandle handle, eRLIOPort port, double dWidth);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioRead) (rlHandle handle, eRLIOPort port, uint32_t & nValue);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioGetOutput) (rlHandle handle, eRLIOPort port, uint32_t& nValue);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGpioWrite) (rlHandle handle, eRLIOPort port, eRLPinAction action, uint32_t nValue);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemInitConfig) (rlSystemConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetConfig) (rlHandle handle, rlSystemConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemSetConfig) (rlHandle handle, rlSystemConfig* pConfig);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetFirmwareVersion) (rlHandle handle, char * pBuffer, uint32_t nBufferSize, uint32_t & nStringLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetFirmwareProductVersion) (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t & nStringLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetFPGAVersion) (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetOSVersion) (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetCardSerialNumber) (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetConnectionLimit) (rlHandle handle, int32_t nConnectionLimit);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemResetOperatingState) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSystemGetOperatingState) (rlHandle handle, eRLOperatingState & operatingState);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortInitConfig) (rlSerialPortConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortGetConfig) (rlHandle handle, rlSerialPortConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortSetConfig) (rlHandle handle, rlSerialPortConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortTransmitString) (rlHandle handle, const char * pTransmitMessage, uint32_t nTransmitMessageLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortTransmitLine) (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortTransmitBytes) (rlHandle handle, const uint8_t* pTransmitMessage, uint32_t nTransmitMessageLength);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortTryVerifyResponseString) (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength, const char * pGoodresponse, uint32_t nGoodResponseLength, uint32_t & nSuccessFlag);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortTryVerifyResponseLine) (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength, const char* pGoodresponse, uint32_t nGoodResponseLength, uint32_t& nSuccessFlag);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortTryVerifyResponseBytes) (rlHandle handle, const uint8_t* pTransmitMessage, uint32_t nTransmitMessageLength, const uint8_t* pGoodresponse, uint32_t nGoodResponseLength, uint32_t& nSuccessFlag);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortReceiveString) (rlHandle handle, uint32_t nReadCount, char * pOutBuffer, uint32_t nBufferLength, uint32_t & nReceivedLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortReceiveLine) (rlHandle handle, char* pOutBuffer, uint32_t nBufferLength, uint32_t& nReceivedLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSerialPortReceiveBytes) (rlHandle handle, uint32_t nReadCount, uint8_t* pOutBuffer, uint32_t nBufferLength, uint32_t& nReceivedLength);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserInitConfig) (rlLaserConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserGetConfig) (rlHandle handle, rlLaserConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserSetConfig) (rlHandle handle, rlLaserConfig* pConfig);


		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserCreatePowerCalibration) (rlHandle handle, eRLPowerChannels powerTargetChannel);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserLoadPowerCalibration) (rlHandle handle, eRLPowerChannels powerTargetChannel);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserStorePowerCalibration) (rlHandle handle, eRLPowerChannels powerTargetChannel);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserGetPowerCalibration) (rlHandle handle, eRLPowerChannels powerTargetChannel, uint32_t* pBuffer, int32_t nBufferSize, int32_t & calibrationValuesCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserSetPowerCalibration) (rlHandle handle, eRLPowerChannels powerTargetChannel, uint32_t* pCalibrationValues, int32_t calibrationValuesLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserPowerCalibrationSet) (rlHandle handle, eRLPowerChannels powerTargetChannel, const char * pLocalFileName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserPowerCalibrationGet) (rlHandle handle, eRLPowerChannels powerTargetChannel, const char* pLocalFileName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserGetPowerScale) (rlHandle handle, eRLPowerChannels powerTargetChannel, double & dPowerScale);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlLaserSetPowerScale) (rlHandle handle, eRLPowerChannels powerTargetChannel, double dPowerScale);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerInitConfig) (rlScannerConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerSetConfig) (rlHandle handle, rlScannerConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerGetConfig) (rlHandle handle, rlScannerConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerGetNominalPosition) (rlHandle handle, double & dX, double & dY, double & dZ);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerReadPositions) (rlHandle handle, double * pdPositions);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerReadLastPositions) (rlHandle handle, double* pdPositions);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatGetHeadCount) (eRLScanHeadFormat headFormat, int32_t & nOut);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatGetAxisCount) (eRLScanHeadFormat headFormat, int32_t & nOut);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatGetDimensions) (eRLScanHeadFormat headFormat, int32_t * pDimensions);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatGetAxes) (eRLScanHeadFormat headFormat, eRLAxes & axes);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatGetIndexOfAxis) (eRLScanHeadFormat headFormat, int32_t nIndex, eRLAxis & axis);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatHasOpticalZoom) (eRLScanHeadFormat headFormat, uint32_t & bHasZoom);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatHasAuxiliary) (eRLScanHeadFormat headFormat, uint32_t& bHasAuxiliary);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerHeadFormatIsExtended) (eRLScanHeadFormat headFormat, uint32_t& bIsExtended);		
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerGetHeadBias) (rlHandle handle, int32_t nHead, rlPoint3D & bias);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerSetHeadBias) (rlHandle handle, int32_t nHead, eRLAxes axes, double * pBiases, int32_t nLength);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardGetConfig) (rlHandle handle, eRLIOPort port, rlAdapterBoardConfig * pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardSetConfig) (rlHandle handle, eRLIOPort port, rlAdapterBoardConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardSetConfigByName) (rlHandle handle, eRLIOPort port, const char * pName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardEnable) (rlHandle handle, eRLIOPort port, uint32_t bEnable);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardEnabled) (rlHandle handle, eRLIOPort port, uint32_t & bEnabled);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardSupportsAutoConfiguration) (rlHandle handle, eRLIOPort port, uint32_t& bSupportsAutoConfiguration);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardValid) (rlHandle handle, eRLIOPort port, uint32_t& bValid);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardGetAvailableAdapterBoardNames) (rlHandle handle, eRLIOPort port, char * pBuffer, uint32_t nBufferLen, uint32_t & outLen, int32_t * pOutItems);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlAdapterBoardGetCurrentAdapterBoardName) (rlHandle handle, eRLIOPort port, char* pBuffer, uint32_t nBufferLen, uint32_t& outLen);


		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessInitVariables) (rlProcessVariables * pVariables);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessLoadVariables) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessStoreVariables) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessGetVariables) (rlHandle handle, rlProcessVariables* pVariables);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessSetVariables) (rlHandle handle, rlProcessVariables* pVariables);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessGetSpeedScale) (rlHandle handle, double & dSpeedScale);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessSetSpeedScale) (rlHandle handle, double dSpeedScale);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessGetTransformation) (rlHandle handle, rlTransform3D * pTransformation);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessSetTransformation) (rlHandle handle, rlTransform3D* pTransformation);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessWobbleSetCircle) (rlHandle handle, rlProcessVariables* pVariables, double dAmplitude, double dFrequency);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlProcessWobbleSetEight) (rlHandle handle, rlProcessVariables* pVariables, rlPoint2D * pAmplitude, double dFrequency);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionExists) (rlHandle handle, int32_t nHead, int32_t nCorrection, uint32_t & bExists);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionDelete) (rlHandle handle, int32_t nHead, int32_t nCorrection);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionGet) (rlHandle handle, int32_t nHead, int32_t nCorrection, const char * pLocalFileName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionSet) (rlHandle handle, int32_t nHead, int32_t nCorrection, const char* pLocalFileName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionLoad) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionStore) (rlHandle handle);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionInitOpticalParameters) (rlOpticalParameters* pParameters);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionGetOpticalParameters) (rlHandle handle, int32_t nHead, int32_t nCorrection, rlOpticalParameters* pParameters);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionGetHash) (rlHandle handle, int32_t nHead, int32_t nCorrection, char * pOutBuffer, uint32_t nBufferLen, uint32_t & nOutLen);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionSelectCorrection) (rlHandle handle, int32_t nHead, int32_t nCorrection);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlHeadCorrectionGetSelectedCorrection) (rlHandle handle, int32_t nHead, int32_t & nSelectedCorrection);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderInitConfig) (rlQuadratureDecoderConfig * pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderGetConfig) (rlHandle handle, rlQuadratureDecoderConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderSetConfig) (rlHandle handle, rlQuadratureDecoderConfig* pConfig);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfInitConfig) (rlMotfConfig * pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfGetConfig) (rlHandle handle, rlMotfConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfSetConfig) (rlHandle handle, rlMotfConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfConfigSetDirectionFromAngle) (rlMotfConfig* pConfig, double dAngle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderResetEncoderPosition) (rlHandle handle, int32_t nEncoderID);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfQuadratureDecoderReadEncoderPosition) (rlHandle handle, int32_t nEncoderID, int32_t & nPosition);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfGetPendingParts) (rlHandle handle, int32_t & nPendingPart);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlMotfResetState) (rlHandle handle);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioRead) (rlHandle handle, eRLSfRegister, uint32_t & nValue);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiInitConfig) (rlSpiConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiLoadConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiStoreConfig) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiGetConfig) (rlHandle handle, rlSpiConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiSetConfig) (rlHandle handle, rlSpiConfig* pConfig);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiTransmit) (rlHandle handle, int32_t nModule, uint32_t * pTransmitMessage, uint32_t nTransmitMessageLen, uint32_t bAsync);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiTransceive) (rlHandle handle, int32_t nModule, uint32_t* pTransmitMessage, uint32_t nTransmitMessageLen, int32_t nTimeoutInMS, uint32_t * pBuffer, uint32_t nBufferSize, uint32_t nReceiveLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiReceive) (rlHandle handle, int32_t nModule,uint32_t nReadCount, int32_t nTimeoutInMS, uint32_t* pBuffer, uint32_t nBufferSize, uint32_t nReceiveLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSfioSpiWaitForActiveTransfersDone) (rlHandle handle, int32_t nModule);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionCreate) (rlHandle handle, rlOpticalParameters * parameters);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionSet) (rlHandle handle, const char* pLocalFileName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionGet) (rlHandle handle, const char* pLocalFileName);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionLoad) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionStore) (rlHandle handle);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionChangeValue)(rlHandle handle, rlPoint3D position, double dFactor);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlPowerCorrectionGetOpticalParameters)(rlHandle handle, rlOpticalParameters * pParameters);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerCommunicationTransceiveEnhanced)(rlHandle handle, int32_t nHead, eRLAxes axes, const uint32_t* commands, uint32_t commandsLength, int32_t timeoutMilliseconds, uint32_t* buffer, uint32_t bufferSize, uint32_t& receiveCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerCommunicationTransmitEnhanced)(rlHandle handle, int32_t nHead, eRLAxes axes, const uint32_t* commands, uint32_t commandsLength, int32_t timeoutMilliseconds);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerCommunicationReceiveEnhanced)(rlHandle handle, int32_t nHead, eRLAxes axes, int32_t timeoutMilliseconds, uint32_t* buffer, uint32_t bufferSize, uint32_t& receiveCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerCommunicationTransceiveRaylase)(rlHandle handle, int32_t nHead, eRLAxis axis, const unsigned char* transmitMessage, uint32_t transmitMessageLength, int32_t timeoutMilliseconds, unsigned char* buffer, uint32_t bufferSize, uint32_t& receiveCount);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlScannerCommunicationTransmitRaylase)(rlHandle handle, int32_t nHead, eRLAxis axis, const unsigned char* transmitMessage, uint32_t transmitMessageLength, int32_t timeoutMilliseconds);


		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanCreate)(rlHandle handle, const char* name, bool initialValue);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanDelete)(rlHandle handle, const char* name, bool fromDisk);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanExists)(rlHandle handle, const char* name, bool& exists);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanLoad)(rlHandle handle, const char* name);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanStore)(rlHandle handle, const char* name);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanGetValue)(rlHandle handle, const char* name, bool& value);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanSetValue)(rlHandle handle, const char* name, bool value);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableBooleanToggleValue)(rlHandle handle, const char* name);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerCreate)(rlHandle handle, const char* name, int32_t initialValue);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerDelete)(rlHandle handle, const char* name, bool fromDisk);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerExists)(rlHandle handle, const char* name, bool& exists);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerLoad)(rlHandle handle, const char* name);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerStore)(rlHandle handle, const char* name);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerGetValue)(rlHandle handle, const char* name, int32_t& value);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerSetValue)(rlHandle handle, const char* name, int32_t value);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerIncrementValue)(rlHandle handle, const char* name);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableIntegerDecrementValue)(rlHandle handle, const char* name);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterCreate)(rlHandle handle, const char* name, int32_t initialValue);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterDelete)(rlHandle handle, const char* name, bool fromDisk);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterExists)(rlHandle handle, const char* name, bool& exists);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterLoad)(rlHandle handle, const char* name);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterStore)(rlHandle handle, const char* name);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterGetValue)(rlHandle handle, const char* name, int32_t& value);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterSetValue)(rlHandle handle, const char* name, int32_t value);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterGetAndResetValue)(rlHandle handle, const char* name, int32_t& value);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterGetMarkTimeFormat)(rlHandle handle, char* buffer, uint32_t bufferSize, uint32_t& stringLength);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlVariableMarkCounterSetMarkTimeFormat)(rlHandle handle, const char* format);



		class CRaylaseSDK {
		private:

			bool m_bEnableJournal;

			PrlConnect ptrConnect = nullptr;
			PrlIsConnected ptrIsConnected = nullptr;
			PrlDisconnect ptrDisconnect = nullptr;
			PrlSystemResetToDefaults ptrSystemResetToDefaults = nullptr;
			PrlLaserLaserOn ptrLaserLaserOn = nullptr;
			PrlLaserLaserOff ptrLaserLaserOff = nullptr;
			PrlLaserIsPilotEnabled ptrLaserIsPilotEnabled = nullptr;
			PrlLaserEnablePilot ptrLaserEnablePilot = nullptr;
			PrlLaserIsLaserArmed ptrLaserIsLaserArmed = nullptr;
			PrlLaserArmLaser ptrLaserArmLaser = nullptr;
			PrlLaserReadLaserStatus ptrLaserReadLaserStatus = nullptr;

			PrlListAllocate ptrListAllocate = nullptr;
			PrlListReleaseHandle ptrListReleaseHandle = nullptr;
			PrlListAppendLaserOn ptrListAppendLaserOn = nullptr;
			PrlListAppendLaserOff ptrListAppendLaserOff = nullptr;
			PrlListAppendPower ptrListAppendPower = nullptr;
			PrlListAppendJumpSpeed ptrListAppendJumpSpeed = nullptr;
			PrlListAppendMarkSpeed ptrListAppendMarkSpeed = nullptr;
			PrlListAppendJumpDelay ptrListAppendJumpDelay = nullptr;
			PrlListAppendMinJumpDelay ptrListAppendMinJumpDelay = nullptr;
			PrlListAppendMarkDelay ptrListAppendMarkDelay = nullptr;
			PrlListAppendPolyDelay ptrListAppendPolyDelay = nullptr;
			PrlListAppendLaserOnDelay ptrListAppendLaserOnDelay = nullptr;
			PrlListAppendLaserOffDelay ptrListAppendLaserOffDelay = nullptr;
			PrlListAppendJumpAbs2D ptrListAppendJumpAbs2D = nullptr;
			PrlListAppendMarkAbs2D ptrListAppendMarkAbs2D = nullptr;
			PrlListSet ptrListSet = nullptr;
			PrlListExecute ptrListExecute = nullptr;
			PrlListWaitForListIdle ptrListWaitForListIdle = nullptr;
			PrlListWaitForListDone ptrListWaitForListDone = nullptr;
			PrlListDelete ptrListDelete = nullptr;
			PrlListResetExecution ptrListResetExecution = nullptr;
			PrlListAbortExecution ptrListAbortExecution = nullptr;
			PrlListIsExecutionInProgress ptrListIsExecutionInProgress = nullptr;

			PrlGetLastError ptrGetLastError = nullptr;
			PrlGetLastErrorLen ptrGetLastErrorLen = nullptr;

			PrlEnableCommandLogging ptrEnableCommandLogging = nullptr;
			PrlDisableCommandLogging ptrDisableCommandLogging = nullptr;
			PrlSetTimeout ptrSetTimeout = nullptr;
			PrlGetTimeout ptrGetTimeout = nullptr;
			PrlConfigurationBeginUpdate ptrConfigurationBeginUpdate = nullptr;
			PrlConfigurationEndUpdate ptrConfigurationEndUpdate = nullptr;
			PrlConfigurationAbortUpdate ptrConfigurationAbortUpdate = nullptr;
			PrlConfigurationIsUpdateActive ptrConfigurationIsUpdateActive = nullptr;

			PrlGpioInitConfig ptrGpioInitConfig = nullptr;
			PrlGpioLoadConfig ptrGpioLoadConfig = nullptr;
			PrlGpioStoreConfig ptrGpioStoreConfig = nullptr;
			PrlGpioGetConfig ptrGpioGetConfig = nullptr;
			PrlGpioSetConfig ptrGpioSetConfig = nullptr;
			PrlGpioGetPinCount ptrGpioGetPinCount = nullptr;
			PrlGpioGetGroupCount ptrGpioGetGroupCount = nullptr;
			PrlGpioGetSupportedIOLevels ptrGpioGetSupportedIOLevels = nullptr;
			PrlGpioGetSupportedDirections ptrGpioGetSupportedDirections = nullptr;
			PrlGpioGetIOLevel ptrGpioGetIOLevel = nullptr;
			PrlGpioSetIOLevel ptrGpioSetIOLevel = nullptr;
			PrlGpioGetDirections ptrGpioGetDirections = nullptr;
			PrlGpioSetDirections ptrGpioSetDirections = nullptr;
			PrlGpioGetPolarities ptrGpioGetPolarities = nullptr;
			PrlGpioSetPolarities ptrGpioSetPolarities = nullptr;
			PrlGpioGetWriteDelay ptrGpioGetWriteDelay = nullptr;
			PrlGpioSetWriteDelay ptrGpioSetWriteDelay = nullptr;
			PrlGpioGetWriteWidth ptrGpioGetWriteWidth = nullptr;
			PrlGpioSetWriteWidth ptrGpioSetWriteWidth = nullptr;
			PrlGpioRead ptrGpioRead = nullptr;
			PrlGpioGetOutput ptrGpioGetOutput = nullptr;
			PrlGpioWrite ptrGpioWrite = nullptr;
			PrlSystemInitConfig ptrSystemInitConfig = nullptr;
			PrlSystemLoadConfig ptrSystemLoadConfig = nullptr;
			PrlSystemStoreConfig ptrSystemStoreConfig = nullptr;
			PrlSystemGetConfig ptrSystemGetConfig = nullptr;
			PrlSystemSetConfig ptrSystemSetConfig = nullptr;
			PrlSystemGetFirmwareVersion ptrSystemGetFirmwareVersion = nullptr;
			PrlSystemGetFirmwareProductVersion ptrSystemGetFirmwareProductVersion = nullptr;
			PrlSystemGetFPGAVersion ptrSystemGetFPGAVersion = nullptr;
			PrlSystemGetOSVersion ptrSystemGetOSVersion = nullptr;
			PrlSystemGetCardSerialNumber ptrSystemGetCardSerialNumber = nullptr;
			PrlSystemGetConnectionLimit ptrSystemGetConnectionLimit = nullptr;
			PrlSystemResetOperatingState ptrSystemResetOperatingState = nullptr;
			PrlSystemGetOperatingState ptrSystemGetOperatingState = nullptr;
			PrlSerialPortInitConfig ptrSerialPortInitConfig = nullptr;
			PrlSerialPortLoadConfig ptrSerialPortLoadConfig = nullptr;
			PrlSerialPortStoreConfig ptrSerialPortStoreConfig = nullptr;
			PrlSerialPortGetConfig ptrSerialPortGetConfig = nullptr;
			PrlSerialPortSetConfig ptrSerialPortSetConfig = nullptr;
			PrlSerialPortTransmitString ptrSerialPortTransmitString = nullptr;
			PrlSerialPortTransmitLine ptrSerialPortTransmitLine = nullptr;
			PrlSerialPortTransmitBytes ptrSerialPortTransmitBytes = nullptr;
			PrlSerialPortTryVerifyResponseString ptrSerialPortTryVerifyResponseString = nullptr;
			PrlSerialPortTryVerifyResponseLine ptrSerialPortTryVerifyResponseLine = nullptr;
			PrlSerialPortTryVerifyResponseBytes ptrSerialPortTryVerifyResponseBytes = nullptr;
			PrlSerialPortReceiveString ptrSerialPortReceiveString = nullptr;
			PrlSerialPortReceiveLine ptrSerialPortReceiveLine = nullptr;
			PrlSerialPortReceiveBytes ptrSerialPortReceiveBytes = nullptr;
			PrlLaserInitConfig ptrLaserInitConfig = nullptr;
			PrlLaserLoadConfig ptrLaserLoadConfig = nullptr;
			PrlLaserStoreConfig ptrLaserStoreConfig = nullptr;
			PrlLaserGetConfig ptrLaserGetConfig = nullptr;
			PrlLaserSetConfig ptrLaserSetConfig = nullptr;
			PrlLaserCreatePowerCalibration ptrLaserCreatePowerCalibration = nullptr;
			PrlLaserLoadPowerCalibration ptrLaserLoadPowerCalibration = nullptr;
			PrlLaserStorePowerCalibration ptrLaserStorePowerCalibration = nullptr;
			PrlLaserGetPowerCalibration ptrLaserGetPowerCalibration = nullptr;
			PrlLaserSetPowerCalibration ptrLaserSetPowerCalibration = nullptr;
			PrlLaserPowerCalibrationSet ptrLaserPowerCalibrationSet = nullptr;
			PrlLaserPowerCalibrationGet ptrLaserPowerCalibrationGet = nullptr;
			PrlLaserGetPowerScale ptrLaserGetPowerScale = nullptr;
			PrlLaserSetPowerScale ptrLaserSetPowerScale = nullptr;

			PrlScannerInitConfig ptrScannerInitConfig = nullptr;
			PrlScannerSetConfig ptrScannerSetConfig = nullptr;
			PrlScannerGetConfig ptrScannerGetConfig = nullptr;
			PrlScannerLoadConfig ptrScannerLoadConfig = nullptr;
			PrlScannerStoreConfig ptrScannerStoreConfig = nullptr;
			PrlScannerGetNominalPosition ptrScannerGetNominalPosition = nullptr;
			PrlScannerReadPositions ptrScannerReadPositions = nullptr;
			PrlScannerReadLastPositions ptrScannerReadLastPositions = nullptr;
			PrlScannerHeadFormatGetHeadCount ptrScannerHeadFormatGetHeadCount = nullptr;
			PrlScannerHeadFormatGetAxisCount ptrScannerHeadFormatGetAxisCount = nullptr;
			PrlScannerHeadFormatGetDimensions ptrScannerHeadFormatGetDimensions = nullptr;
			PrlScannerHeadFormatGetAxes ptrScannerHeadFormatGetAxes = nullptr;
			PrlScannerHeadFormatGetIndexOfAxis ptrScannerHeadFormatGetIndexOfAxis = nullptr;
			PrlScannerHeadFormatHasOpticalZoom ptrScannerHeadFormatHasOpticalZoom = nullptr;
			PrlScannerHeadFormatHasAuxiliary ptrScannerHeadFormatHasAuxiliary = nullptr;
			PrlScannerHeadFormatIsExtended ptrScannerHeadFormatIsExtended = nullptr;
			PrlScannerGetHeadBias ptrScannerGetHeadBias = nullptr;
			PrlScannerSetHeadBias ptrScannerSetHeadBias = nullptr;


			PrlAdapterBoardGetConfig ptrAdapterBoardGetConfig = nullptr;
			PrlAdapterBoardSetConfig ptrAdapterBoardSetConfig = nullptr;
			PrlAdapterBoardSetConfigByName ptrAdapterBoardSetConfigByName = nullptr;
			PrlAdapterBoardEnable ptrAdapterBoardEnable = nullptr;
			PrlAdapterBoardEnabled ptrAdapterBoardEnabled = nullptr;
			PrlAdapterBoardSupportsAutoConfiguration ptrAdapterBoardSupportsAutoConfiguration = nullptr;
			PrlAdapterBoardValid ptrAdapterBoardValid = nullptr;
			PrlAdapterBoardGetAvailableAdapterBoardNames ptrAdapterBoardGetAvailableAdapterBoardNames = nullptr;
			PrlAdapterBoardGetCurrentAdapterBoardName ptrAdapterBoardGetCurrentAdapterBoardName = nullptr;

			PrlProcessInitVariables ptrProcessInitVariables = nullptr;
			PrlProcessLoadVariables ptrProcessLoadVariables = nullptr;
			PrlProcessStoreVariables ptrProcessStoreVariables = nullptr;
			PrlProcessGetVariables ptrProcessGetVariables = nullptr;
			PrlProcessSetVariables ptrProcessSetVariables = nullptr;

			PrlProcessGetSpeedScale ptrProcessGetSpeedScale = nullptr;
			PrlProcessSetSpeedScale ptrProcessSetSpeedScale = nullptr;
			PrlProcessGetTransformation ptrProcessGetTransformation = nullptr;
			PrlProcessSetTransformation ptrProcessSetTransformation = nullptr;
			PrlProcessWobbleSetCircle ptrProcessWobbleSetCircle = nullptr;
			PrlProcessWobbleSetEight ptrProcessWobbleSetEight = nullptr;

			PrlHeadCorrectionExists ptrHeadCorrectionExists = nullptr;
			PrlHeadCorrectionDelete ptrHeadCorrectionDelete = nullptr;
			PrlHeadCorrectionGet ptrHeadCorrectionGet = nullptr;
			PrlHeadCorrectionSet ptrHeadCorrectionSet = nullptr;
			PrlHeadCorrectionLoad ptrHeadCorrectionLoad = nullptr;
			PrlHeadCorrectionStore ptrHeadCorrectionStore = nullptr;

			PrlHeadCorrectionInitOpticalParameters ptrHeadCorrectionInitOpticalParameters = nullptr;
			PrlHeadCorrectionGetOpticalParameters ptrHeadCorrectionGetOpticalParameters = nullptr;
			PrlHeadCorrectionGetHash ptrHeadCorrectionGetHash = nullptr;
			PrlHeadCorrectionSelectCorrection ptrHeadCorrectionSelectCorrection = nullptr;
			PrlHeadCorrectionGetSelectedCorrection ptrHeadCorrectionGetSelectedCorrection = nullptr;

			PrlMotfQuadratureDecoderInitConfig ptrMotfQuadratureDecoderInitConfig = nullptr;
			PrlMotfQuadratureDecoderLoadConfig ptrMotfQuadratureDecoderLoadConfig = nullptr;
			PrlMotfQuadratureDecoderStoreConfig ptrMotfQuadratureDecoderStoreConfig = nullptr;
			PrlMotfQuadratureDecoderGetConfig ptrMotfQuadratureDecoderGetConfig = nullptr;
			PrlMotfQuadratureDecoderSetConfig ptrMotfQuadratureDecoderSetConfig = nullptr;

			PrlMotfInitConfig ptrMotfInitConfig = nullptr;
			PrlMotfGetConfig ptrMotfGetConfig = nullptr;
			PrlMotfSetConfig ptrMotfSetConfig = nullptr;
			PrlMotfLoadConfig ptrMotfLoadConfig = nullptr;
			PrlMotfStoreConfig ptrMotfStoreConfig = nullptr;
			PrlMotfConfigSetDirectionFromAngle ptrMotfConfigSetDirectionFromAngle = nullptr;
			PrlMotfQuadratureDecoderResetEncoderPosition ptrMotfQuadratureDecoderResetEncoderPosition = nullptr;
			PrlMotfQuadratureDecoderReadEncoderPosition ptrMotfQuadratureDecoderReadEncoderPosition = nullptr;
			PrlMotfGetPendingParts ptrMotfGetPendingParts = nullptr;
			PrlMotfResetState ptrMotfResetState = nullptr;

			PrlSfioRead ptrSfioRead = nullptr;
			PrlSfioSpiInitConfig ptrSfioSpiInitConfig = nullptr;
			PrlSfioSpiLoadConfig ptrSfioSpiLoadConfig = nullptr;
			PrlSfioSpiStoreConfig ptrSfioSpiStoreConfig = nullptr;
			PrlSfioSpiGetConfig ptrSfioSpiGetConfig = nullptr;
			PrlSfioSpiSetConfig ptrSfioSpiSetConfig = nullptr;
			PrlSfioSpiTransmit ptrSfioSpiTransmit = nullptr;
			PrlSfioSpiTransceive ptrSfioSpiTransceive = nullptr;
			PrlSfioSpiReceive ptrSfioSpiReceive = nullptr;
			PrlSfioSpiWaitForActiveTransfersDone ptrSfioSpiWaitForActiveTransfersDone = nullptr;

			PrlPowerCorrectionCreate ptrPowerCorrectionCreate = nullptr;
			PrlPowerCorrectionSet ptrPowerCorrectionSet = nullptr;
			PrlPowerCorrectionGet ptrPowerCorrectionGet = nullptr;
			PrlPowerCorrectionLoad ptrPowerCorrectionLoad = nullptr;
			PrlPowerCorrectionStore ptrPowerCorrectionStore = nullptr;

			PrlPowerCorrectionChangeValue ptrPowerCorrectionChangeValue = nullptr;

			PrlPowerCorrectionGetOpticalParameters ptrPowerCorrectionGetOpticalParameters = nullptr;

			PrlScannerCommunicationTransceiveEnhanced ptrScannerCommunicationTransceiveEnhanced = nullptr;
			PrlScannerCommunicationTransmitEnhanced ptrScannerCommunicationTransmitEnhanced = nullptr;
			PrlScannerCommunicationReceiveEnhanced ptrScannerCommunicationReceiveEnhanced = nullptr;
			PrlScannerCommunicationTransceiveRaylase ptrScannerCommunicationTransceiveRaylase = nullptr;
			PrlScannerCommunicationTransmitRaylase ptrScannerCommunicationTransmitRaylase = nullptr;

			PrlVariableBooleanCreate ptrVariableBooleanCreate = nullptr;
			PrlVariableBooleanDelete ptrVariableBooleanDelete = nullptr;
			PrlVariableBooleanExists ptrVariableBooleanExists = nullptr;
			PrlVariableBooleanLoad ptrVariableBooleanLoad = nullptr;
			PrlVariableBooleanStore ptrVariableBooleanStore = nullptr;

			PrlVariableBooleanGetValue ptrVariableBooleanGetValue = nullptr;
			PrlVariableBooleanSetValue ptrVariableBooleanSetValue = nullptr;
			PrlVariableBooleanToggleValue ptrVariableBooleanToggleValue = nullptr;

			PrlVariableIntegerCreate ptrVariableIntegerCreate = nullptr;
			PrlVariableIntegerDelete ptrVariableIntegerDelete = nullptr;
			PrlVariableIntegerExists ptrVariableIntegerExists = nullptr;
			PrlVariableIntegerLoad ptrVariableIntegerLoad = nullptr;
			PrlVariableIntegerStore ptrVariableIntegerStore = nullptr;

			PrlVariableIntegerGetValue ptrVariableIntegerGetValue = nullptr;
			PrlVariableIntegerSetValue ptrVariableIntegerSetValue = nullptr;
			PrlVariableIntegerIncrementValue ptrVariableIntegerIncrementValue = nullptr;
			PrlVariableIntegerDecrementValue ptrVariableIntegerDecrementValue = nullptr;

			PrlVariableMarkCounterCreate ptrVariableMarkCounterCreate = nullptr;
			PrlVariableMarkCounterDelete ptrVariableMarkCounterDelete = nullptr;
			PrlVariableMarkCounterExists ptrVariableMarkCounterExists = nullptr;
			PrlVariableMarkCounterLoad ptrVariableMarkCounterLoad = nullptr;
			PrlVariableMarkCounterStore ptrVariableMarkCounterStore = nullptr;

			PrlVariableMarkCounterGetValue ptrVariableMarkCounterGetValue = nullptr;
			PrlVariableMarkCounterSetValue ptrVariableMarkCounterSetValue = nullptr;
			PrlVariableMarkCounterGetAndResetValue ptrVariableMarkCounterGetAndResetValue = nullptr;

			PrlVariableMarkCounterGetMarkTimeFormat ptrVariableMarkCounterGetMarkTimeFormat = nullptr;
			PrlVariableMarkCounterSetMarkTimeFormat ptrVariableMarkCounterSetMarkTimeFormat = nullptr;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();

			void logJournal(const std::string & sLine);

		public:

			
			CRaylaseSDK(const std::string & sDLLNameUTF8);
			~CRaylaseSDK();

			void checkError(int32_t statusCode, const std::string & sDebugMessage = "");

			rlHandle rlConnect (const char* pszAddress, int32_t nPort);
			rlResult rlIsConnected (rlHandle handle, bool* pbIsConnected);
			rlResult rlDisconnect (rlHandle handle);
			rlResult rlSystemResetToDefaults (rlHandle handle);
			rlResult rlLaserLaserOn (rlHandle handle);
			rlResult rlLaserLaserOff (rlHandle handle);
			rlResult rlLaserIsPilotEnabled (rlHandle handle, bool& enabled);
			rlResult rlLaserEnablePilot (rlHandle handle, bool enable);
			rlResult rlLaserIsLaserArmed (rlHandle handle, bool& armed);
			rlResult rlLaserArmLaser (rlHandle handle, bool arm);
			rlResult rlLaserReadLaserStatus (rlHandle handle, uint32_t & laserStatusField);

			rlListHandle rlListAllocate (rlHandle handle);

			rlResult rlListReleaseHandle (rlListHandle handle);
			rlResult rlListAppendLaserOn (rlListHandle handle);
			rlResult rlListAppendLaserOff (rlListHandle handle);
			rlResult rlListAppendPower (rlListHandle handle, uint16_t nPower, eRLPowerChannels powerTargetChannel);
			rlResult rlListAppendJumpSpeed (rlListHandle handle, double jumpSpeed);
			rlResult rlListAppendMarkSpeed (rlListHandle handle, double markSpeed);
			rlResult rlListAppendJumpDelay (rlListHandle handle, double jumpDelay);
			rlResult rlListAppendMinJumpDelay (rlListHandle handle, double minJumpDelay);
			rlResult rlListAppendMarkDelay (rlListHandle handle, double markDelay);
			rlResult rlListAppendPolyDelay (rlListHandle handle, double polyDelay);
			rlResult rlListAppendLaserOnDelay (rlListHandle handle, double laserOnDelay);
			rlResult rlListAppendLaserOffDelay (rlListHandle handle, double laserOffDelay);
			rlResult rlListAppendJumpAbs2D (rlListHandle handle, double x, double y);
			rlResult rlListAppendMarkAbs2D (rlListHandle handle, double x, double y);

			rlResult rlListSet (rlHandle handle, int32_t listID, rlListHandle listHandle, bool append, int32_t executionLimit);
			rlResult rlListExecute (rlHandle handle, int32_t listID);
			rlResult rlListWaitForListIdle (rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID);
			rlResult rlListWaitForListDone (rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID);
			rlResult rlListDelete (rlHandle handle, int32_t listID, bool bFromDisk);

			rlResult rlListResetExecution(rlHandle handle);
			rlResult rlListAbortExecution(rlHandle handle);
			rlResult rlListIsExecutionInProgress(rlHandle handle, bool & bInProgress);

			rlResult rlEnableCommandLogging (rlHandle handle, const char* pszPath, int32_t truncateArguments);
			rlResult rlDisableCommandLogging (rlHandle handle);
			rlResult rlSetTimeout (rlHandle handle, uint32_t nTimeOut);
			rlResult rlGetTimeout (rlHandle handle, uint32_t* pnTimeOut);

			rlResult rlConfigurationBeginUpdate (rlHandle handle);
			rlResult rlConfigurationEndUpdate (rlHandle handle);
			rlResult rlConfigurationAbortUpdate (rlHandle handle);
			rlResult rlConfigurationIsUpdateActive (rlHandle handle, uint32_t* pbIsActive);

			uint32_t rlGpioConfigMemorySize ();
			rlResult rlGpioInitConfig (rlGpioConfig* pConfig);
			rlResult rlGpioLoadConfig (rlHandle handle);
			rlResult rlGpioStoreConfig (rlHandle handle);
			rlResult rlGpioGetConfig (rlHandle handle, rlGpioConfig* pConfig);
			rlResult rlGpioSetConfig (rlHandle handle, rlGpioConfig* pConfig);
			rlResult rlGpioGetPinCount (rlHandle handle, eRLIOPort port, int32_t& pPinCount);
			rlResult rlGpioGetGroupCount (rlHandle handle, eRLIOPort port, int32_t& pGroupCount);
			rlResult rlGpioGetSupportedIOLevels (rlHandle handle, eRLIOPort port, eRLIOVoltage* pIOLevels, uint32_t nIOLevelSize, uint32_t& nLevelsCount);
			rlResult rlGpioGetSupportedDirections (rlHandle handle, eRLIOPort port, eRLIODirection* pIODirections, uint32_t nIODirectionsSize, uint32_t& nDirectionsCount);
			rlResult rlGpioGetIOLevel (rlHandle handle, eRLIOPort port, eRLIOVoltage& ioLevel);
			rlResult rlGpioSetIOLevel (rlHandle handle, eRLIOPort port, eRLIOVoltage ioLevel);
			rlResult rlGpioGetDirections (rlHandle handle, eRLIOPort port, eRLIODirection* pIODirections, uint32_t nIODirectionsSize, uint32_t& nDirectionsCount);
			rlResult rlGpioSetDirections (rlHandle handle, eRLIOPort port, eRLIODirection* pIODirections, uint32_t nDirectionsCount);
			rlResult rlGpioGetPolarities (rlHandle handle, eRLIOPort port, eRLPolarity* pPolarities, uint32_t nPolaritiesSize, uint32_t& nPolaritiesCount);
			rlResult rlGpioSetPolarities (rlHandle handle, eRLIOPort port, eRLPolarity* pPolarities, uint32_t nPolaritiesCount);
			rlResult rlGpioGetWriteDelay (rlHandle handle, eRLIOPort port, double& dDelay);
			rlResult rlGpioSetWriteDelay (rlHandle handle, eRLIOPort port, double dDelay);
			rlResult rlGpioGetWriteWidth (rlHandle handle, eRLIOPort port, double& dWidth);
			rlResult rlGpioSetWriteWidth (rlHandle handle, eRLIOPort port, double dWidth);
			rlResult rlGpioRead (rlHandle handle, eRLIOPort port, uint32_t& nValue);
			rlResult rlGpioGetOutput (rlHandle handle, eRLIOPort port, uint32_t& nValue);
			rlResult rlGpioWrite (rlHandle handle, eRLIOPort port, eRLPinAction action, uint32_t nValue);

			uint32_t rlSystemConfigMemorySize ();
			rlResult rlSystemInitConfig (rlSystemConfig* pConfig);
			rlResult rlSystemLoadConfig (rlHandle handle);
			rlResult rlSystemStoreConfig (rlHandle handle);
			rlResult rlSystemGetConfig (rlHandle handle, rlSystemConfig* pConfig);
			rlResult rlSystemSetConfig (rlHandle handle, rlSystemConfig* pConfig);

			rlResult rlSystemGetFirmwareVersion (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);
			rlResult rlSystemGetFirmwareProductVersion (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);
			rlResult rlSystemGetFPGAVersion (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);
			rlResult rlSystemGetOSVersion (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);
			rlResult rlSystemGetCardSerialNumber (rlHandle handle, char* pBuffer, uint32_t nBufferSize, uint32_t& nStringLength);

			rlResult rlSystemGetConnectionLimit (rlHandle handle, int32_t nConnectionLimit);
			rlResult rlSystemResetOperatingState (rlHandle handle);
			rlResult rlSystemGetOperatingState (rlHandle handle, eRLOperatingState& operatingState);

			rlResult rlSerialPortInitConfig (rlSerialPortConfig* pConfig);
			rlResult rlSerialPortLoadConfig (rlHandle handle);
			rlResult rlSerialPortStoreConfig (rlHandle handle);
			rlResult rlSerialPortGetConfig (rlHandle handle, rlSerialPortConfig* pConfig);
			rlResult rlSerialPortSetConfig (rlHandle handle, rlSerialPortConfig* pConfig);
			rlResult rlSerialPortTransmitString (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength);
			rlResult rlSerialPortTransmitLine (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength);
			rlResult rlSerialPortTransmitBytes (rlHandle handle, const uint8_t* pTransmitMessage, uint32_t nTransmitMessageLength);

			rlResult rlSerialPortTryVerifyResponseString (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength, const char* pGoodresponse, uint32_t nGoodResponseLength, uint32_t& nSuccessFlag);
			rlResult rlSerialPortTryVerifyResponseLine (rlHandle handle, const char* pTransmitMessage, uint32_t nTransmitMessageLength, const char* pGoodresponse, uint32_t nGoodResponseLength, uint32_t& nSuccessFlag);
			rlResult rlSerialPortTryVerifyResponseBytes (rlHandle handle, const uint8_t* pTransmitMessage, uint32_t nTransmitMessageLength, const uint8_t* pGoodresponse, uint32_t nGoodResponseLength, uint32_t& nSuccessFlag);

			rlResult rlSerialPortReceiveString (rlHandle handle, uint32_t nReadCount, char* pOutBuffer, uint32_t nBufferLength, uint32_t& nReceivedLength);
			rlResult rlSerialPortReceiveLine (rlHandle handle, char* pOutBuffer, uint32_t nBufferLength, uint32_t& nReceivedLength);
			rlResult rlSerialPortReceiveBytes (rlHandle handle, uint32_t nReadCount, uint8_t* pOutBuffer, uint32_t nBufferLength, uint32_t& nReceivedLength);

			rlResult rlLaserInitConfig (rlLaserConfig* pConfig);
			rlResult rlLaserLoadConfig (rlHandle handle);
			rlResult rlLaserStoreConfig (rlHandle handle);
			rlResult rlLaserGetConfig (rlHandle handle, rlLaserConfig* pConfig);
			rlResult rlLaserSetConfig (rlHandle handle, rlLaserConfig* pConfig);


			rlResult rlLaserCreatePowerCalibration (rlHandle handle, eRLPowerChannels powerTargetChannel);
			rlResult rlLaserLoadPowerCalibration (rlHandle handle, eRLPowerChannels powerTargetChannel);
			rlResult rlLaserStorePowerCalibration (rlHandle handle, eRLPowerChannels powerTargetChannel);
			rlResult rlLaserGetPowerCalibration (rlHandle handle, eRLPowerChannels powerTargetChannel, uint32_t* pBuffer, int32_t nBufferSize, int32_t& calibrationValuesCount);
			rlResult rlLaserSetPowerCalibration (rlHandle handle, eRLPowerChannels powerTargetChannel, uint32_t* pCalibrationValues, int32_t calibrationValuesLength);
			rlResult rlLaserPowerCalibrationSet (rlHandle handle, eRLPowerChannels powerTargetChannel, const char* pLocalFileName);
			rlResult rlLaserPowerCalibrationGet (rlHandle handle, eRLPowerChannels powerTargetChannel, const char* pLocalFileName);
			rlResult rlLaserGetPowerScale (rlHandle handle, eRLPowerChannels powerTargetChannel, double& dPowerScale);
			rlResult rlLaserSetPowerScale (rlHandle handle, eRLPowerChannels powerTargetChannel, double dPowerScale);
				
			rlResult rlScannerInitConfig (rlScannerConfig* pConfig);
			rlResult rlScannerSetConfig (rlHandle handle, rlScannerConfig* pConfig);
			rlResult rlScannerGetConfig (rlHandle handle, rlScannerConfig* pConfig);
			rlResult rlScannerLoadConfig (rlHandle handle);
			rlResult rlScannerStoreConfig (rlHandle handle);
			rlResult rlScannerGetNominalPosition (rlHandle handle, double& dX, double& dY, double& dZ);
			rlResult rlScannerReadPositions (rlHandle handle, double* pdPositions);
			rlResult rlScannerReadLastPositions (rlHandle handle, double* pdPositions);
			rlResult rlScannerHeadFormatGetHeadCount (eRLScanHeadFormat headFormat, int32_t& nOut);
			rlResult rlScannerHeadFormatGetAxisCount (eRLScanHeadFormat headFormat, int32_t& nOut);
			rlResult rlScannerHeadFormatGetDimensions (eRLScanHeadFormat headFormat, int32_t* pDimensions);
			rlResult rlScannerHeadFormatGetAxes (eRLScanHeadFormat headFormat, eRLAxes& axes);
			rlResult rlScannerHeadFormatGetIndexOfAxis (eRLScanHeadFormat headFormat, int32_t nIndex, eRLAxis& axis);
			rlResult rlScannerHeadFormatHasOpticalZoom (eRLScanHeadFormat headFormat, uint32_t& bHasZoom);
			rlResult rlScannerHeadFormatHasAuxiliary (eRLScanHeadFormat headFormat, uint32_t& bHasAuxiliary);
			rlResult rlScannerHeadFormatIsExtended (eRLScanHeadFormat headFormat, uint32_t& bIsExtended);
			rlResult rlScannerGetHeadBias (rlHandle handle, int32_t nHead, rlPoint3D& bias);
			rlResult rlScannerSetHeadBias (rlHandle handle, int32_t nHead, eRLAxes axes, double* pBiases, int32_t nLength);

			rlResult rlAdapterBoardGetConfig (rlHandle handle, eRLIOPort port, rlAdapterBoardConfig* pConfig);
			rlResult rlAdapterBoardSetConfig (rlHandle handle, eRLIOPort port, rlAdapterBoardConfig* pConfig);
			rlResult rlAdapterBoardSetConfigByName (rlHandle handle, eRLIOPort port, const char* pName);
			rlResult rlAdapterBoardEnable (rlHandle handle, eRLIOPort port, uint32_t bEnable);
			rlResult rlAdapterBoardEnabled (rlHandle handle, eRLIOPort port, uint32_t& bEnabled);
			rlResult rlAdapterBoardSupportsAutoConfiguration (rlHandle handle, eRLIOPort port, uint32_t& bSupportsAutoConfiguration);
			rlResult rlAdapterBoardValid (rlHandle handle, eRLIOPort port, uint32_t& bValid);
			rlResult rlAdapterBoardGetAvailableAdapterBoardNames (rlHandle handle, eRLIOPort port, char* pBuffer, uint32_t nBufferLen, uint32_t& outLen, int32_t* pOutItems);
			rlResult rlAdapterBoardGetCurrentAdapterBoardName (rlHandle handle, eRLIOPort port, char* pBuffer, uint32_t nBufferLen, uint32_t& outLen);


			rlResult rlProcessInitVariables (rlProcessVariables* pVariables);
			rlResult rlProcessLoadVariables (rlHandle handle);
			rlResult rlProcessStoreVariables (rlHandle handle);
			rlResult rlProcessGetVariables (rlHandle handle, rlProcessVariables* pVariables);
			rlResult rlProcessSetVariables (rlHandle handle, rlProcessVariables* pVariables);

			rlResult rlProcessGetSpeedScale (rlHandle handle, double& dSpeedScale);
			rlResult rlProcessSetSpeedScale (rlHandle handle, double dSpeedScale);
			rlResult rlProcessGetTransformation (rlHandle handle, rlTransform3D* pTransformation);
			rlResult rlProcessSetTransformation (rlHandle handle, rlTransform3D* pTransformation);
			rlResult rlProcessWobbleSetCircle (rlHandle handle, rlProcessVariables* pVariables, double dAmplitude, double dFrequency);
			rlResult rlProcessWobbleSetEight (rlHandle handle, rlProcessVariables* pVariables, rlPoint2D* pAmplitude, double dFrequency);

			rlResult rlHeadCorrectionExists (rlHandle handle, int32_t nHead, int32_t nCorrection, uint32_t& bExists);
			rlResult rlHeadCorrectionDelete (rlHandle handle, int32_t nHead, int32_t nCorrection);
			rlResult rlHeadCorrectionGet (rlHandle handle, int32_t nHead, int32_t nCorrection, const char* pLocalFileName);
			rlResult rlHeadCorrectionSet (rlHandle handle, int32_t nHead, int32_t nCorrection, const char* pLocalFileName);
			rlResult rlHeadCorrectionLoad (rlHandle handle);
			rlResult rlHeadCorrectionStore (rlHandle handle);

			rlResult rlHeadCorrectionInitOpticalParameters (rlOpticalParameters* pParameters);
			rlResult rlHeadCorrectionGetOpticalParameters (rlHandle handle, int32_t nHead, int32_t nCorrection, rlOpticalParameters* pParameters);
			rlResult rlHeadCorrectionGetHash (rlHandle handle, int32_t nHead, int32_t nCorrection, char* pOutBuffer, uint32_t nBufferLen, uint32_t& nOutLen);
			rlResult rlHeadCorrectionSelectCorrection (rlHandle handle, int32_t nHead, int32_t nCorrection);
			rlResult rlHeadCorrectionGetSelectedCorrection (rlHandle handle, int32_t nHead, int32_t& nSelectedCorrection);

			rlResult rlMotfQuadratureDecoderInitConfig (rlQuadratureDecoderConfig* pConfig);
			rlResult rlMotfQuadratureDecoderLoadConfig (rlHandle handle);
			rlResult rlMotfQuadratureDecoderStoreConfig (rlHandle handle);
			rlResult rlMotfQuadratureDecoderGetConfig (rlHandle handle, rlQuadratureDecoderConfig* pConfig);
			rlResult rlMotfQuadratureDecoderSetConfig (rlHandle handle, rlQuadratureDecoderConfig* pConfig);

			rlResult rlMotfInitConfig (rlMotfConfig* pConfig);
			rlResult rlMotfGetConfig (rlHandle handle, rlMotfConfig* pConfig);
			rlResult rlMotfSetConfig (rlHandle handle, rlMotfConfig* pConfig);
			rlResult rlMotfLoadConfig (rlHandle handle);
			rlResult rlMotfStoreConfig (rlHandle handle);
			rlResult rlMotfConfigSetDirectionFromAngle (rlMotfConfig* pConfig, double dAngle);
			rlResult rlMotfQuadratureDecoderResetEncoderPosition (rlHandle handle, int32_t nEncoderID);
			rlResult rlMotfQuadratureDecoderReadEncoderPosition (rlHandle handle, int32_t nEncoderID, int32_t& nPosition);
			rlResult rlMotfGetPendingParts (rlHandle handle, int32_t& nPendingPart);
			rlResult rlMotfResetState (rlHandle handle);

			rlResult rlSfioRead (rlHandle handle, eRLSfRegister, uint32_t& nValue);
			rlResult rlSfioSpiInitConfig (rlSpiConfig* pConfig);
			rlResult rlSfioSpiLoadConfig (rlHandle handle);
			rlResult rlSfioSpiStoreConfig (rlHandle handle);
			rlResult rlSfioSpiGetConfig (rlHandle handle, rlSpiConfig* pConfig);
			rlResult rlSfioSpiSetConfig (rlHandle handle, rlSpiConfig* pConfig);
			rlResult rlSfioSpiTransmit (rlHandle handle, int32_t nModule, uint32_t* pTransmitMessage, uint32_t nTransmitMessageLen, uint32_t bAsync);
			rlResult rlSfioSpiTransceive (rlHandle handle, int32_t nModule, uint32_t* pTransmitMessage, uint32_t nTransmitMessageLen, int32_t nTimeoutInMS, uint32_t* pBuffer, uint32_t nBufferSize, uint32_t nReceiveLength);
			rlResult rlSfioSpiReceive (rlHandle handle, int32_t nModule, uint32_t nReadCount, int32_t nTimeoutInMS, uint32_t* pBuffer, uint32_t nBufferSize, uint32_t nReceiveLength);
			rlResult rlSfioSpiWaitForActiveTransfersDone (rlHandle handle, int32_t nModule);

			rlResult rlPowerCorrectionCreate (rlHandle handle, rlOpticalParameters* parameters);
			rlResult rlPowerCorrectionSet (rlHandle handle, const char* pLocalFileName);
			rlResult rlPowerCorrectionGet (rlHandle handle, const char* pLocalFileName);
			rlResult rlPowerCorrectionLoad (rlHandle handle);
			rlResult rlPowerCorrectionStore (rlHandle handle);

			rlResult rlPowerCorrectionChangeValue (rlHandle handle, rlPoint3D position, double dFactor);

			rlResult rlPowerCorrectionGetOpticalParameters (rlHandle handle, rlOpticalParameters* pParameters);

			rlResult rlScannerCommunicationTransceiveEnhanced (rlHandle handle, int32_t nHead, eRLAxes axes, const uint32_t* commands, uint32_t commandsLength, int32_t timeoutMilliseconds, uint32_t* buffer, uint32_t bufferSize, uint32_t& receiveCount);
			rlResult rlScannerCommunicationTransmitEnhanced (rlHandle handle, int32_t nHead, eRLAxes axes, const uint32_t* commands, uint32_t commandsLength, int32_t timeoutMilliseconds);
			rlResult rlScannerCommunicationReceiveEnhanced (rlHandle handle, int32_t nHead, eRLAxes axes, int32_t timeoutMilliseconds, uint32_t* buffer, uint32_t bufferSize, uint32_t& receiveCount);
			rlResult rlScannerCommunicationTransceiveRaylase (rlHandle handle, int32_t nHead, eRLAxis axis, const unsigned char* transmitMessage, uint32_t transmitMessageLength, int32_t timeoutMilliseconds, unsigned char* buffer, uint32_t bufferSize, uint32_t& receiveCount);
			rlResult rlScannerCommunicationTransmitRaylase (rlHandle handle, int32_t nHead, eRLAxis axis, const unsigned char* transmitMessage, uint32_t transmitMessageLength, int32_t timeoutMilliseconds);


			rlResult rlVariableBooleanCreate (rlHandle handle, const char* name, bool initialValue);
			rlResult rlVariableBooleanDelete (rlHandle handle, const char* name, bool fromDisk);
			rlResult rlVariableBooleanExists (rlHandle handle, const char* name, bool& exists);
			rlResult rlVariableBooleanLoad (rlHandle handle, const char* name);
			rlResult rlVariableBooleanStore (rlHandle handle, const char* name);

			rlResult rlVariableBooleanGetValue (rlHandle handle, const char* name, bool& value);
			rlResult rlVariableBooleanSetValue (rlHandle handle, const char* name, bool value);
			rlResult rlVariableBooleanToggleValue (rlHandle handle, const char* name);

			rlResult rlVariableIntegerCreate (rlHandle handle, const char* name, int32_t initialValue);
			rlResult rlVariableIntegerDelete (rlHandle handle, const char* name, bool fromDisk);
			rlResult rlVariableIntegerExists (rlHandle handle, const char* name, bool& exists);
			rlResult rlVariableIntegerLoad (rlHandle handle, const char* name);
			rlResult rlVariableIntegerStore (rlHandle handle, const char* name);

			rlResult rlVariableIntegerGetValue (rlHandle handle, const char* name, int32_t& value);
			rlResult rlVariableIntegerSetValue (rlHandle handle, const char* name, int32_t value);
			rlResult rlVariableIntegerIncrementValue (rlHandle handle, const char* name);
			rlResult rlVariableIntegerDecrementValue (rlHandle handle, const char* name);

			rlResult rlVariableMarkCounterCreate (rlHandle handle, const char* name, int32_t initialValue);
			rlResult rlVariableMarkCounterDelete (rlHandle handle, const char* name, bool fromDisk);
			rlResult rlVariableMarkCounterExists (rlHandle handle, const char* name, bool& exists);
			rlResult rlVariableMarkCounterLoad (rlHandle handle, const char* name);
			rlResult rlVariableMarkCounterStore (rlHandle handle, const char* name);

			rlResult rlVariableMarkCounterGetValue (rlHandle handle, const char* name, int32_t& value);
			rlResult rlVariableMarkCounterSetValue (rlHandle handle, const char* name, int32_t value);
			rlResult rlVariableMarkCounterGetAndResetValue (rlHandle handle, const char* name, int32_t& value);

			rlResult rlVariableMarkCounterGetMarkTimeFormat (rlHandle handle, char* buffer, uint32_t bufferSize, uint32_t& stringLength);
			rlResult rlVariableMarkCounterSetMarkTimeFormat (rlHandle handle, const char* format);


		};


		typedef std::shared_ptr<CRaylaseSDK> PRaylaseSDK;

	} 
} 

#endif // __LIBMCDRIVER_RAYLASE_SDK
