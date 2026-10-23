/*++

Copyright (C) 2025 Autodesk Inc.

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

#include "libmcdriver_spinnaker_sdk.hpp"
#include "libmcdriver_spinnaker_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_Spinnaker::Impl;

#define SPINNAKER_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadSpinnakerAddress(HMODULE hLibrary, const std::string& sSymbolName) {
	void* pFuncPtr = (void*)GetProcAddress(hLibrary, sSymbolName.c_str());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadSpinnakerAddress(void* hLibrary, const std::string& sSymbolName) {
	void* pFuncPtr = (void*)dlsym(hLibrary, sSymbolName.c_str());
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif

static void* _tryLoadSpinnakerAddress(void* hLibrary, const std::string& sSymbolName)
{
#ifdef _WIN32
	void* pFuncPtr = (void*)GetProcAddress((HMODULE)hLibrary, sSymbolName.c_str());
#else
	void* pFuncPtr = (void*)dlsym(hLibrary, sSymbolName.c_str());
	dlerror();
#endif
	return pFuncPtr;
}

CLibSpinnakerSDK::CLibSpinnakerSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle(nullptr)
{
	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > SPINNAKER_MAXDLLNAMELENGTH)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->SpinSystemGetInstance = (PSpinSystemGetInstance)_loadSpinnakerAddress(hLibrary, "spinSystemGetInstance");
	this->SpinSystemReleaseInstance = (PSpinSystemReleaseInstance)_loadSpinnakerAddress(hLibrary, "spinSystemReleaseInstance");
	this->SpinSystemGetCameras = (PSpinSystemGetCameras)_loadSpinnakerAddress(hLibrary, "spinSystemGetCameras");

	this->SpinCameraListCreateEmpty = (PSpinCameraListCreateEmpty)_loadSpinnakerAddress(hLibrary, "spinCameraListCreateEmpty");
	this->SpinCameraListDestroy = (PSpinCameraListDestroy)_loadSpinnakerAddress(hLibrary, "spinCameraListDestroy");
	this->SpinCameraListGetSize = (PSpinCameraListGetSize)_loadSpinnakerAddress(hLibrary, "spinCameraListGetSize");
	this->SpinCameraListGet = (PSpinCameraListGet)_loadSpinnakerAddress(hLibrary, "spinCameraListGet");

	this->SpinCameraInit = (PSpinCameraInit)_loadSpinnakerAddress(hLibrary, "spinCameraInit");
	this->SpinCameraDeInit = (PSpinCameraDeInit)_loadSpinnakerAddress(hLibrary, "spinCameraDeInit");
	this->SpinCameraRelease = (PSpinCameraRelease)_loadSpinnakerAddress(hLibrary, "spinCameraRelease");
	this->SpinCameraGetNodeMap = (PSpinCameraGetNodeMap)_loadSpinnakerAddress(hLibrary, "spinCameraGetNodeMap");

	this->SpinNodeMapGetNode = (PSpinNodeMapGetNode)_loadSpinnakerAddress(hLibrary, "spinNodeMapGetNode");
	this->SpinNodeIsAvailable = (PSpinNodeIsAvailable)_loadSpinnakerAddress(hLibrary, "spinNodeIsAvailable");
	this->SpinNodeIsReadable = (PSpinNodeIsReadable)_loadSpinnakerAddress(hLibrary, "spinNodeIsReadable");
	this->SpinNodeIsWritable = (PSpinNodeIsWritable)_loadSpinnakerAddress(hLibrary, "spinNodeIsWritable");

	this->SpinIntegerSetValue = (PSpinIntegerSetValue)_loadSpinnakerAddress(hLibrary, "spinIntegerSetValue");
	this->SpinIntegerGetValue = (PSpinIntegerGetValue)_loadSpinnakerAddress(hLibrary, "spinIntegerGetValue");
	this->SpinIntegerGetMin = (PSpinIntegerGetMin)_loadSpinnakerAddress(hLibrary, "spinIntegerGetMin");
	this->SpinIntegerGetMax = (PSpinIntegerGetMax)_loadSpinnakerAddress(hLibrary, "spinIntegerGetMax");

	this->SpinFloatSetValue = (PSpinFloatSetValue)_loadSpinnakerAddress(hLibrary, "spinFloatSetValue");
	this->SpinFloatGetValue = (PSpinFloatGetValue)_loadSpinnakerAddress(hLibrary, "spinFloatGetValue");
	this->SpinFloatGetMin = (PSpinFloatGetMin)_loadSpinnakerAddress(hLibrary, "spinFloatGetMin");
	this->SpinFloatGetMax = (PSpinFloatGetMax)_loadSpinnakerAddress(hLibrary, "spinFloatGetMax");

	this->SpinBooleanSetValue = (PSpinBooleanSetValue)_loadSpinnakerAddress(hLibrary, "spinBooleanSetValue");
	this->SpinBooleanGetValue = (PSpinBooleanGetValue)_loadSpinnakerAddress(hLibrary, "spinBooleanGetValue");

	this->SpinEnumerationGetEntryByName = (PSpinEnumerationGetEntryByName)_loadSpinnakerAddress(hLibrary, "spinEnumerationGetEntryByName");
	this->SpinEnumerationSetIntValue = (PSpinEnumerationSetIntValue)_loadSpinnakerAddress(hLibrary, "spinEnumerationSetIntValue");
	this->SpinEnumerationGetCurrentEntry = (PSpinEnumerationGetCurrentEntry)_loadSpinnakerAddress(hLibrary, "spinEnumerationGetCurrentEntry");
	this->SpinEnumerationEntryGetIntValue = (PSpinEnumerationEntryGetIntValue)_loadSpinnakerAddress(hLibrary, "spinEnumerationEntryGetIntValue");
	this->SpinEnumerationEntryGetSymbolic = (PSpinEnumerationEntryGetSymbolic)_tryLoadSpinnakerAddress(hLibrary, "spinEnumerationEntryGetSymbolic");

	this->SpinCameraBeginAcquisition = (PSpinCameraBeginAcquisition)_loadSpinnakerAddress(hLibrary, "spinCameraBeginAcquisition");
	this->SpinCameraEndAcquisition = (PSpinCameraEndAcquisition)_loadSpinnakerAddress(hLibrary, "spinCameraEndAcquisition");
	this->SpinCameraGetNextImage = (PSpinCameraGetNextImage)_loadSpinnakerAddress(hLibrary, "spinCameraGetNextImage");
	this->SpinCameraGetNextImageEx = (PSpinCameraGetNextImageEx)_tryLoadSpinnakerAddress(hLibrary, "spinCameraGetNextImageEx");

	this->SpinImageIsIncomplete = (PSpinImageIsIncomplete)_loadSpinnakerAddress(hLibrary, "spinImageIsIncomplete");
	this->SpinImageGetWidth = (PSpinImageGetWidth)_loadSpinnakerAddress(hLibrary, "spinImageGetWidth");
	this->SpinImageGetHeight = (PSpinImageGetHeight)_loadSpinnakerAddress(hLibrary, "spinImageGetHeight");
	this->SpinImageGetData = (PSpinImageGetData)_loadSpinnakerAddress(hLibrary, "spinImageGetData");
	this->SpinImageGetDataSize = (PSpinImageGetDataSize)_loadSpinnakerAddress(hLibrary, "spinImageGetDataSize");
	this->SpinImageRelease = (PSpinImageRelease)_loadSpinnakerAddress(hLibrary, "spinImageRelease");

	m_LibraryHandle = (void*)hLibrary;
}

CLibSpinnakerSDK::~CLibSpinnakerSDK()
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

void CLibSpinnakerSDK::resetFunctionPtrs()
{
	this->SpinSystemGetInstance = nullptr;
	this->SpinSystemReleaseInstance = nullptr;
	this->SpinSystemGetCameras = nullptr;
	this->SpinCameraListCreateEmpty = nullptr;
	this->SpinCameraListDestroy = nullptr;
	this->SpinCameraListGetSize = nullptr;
	this->SpinCameraListGet = nullptr;
	this->SpinCameraInit = nullptr;
	this->SpinCameraDeInit = nullptr;
	this->SpinCameraRelease = nullptr;
	this->SpinCameraGetNodeMap = nullptr;
	this->SpinNodeMapGetNode = nullptr;
	this->SpinNodeIsAvailable = nullptr;
	this->SpinNodeIsReadable = nullptr;
	this->SpinNodeIsWritable = nullptr;
	this->SpinIntegerSetValue = nullptr;
	this->SpinIntegerGetValue = nullptr;
	this->SpinIntegerGetMin = nullptr;
	this->SpinIntegerGetMax = nullptr;
	this->SpinFloatSetValue = nullptr;
	this->SpinFloatGetValue = nullptr;
	this->SpinFloatGetMin = nullptr;
	this->SpinFloatGetMax = nullptr;
	this->SpinBooleanSetValue = nullptr;
	this->SpinBooleanGetValue = nullptr;
	this->SpinEnumerationGetEntryByName = nullptr;
	this->SpinEnumerationSetIntValue = nullptr;
	this->SpinEnumerationGetCurrentEntry = nullptr;
	this->SpinEnumerationEntryGetIntValue = nullptr;
	this->SpinEnumerationEntryGetSymbolic = nullptr;
	this->SpinCameraBeginAcquisition = nullptr;
	this->SpinCameraEndAcquisition = nullptr;
	this->SpinCameraGetNextImage = nullptr;
	this->SpinCameraGetNextImageEx = nullptr;
	this->SpinImageIsIncomplete = nullptr;
	this->SpinImageGetWidth = nullptr;
	this->SpinImageGetHeight = nullptr;
	this->SpinImageGetData = nullptr;
	this->SpinImageGetDataSize = nullptr;
	this->SpinImageRelease = nullptr;
}

void CLibSpinnakerSDK::checkError(spinnakerResult statusCode)
{
	if (statusCode != 0) {
		throw ELibMCDriver_SpinnakerInterfaceException(LIBMCDRIVER_SPINNAKER_ERROR_SPINNAKERSDKERROR,
			"Spinnaker SDK Error #" + std::to_string(statusCode));
	}
}
