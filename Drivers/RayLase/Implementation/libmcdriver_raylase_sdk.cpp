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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "libmcdriver_raylase_sdk.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_Raylase::Impl;

#define RAYLASE_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadRaylaseAddress (HMODULE hLibrary, const std::string & sSymbolName) {

	void * pFuncPtr = (void*) GetProcAddress(hLibrary, sSymbolName.c_str ());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadRaylaseAddress(void * hLibrary, const char* pSymbolName) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CRaylaseSDK::CRaylaseSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr), m_bEnableJournal (false)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > RAYLASE_MAXDLLNAMELENGTH)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32


	this->ptrConnect = (PrlConnect)_loadRaylaseAddress(hLibrary, "rlConnect");
	this->ptrIsConnected = (PrlIsConnected)_loadRaylaseAddress(hLibrary, "rlIsConnected");
	this->ptrDisconnect = (PrlDisconnect)_loadRaylaseAddress(hLibrary, "rlDisconnect");
	this->ptrSystemResetToDefaults = (PrlDisconnect)_loadRaylaseAddress(hLibrary, "rlSystemResetToDefaults");
	this->ptrLaserLaserOn = (PrlLaserLaserOn)_loadRaylaseAddress(hLibrary, "rlLaserLaserOn");
	this->ptrLaserLaserOff = (PrlLaserLaserOff)_loadRaylaseAddress(hLibrary, "rlLaserLaserOff");
	this->ptrLaserIsPilotEnabled = (PrlLaserIsPilotEnabled)_loadRaylaseAddress(hLibrary, "rlLaserIsPilotEnabled");
	this->ptrLaserEnablePilot = (PrlLaserEnablePilot)_loadRaylaseAddress(hLibrary, "rlLaserEnablePilot");
	this->ptrLaserIsLaserArmed = (PrlLaserIsLaserArmed)_loadRaylaseAddress(hLibrary, "rlLaserIsLaserArmed");
	this->ptrLaserArmLaser = (PrlLaserArmLaser)_loadRaylaseAddress(hLibrary, "rlLaserArmLaser");
	this->ptrLaserReadLaserStatus = (PrlLaserReadLaserStatus)_loadRaylaseAddress(hLibrary, "rlLaserReadLaserStatus");

	this->ptrListAllocate = (PrlListAllocate)_loadRaylaseAddress(hLibrary, "rlListAllocate");
	this->ptrListReleaseHandle = (PrlListReleaseHandle)_loadRaylaseAddress(hLibrary, "rlListReleaseHandle");
	this->ptrListAppendLaserOn = (PrlListAppendLaserOn)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOn");
	this->ptrListAppendLaserOff = (PrlListAppendLaserOff)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOff");
	this->ptrListAppendPower = (PrlListAppendPower)_loadRaylaseAddress(hLibrary, "rlListAppendPower");
	this->ptrListAppendJumpSpeed = (PrlListAppendJumpSpeed)_loadRaylaseAddress(hLibrary, "rlListAppendJumpSpeed");
	this->ptrListAppendMarkSpeed = (PrlListAppendMarkSpeed)_loadRaylaseAddress(hLibrary, "rlListAppendMarkSpeed");
	this->ptrListAppendJumpDelay = (PrlListAppendJumpDelay)_loadRaylaseAddress(hLibrary, "rlListAppendJumpDelay");
	this->ptrListAppendMinJumpDelay = (PrlListAppendMinJumpDelay)_loadRaylaseAddress(hLibrary, "rlListAppendMinJumpDelay");
	this->ptrListAppendMarkDelay = (PrlListAppendMarkDelay)_loadRaylaseAddress(hLibrary, "rlListAppendMarkDelay");
	this->ptrListAppendPolyDelay = (PrlListAppendPolyDelay)_loadRaylaseAddress(hLibrary, "rlListAppendPolyDelay");
	this->ptrListAppendLaserOnDelay = (PrlListAppendLaserOnDelay)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOnDelay");
	this->ptrListAppendLaserOffDelay = (PrlListAppendLaserOffDelay)_loadRaylaseAddress(hLibrary, "rlListAppendLaserOffDelay");
	this->ptrListAppendJumpAbs2D = (PrlListAppendJumpAbs2D)_loadRaylaseAddress(hLibrary, "rlListAppendJumpAbs2D");
	this->ptrListAppendMarkAbs2D = (PrlListAppendMarkAbs2D)_loadRaylaseAddress(hLibrary, "rlListAppendMarkAbs2D");
	this->ptrListSet = (PrlListSet)_loadRaylaseAddress(hLibrary, "rlListSet");
	this->ptrListExecute = (PrlListExecute)_loadRaylaseAddress(hLibrary, "rlListExecute");
	this->ptrListWaitForListIdle = (PrlListWaitForListIdle)_loadRaylaseAddress(hLibrary, "rlListWaitForListIdle");
	this->ptrListWaitForListDone = (PrlListWaitForListDone)_loadRaylaseAddress(hLibrary, "rlListWaitForListDone");
	this->ptrListDelete = (PrlListDelete)_loadRaylaseAddress(hLibrary, "rlListDelete");
	this->ptrGetLastError = (PrlGetLastError)_loadRaylaseAddress(hLibrary, "rlGetLastError");
	this->ptrGetLastErrorLen = (PrlGetLastErrorLen)_loadRaylaseAddress(hLibrary, "rlGetLastErrorLen");

	this->ptrEnableCommandLogging = (PrlEnableCommandLogging)_loadRaylaseAddress(hLibrary, "rlEnableCommandLogging");
	this->ptrDisableCommandLogging = (PrlDisableCommandLogging)_loadRaylaseAddress(hLibrary, "rlDisableCommandLogging");
	this->ptrSetTimeout = (PrlSetTimeout)_loadRaylaseAddress(hLibrary, "rlSetTimeout");
	this->ptrGetTimeout = (PrlGetTimeout)_loadRaylaseAddress(hLibrary, "rlGetTimeout");
	this->ptrConfigurationBeginUpdate = (PrlConfigurationBeginUpdate)_loadRaylaseAddress(hLibrary, "rlConfigurationBeginUpdate");
	this->ptrConfigurationEndUpdate = (PrlConfigurationEndUpdate)_loadRaylaseAddress(hLibrary, "rlConfigurationEndUpdate");
	this->ptrConfigurationAbortUpdate = (PrlConfigurationAbortUpdate)_loadRaylaseAddress(hLibrary, "rlConfigurationAbortUpdate");
	this->ptrConfigurationIsUpdateActive = (PrlConfigurationIsUpdateActive)_loadRaylaseAddress(hLibrary, "rlConfigurationIsUpdateActive");

	this->ptrGpioGetPinCount = (PrlGpioGetPinCount)_loadRaylaseAddress(hLibrary, "rlGpioGetPinCount");
	this->ptrGpioGetGroupCount = (PrlGpioGetGroupCount)_loadRaylaseAddress(hLibrary, "rlGpioGetGroupCount");
	this->ptrGpioGetSupportedIOLevels = (PrlGpioGetSupportedIOLevels)_loadRaylaseAddress(hLibrary, "rlGpioGetSupportedIOLevels");
	this->ptrGpioGetSupportedDirections = (PrlGpioGetSupportedDirections)_loadRaylaseAddress(hLibrary, "rlGpioGetSupportedDirections");
	this->ptrGpioGetIOLevel = (PrlGpioGetIOLevel)_loadRaylaseAddress(hLibrary, "rlGpioGetIOLevel");
	this->ptrGpioSetIOLevel = (PrlGpioSetIOLevel)_loadRaylaseAddress(hLibrary, "rlGpioSetIOLevel");
	this->ptrGpioGetDirections = (PrlGpioGetDirections)_loadRaylaseAddress(hLibrary, "rlGpioGetDirections");
	this->ptrGpioSetDirections = (PrlGpioSetDirections)_loadRaylaseAddress(hLibrary, "rlGpioSetDirections");
	this->ptrGpioGetPolarities = (PrlGpioGetPolarities)_loadRaylaseAddress(hLibrary, "rlGpioGetPolarities");
	this->ptrGpioSetPolarities = (PrlGpioSetPolarities)_loadRaylaseAddress(hLibrary, "rlGpioSetPolarities");
	this->ptrGpioGetWriteDelay = (PrlGpioGetWriteDelay)_loadRaylaseAddress(hLibrary, "rlGpioGetWriteDelay");
	this->ptrGpioSetWriteDelay = (PrlGpioSetWriteDelay)_loadRaylaseAddress(hLibrary, "rlGpioSetWriteDelay");
	this->ptrGpioGetWriteWidth = (PrlGpioGetWriteWidth)_loadRaylaseAddress(hLibrary, "rlGpioGetWriteWidth");
	this->ptrGpioSetWriteWidth = (PrlGpioSetWriteWidth)_loadRaylaseAddress(hLibrary, "rlGpioSetWriteWidth");
	this->ptrGpioRead = (PrlGpioRead)_loadRaylaseAddress(hLibrary, "rlGpioRead");
	this->ptrGpioGetOutput = (PrlGpioGetOutput)_loadRaylaseAddress(hLibrary, "rlGpioGetOutput");
	this->ptrGpioWrite = (PrlGpioWrite)_loadRaylaseAddress(hLibrary, "rlGpioWrite");

	this->ptrSystemLoadConfig = (PrlSystemLoadConfig)_loadRaylaseAddress(hLibrary, "rlSystemLoadConfig");
	this->ptrSystemStoreConfig = (PrlSystemStoreConfig)_loadRaylaseAddress(hLibrary, "rlSystemStoreConfig");
	this->ptrSystemGetConfig = (PrlSystemGetConfig)_loadRaylaseAddress(hLibrary, "rlSystemGetConfig");
	this->ptrSystemSetConfig = (PrlSystemSetConfig)_loadRaylaseAddress(hLibrary, "rlSystemSetConfig");
	this->ptrSystemGetFirmwareVersion = (PrlSystemGetFirmwareVersion)_loadRaylaseAddress(hLibrary, "rlSystemGetFirmwareVersion");
	this->ptrSystemGetFirmwareProductVersion = (PrlSystemGetFirmwareProductVersion)_loadRaylaseAddress(hLibrary, "rlSystemGetFirmwareProductVersion");
	this->ptrSystemGetFPGAVersion = (PrlSystemGetFPGAVersion)_loadRaylaseAddress(hLibrary, "rlSystemGetFPGAVersion");
	this->ptrSystemGetOSVersion = (PrlSystemGetOSVersion)_loadRaylaseAddress(hLibrary, "rlSystemGetOSVersion");
	this->ptrSystemGetCardSerialNumber = (PrlSystemGetCardSerialNumber)_loadRaylaseAddress(hLibrary, "rlSystemGetCardSerialNumber");
	this->ptrSystemGetConnectionLimit = (PrlSystemGetConnectionLimit)_loadRaylaseAddress(hLibrary, "rlSystemGetConnectionLimit");
	this->ptrSystemResetOperatingState = (PrlSystemResetOperatingState)_loadRaylaseAddress(hLibrary, "rlSystemResetOperatingState");
	this->ptrSystemGetOperatingState = (PrlSystemGetOperatingState)_loadRaylaseAddress(hLibrary, "rlSystemGetOperatingState");
	
	this->ptrSerialPortInitConfig = (PrlSerialPortInitConfig)_loadRaylaseAddress(hLibrary, "rlSerialPortInitConfig");
	this->ptrSerialPortLoadConfig = (PrlSerialPortLoadConfig)_loadRaylaseAddress(hLibrary, "rlSerialPortLoadConfig");
	this->ptrSerialPortStoreConfig = (PrlSerialPortStoreConfig)_loadRaylaseAddress(hLibrary, "rlSerialPortStoreConfig");
	this->ptrSerialPortGetConfig = (PrlSerialPortGetConfig)_loadRaylaseAddress(hLibrary, "rlSerialPortGetConfig");
	this->ptrSerialPortSetConfig = (PrlSerialPortSetConfig)_loadRaylaseAddress(hLibrary, "rlSerialPortSetConfig");
	this->ptrSerialPortTransmitString = (PrlSerialPortTransmitString)_loadRaylaseAddress(hLibrary, "rlSerialPortTransmitString");
	this->ptrSerialPortTransmitLine = (PrlSerialPortTransmitLine)_loadRaylaseAddress(hLibrary, "rlSerialPortTransmitLine");
	this->ptrSerialPortTransmitBytes = (PrlSerialPortTransmitBytes)_loadRaylaseAddress(hLibrary, "rlSerialPortTransmitBytes");
	this->ptrSerialPortTryVerifyResponseString = (PrlSerialPortTryVerifyResponseString)_loadRaylaseAddress(hLibrary, "rlSerialPortTryVerifyResponseString");
	this->ptrSerialPortTryVerifyResponseLine = (PrlSerialPortTryVerifyResponseLine)_loadRaylaseAddress(hLibrary, "rlSerialPortTryVerifyResponseLine");
	this->ptrSerialPortTryVerifyResponseBytes = (PrlSerialPortTryVerifyResponseBytes)_loadRaylaseAddress(hLibrary, "rlSerialPortTryVerifyResponseBytes");
	this->ptrSerialPortReceiveString = (PrlSerialPortReceiveString)_loadRaylaseAddress(hLibrary, "rlSerialPortReceiveString");
	this->ptrSerialPortReceiveLine = (PrlSerialPortReceiveLine)_loadRaylaseAddress(hLibrary, "rlSerialPortReceiveLine");
	this->ptrSerialPortReceiveBytes = (PrlSerialPortReceiveBytes)_loadRaylaseAddress(hLibrary, "rlSerialPortReceiveBytes");

	this->ptrLaserInitConfig = (PrlLaserInitConfig)_loadRaylaseAddress(hLibrary, "rlLaserInitConfig");
	this->ptrLaserLoadConfig = (PrlLaserLoadConfig)_loadRaylaseAddress(hLibrary, "rlLaserLoadConfig");
	this->ptrLaserStoreConfig = (PrlLaserStoreConfig)_loadRaylaseAddress(hLibrary, "rlLaserStoreConfig");
	this->ptrLaserGetConfig = (PrlLaserGetConfig)_loadRaylaseAddress(hLibrary, "rlLaserGetConfig");
	this->ptrLaserSetConfig = (PrlLaserSetConfig)_loadRaylaseAddress(hLibrary, "rlLaserSetConfig");
	this->ptrLaserCreatePowerCalibration = (PrlLaserCreatePowerCalibration)_loadRaylaseAddress(hLibrary, "rlLaserCreatePowerCalibration");
	this->ptrLaserLoadPowerCalibration = (PrlLaserLoadPowerCalibration)_loadRaylaseAddress(hLibrary, "rlLaserLoadPowerCalibration");
	this->ptrLaserStorePowerCalibration = (PrlLaserStorePowerCalibration)_loadRaylaseAddress(hLibrary, "rlLaserStorePowerCalibration");
	this->ptrLaserGetPowerCalibration = (PrlLaserGetPowerCalibration)_loadRaylaseAddress(hLibrary, "rlLaserGetPowerCalibration");
	this->ptrLaserSetPowerCalibration = (PrlLaserSetPowerCalibration)_loadRaylaseAddress(hLibrary, "rlLaserSetPowerCalibration");
	this->ptrLaserPowerCalibrationSet = (PrlLaserPowerCalibrationSet)_loadRaylaseAddress(hLibrary, "rlLaserPowerCalibrationSet");
	this->ptrLaserPowerCalibrationGet = (PrlLaserPowerCalibrationGet)_loadRaylaseAddress(hLibrary, "rlLaserPowerCalibrationGet");
	this->ptrLaserGetPowerScale = (PrlLaserGetPowerScale)_loadRaylaseAddress(hLibrary, "rlLaserGetPowerScale");
	this->ptrLaserSetPowerScale = (PrlLaserSetPowerScale)_loadRaylaseAddress(hLibrary, "rlLaserSetPowerScale");

	this->ptrScannerInitConfig = (PrlScannerInitConfig)_loadRaylaseAddress(hLibrary, "rlScannerInitConfig");
	this->ptrScannerSetConfig = (PrlScannerSetConfig)_loadRaylaseAddress(hLibrary, "rlScannerSetConfig");
	this->ptrScannerGetConfig = (PrlScannerGetConfig)_loadRaylaseAddress(hLibrary, "rlScannerGetConfig");
	this->ptrScannerLoadConfig = (PrlScannerLoadConfig)_loadRaylaseAddress(hLibrary, "rlScannerLoadConfig");
	this->ptrScannerStoreConfig = (PrlScannerStoreConfig)_loadRaylaseAddress(hLibrary, "rlScannerStoreConfig");
	this->ptrScannerGetNominalPosition = (PrlScannerGetNominalPosition)_loadRaylaseAddress(hLibrary, "rlScannerGetNominalPosition");
	this->ptrScannerReadPositions = (PrlScannerReadPositions)_loadRaylaseAddress(hLibrary, "rlScannerReadPositions");
	this->ptrScannerReadLastPositions = (PrlScannerReadLastPositions)_loadRaylaseAddress(hLibrary, "rlScannerReadLastPositions");
	this->ptrScannerHeadFormatGetHeadCount = (PrlScannerHeadFormatGetHeadCount)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatGetHeadCount");
	this->ptrScannerHeadFormatGetAxisCount = (PrlScannerHeadFormatGetAxisCount)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatGetAxisCount");
	this->ptrScannerHeadFormatGetDimensions = (PrlScannerHeadFormatGetDimensions)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatGetDimensions");
	this->ptrScannerHeadFormatGetAxes = (PrlScannerHeadFormatGetAxes)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatGetAxes");
	this->ptrScannerHeadFormatGetIndexOfAxis = (PrlScannerHeadFormatGetIndexOfAxis)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatGetIndexOfAxis");
	this->ptrScannerHeadFormatHasOpticalZoom = (PrlScannerHeadFormatHasOpticalZoom)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatHasOpticalZoom");
	this->ptrScannerHeadFormatHasAuxiliary = (PrlScannerHeadFormatHasAuxiliary)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatHasAuxiliary");
	this->ptrScannerHeadFormatIsExtended = (PrlScannerHeadFormatIsExtended)_loadRaylaseAddress(hLibrary, "rlScannerHeadFormatIsExtended");
	this->ptrScannerGetHeadBias = (PrlScannerGetHeadBias)_loadRaylaseAddress(hLibrary, "rlScannerGetHeadBias");
	this->ptrScannerSetHeadBias = (PrlScannerSetHeadBias)_loadRaylaseAddress(hLibrary, "rlScannerSetHeadBias");

	this->ptrAdapterBoardGetConfig = (PrlAdapterBoardGetConfig)_loadRaylaseAddress(hLibrary, "rlAdapterBoardGetConfig");
	this->ptrAdapterBoardSetConfig = (PrlAdapterBoardSetConfig)_loadRaylaseAddress(hLibrary, "rlAdapterBoardSetConfig");
	this->ptrAdapterBoardSetConfigByName = (PrlAdapterBoardSetConfigByName)_loadRaylaseAddress(hLibrary, "rlAdapterBoardSetConfigByName");
	this->ptrAdapterBoardEnable = (PrlAdapterBoardEnable)_loadRaylaseAddress(hLibrary, "rlAdapterBoardEnable");
	this->ptrAdapterBoardEnabled = (PrlAdapterBoardEnabled)_loadRaylaseAddress(hLibrary, "rlAdapterBoardEnabled");
	this->ptrAdapterBoardSupportsAutoConfiguration = (PrlAdapterBoardSupportsAutoConfiguration)_loadRaylaseAddress(hLibrary, "rlAdapterBoardSupportsAutoConfiguration");
	this->ptrAdapterBoardValid = (PrlAdapterBoardValid)_loadRaylaseAddress(hLibrary, "rlAdapterBoardValid");
	this->ptrAdapterBoardGetAvailableAdapterBoardNames = (PrlAdapterBoardGetAvailableAdapterBoardNames)_loadRaylaseAddress(hLibrary, "rlAdapterBoardGetAvailableAdapterBoardNames");
	this->ptrAdapterBoardGetCurrentAdapterBoardName = (PrlAdapterBoardGetCurrentAdapterBoardName)_loadRaylaseAddress(hLibrary, "rlAdapterBoardGetCurrentAdapterBoardName");

	this->ptrProcessInitVariables = (PrlProcessInitVariables)_loadRaylaseAddress(hLibrary, "rlProcessInitVariables");
	this->ptrProcessLoadVariables = (PrlProcessLoadVariables)_loadRaylaseAddress(hLibrary, "rlProcessLoadVariables");
	this->ptrProcessStoreVariables = (PrlProcessStoreVariables)_loadRaylaseAddress(hLibrary, "rlProcessStoreVariables");
	this->ptrProcessGetVariables = (PrlProcessGetVariables)_loadRaylaseAddress(hLibrary, "rlProcessGetVariables");
	this->ptrProcessSetVariables = (PrlProcessSetVariables)_loadRaylaseAddress(hLibrary, "rlProcessSetVariables");
	this->ptrProcessGetSpeedScale = (PrlProcessGetSpeedScale)_loadRaylaseAddress(hLibrary, "rlProcessGetSpeedScale");
	this->ptrProcessSetSpeedScale = (PrlProcessSetSpeedScale)_loadRaylaseAddress(hLibrary, "rlProcessSetSpeedScale");
	this->ptrProcessGetTransformation = (PrlProcessGetTransformation)_loadRaylaseAddress(hLibrary, "rlProcessGetTransformation");
	this->ptrProcessSetTransformation = (PrlProcessSetTransformation)_loadRaylaseAddress(hLibrary, "rlProcessSetTransformation");
	this->ptrProcessWobbleSetCircle = (PrlProcessWobbleSetCircle)_loadRaylaseAddress(hLibrary, "rlProcessWobbleSetCircle");
	this->ptrProcessWobbleSetEight = (PrlProcessWobbleSetEight)_loadRaylaseAddress(hLibrary, "rlProcessWobbleSetEight");


	this->ptrHeadCorrectionExists = (PrlHeadCorrectionExists)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionExists");
	this->ptrHeadCorrectionDelete = (PrlHeadCorrectionDelete)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionDelete");
	this->ptrHeadCorrectionGet = (PrlHeadCorrectionGet)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionGet");
	this->ptrHeadCorrectionSet = (PrlHeadCorrectionSet)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionSet");
	this->ptrHeadCorrectionLoad = (PrlHeadCorrectionLoad)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionLoad");
	this->ptrHeadCorrectionStore = (PrlHeadCorrectionStore)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionStore");

	this->ptrHeadCorrectionInitOpticalParameters = (PrlHeadCorrectionInitOpticalParameters)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionInitOpticalParameters");
	this->ptrHeadCorrectionGetOpticalParameters = (PrlHeadCorrectionGetOpticalParameters)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionGetOpticalParameters");
	this->ptrHeadCorrectionGetHash = (PrlHeadCorrectionGetHash)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionGetHash");
	this->ptrHeadCorrectionSelectCorrection = (PrlHeadCorrectionSelectCorrection)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionSelectCorrection");
	this->ptrHeadCorrectionGetSelectedCorrection = (PrlHeadCorrectionGetSelectedCorrection)_loadRaylaseAddress(hLibrary, "rlHeadCorrectionGetSelectedCorrection");


	this->ptrMotfQuadratureDecoderInitConfig = (PrlMotfQuadratureDecoderInitConfig)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderInitConfig");
	this->ptrMotfQuadratureDecoderLoadConfig = (PrlMotfQuadratureDecoderLoadConfig)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderLoadConfig");
	this->ptrMotfQuadratureDecoderStoreConfig = (PrlMotfQuadratureDecoderStoreConfig)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderStoreConfig");
	this->ptrMotfQuadratureDecoderGetConfig = (PrlMotfQuadratureDecoderGetConfig)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderGetConfig");
	this->ptrMotfQuadratureDecoderSetConfig = (PrlMotfQuadratureDecoderSetConfig)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderSetConfig");


	this->ptrMotfInitConfig = (PrlMotfInitConfig)_loadRaylaseAddress(hLibrary, "rlMotfInitConfig");
	this->ptrMotfGetConfig = (PrlMotfGetConfig)_loadRaylaseAddress(hLibrary, "rlMotfGetConfig");
	this->ptrMotfSetConfig = (PrlMotfSetConfig)_loadRaylaseAddress(hLibrary, "rlMotfSetConfig");
	this->ptrMotfLoadConfig = (PrlMotfLoadConfig)_loadRaylaseAddress(hLibrary, "rlMotfLoadConfig");
	this->ptrMotfStoreConfig = (PrlMotfStoreConfig)_loadRaylaseAddress(hLibrary, "rlMotfStoreConfig");
	this->ptrMotfConfigSetDirectionFromAngle = (PrlMotfConfigSetDirectionFromAngle)_loadRaylaseAddress(hLibrary, "rlMotfConfigSetDirectionFromAngle");
	this->ptrMotfQuadratureDecoderResetEncoderPosition = (PrlMotfQuadratureDecoderResetEncoderPosition)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderResetEncoderPosition");
	this->ptrMotfQuadratureDecoderReadEncoderPosition = (PrlMotfQuadratureDecoderReadEncoderPosition)_loadRaylaseAddress(hLibrary, "rlMotfQuadratureDecoderReadEncoderPosition");
	this->ptrMotfGetPendingParts = (PrlMotfGetPendingParts)_loadRaylaseAddress(hLibrary, "rlMotfGetPendingParts");
	this->ptrMotfResetState = (PrlMotfResetState)_loadRaylaseAddress(hLibrary, "rlMotfResetState");
	
	
	this->ptrSfioRead = (PrlSfioRead)_loadRaylaseAddress(hLibrary, "rlSfioRead");
	this->ptrSfioSpiInitConfig = (PrlSfioSpiInitConfig)_loadRaylaseAddress(hLibrary, "rlSfioSpiInitConfig");
	this->ptrSfioSpiLoadConfig = (PrlSfioSpiLoadConfig)_loadRaylaseAddress(hLibrary, "rlSfioSpiLoadConfig");
	this->ptrSfioSpiStoreConfig = (PrlSfioSpiStoreConfig)_loadRaylaseAddress(hLibrary, "rlSfioSpiStoreConfig");
	this->ptrSfioSpiGetConfig = (PrlSfioSpiGetConfig)_loadRaylaseAddress(hLibrary, "rlSfioSpiGetConfig");
	this->ptrSfioSpiSetConfig = (PrlSfioSpiSetConfig)_loadRaylaseAddress(hLibrary, "rlSfioSpiSetConfig");
	this->ptrSfioSpiTransmit = (PrlSfioSpiTransmit)_loadRaylaseAddress(hLibrary, "rlSfioSpiTransmit");
	this->ptrSfioSpiTransceive = (PrlSfioSpiTransceive)_loadRaylaseAddress(hLibrary, "rlSfioSpiTransceive");
	this->ptrSfioSpiReceive = (PrlSfioSpiReceive)_loadRaylaseAddress(hLibrary, "rlSfioSpiReceive");
	this->ptrSfioSpiWaitForActiveTransfersDone = (PrlSfioSpiWaitForActiveTransfersDone)_loadRaylaseAddress(hLibrary, "rlSfioSpiWaitForActiveTransfersDone");

	this->ptrPowerCorrectionCreate = (PrlPowerCorrectionCreate)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionCreate");
	this->ptrPowerCorrectionSet = (PrlPowerCorrectionSet)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionSet");
	this->ptrPowerCorrectionGet = (PrlPowerCorrectionGet)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionGet");
	this->ptrPowerCorrectionLoad = (PrlPowerCorrectionLoad)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionLoad");
	this->ptrPowerCorrectionStore = (PrlPowerCorrectionStore)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionStore");
	this->ptrPowerCorrectionChangeValue = (PrlPowerCorrectionChangeValue)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionChangeValue");
	this->ptrPowerCorrectionGetOpticalParameters = (PrlPowerCorrectionGetOpticalParameters)_loadRaylaseAddress(hLibrary, "rlPowerCorrectionGetOpticalParameters");

	this->ptrScannerCommunicationTransceiveEnhanced = (PrlScannerCommunicationTransceiveEnhanced)_loadRaylaseAddress(hLibrary, "rlScannerCommunicationTransceiveEnhanced");
	this->ptrScannerCommunicationTransmitEnhanced = (PrlScannerCommunicationTransmitEnhanced)_loadRaylaseAddress(hLibrary, "rlScannerCommunicationTransmitEnhanced");
	this->ptrScannerCommunicationReceiveEnhanced = (PrlScannerCommunicationReceiveEnhanced)_loadRaylaseAddress(hLibrary, "rlScannerCommunicationReceiveEnhanced");
	this->ptrScannerCommunicationTransceiveRaylase = (PrlScannerCommunicationTransceiveRaylase)_loadRaylaseAddress(hLibrary, "rlScannerCommunicationTransceiveRaylase");
	this->ptrScannerCommunicationTransmitRaylase = (PrlScannerCommunicationTransmitRaylase)_loadRaylaseAddress(hLibrary, "rlScannerCommunicationTransmitRaylase");

	this->ptrVariableBooleanCreate = (PrlVariableBooleanCreate)_loadRaylaseAddress(hLibrary, "rlVariableBooleanCreate");
	this->ptrVariableBooleanDelete = (PrlVariableBooleanDelete)_loadRaylaseAddress(hLibrary, "rlVariableBooleanDelete");
	this->ptrVariableBooleanExists = (PrlVariableBooleanExists)_loadRaylaseAddress(hLibrary, "rlVariableBooleanExists");
	this->ptrVariableBooleanLoad = (PrlVariableBooleanLoad)_loadRaylaseAddress(hLibrary, "rlVariableBooleanLoad");
	this->ptrVariableBooleanStore = (PrlVariableBooleanStore)_loadRaylaseAddress(hLibrary, "rlVariableBooleanStore");
	this->ptrVariableBooleanGetValue = (PrlVariableBooleanGetValue)_loadRaylaseAddress(hLibrary, "rlVariableBooleanGetValue");
	this->ptrVariableBooleanSetValue = (PrlVariableBooleanSetValue)_loadRaylaseAddress(hLibrary, "rlVariableBooleanSetValue");
	this->ptrVariableBooleanToggleValue = (PrlVariableBooleanToggleValue)_loadRaylaseAddress(hLibrary, "rlVariableBooleanToggleValue");
	this->ptrVariableIntegerCreate = (PrlVariableIntegerCreate)_loadRaylaseAddress(hLibrary, "rlVariableIntegerCreate");
	this->ptrVariableIntegerDelete = (PrlVariableIntegerDelete)_loadRaylaseAddress(hLibrary, "rlVariableIntegerDelete");
	this->ptrVariableIntegerExists = (PrlVariableIntegerExists)_loadRaylaseAddress(hLibrary, "rlVariableIntegerExists");
	this->ptrVariableIntegerLoad = (PrlVariableIntegerLoad)_loadRaylaseAddress(hLibrary, "rlVariableIntegerLoad");
	this->ptrVariableIntegerStore = (PrlVariableIntegerStore)_loadRaylaseAddress(hLibrary, "rlVariableIntegerStore");
	
	this->ptrVariableIntegerGetValue = (PrlVariableIntegerGetValue)_loadRaylaseAddress(hLibrary, "rlVariableIntegerGetValue");
	this->ptrVariableIntegerSetValue = (PrlVariableIntegerSetValue)_loadRaylaseAddress(hLibrary, "rlVariableIntegerSetValue");
	this->ptrVariableIntegerIncrementValue = (PrlVariableIntegerIncrementValue)_loadRaylaseAddress(hLibrary, "rlVariableIntegerIncrementValue");
	this->ptrVariableIntegerDecrementValue = (PrlVariableIntegerDecrementValue)_loadRaylaseAddress(hLibrary, "rlVariableIntegerDecrementValue");
	this->ptrVariableMarkCounterCreate = (PrlVariableMarkCounterCreate)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterCreate");
	this->ptrVariableMarkCounterDelete = (PrlVariableMarkCounterDelete)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterDelete");
	this->ptrVariableMarkCounterExists = (PrlVariableMarkCounterExists)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterExists");
	this->ptrVariableMarkCounterLoad = (PrlVariableMarkCounterLoad)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterLoad");
	this->ptrVariableMarkCounterStore = (PrlVariableMarkCounterStore)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterStore");
	this->ptrVariableMarkCounterGetValue = (PrlVariableMarkCounterGetValue)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterGetValue");
	this->ptrVariableMarkCounterSetValue = (PrlVariableMarkCounterSetValue)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterSetValue");
	this->ptrVariableMarkCounterGetAndResetValue = (PrlVariableMarkCounterGetAndResetValue)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterGetAndResetValue");
	this->ptrVariableMarkCounterGetMarkTimeFormat = (PrlVariableMarkCounterGetMarkTimeFormat)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterGetMarkTimeFormat");
	this->ptrVariableMarkCounterSetMarkTimeFormat = (PrlVariableMarkCounterSetMarkTimeFormat)_loadRaylaseAddress(hLibrary, "rlVariableMarkCounterSetMarkTimeFormat");

	m_LibraryHandle = (void*) hLibrary;
}


