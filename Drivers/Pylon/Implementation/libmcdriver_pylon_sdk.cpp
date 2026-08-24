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

#include "libmcdriver_pylon_sdk.hpp"
#include "libmcdriver_pylon_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_Pylon::Impl;

#define PYLON_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadPylonAddress (HMODULE hLibrary, const std::string & sSymbolName, uint32_t nParameterByteCount) {

	std::string sAnnotatedSymbolName;
	if (sizeof(uint32_t) == sizeof(intptr_t)) { // On 32 Bit Windows, DLL names are annotated
		sAnnotatedSymbolName = "_" + sSymbolName + "@" + std::to_string(nParameterByteCount);
	}
	else {
		sAnnotatedSymbolName = sSymbolName;
	}

	void * pFuncPtr = (void*) GetProcAddress(hLibrary, sAnnotatedSymbolName.c_str ());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadPylonAddress(void * hLibrary, const char* pSymbolName, uint32_t nParameterByteCount) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CLibPylonSDK::CLibPylonSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > PYLON_MAXDLLNAMELENGTH)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->PylonInitialize = (PPylonInitialize)_loadPylonAddress(hLibrary, "PylonInitialize", 0);
	this->PylonTerminate = (PPylonTerminate)_loadPylonAddress(hLibrary, "PylonTerminate", 0);
	this->PylonEnumerateDevices = (PPylonEnumerateDevices)_loadPylonAddress(hLibrary, "PylonEnumerateDevices", 8);
	this->PylonGigEEnumerateAllDevices = (PPylonGigEEnumerateAllDevices)_loadPylonAddress(hLibrary, "PylonGigEEnumerateAllDevices", 8);
	this->PylonGetDeviceInfoHandle = (PPylonGetDeviceInfoHandle)_loadPylonAddress(hLibrary, "PylonGetDeviceInfoHandle", 8);
	this->PylonDeviceInfoGetNumProperties = (PPylonDeviceInfoGetNumProperties)_loadPylonAddress(hLibrary, "PylonDeviceInfoGetNumProperties", 8);
	this->PylonDeviceInfoGetPropertyValueByIndex = (PPylonDeviceInfoGetPropertyValueByIndex)_loadPylonAddress(hLibrary, "PylonDeviceInfoGetPropertyValueByIndex", 12);
	this->PylonDeviceInfoGetPropertyName = (PPylonDeviceInfoGetPropertyName)_loadPylonAddress(hLibrary, "PylonDeviceInfoGetPropertyName", 12);
	this->PylonCreateDeviceByIndex = (PPylonCreateDeviceByIndex)_loadPylonAddress(hLibrary, "PylonCreateDeviceByIndex", 8);
	this->PylonDestroyDevice = (PPylonDestroyDevice)_loadPylonAddress(hLibrary, "PylonDestroyDevice", 4);
	this->PylonIsDeviceAccessible = (PPylonIsDeviceAccessible)_loadPylonAddress(hLibrary, "PylonIsDeviceAccessible", 8);
	this->PylonDeviceOpen = (PPylonDeviceOpen)_loadPylonAddress(hLibrary, "PylonDeviceOpen", 4);
	this->PylonDeviceClose = (PPylonDeviceClose)_loadPylonAddress(hLibrary, "PylonDeviceClose", 4);
	this->PylonDeviceIsOpen = (PPylonDeviceIsOpen)_loadPylonAddress(hLibrary, "PylonDeviceIsOpen", 8);
	this->PylonDeviceAccessMode = (PPylonDeviceAccessMode)_loadPylonAddress(hLibrary, "PylonDeviceAccessMode", 8);
	this->PylonDeviceFeatureIsImplemented = (PPylonDeviceFeatureIsImplemented)_loadPylonAddress(hLibrary, "PylonDeviceFeatureIsImplemented", 8);
	this->PylonDeviceFeatureIsAvailable = (PPylonDeviceFeatureIsAvailable)_loadPylonAddress(hLibrary, "PylonDeviceFeatureIsAvailable", 8);
	this->PylonDeviceFeatureIsReadable = (PPylonDeviceFeatureIsReadable)_loadPylonAddress(hLibrary, "PylonDeviceFeatureIsReadable", 8);
	this->PylonDeviceFeatureIsWritable = (PPylonDeviceFeatureIsWritable)_loadPylonAddress(hLibrary, "PylonDeviceFeatureIsWritable", 8);
	this->PylonDeviceSetIntegerFeature = (PPylonDeviceSetIntegerFeature)_loadPylonAddress(hLibrary, "PylonDeviceSetIntegerFeature", 12);
	this->PylonDeviceGetIntegerFeature = (PPylonDeviceGetIntegerFeature)_loadPylonAddress(hLibrary, "PylonDeviceGetIntegerFeature", 12);
	this->PylonDeviceGetIntegerFeatureMin = (PPylonDeviceGetIntegerFeatureMin)_loadPylonAddress(hLibrary, "PylonDeviceGetIntegerFeatureMin", 12);
	this->PylonDeviceGetIntegerFeatureMax = (PPylonDeviceGetIntegerFeatureMax)_loadPylonAddress(hLibrary, "PylonDeviceGetIntegerFeatureMax", 12);
	this->PylonDeviceGetIntegerFeatureInc = (PPylonDeviceGetIntegerFeatureInc)_loadPylonAddress(hLibrary, "PylonDeviceGetIntegerFeatureInc", 12);
	this->PylonDeviceSetFloatFeature = (PPylonDeviceSetFloatFeature)_loadPylonAddress(hLibrary, "PylonDeviceSetFloatFeature", 12);
	this->PylonDeviceGetFloatFeature = (PPylonDeviceGetFloatFeature)_loadPylonAddress(hLibrary, "PylonDeviceGetFloatFeature", 12);
	this->PylonDeviceGetFloatFeatureMin = (PPylonDeviceGetFloatFeatureMin)_loadPylonAddress(hLibrary, "PylonDeviceGetFloatFeatureMin", 12);
	this->PylonDeviceGetFloatFeatureMax = (PPylonDeviceGetFloatFeatureMax)_loadPylonAddress(hLibrary, "PylonDeviceGetFloatFeatureMax", 12);
	this->PylonDeviceSetBooleanFeature = (PPylonDeviceSetBooleanFeature)_loadPylonAddress(hLibrary, "PylonDeviceSetBooleanFeature", 12);
	this->PylonDeviceGetBooleanFeature = (PPylonDeviceGetBooleanFeature)_loadPylonAddress(hLibrary, "PylonDeviceGetBooleanFeature", 12);
	this->PylonDeviceGrabSingleFrame = (PPylonDeviceGrabSingleFrame)_loadPylonAddress(hLibrary, "PylonDeviceGrabSingleFrame", 32);
	this->GenApiGetLastErrorDetail = (PGenApiGetLastErrorDetail)_loadPylonAddress(hLibrary, "GenApiGetLastErrorDetail", 8);

	m_LibraryHandle = (void*) hLibrary;
}


