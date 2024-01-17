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

#include "libmcdriver_tml_sdk.hpp"
#include "libmcdriver_tml_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_TML::Impl;

#define TML_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadTMLAddress (HMODULE hLibrary, const std::string & sSymbolName, uint32_t nParameterByteCount) {

	std::string sAnnotatedSymbolName;
	if (sizeof(uint32_t) == sizeof(intptr_t)) { // On 32 Bit Windows, DLL names are annotated
		sAnnotatedSymbolName = "_" + sSymbolName + "@" + std::to_string(nParameterByteCount);
	}
	else {
		sAnnotatedSymbolName = sSymbolName;
	}

	void * pFuncPtr = (void*) GetProcAddress(hLibrary, sAnnotatedSymbolName.c_str ());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadTMLAddress(void * hLibrary, const char* pSymbolName, uint32_t nParameterByteCount) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CTMLSDK::CTMLSDK(const std::string& sDLLNameUTF8, const std::string& sDLLDirectoryUTF8)
	: m_LibraryHandle (nullptr)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > TML_MAXDLLNAMELENGTH)
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_COULDNOTLOADLIBRARY);

	int nPathLength = (int)sDLLDirectoryUTF8.length();
	int nPathBufferSize = nPathLength * 2 + 2;
	std::vector<wchar_t> wsDLLPath(nPathBufferSize);
	int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLDirectoryUTF8.c_str(), nPathLength, &wsDLLPath[0], nPathBufferSize);
	if (nPathResult == 0)
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_COULDNOTLOADLIBRARY);

	m_sDLLDirectoryW = std::wstring(wsDLLPath.data());

	auto pDirectoryCache = cacheDllDirectory();

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32
	
	this->TS_LoadSetup = (PTS_LoadSetup)_loadTMLAddress(hLibrary, "TS_LoadSetup", 0);
	this->TS_OpenChannel = (PTS_OpenChannel)_loadTMLAddress(hLibrary, "TS_OpenChannel", 0);
	this->TS_CloseChannel = (PTS_CloseChannel)_loadTMLAddress(hLibrary, "TS_CloseChannel", 0);
	this->TS_DriveInitialisation = (PTS_DriveInitialisation)_loadTMLAddress(hLibrary, "TS_DriveInitialisation", 0);
	this->TS_SetupAxis = (PTS_SetupAxis)_loadTMLAddress(hLibrary, "TS_SetupAxis", 0);
	this->TS_SelectAxis = (PTS_SelectAxis)_loadTMLAddress(hLibrary, "TS_SelectAxis", 0);
	this->TS_LoadSetup = (PTS_LoadSetup)_loadTMLAddress(hLibrary, "TS_LoadSetup", 0);
	this->TS_ResetFault = (PTS_ResetFault)_loadTMLAddress(hLibrary, "TS_ResetFault", 0);
	this->TS_Power = (PTS_Power)_loadTMLAddress(hLibrary, "TS_Power", 0);
	this->TS_ReadStatus = (PTS_ReadStatus)_loadTMLAddress(hLibrary, "TS_ReadStatus", 0);
	this->TS_GetLastErrorText = (PTS_GetLastErrorText)_loadTMLAddress(hLibrary, "TS_GetLastErrorText", 0);

	m_LibraryHandle = (void*) hLibrary;
}


CTMLSDK::~CTMLSDK()
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


void CTMLSDK::checkError(bool bSuccess)
{
	if (!bSuccess) {
		raiseLastError();
	}
}

void CTMLSDK::raiseLastError()
{
	const char* pLastErrorText = nullptr;
	if (TS_GetLastErrorText != nullptr) {
		pLastErrorText = TS_GetLastErrorText();
	}

	if (pLastErrorText != nullptr) {
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_TMLSDKERROR, "TML SDK Error: " + std::string (pLastErrorText));
	}
	else {
		throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_UNKNOWNTMLSDKERROR);
	}

}

void CTMLSDK::resetFunctionPtrs()
{
	TS_LoadSetup = nullptr;
	TS_OpenChannel = nullptr;
	TS_CloseChannel = nullptr;
	TS_DriveInitialisation = nullptr;
	TS_SetupAxis = nullptr;
	TS_SelectAxis = nullptr;
	TS_ResetFault = nullptr;
	TS_Power = nullptr;
	TS_ReadStatus = nullptr;
	TS_GetLastErrorText = nullptr;

}

PTMLSDK_DLLDirectoryCache CTMLSDK::cacheDllDirectory()
{
	auto pCache = std::make_shared<CTMLSDK_DLLDirectoryCache>();

#ifdef _WIN32
	SetDllDirectoryW(m_sDLLDirectoryW.c_str());
#endif // _WIN32

	return pCache;

}

CTMLSDK_DLLDirectoryCache::CTMLSDK_DLLDirectoryCache()
{
#ifdef _WIN32
	std::vector<wchar_t> buffer;
	buffer.resize(MAX_PATH + 1);
	GetDllDirectoryW(MAX_PATH, buffer.data());

	buffer.at(MAX_PATH) = 0;
	m_sCachedDLLDirectoryW = std::wstring(buffer.data());
#endif // _WIN32
}

CTMLSDK_DLLDirectoryCache::~CTMLSDK_DLLDirectoryCache()
{
#ifdef _WIN32
	if (!m_sCachedDLLDirectoryW.empty()) {
		SetDllDirectoryW(m_sCachedDLLDirectoryW.c_str());
	}
#endif // _WIN32
}