CRaylaseSDK::~CRaylaseSDK()
{

	if (m_LibraryHandle != nullptr) {
#ifdef _WIN32
		HMODULE hModule = (HMODULE)m_LibraryHandle;
		FreeLibrary(hModule);
#else // _WIN32
		dlclose(m_LibraryHandle);
#endif // _WIN32

		m_LibraryHandle = nullptr;
		resetFunctionPtrs();
	}
}


void CRaylaseSDK::checkError(int32_t statusCode, const std::string& sDebugMessage)
{
		if (statusCode != 0) {
		std::string sErrorMessage = "Raylase Error " + std::to_string(statusCode) + " (";
		if ((ptrGetLastErrorLen != nullptr) && (ptrGetLastError != nullptr)) {
			int32_t nLength = ptrGetLastErrorLen();
			if (nLength > 0) {
				std::vector<char> messageBuffer;
				messageBuffer.resize((size_t)nLength + 2);

				ptrGetLastError(messageBuffer.data(), nLength + 1);
				messageBuffer.at(nLength) = 0;

				sErrorMessage += std::string(messageBuffer.data());
			}
			else {
				sErrorMessage += "<unknown error>";
			}

		}
		else {
			sErrorMessage += "<unknown error>";
		}

		if (!sDebugMessage.empty ())
			sErrorMessage += " - " + sDebugMessage;
		sErrorMessage += ")";

		throw std::runtime_error(sErrorMessage);
	}
}

