/*++

Copyright (C) 2025 Autodesk Inc.

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


#ifndef __LIBMCDRIVER_PYLON_SDK
#define __LIBMCDRIVER_PYLON_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define PYLON_CALLINGCONVENTION __cdecl

#else

#define PYLON_CALLINGCONVENTION 

#endif


#define PYLON_MAXFILENAME_LENGTH 260
#define PYLON_MAX_DEVICEINFO_LENGTH 64


namespace LibMCDriver_Pylon {
	namespace Impl {

		typedef void* libPylonDeviceInfoHandle;
		typedef void* libPylonDeviceHandle;
		typedef void* libPylonStreamBufferHandle;
		typedef int32_t pylonResult;
		typedef int8_t pylonBool;

		enum class ePylonGrabStatus : int32_t
		{
			pgsUndefined = -1,
			pgsIdle = 0,
			pgsQueued = 1,
			pgsGrabbed = 2,
			pgsCanceled = 3,
			pgsFailed = 4
		};

		enum class ePylonPayloadType : int32_t
		{
			pptUndefined = -1,
			pptImage = 0,
			pptRawData = 1,
			pptFile = 2,
			pptChunkData = 3,
			pptGenDC = 4,
			pptDeviceSpecific = 0x8000
		};


#pragma	pack(push, 8)

		typedef struct _sPylonGrabResult
		{
			void* m_pContext;
			libPylonStreamBufferHandle m_pHandle;
			void* m_pBuffer;

			ePylonGrabStatus m_Status;
			ePylonPayloadType m_PayloadType;
			int32_t m_PixelType;
			uint64_t m_TimeStamp;
			int32_t m_SizeX;
			int32_t m_SizeY;
			int32_t m_OffsetX;
			int32_t m_OffsetY;
			int32_t m_PaddingX;
			int32_t m_PaddingY;
			uint64_t m_PayloadSize;
			uint32_t m_nErrorCode;
			uint64_t m_BlockID;

		} sPylonGrabResult;

#pragma	pack(pop)


		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonInitialize) ();
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonTerminate) ();

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonEnumerateDevices) (size_t* pNumDevices);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonGigEEnumerateAllDevices) (size_t* pNumDevices);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonGetDeviceInfoHandle) (size_t nIndex, libPylonDeviceInfoHandle* ppDeviceInfoHandle);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceInfoGetNumProperties) (libPylonDeviceInfoHandle pDeviceInfoHandle, size_t * pNumProperties);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceInfoGetPropertyValueByIndex) (libPylonDeviceInfoHandle pDeviceInfoHandle, size_t nIndex, char* pValueBuffer, size_t* pBufLen);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceInfoGetPropertyName) (libPylonDeviceInfoHandle pDeviceInfoHandle, size_t nIndex, char* pNameBuffer, size_t* pBufLen);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonCreateDeviceByIndex) (size_t nIndex, libPylonDeviceHandle* ppDeviceHandle);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDestroyDevice) (libPylonDeviceHandle pDeviceHandle);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonIsDeviceAccessible) (size_t nIndex, int32_t accessMode, pylonBool * pbIsAccessible);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceOpen) (libPylonDeviceHandle pDeviceHandle, int32_t nAccessMode);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceClose) (libPylonDeviceHandle pDeviceHandle);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceIsOpen) (libPylonDeviceHandle pDeviceHandle, pylonBool * pbIsOpen);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceAccessMode) (libPylonDeviceHandle pDeviceHandle, int32_t * pnAccessMode);

		typedef pylonBool(PYLON_CALLINGCONVENTION* PPylonDeviceFeatureIsImplemented) (libPylonDeviceHandle pDeviceHandle, const char *pszName);
		typedef pylonBool(PYLON_CALLINGCONVENTION* PPylonDeviceFeatureIsAvailable) (libPylonDeviceHandle pDeviceHandle, const char* pszName);
		typedef pylonBool(PYLON_CALLINGCONVENTION* PPylonDeviceFeatureIsReadable) (libPylonDeviceHandle pDeviceHandle, const char* pszName);
		typedef pylonBool(PYLON_CALLINGCONVENTION* PPylonDeviceFeatureIsWritable) (libPylonDeviceHandle pDeviceHandle, const char* pszName);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceSetIntegerFeature) (libPylonDeviceHandle pDeviceHandle, const char* pszName, int64_t nValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetIntegerFeature) (libPylonDeviceHandle pDeviceHandle, const char* pszName, int64_t * pnValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetIntegerFeatureMin) (libPylonDeviceHandle pDeviceHandle, const char* pszName, int64_t* pnValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetIntegerFeatureMax) (libPylonDeviceHandle pDeviceHandle, const char* pszName, int64_t* pnValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetIntegerFeatureInc) (libPylonDeviceHandle pDeviceHandle, const char* pszName, int64_t* pnValue);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceSetFloatFeature) (libPylonDeviceHandle pDeviceHandle, const char* pszName, double dValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetFloatFeature) (libPylonDeviceHandle pDeviceHandle, const char* pszName, double * pdValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetFloatFeatureMin) (libPylonDeviceHandle pDeviceHandle, const char* pszName, double* pdValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetFloatFeatureMax) (libPylonDeviceHandle pDeviceHandle, const char* pszName, double* pdValue);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceSetBooleanFeature) (libPylonDeviceHandle pDeviceHandle, const char* pszName, pylonBool bValue);
		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGetBooleanFeature) (libPylonDeviceHandle pDeviceHandle, const char* pszName, pylonBool * pbValue);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PPylonDeviceGrabSingleFrame) (libPylonDeviceHandle pDeviceHandle, size_t nChannel, uint8_t * pBuffer, size_t nBufferSize, sPylonGrabResult * pGrabResult, pylonBool * pbReady, uint32_t nTimeOut);

		typedef pylonResult(PYLON_CALLINGCONVENTION* PGenApiGetLastErrorDetail) (char * pszBuffer, size_t * pnBufLen);


		class CLibPylonSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			PPylonInitialize PylonInitialize = nullptr;
			PPylonTerminate PylonTerminate = nullptr;
			PPylonEnumerateDevices PylonEnumerateDevices = nullptr;
			PPylonGigEEnumerateAllDevices PylonGigEEnumerateAllDevices = nullptr;
			PPylonGetDeviceInfoHandle PylonGetDeviceInfoHandle = nullptr;
			PPylonDeviceInfoGetNumProperties PylonDeviceInfoGetNumProperties = nullptr;
			PPylonDeviceInfoGetPropertyValueByIndex PylonDeviceInfoGetPropertyValueByIndex = nullptr;
			PPylonDeviceInfoGetPropertyName PylonDeviceInfoGetPropertyName = nullptr;
			PPylonCreateDeviceByIndex PylonCreateDeviceByIndex = nullptr;
			PPylonDestroyDevice PylonDestroyDevice = nullptr;
			PPylonIsDeviceAccessible PylonIsDeviceAccessible = nullptr;
			PPylonDeviceOpen PylonDeviceOpen = nullptr;
			PPylonDeviceClose PylonDeviceClose = nullptr;
			PPylonDeviceIsOpen PylonDeviceIsOpen = nullptr;
			PPylonDeviceAccessMode PylonDeviceAccessMode = nullptr;
			PPylonDeviceFeatureIsImplemented PylonDeviceFeatureIsImplemented = nullptr;
			PPylonDeviceFeatureIsAvailable PylonDeviceFeatureIsAvailable = nullptr;
			PPylonDeviceFeatureIsReadable PylonDeviceFeatureIsReadable = nullptr;
			PPylonDeviceFeatureIsWritable PylonDeviceFeatureIsWritable = nullptr;
			PPylonDeviceSetIntegerFeature PylonDeviceSetIntegerFeature = nullptr;
			PPylonDeviceGetIntegerFeature PylonDeviceGetIntegerFeature = nullptr;
			PPylonDeviceGetIntegerFeatureMin PylonDeviceGetIntegerFeatureMin = nullptr;
			PPylonDeviceGetIntegerFeatureMax PylonDeviceGetIntegerFeatureMax = nullptr;
			PPylonDeviceGetIntegerFeatureInc PylonDeviceGetIntegerFeatureInc = nullptr;
			PPylonDeviceSetFloatFeature PylonDeviceSetFloatFeature = nullptr;
			PPylonDeviceGetFloatFeature PylonDeviceGetFloatFeature = nullptr;
			PPylonDeviceGetFloatFeatureMin PylonDeviceGetFloatFeatureMin = nullptr;
			PPylonDeviceGetFloatFeatureMax PylonDeviceGetFloatFeatureMax = nullptr;
			PPylonDeviceSetBooleanFeature PylonDeviceSetBooleanFeature = nullptr;
			PPylonDeviceGetBooleanFeature PylonDeviceGetBooleanFeature = nullptr;
			PPylonDeviceGrabSingleFrame PylonDeviceGrabSingleFrame = nullptr;
			PGenApiGetLastErrorDetail GenApiGetLastErrorDetail = nullptr;

			CLibPylonSDK(const std::string & sDLLNameUTF8);

			virtual ~CLibPylonSDK();

			void checkError(pylonResult statusCode);

		};


		typedef std::shared_ptr<CLibPylonSDK> PLibPylonSDK;

	} 
} 

#endif // __LIBMCDRIVER_PYLON_SDK
