/*++

Copyright (C) 2020 Autodesk Inc.

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


#ifndef __LIBMCDRIVER_TML_SDK
#define __LIBMCDRIVER_TML_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define TML_CALLINGCONVENTION __stdcall

#else

#define TML_CALLINGCONVENTION 

#endif


#define TML_MAXNAMELENGTH 4096


namespace LibMCDriver_TML {
	namespace Impl {


		typedef int32_t tmlInt;
		typedef int32_t tmlBool;
		typedef int8_t tmlByte;
		typedef int16_t tmlShort;
		typedef uint16_t tmlWord;
		typedef const char* tmlLPCSTR;

		typedef tmlInt (TML_CALLINGCONVENTION* PTS_LoadSetup) (const char * pszSetupPath);
		typedef tmlInt (TML_CALLINGCONVENTION* PTS_OpenChannel) (const char * pszDeviceName, uint8_t nType, uint8_t nHostID, uint32_t nBaudRate);		
		typedef void(TML_CALLINGCONVENTION* PTS_CloseChannel) (tmlInt nFileDescriptor);		
		typedef tmlBool(TML_CALLINGCONVENTION* PTS_DriveInitialisation) ();
		typedef tmlBool(TML_CALLINGCONVENTION* PTS_SetupAxis) (tmlByte nAxisID, tmlInt nSetupID);
		typedef tmlBool(TML_CALLINGCONVENTION* PTS_SelectAxis) (tmlByte nAxisID);
		typedef tmlBool(TML_CALLINGCONVENTION* PTS_ResetFault) ();
		typedef tmlBool(TML_CALLINGCONVENTION* PTS_Power) (tmlBool bEnable);
		typedef tmlBool(TML_CALLINGCONVENTION* PTS_ReadStatus) (tmlShort nSelectionIndex, tmlWord & nStatus);
		typedef tmlLPCSTR(TML_CALLINGCONVENTION* PTS_GetLastErrorText) ();
		
		
		class CTMLSDK_DLLDirectoryCache {
		private:
#ifdef _WIN32
			std::wstring m_sCachedDLLDirectoryW;
#endif // _WIN32

		public:
			CTMLSDK_DLLDirectoryCache();
			virtual ~CTMLSDK_DLLDirectoryCache();

		};

		typedef std::shared_ptr<CTMLSDK_DLLDirectoryCache> PTMLSDK_DLLDirectoryCache;

		class CTMLSDK {
		private:
			std::wstring m_sDLLDirectoryW;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();

			PTMLSDK_DLLDirectoryCache cacheDllDirectory();
		public:

			PTS_LoadSetup TS_LoadSetup = nullptr;
			PTS_OpenChannel TS_OpenChannel = nullptr;
			PTS_CloseChannel TS_CloseChannel = nullptr;
			PTS_DriveInitialisation TS_DriveInitialisation = nullptr;
			PTS_SetupAxis TS_SetupAxis = nullptr;
			PTS_SelectAxis TS_SelectAxis = nullptr;
			PTS_ResetFault TS_ResetFault = nullptr;
			PTS_Power TS_Power = nullptr;
			PTS_ReadStatus TS_ReadStatus = nullptr;
			PTS_GetLastErrorText TS_GetLastErrorText = nullptr;

			CTMLSDK(const std::string & sDLLNameUTF8, const std::string & sDLLDirectoryUTF8);			
			virtual ~CTMLSDK();

			void checkError(bool bSuccess);
			void raiseLastError();

		};


		typedef std::shared_ptr<CTMLSDK> PTMLSDK;

	} 
} 

#endif // __LIBMCDRIVER_TML_SDK