void CRaylaseSDK::logJournal(const std::string& sLine)
{
	// TODO
}

void CRaylaseSDK::resetFunctionPtrs()
{

	ptrConnect = nullptr;
	ptrIsConnected = nullptr;
	ptrDisconnect = nullptr;
	ptrSystemResetToDefaults = nullptr;
	ptrLaserLaserOn = nullptr;
	ptrLaserLaserOff = nullptr;
	ptrLaserIsPilotEnabled = nullptr;
	ptrLaserEnablePilot = nullptr;
	ptrLaserIsLaserArmed = nullptr;
	ptrLaserArmLaser = nullptr;
	ptrLaserReadLaserStatus = nullptr;
	ptrListAllocate = nullptr;
	ptrListReleaseHandle = nullptr;
	ptrListAppendLaserOn = nullptr;
	ptrListAppendLaserOff = nullptr;
	ptrListAppendPower = nullptr;
	ptrListAppendJumpSpeed = nullptr;
	ptrListAppendMarkSpeed = nullptr;
	ptrListAppendJumpDelay = nullptr;
	ptrListAppendMinJumpDelay = nullptr;
	ptrListAppendMarkDelay = nullptr;
	ptrListAppendPolyDelay = nullptr;
	ptrListAppendLaserOnDelay = nullptr;
	ptrListAppendLaserOffDelay = nullptr;
	ptrListAppendJumpAbs2D = nullptr;
	ptrListAppendMarkAbs2D = nullptr;
	ptrListSet = nullptr;
	ptrListExecute = nullptr;
	ptrListWaitForListIdle = nullptr;
	ptrListWaitForListDone = nullptr;
	ptrListDelete = nullptr;
	ptrGetLastError = nullptr;
	ptrGetLastErrorLen = nullptr;
	ptrEnableCommandLogging = nullptr;
	ptrDisableCommandLogging = nullptr;
	ptrSetTimeout = nullptr;
	ptrGetTimeout = nullptr;
	ptrConfigurationBeginUpdate = nullptr;
	ptrConfigurationEndUpdate = nullptr;
	ptrConfigurationAbortUpdate = nullptr;
	ptrConfigurationIsUpdateActive = nullptr;

	ptrGpioGetPinCount = nullptr;
	ptrGpioGetGroupCount = nullptr;
	ptrGpioGetSupportedIOLevels = nullptr;
	ptrGpioGetSupportedDirections = nullptr;
	ptrGpioGetIOLevel = nullptr;
	ptrGpioSetIOLevel = nullptr;
	ptrGpioGetDirections = nullptr;
	ptrGpioSetDirections = nullptr;
	ptrGpioGetPolarities = nullptr;
	ptrGpioSetPolarities = nullptr;
	ptrGpioGetWriteDelay = nullptr;
	ptrGpioSetWriteDelay = nullptr;
	ptrGpioGetWriteWidth = nullptr;
	ptrGpioSetWriteWidth = nullptr;
	ptrGpioRead = nullptr;
	ptrGpioGetOutput = nullptr;
	ptrGpioWrite = nullptr;
	ptrSystemInitConfig = nullptr;
	ptrSystemLoadConfig = nullptr;
	ptrSystemStoreConfig = nullptr;
	ptrSystemGetConfig = nullptr;
	ptrSystemSetConfig = nullptr;
	ptrSystemGetFirmwareVersion = nullptr;
	ptrSystemGetFirmwareProductVersion = nullptr;
	ptrSystemGetFPGAVersion = nullptr;
	ptrSystemGetOSVersion = nullptr;
	ptrSystemGetCardSerialNumber = nullptr;
	ptrSystemGetConnectionLimit = nullptr;
	ptrSystemResetOperatingState = nullptr;
	ptrSystemGetOperatingState = nullptr;
	ptrSerialPortInitConfig = nullptr;
	ptrSerialPortLoadConfig = nullptr;
	ptrSerialPortStoreConfig = nullptr;
	ptrSerialPortGetConfig = nullptr;
	ptrSerialPortSetConfig = nullptr;
	ptrSerialPortTransmitString = nullptr;
	ptrSerialPortTransmitLine = nullptr;
	ptrSerialPortTransmitBytes = nullptr;
	ptrSerialPortTryVerifyResponseString = nullptr;
	ptrSerialPortTryVerifyResponseLine = nullptr;
	ptrSerialPortTryVerifyResponseBytes = nullptr;
	ptrSerialPortReceiveString = nullptr;
	ptrSerialPortReceiveLine = nullptr;
	ptrSerialPortReceiveBytes = nullptr;
	ptrLaserInitConfig = nullptr;
	ptrLaserLoadConfig = nullptr;
	ptrLaserStoreConfig = nullptr;
	ptrLaserGetConfig = nullptr;
	ptrLaserSetConfig = nullptr;
	ptrLaserCreatePowerCalibration = nullptr;
	ptrLaserLoadPowerCalibration = nullptr;
	ptrLaserStorePowerCalibration = nullptr;
	ptrLaserGetPowerCalibration = nullptr;
	ptrLaserSetPowerCalibration = nullptr;
	ptrLaserPowerCalibrationSet = nullptr;
	ptrLaserPowerCalibrationGet = nullptr;
	ptrLaserGetPowerScale = nullptr;
	ptrLaserSetPowerScale = nullptr;
	ptrScannerInitConfig = nullptr;
	ptrScannerSetConfig = nullptr;
	ptrScannerGetConfig = nullptr;
	ptrScannerLoadConfig = nullptr;
	ptrScannerStoreConfig = nullptr;
	ptrScannerGetNominalPosition = nullptr;
	ptrScannerReadPositions = nullptr;
	ptrScannerReadLastPositions = nullptr;
	ptrScannerHeadFormatGetHeadCount = nullptr;
	ptrScannerHeadFormatGetAxisCount = nullptr;
	ptrScannerHeadFormatGetDimensions = nullptr;
	ptrScannerHeadFormatGetAxes = nullptr;
	ptrScannerHeadFormatGetIndexOfAxis = nullptr;
	ptrScannerHeadFormatHasOpticalZoom = nullptr;
	ptrScannerHeadFormatHasAuxiliary = nullptr;
	ptrScannerHeadFormatIsExtended = nullptr;
	ptrScannerGetHeadBias = nullptr;
	ptrScannerSetHeadBias = nullptr;

	ptrAdapterBoardGetConfig = nullptr;
	ptrAdapterBoardSetConfig = nullptr;
	ptrAdapterBoardSetConfigByName = nullptr;
	ptrAdapterBoardEnable = nullptr;
	ptrAdapterBoardEnabled = nullptr;
	ptrAdapterBoardSupportsAutoConfiguration = nullptr;
	ptrAdapterBoardValid = nullptr;
	ptrAdapterBoardGetAvailableAdapterBoardNames = nullptr;
	ptrAdapterBoardGetCurrentAdapterBoardName = nullptr;

	ptrProcessInitVariables = nullptr;
	ptrProcessLoadVariables = nullptr;
	ptrProcessStoreVariables = nullptr;
	ptrProcessGetVariables = nullptr;
	ptrProcessSetVariables = nullptr;

	ptrProcessGetSpeedScale = nullptr;
	ptrProcessSetSpeedScale = nullptr;
	ptrProcessGetTransformation = nullptr;
	ptrProcessSetTransformation = nullptr;
	ptrProcessWobbleSetCircle = nullptr;
	ptrProcessWobbleSetEight = nullptr;

	ptrHeadCorrectionExists = nullptr;
	ptrHeadCorrectionDelete = nullptr;
	ptrHeadCorrectionGet = nullptr;
	ptrHeadCorrectionSet = nullptr;
	ptrHeadCorrectionLoad = nullptr;
	ptrHeadCorrectionStore = nullptr;

	ptrHeadCorrectionInitOpticalParameters = nullptr;
	ptrHeadCorrectionGetOpticalParameters = nullptr;
	ptrHeadCorrectionGetHash = nullptr;
	ptrHeadCorrectionSelectCorrection = nullptr;
	ptrHeadCorrectionGetSelectedCorrection = nullptr;

	ptrMotfQuadratureDecoderInitConfig = nullptr;
	ptrMotfQuadratureDecoderLoadConfig = nullptr;
	ptrMotfQuadratureDecoderStoreConfig = nullptr;
	ptrMotfQuadratureDecoderGetConfig = nullptr;
	ptrMotfQuadratureDecoderSetConfig = nullptr;

	ptrMotfInitConfig = nullptr;
	ptrMotfGetConfig = nullptr;
	ptrMotfSetConfig = nullptr;
	ptrMotfLoadConfig = nullptr;
	ptrMotfStoreConfig = nullptr;
	ptrMotfConfigSetDirectionFromAngle = nullptr;
	ptrMotfQuadratureDecoderResetEncoderPosition = nullptr;
	ptrMotfQuadratureDecoderReadEncoderPosition = nullptr;
	ptrMotfGetPendingParts = nullptr;
	ptrMotfResetState = nullptr;

	ptrSfioRead = nullptr;
	ptrSfioSpiInitConfig = nullptr;
	ptrSfioSpiLoadConfig = nullptr;
	ptrSfioSpiStoreConfig = nullptr;
	ptrSfioSpiGetConfig = nullptr;
	ptrSfioSpiSetConfig = nullptr;
	ptrSfioSpiTransmit = nullptr;
	ptrSfioSpiTransceive = nullptr;
	ptrSfioSpiReceive = nullptr;
	ptrSfioSpiWaitForActiveTransfersDone = nullptr;

	ptrPowerCorrectionCreate = nullptr;
	ptrPowerCorrectionSet = nullptr;
	ptrPowerCorrectionGet = nullptr;
	ptrPowerCorrectionLoad = nullptr;
	ptrPowerCorrectionStore = nullptr;

	ptrPowerCorrectionChangeValue = nullptr;

	ptrPowerCorrectionGetOpticalParameters = nullptr;

	ptrScannerCommunicationTransceiveEnhanced = nullptr;
	ptrScannerCommunicationTransmitEnhanced = nullptr;
	ptrScannerCommunicationReceiveEnhanced = nullptr;
	ptrScannerCommunicationTransceiveRaylase = nullptr;
	ptrScannerCommunicationTransmitRaylase = nullptr;

	ptrVariableBooleanCreate = nullptr;
	ptrVariableBooleanDelete = nullptr;
	ptrVariableBooleanExists = nullptr;
	ptrVariableBooleanLoad = nullptr;
	ptrVariableBooleanStore = nullptr;

	ptrVariableBooleanGetValue = nullptr;
	ptrVariableBooleanSetValue = nullptr;
	ptrVariableBooleanToggleValue = nullptr;

	ptrVariableIntegerCreate = nullptr;
	ptrVariableIntegerDelete = nullptr;
	ptrVariableIntegerExists = nullptr;
	ptrVariableIntegerLoad = nullptr;
	ptrVariableIntegerStore = nullptr;

	ptrVariableIntegerGetValue = nullptr;
	ptrVariableIntegerSetValue = nullptr;
	ptrVariableIntegerIncrementValue = nullptr;
	ptrVariableIntegerDecrementValue = nullptr;

	ptrVariableMarkCounterCreate = nullptr;
	ptrVariableMarkCounterDelete = nullptr;
	ptrVariableMarkCounterExists = nullptr;
	ptrVariableMarkCounterLoad = nullptr;
	ptrVariableMarkCounterStore = nullptr;

	ptrVariableMarkCounterGetValue = nullptr;
	ptrVariableMarkCounterSetValue = nullptr;
	ptrVariableMarkCounterGetAndResetValue = nullptr;

	ptrVariableMarkCounterGetMarkTimeFormat = nullptr;
	ptrVariableMarkCounterSetMarkTimeFormat = nullptr;
}

