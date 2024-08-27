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

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlEnableCommandLogging) (rlHandle handle, const char* pszPath, int32_t truncateArguments);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlDisableCommandLogging) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlSetTimeout) (rlHandle handle, uint32_t nTimeOut);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlGetTimeout) (rlHandle handle, uint32_t* pnTimeOut);

		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationBeginUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationEndUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationAbortUpdate) (rlHandle handle);
		typedef rlResult(RAYLASE_CALLINGCONVENTION* PrlConfigurationIsUpdateActive) (rlHandle handle, uint32_t * pbIsActive);


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
			rlResult rlLaserReadLaserStatus (rlHandle handle, uint32_t& laserStatus);

			rlListHandle rlListAllocate (rlHandle handle);

			rlResult rlListReleaseHandle (rlListHandle handle);
			rlResult rlListAppendLaserOn (rlListHandle handle);
			rlResult rlListAppendLaserOff (rlListHandle handle);
			rlResult rlListAppendPower (rlListHandle handle, uint16_t nPower);
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

			rlResult rlEnableCommandLogging (rlHandle handle, const char* pszPath, int32_t truncateArguments);
			rlResult rlDisableCommandLogging (rlHandle handle);
			rlResult rlSetTimeout (rlHandle handle, uint32_t nTimeOut);
			rlResult rlGetTimeout (rlHandle handle, uint32_t* pnTimeOut);

			rlResult rlConfigurationBeginUpdate (rlHandle handle);
			rlResult rlConfigurationEndUpdate (rlHandle handle);
			rlResult rlConfigurationAbortUpdate (rlHandle handle);
			rlResult rlConfigurationIsUpdateActive (rlHandle handle, uint32_t* pbIsActive);

		};


		typedef std::shared_ptr<CRaylaseSDK> PRaylaseSDK;

	} 
} 

#endif // __LIBMCDRIVER_RAYLASE_SDK
