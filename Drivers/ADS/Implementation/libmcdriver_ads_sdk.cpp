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

#include "libmcdriver_ads_sdk.hpp"
#include "libmcdriver_ads_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_ADS::Impl;

#define ADS_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadADSAddress(HMODULE hLibrary, const std::string& sSymbolName, uint32_t nParameterByteCount) {

	std::string sAnnotatedSymbolName;
	if (sizeof(uint32_t) == sizeof(intptr_t)) { // On 32 Bit Windows, DLL names are annotated
		sAnnotatedSymbolName = "_" + sSymbolName + "@" + std::to_string(nParameterByteCount);
	}
	else {
		sAnnotatedSymbolName = sSymbolName;
	}

	void* pFuncPtr = (void*)GetProcAddress(hLibrary, sAnnotatedSymbolName.c_str());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadADSAddress(void* hLibrary, const char* pSymbolName, uint32_t nParameterByteCount) {
	void* pFuncPtr = (void*)dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CADSSDK::CADSSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle(nullptr)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > ADS_MAXDLLNAMELENGTH)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_ADSInterfaceException(LIBMCDRIVER_ADS_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->AdsGetDllVersion = (PAdsGetDllVersion)_loadADSAddress(hLibrary, "AdsGetDllVersion", 0);
	this->AdsPortOpenEx = (PAdsPortOpenEx)_loadADSAddress(hLibrary, "AdsPortOpenEx", 0);
	this->AdsPortCloseEx = (PAdsPortCloseEx)_loadADSAddress(hLibrary, "AdsPortCloseEx", 0);
	this->AdsGetLocalAddressEx = (PAdsGetLocalAddressEx)_loadADSAddress(hLibrary, "AdsGetLocalAddressEx", 8);
	this->AdsSyncWriteReqEx = (PAdsSyncWriteReqEx)_loadADSAddress(hLibrary, "AdsSyncWriteReqEx", 24);
	this->AdsSyncReadReqEx2 = (PAdsSyncReadReqEx2)_loadADSAddress(hLibrary, "AdsSyncReadReqEx2", 28);
	this->AdsSyncReadWriteReqEx2 = (PAdsSyncReadWriteReqEx2)_loadADSAddress(hLibrary, "AdsSyncReadWriteReqEx2", 36);

	m_LibraryHandle = (void*)hLibrary;
}


CADSSDK::~CADSSDK()
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


void CADSSDK::checkError(int32_t statusCode)
{
	if (statusCode != 0)
		throw std::runtime_error("ADS Error: " + std::to_string(statusCode));
}

void CADSSDK::resetFunctionPtrs()
{
	AdsGetDllVersion = nullptr;
	AdsPortOpenEx = nullptr;
	AdsPortCloseEx = nullptr;
	AdsGetLocalAddressEx = nullptr;
	AdsSyncWriteReqEx = nullptr;
	AdsSyncReadReqEx2 = nullptr;
	AdsSyncReadWriteReqEx2 = nullptr;

}