rlHandle CRaylaseSDK::rlConnect(const char* pszAddress, int32_t nPort)
{
	if (pszAddress == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	if (m_bEnableJournal) {
		logJournal("rlConnect (" + std::string (pszAddress) + ", " + std::to_string (nPort) + ");");
	}

	return ptrConnect(pszAddress, nPort);
	
}

rlResult CRaylaseSDK::rlIsConnected(rlHandle handle, bool* pbIsConnected)
{
	if (m_bEnableJournal) {
		logJournal("rlIsConnected (" + std::to_string(handle) + ", &bIsConnected);");
	}

	return ptrIsConnected(handle, pbIsConnected);
	

}

rlResult CRaylaseSDK::rlDisconnect(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlDisconnect (" + std::to_string(handle) + "); ");
	}

	return ptrDisconnect(handle);
	
}

rlResult CRaylaseSDK::rlSystemResetToDefaults(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlSystemResetToDefaults (" + std::to_string(handle) + "); ");
	}

	return ptrSystemResetToDefaults(handle);
	
}

rlResult CRaylaseSDK::rlLaserLaserOn(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserLaserOn (" + std::to_string(handle) + "); ");
	}

	return ptrLaserLaserOn(handle);

}

rlResult CRaylaseSDK::rlLaserLaserOff(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserLaserOff (" + std::to_string(handle) + "); ");
	}

	return ptrLaserLaserOff(handle);

}

