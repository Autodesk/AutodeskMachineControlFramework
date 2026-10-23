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

#include "libmcdriver_euler_sdk.hpp"
#include "libmcdriver_euler_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32

#include <vector>

using namespace LibMCDriver_Euler::Impl;

#define EULER_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadEulerAddress (HMODULE hLibrary, const std::string & sSymbolName, uint32_t nParameterByteCount) {

	std::string sAnnotatedSymbolName;
	if (sizeof(uint32_t) == sizeof(intptr_t)) { // On 32 Bit Windows, DLL names are annotated
		sAnnotatedSymbolName = "_" + sSymbolName + "@" + std::to_string(nParameterByteCount);
	}
	else {
		sAnnotatedSymbolName = sSymbolName;
	}

	void * pFuncPtr = (void*) GetProcAddress(hLibrary, sAnnotatedSymbolName.c_str ());
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#else
void* _loadEulerAddress(void * hLibrary, const char* pSymbolName, uint32_t nParameterByteCount) {
	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTFINDLIBRARYEXPORT);

	return pFuncPtr;
}
#endif




CLibEulerSDK::CLibEulerSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > EULER_MAXDLLNAMELENGTH)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_EulerInterfaceException(LIBMCDRIVER_EULER_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->euler_connect_new = (Peuler_connect_new)_loadEulerAddress(hLibrary, "euler_connect_new", 0);
	this->euler_connect_start_heartbeat = (Peuler_connect_start_heartbeat)_loadEulerAddress(hLibrary, "euler_connect_start_heartbeat", 0);
	this->euler_connect_stop_heartbeat = (Peuler_connect_stop_heartbeat)_loadEulerAddress(hLibrary, "euler_connect_stop_heartbeat", 0);
	this->euler_connect_new_build_job = (Peuler_connect_new_build_job)_loadEulerAddress(hLibrary, "euler_connect_new_build_job", 0);
	this->euler_connect_upload_layer_image = (Peuler_connect_upload_layer_image)_loadEulerAddress(hLibrary, "euler_connect_upload_layer_image", 0);
	this->euler_connect_set_job_status = (Peuler_connect_set_job_status)_loadEulerAddress(hLibrary, "euler_connect_set_job_status", 0);
	this->euler_connect_free_string = (Peuler_connect_free_string)_loadEulerAddress(hLibrary, "euler_connect_free_string", 0);
	this->euler_connect_destroy = (Peuler_connect_destroy)_loadEulerAddress(hLibrary, "euler_connect_destroy", 0);
	this->euler_connect_read_license = (Peuler_connect_read_license)_loadEulerAddress(hLibrary, "euler_connect_read_license", 0);
	this->euler_connect_free_license_info = (Peuler_connect_free_license_info)_loadEulerAddress(hLibrary, "euler_connect_free_license_info", 0);

	m_LibraryHandle = (void*) hLibrary;
}


CLibEulerSDK::~CLibEulerSDK()
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



void CLibEulerSDK::resetFunctionPtrs()
{
	this->euler_connect_new = nullptr;
	this->euler_connect_start_heartbeat = nullptr;
	this->euler_connect_stop_heartbeat = nullptr;
	this->euler_connect_new_build_job = nullptr;
	this->euler_connect_upload_layer_image = nullptr;
	this->euler_connect_set_job_status = nullptr;
	this->euler_connect_free_string = nullptr;
	this->euler_connect_destroy = nullptr;
	this->euler_connect_read_license = nullptr;
	this->euler_connect_free_license_info = nullptr;


}