CLibPylonSDK::~CLibPylonSDK()
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



void CLibPylonSDK::resetFunctionPtrs()
{
	this->PylonInitialize = nullptr;
	this->PylonTerminate = nullptr;
	this->PylonEnumerateDevices = nullptr;
	this->PylonGigEEnumerateAllDevices = nullptr;
	this->PylonGetDeviceInfoHandle = nullptr;
	this->PylonDeviceInfoGetNumProperties = nullptr;
	this->PylonDeviceInfoGetPropertyValueByIndex = nullptr;
	this->PylonDeviceInfoGetPropertyName = nullptr;
	this->PylonCreateDeviceByIndex = nullptr;
	this->PylonDestroyDevice = nullptr;
	this->PylonIsDeviceAccessible = nullptr;
	this->PylonDeviceOpen = nullptr;
	this->PylonDeviceClose = nullptr;
	this->PylonDeviceIsOpen = nullptr;
	this->PylonDeviceAccessMode = nullptr;
	this->PylonDeviceFeatureIsImplemented = nullptr;
	this->PylonDeviceFeatureIsAvailable = nullptr;
	this->PylonDeviceFeatureIsReadable = nullptr;
	this->PylonDeviceFeatureIsWritable = nullptr;
	this->PylonDeviceSetIntegerFeature = nullptr;
	this->PylonDeviceGetIntegerFeature = nullptr;
	this->PylonDeviceGetIntegerFeatureMin = nullptr;
	this->PylonDeviceGetIntegerFeatureMax = nullptr;
	this->PylonDeviceGetIntegerFeatureInc = nullptr;
	this->PylonDeviceSetFloatFeature = nullptr;
	this->PylonDeviceGetFloatFeature = nullptr;
	this->PylonDeviceGetFloatFeatureMin = nullptr;
	this->PylonDeviceGetFloatFeatureMax = nullptr;
	this->PylonDeviceSetBooleanFeature = nullptr;
	this->PylonDeviceGetBooleanFeature = nullptr;
	this->PylonDeviceGrabSingleFrame = nullptr;
	this->GenApiGetLastErrorDetail = nullptr;

}

void CLibPylonSDK::checkError(pylonResult statusCode)
{
	if (statusCode != 0) {
		std::string sErrorMessage = "unknown error";

		if (this->GenApiGetLastErrorDetail != nullptr) {
			size_t nBufferSize = 0;
			this->GenApiGetLastErrorDetail (nullptr, &nBufferSize);
			if (nBufferSize > 0) {
				std::vector<char> buffer;
				buffer.resize(nBufferSize + 1);
				this->GenApiGetLastErrorDetail(&buffer[0], &nBufferSize);
				buffer.at(nBufferSize) = 0;
				sErrorMessage = std::string(&buffer[0]);
			}
		}
		throw ELibMCDriver_PylonInterfaceException(LIBMCDRIVER_PYLON_ERROR_PYLONSDKERROR, "Pylon SDK Error #" + std::to_string(statusCode) + ": " + sErrorMessage);
	}
}