rlResult CRaylaseSDK::rlLaserIsPilotEnabled(rlHandle handle, bool& enabled)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserIsPilotEnabled (" + std::to_string(handle) + ", bEnabled); ");
	}

	return ptrLaserIsPilotEnabled(handle, enabled);
	
}

rlResult CRaylaseSDK::rlLaserEnablePilot(rlHandle handle, bool enable)
{
	if (m_bEnableJournal) {
		if (enable) 
			logJournal("rlLaserEnablePilot (" + std::to_string(handle) + ", true); ");
		else
			logJournal("rlLaserEnablePilot (" + std::to_string(handle) + ", false); ");
	}

	return ptrLaserEnablePilot(handle, enable);

}

rlResult CRaylaseSDK::rlLaserIsLaserArmed(rlHandle handle, bool& armed)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserEnablePilot (" + std::to_string(handle) + ", bArmed); ");
	}


	return ptrLaserEnablePilot(handle, armed);

}

rlResult CRaylaseSDK::rlLaserArmLaser(rlHandle handle, bool arm)
{
	if (m_bEnableJournal) {
		if (arm)
			logJournal("rlLaserArmLaser (" + std::to_string(handle) + ", true); ");
		else
			logJournal("rlLaserArmLaser (" + std::to_string(handle) + ", false); ");
	}

	return ptrLaserArmLaser(handle, arm);

}

