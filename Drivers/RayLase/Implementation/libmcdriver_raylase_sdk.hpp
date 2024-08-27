/*++

Copyright (C) 2022 Autodesk Inc.

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
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlListAppendPower) (rlListHandle handle, uint16_t nPower);
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

		typedef void(RAYLASE_CALLINGCONVENTION* PrlGetLastError) (char * pBuffer, int32_t nBufferSize);
		typedef int32_t(RAYLASE_CALLINGCONVENTION* PrlGetLastErrorLen) ();

		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlEnableCommandLogging) (rlHandle handle, const char* pszPath, int32_t truncateArguments);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlDisableCommandLogging) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlSetTimeout) (rlHandle handle, uint32_t nTimeOut);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlGetTimeout) (rlHandle handle, uint32_t* pnTimeOut);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlConfigurationBeginUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlConfigurationEndUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlConfigurationAbortUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* rlConfigurationIsUpdateActive) (rlHandle handle, uint32_t * pbIsActive);


		class CRaylaseSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			PrlConnect rlConnect = nullptr;
			PrlIsConnected rlIsConnected = nullptr;
			PrlDisconnect rlDisconnect = nullptr;
			PrlSystemResetToDefaults rlSystemResetToDefaults = nullptr;
			PrlLaserLaserOn rlLaserLaserOn = nullptr;
			PrlLaserLaserOff rlLaserLaserOff = nullptr;
			PrlLaserIsPilotEnabled rlLaserIsPilotEnabled = nullptr;
			PrlLaserEnablePilot rlLaserEnablePilot = nullptr;
			PrlLaserIsLaserArmed rlLaserIsLaserArmed = nullptr;
			PrlLaserArmLaser rlLaserArmLaser = nullptr;
			PrlLaserReadLaserStatus rlLaserReadLaserStatus = nullptr;
			PrlListAllocate rlListAllocate = nullptr;
			PrlListReleaseHandle rlListReleaseHandle = nullptr;
			PrlListAppendLaserOn rlListAppendLaserOn = nullptr;
			PrlListAppendLaserOff rlListAppendLaserOff = nullptr;
			PrlListAppendPower rlListAppendPower = nullptr;
			PrlListAppendJumpSpeed rlListAppendJumpSpeed = nullptr;
			PrlListAppendMarkSpeed rlListAppendMarkSpeed = nullptr;
			PrlListAppendJumpDelay rlListAppendJumpDelay = nullptr;
			PrlListAppendMinJumpDelay rlListAppendMinJumpDelay = nullptr;
			PrlListAppendMarkDelay rlListAppendMarkDelay = nullptr;
			PrlListAppendPolyDelay rlListAppendPolyDelay = nullptr;
			PrlListAppendLaserOnDelay rlListAppendLaserOnDelay = nullptr;
			PrlListAppendLaserOffDelay rlListAppendLaserOffDelay = nullptr;
			PrlListAppendJumpAbs2D rlListAppendJumpAbs2D = nullptr;
			PrlListAppendMarkAbs2D rlListAppendMarkAbs2D = nullptr;
			PrlListSet rlListSet = nullptr;
			PrlListExecute rlListExecute = nullptr;
			PrlListWaitForListIdle rlListWaitForListIdle = nullptr;
			PrlListWaitForListDone rlListWaitForListDone = nullptr;
			PrlListDelete rlListDelete = nullptr;

			PrlGetLastError rlGetLastError = nullptr;
			PrlGetLastErrorLen rlGetLastErrorLen = nullptr;

			CRaylaseSDK(const std::string & sDLLNameUTF8);
			~CRaylaseSDK();

			void checkError(int32_t statusCode, const std::string & sDebugMessage = "");

		};


		typedef std::shared_ptr<CRaylaseSDK> PRaylaseSDK;

	} 
} 

#endif // __LIBMCDRIVER_RAYLASE_SDK
