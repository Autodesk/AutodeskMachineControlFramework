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


#ifndef __LIBMCDRIVER_SPINNAKER_SDK
#define __LIBMCDRIVER_SPINNAKER_SDK

#include <cstdint>
#include <memory>
#include <string>

#ifdef _WIN32

#define SPINNAKER_CALLINGCONVENTION __cdecl

#else

#define SPINNAKER_CALLINGCONVENTION

#endif


namespace LibMCDriver_Spinnaker {
	namespace Impl {

		typedef void* libSpinnakerSystemHandle;
		typedef void* libSpinnakerCameraListHandle;
		typedef void* libSpinnakerCameraHandle;
		typedef void* libSpinnakerImageHandle;
		typedef void* libSpinnakerNodeMapHandle;
		typedef void* libSpinnakerNodeHandle;
		typedef void* libSpinnakerEnumEntryHandle;

		typedef int32_t spinnakerResult;
		typedef uint8_t spinnakerBool;

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinSystemGetInstance) (libSpinnakerSystemHandle* phSystem);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinSystemReleaseInstance) (libSpinnakerSystemHandle hSystem);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinSystemGetCameras) (libSpinnakerSystemHandle hSystem, libSpinnakerCameraListHandle hCameraList);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraListCreateEmpty) (libSpinnakerCameraListHandle* phCameraList);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraListDestroy) (libSpinnakerCameraListHandle hCameraList);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraListGetSize) (libSpinnakerCameraListHandle hCameraList, size_t* pnSize);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraListGet) (libSpinnakerCameraListHandle hCameraList, size_t nIndex, libSpinnakerCameraHandle* phCamera);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraInit) (libSpinnakerCameraHandle hCamera);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraDeInit) (libSpinnakerCameraHandle hCamera);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraRelease) (libSpinnakerCameraHandle hCamera);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraGetNodeMap) (libSpinnakerCameraHandle hCamera, libSpinnakerNodeMapHandle* phNodeMap);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinNodeMapGetNode) (libSpinnakerNodeMapHandle hNodeMap, const char* pName, libSpinnakerNodeHandle* phNode);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinNodeIsAvailable) (libSpinnakerNodeHandle hNode, spinnakerBool* pbAvailable);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinNodeIsReadable) (libSpinnakerNodeHandle hNode, spinnakerBool* pbReadable);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinNodeIsWritable) (libSpinnakerNodeHandle hNode, spinnakerBool* pbWritable);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinIntegerSetValue) (libSpinnakerNodeHandle hNode, int64_t nValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinIntegerGetValue) (libSpinnakerNodeHandle hNode, int64_t* pnValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinIntegerGetMin) (libSpinnakerNodeHandle hNode, int64_t* pnValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinIntegerGetMax) (libSpinnakerNodeHandle hNode, int64_t* pnValue);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinFloatSetValue) (libSpinnakerNodeHandle hNode, double dValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinFloatGetValue) (libSpinnakerNodeHandle hNode, double* pdValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinFloatGetMin) (libSpinnakerNodeHandle hNode, double* pdValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinFloatGetMax) (libSpinnakerNodeHandle hNode, double* pdValue);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinBooleanSetValue) (libSpinnakerNodeHandle hNode, spinnakerBool bValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinBooleanGetValue) (libSpinnakerNodeHandle hNode, spinnakerBool* pbValue);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinEnumerationGetEntryByName) (libSpinnakerNodeHandle hNode, const char* pEntryName, libSpinnakerEnumEntryHandle* phEntry);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinEnumerationSetIntValue) (libSpinnakerNodeHandle hNode, int64_t nValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinEnumerationGetCurrentEntry) (libSpinnakerNodeHandle hNode, libSpinnakerEnumEntryHandle* phEntry);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinEnumerationEntryGetIntValue) (libSpinnakerEnumEntryHandle hEntry, int64_t* pnValue);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinEnumerationEntryGetSymbolic) (libSpinnakerEnumEntryHandle hEntry, char* pSymbolic, size_t* pnSymbolicLen);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraBeginAcquisition) (libSpinnakerCameraHandle hCamera);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraEndAcquisition) (libSpinnakerCameraHandle hCamera);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraGetNextImage) (libSpinnakerCameraHandle hCamera, libSpinnakerImageHandle* phImage);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinCameraGetNextImageEx) (libSpinnakerCameraHandle hCamera, uint32_t nTimeoutMS, libSpinnakerImageHandle* phImage);

		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinImageIsIncomplete) (libSpinnakerImageHandle hImage, spinnakerBool* pbIsIncomplete);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinImageGetWidth) (libSpinnakerImageHandle hImage, size_t* pnWidth);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinImageGetHeight) (libSpinnakerImageHandle hImage, size_t* pnHeight);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinImageGetData) (libSpinnakerImageHandle hImage, void** ppBuffer);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinImageGetDataSize) (libSpinnakerImageHandle hImage, size_t* pnBufferSize);
		typedef spinnakerResult(SPINNAKER_CALLINGCONVENTION* PSpinImageRelease) (libSpinnakerImageHandle hImage);


		class CLibSpinnakerSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs();
		public:

			PSpinSystemGetInstance SpinSystemGetInstance = nullptr;
			PSpinSystemReleaseInstance SpinSystemReleaseInstance = nullptr;
			PSpinSystemGetCameras SpinSystemGetCameras = nullptr;

			PSpinCameraListCreateEmpty SpinCameraListCreateEmpty = nullptr;
			PSpinCameraListDestroy SpinCameraListDestroy = nullptr;
			PSpinCameraListGetSize SpinCameraListGetSize = nullptr;
			PSpinCameraListGet SpinCameraListGet = nullptr;

			PSpinCameraInit SpinCameraInit = nullptr;
			PSpinCameraDeInit SpinCameraDeInit = nullptr;
			PSpinCameraRelease SpinCameraRelease = nullptr;
			PSpinCameraGetNodeMap SpinCameraGetNodeMap = nullptr;

			PSpinNodeMapGetNode SpinNodeMapGetNode = nullptr;
			PSpinNodeIsAvailable SpinNodeIsAvailable = nullptr;
			PSpinNodeIsReadable SpinNodeIsReadable = nullptr;
			PSpinNodeIsWritable SpinNodeIsWritable = nullptr;

			PSpinIntegerSetValue SpinIntegerSetValue = nullptr;
			PSpinIntegerGetValue SpinIntegerGetValue = nullptr;
			PSpinIntegerGetMin SpinIntegerGetMin = nullptr;
			PSpinIntegerGetMax SpinIntegerGetMax = nullptr;

			PSpinFloatSetValue SpinFloatSetValue = nullptr;
			PSpinFloatGetValue SpinFloatGetValue = nullptr;
			PSpinFloatGetMin SpinFloatGetMin = nullptr;
			PSpinFloatGetMax SpinFloatGetMax = nullptr;

			PSpinBooleanSetValue SpinBooleanSetValue = nullptr;
			PSpinBooleanGetValue SpinBooleanGetValue = nullptr;

			PSpinEnumerationGetEntryByName SpinEnumerationGetEntryByName = nullptr;
			PSpinEnumerationSetIntValue SpinEnumerationSetIntValue = nullptr;
			PSpinEnumerationGetCurrentEntry SpinEnumerationGetCurrentEntry = nullptr;
			PSpinEnumerationEntryGetIntValue SpinEnumerationEntryGetIntValue = nullptr;
			PSpinEnumerationEntryGetSymbolic SpinEnumerationEntryGetSymbolic = nullptr;

			PSpinCameraBeginAcquisition SpinCameraBeginAcquisition = nullptr;
			PSpinCameraEndAcquisition SpinCameraEndAcquisition = nullptr;
			PSpinCameraGetNextImage SpinCameraGetNextImage = nullptr;
			PSpinCameraGetNextImageEx SpinCameraGetNextImageEx = nullptr;

			PSpinImageIsIncomplete SpinImageIsIncomplete = nullptr;
			PSpinImageGetWidth SpinImageGetWidth = nullptr;
			PSpinImageGetHeight SpinImageGetHeight = nullptr;
			PSpinImageGetData SpinImageGetData = nullptr;
			PSpinImageGetDataSize SpinImageGetDataSize = nullptr;
			PSpinImageRelease SpinImageRelease = nullptr;

			CLibSpinnakerSDK(const std::string& sDLLNameUTF8);

			virtual ~CLibSpinnakerSDK();

			void checkError(spinnakerResult statusCode);
		};


		typedef std::shared_ptr<CLibSpinnakerSDK> PLibSpinnakerSDK;

	}
}

#endif // __LIBMCDRIVER_SPINNAKER_SDK