rlResult CRaylaseSDK::rlLaserReadLaserStatus(rlHandle handle, uint32_t& laserStatus)
{
	if (m_bEnableJournal) {
		logJournal("rlLaserReadLaserStatus (" + std::to_string(handle) + ", bLaserStatus); ");
	}

	return ptrLaserArmLaser(handle, laserStatus);
}


rlListHandle CRaylaseSDK::rlListAllocate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListAllocate (" + std::to_string(handle) + "); ");
	}

	return ptrListAllocate(handle);

}


rlResult CRaylaseSDK::rlListReleaseHandle(rlListHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListReleaseHandle (" + std::to_string(handle) + "); ");
	}

	return ptrListReleaseHandle(handle);
}

rlResult CRaylaseSDK::rlListAppendLaserOn(rlListHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOn (" + std::to_string(handle) + "); ");
	}

	return ptrListAppendLaserOn(handle);
}

rlResult CRaylaseSDK::rlListAppendLaserOff(rlListHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOn (" + std::to_string(handle) + "); ");
	}

	return ptrListAppendLaserOff(handle);
}

rlResult CRaylaseSDK::rlListAppendPower(rlListHandle handle, uint16_t nPower)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendPower (" + std::to_string(handle) + ", " + std::to_string (nPower) + "); ");
	}

	return ptrListAppendPower(handle, nPower);
}

rlResult CRaylaseSDK::rlListAppendJumpSpeed(rlListHandle handle, double jumpSpeed)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendJumpSpeed (" + std::to_string(handle) + ", " + std::to_string (jumpSpeed) + "); ");
	}

	return ptrListAppendJumpSpeed(handle, jumpSpeed);
}

rlResult CRaylaseSDK::rlListAppendMarkSpeed(rlListHandle handle, double markSpeed)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMarkSpeed (" + std::to_string(handle) + ", " + std::to_string(markSpeed) + "); ");
	}

	return ptrListAppendMarkSpeed(handle, markSpeed);
}

rlResult CRaylaseSDK::rlListAppendJumpDelay(rlListHandle handle, double jumpDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendJumpDelay (" + std::to_string(handle) + ", " + std::to_string(jumpDelay) + "); ");
	}

	return ptrListAppendJumpDelay(handle, jumpDelay);
}

rlResult CRaylaseSDK::rlListAppendMinJumpDelay(rlListHandle handle, double minJumpDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMinJumpDelay (" + std::to_string(handle) + ", " + std::to_string(minJumpDelay) + "); ");
	}

	return ptrListAppendMinJumpDelay(handle, minJumpDelay);
}

rlResult CRaylaseSDK::rlListAppendMarkDelay(rlListHandle handle, double markDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMarkDelay (" + std::to_string(handle) + ", " + std::to_string(markDelay) + "); ");
	}

	return ptrListAppendMarkDelay(handle, markDelay);
}

rlResult CRaylaseSDK::rlListAppendPolyDelay(rlListHandle handle, double polyDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendPolyDelay (" + std::to_string(handle) + ", " + std::to_string(polyDelay) + "); ");
	}

	return ptrListAppendPolyDelay(handle, polyDelay);

}

rlResult CRaylaseSDK::rlListAppendLaserOnDelay(rlListHandle handle, double laserOnDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOnDelay (" + std::to_string(handle) + ", " + std::to_string(laserOnDelay) + "); ");
	}

	return ptrListAppendLaserOnDelay(handle, laserOnDelay);

}

rlResult CRaylaseSDK::rlListAppendLaserOffDelay(rlListHandle handle, double laserOffDelay)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendLaserOffDelay (" + std::to_string(handle) + ", " + std::to_string(laserOffDelay) + "); ");
	}

	return ptrListAppendLaserOffDelay(handle, laserOffDelay);
}

rlResult CRaylaseSDK::rlListAppendJumpAbs2D(rlListHandle handle, double x, double y)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendJumpAbs2D (" + std::to_string(handle) + ", " + std::to_string(x) + ", " + std::to_string(y) + "); ");
	}

	return ptrListAppendJumpAbs2D(handle, x, y);
}

rlResult CRaylaseSDK::rlListAppendMarkAbs2D(rlListHandle handle, double x, double y)
{
	if (m_bEnableJournal) {
		logJournal("rlListAppendMarkAbs2D (" + std::to_string(handle) + ", " + std::to_string(x) + ", " + std::to_string(y) + "); ");
	}

	return ptrListAppendMarkAbs2D(handle, x, y);

}

rlResult CRaylaseSDK::rlListSet(rlHandle handle, int32_t listID, rlListHandle listHandle, bool append, int32_t executionLimit)
{
	if (m_bEnableJournal) {
		logJournal("rlListSet (" + std::to_string(handle) + ", " + std::to_string(listID) + ", " + std::to_string(listHandle) + ", " + std::to_string((int)append) + ", " + std::to_string(executionLimit) + "); ");
	}

	return ptrListSet(handle, listID, listHandle, append, executionLimit);

}

rlResult CRaylaseSDK::rlListExecute(rlHandle handle, int32_t listID)
{
	if (m_bEnableJournal) {
		logJournal("rlListExecute (" + std::to_string(handle) + ", " + std::to_string(listID) + "); ");
	}

	return ptrListExecute(handle, listID);
}

rlResult CRaylaseSDK::rlListWaitForListIdle(rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID)
{
	if (m_bEnableJournal) {
		logJournal("rlListWaitForListIdle (" + std::to_string(handle) + ", " + std::to_string(timeOutInMilliseconds) + ", bSuccessful, nListID); ");
	}

	return ptrListWaitForListIdle(handle, timeOutInMilliseconds, successful, listID);

}

rlResult CRaylaseSDK::rlListWaitForListDone(rlHandle handle, int32_t timeOutInMilliseconds, bool& successful, int32_t& listID)
{
	if (m_bEnableJournal) {
		logJournal("rlListWaitForListDone (" + std::to_string(handle) + ", " + std::to_string(timeOutInMilliseconds) + ", bSuccessful, nListID); ");
	}

	return ptrListWaitForListDone(handle, timeOutInMilliseconds, successful, listID);

}

rlResult CRaylaseSDK::rlListDelete(rlHandle handle, int32_t listID, bool bFromDisk)
{
	if (m_bEnableJournal) {
		logJournal("rlListDelete (" + std::to_string(handle) + ", " + std::to_string(listID) + ", " + std::to_string((int)bFromDisk) + "); ");
	}

	return ptrListDelete(handle, listID, bFromDisk);

}


rlResult CRaylaseSDK::rlEnableCommandLogging(rlHandle handle, const char* pszPath, int32_t truncateArguments)
{
	if (pszPath == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	if (m_bEnableJournal) {
		logJournal("rlEnableCommandLogging (" + std::to_string(handle) + ", " + std::string(pszPath) + ", " + std::to_string (truncateArguments) + "); ");
	}

	return ptrEnableCommandLogging(handle, pszPath, truncateArguments);

}

rlResult CRaylaseSDK::rlDisableCommandLogging(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlDisableCommandLogging (" + std::to_string(handle) + "); ");
	}

	return ptrDisableCommandLogging(handle);

}

rlResult CRaylaseSDK::rlSetTimeout(rlHandle handle, uint32_t nTimeOut)
{
	if (m_bEnableJournal) {
		logJournal("rlSetTimeout (" + std::to_string(handle) + ", " + std::to_string(nTimeOut) + "); ");
	}


	return ptrSetTimeout(handle, nTimeOut);
}

rlResult CRaylaseSDK::rlGetTimeout(rlHandle handle, uint32_t* pnTimeOut)
{
	if (m_bEnableJournal) {
		logJournal("rlGetTimeout (" + std::to_string(handle) + ", &nTimeOut); ");
	}

	return ptrGetTimeout(handle, pnTimeOut);

}

rlResult CRaylaseSDK::rlConfigurationBeginUpdate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationBeginUpdate (" + std::to_string(handle) + "); ");
	}

	return ptrConfigurationBeginUpdate(handle);

}

rlResult CRaylaseSDK::rlConfigurationEndUpdate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationEndUpdate (" + std::to_string(handle) + "); ");
	}

	return ptrConfigurationEndUpdate(handle);
}

rlResult CRaylaseSDK::rlConfigurationAbortUpdate(rlHandle handle)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationAbortUpdate (" + std::to_string(handle) + "); ");
	}

	return ptrConfigurationAbortUpdate(handle);
}

rlResult CRaylaseSDK::rlConfigurationIsUpdateActive(rlHandle handle, uint32_t* pbIsActive)
{
	if (m_bEnableJournal) {
		logJournal("rlConfigurationIsUpdateActive (" + std::to_string(handle) + ", &bIsActive); ");
	}

	return ptrConfigurationIsUpdateActive(handle, pbIsActive);
}

