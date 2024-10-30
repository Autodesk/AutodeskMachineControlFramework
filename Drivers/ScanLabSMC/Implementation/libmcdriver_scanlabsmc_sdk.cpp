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

#include "libmcdriver_scanlabsmc_sdk.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32


#include <vector>
#include <sstream>
#include <iostream>
#include <string>

using namespace LibMCDriver_ScanLabSMC::Impl;


#define SCANLAB_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadScanLabSMCAddress (HMODULE hLibrary, const char * pSymbolName, bool bMandatory = true) {
	void * pFuncPtr = (void*) GetProcAddress(hLibrary, pSymbolName);
	if (bMandatory) {
		if (pFuncPtr == nullptr)
			throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));
	}

	return pFuncPtr;
}
#else
void* _loadScanLabSMCAddress(void * hLibrary, const char* pSymbolName, bool bMandatory = true) {

	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (bMandatory) {
		if (pFuncPtr == nullptr)
			throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));
	}

	return pFuncPtr;
}
#endif

CScanLabSMCSDK_DLLDirectoryCache::CScanLabSMCSDK_DLLDirectoryCache()
{
#ifdef _WIN32
	std::vector<wchar_t> buffer;
	buffer.resize(MAX_PATH + 1);
	GetDllDirectoryW(MAX_PATH, buffer.data());

	buffer.at(MAX_PATH) = 0;
	m_sCachedDLLDirectoryW = std::wstring(buffer.data());
#endif // _WIN32
}

CScanLabSMCSDK_DLLDirectoryCache::~CScanLabSMCSDK_DLLDirectoryCache()
{
#ifdef _WIN32
	if (!m_sCachedDLLDirectoryW.empty()) {
		SetDllDirectoryW(m_sCachedDLLDirectoryW.c_str());
	}
#endif // _WIN32
}


CScanLabSMCSDK::CScanLabSMCSDK(const std::string& sDLLNameUTF8, const std::string& sDLLDirectoryUTF8)
	: m_LibraryHandle (nullptr), m_bIsInitialized (false)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > SCANLAB_MAXDLLNAMELENGTH)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_ScanLabSMCInterfaceException (LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);

	int nPathLength = (int)sDLLDirectoryUTF8.length();
	int nPathBufferSize = nPathLength * 2 + 2;
	std::vector<wchar_t> wsDLLPath(nPathBufferSize);
	int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLDirectoryUTF8.c_str(), nPathLength, &wsDLLPath[0], nPathBufferSize);
	if (nPathResult == 0)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);

	m_sDLLDirectoryW = std::wstring(wsDLLPath.data());

	auto pDirectoryCache = cacheDllDirectory();

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32


	this->slsc_cfg_initialize_from_file = (PScanLabSMCPtr_slsc_cfg_initialize_from_file)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_initialize_from_file");
	this->slsc_cfg_delete = (PScanLabSMCPtr_slsc_cfg_delete)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_delete");
	this->slsc_job_begin = (PScanLabSMCPtr_slsc_job_begin)_loadScanLabSMCAddress(hLibrary, "slsc_job_begin");
	this->slsc_job_end = (PScanLabSMCPtr_slsc_job_end)_loadScanLabSMCAddress(hLibrary, "slsc_job_end");
	this->slsc_job_jump = (PScanLabSMCPtr_slsc_job_jump)_loadScanLabSMCAddress(hLibrary, "slsc_job_jump");
	this->slsc_job_begin_polyline = (PScanLabSMCPtr_slsc_job_begin_polyline)_loadScanLabSMCAddress(hLibrary, "slsc_job_begin_polyline");
	this->slsc_job_end_polyline = (PScanLabSMCPtr_slsc_slsc_job_end_polyline)_loadScanLabSMCAddress(hLibrary, "slsc_job_end_polyline");
	this->slsc_job_line = (PScanLabSMCPtr_slsc_job_line)_loadScanLabSMCAddress(hLibrary, "slsc_job_line");
	this->slsc_ctrl_start_execution = (PScanLabSMCPtr_slsc_ctrl_start_execution)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_start_execution");
	this->slsc_ctrl_stop = (PScanLabSMCPtr_slsc_ctrl_stop)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_stop");
	this->slsc_ctrl_stop_controlled = (PScanLabSMCPtr_slsc_ctrl_stop_controlled)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_stop_controlled");
	this->slsc_ctrl_get_exec_state = (PScanLabSMCPtr_slsc_ctrl_get_exec_state)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_exec_state");
	this->slsc_job_set_jump_speed = (PScanLabSMCPtr_slsc_job_set_jump_speed)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_jump_speed");
	this->slsc_job_set_mark_speed = (PScanLabSMCPtr_slsc_job_set_mark_speed)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_mark_speed");
	this->slsc_job_set_min_mark_speed = (PScanLabSMCPtr_slsc_job_set_min_mark_speed)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_min_mark_speed");
	this->slsc_job_jump_min_time = (PScanLabSMCPtr_slsc_job_jump_min_time)_loadScanLabSMCAddress(hLibrary, "slsc_job_jump_min_time");
	this->slsc_job_set_corner_tolerance = (PScanLabSMCPtr_slsc_job_set_corner_tolerance)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_corner_tolerance");
	this->slsc_ctrl_get_error = (PScanLabSMCPtr_slsc_ctrl_get_error)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_error");
	this->slsc_ctrl_get_error_count = (PScanLabSMCPtr_slsc_ctrl_get_error_count)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_error_count");
	this->slsc_ctrl_get_simulation_filename = (PScanLabSMCPtr_slsc_ctrl_get_simulation_filename)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_simulation_filename");

	m_LibraryHandle = (void*) hLibrary;
}


CScanLabSMCSDK::~CScanLabSMCSDK()
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

void CScanLabSMCSDK::initDLL()
{
	if (!m_bIsInitialized) {

		if (slsc_cfg_initialize_from_file == nullptr)
			throw std::runtime_error("SMC DLL not loaded");

		m_bIsInitialized = true;

	}
}

void CScanLabSMCSDK::checkError(size_t hHandle, uint32_t nSMCError)
{

	if (nSMCError != 0) {
		if ((hHandle != 0) && (slsc_ctrl_get_error != nullptr) && (slsc_ctrl_get_error_count != nullptr)) {

			std::lock_guard<std::mutex> lockGuard(m_ListErrorMutex);

			size_t nErrorCount = 0;
			uint32_t nErrorCountReturn = this->slsc_ctrl_get_error_count(hHandle, nErrorCount);

			if (nErrorCountReturn == 0) {

				std::stringstream sErrorStream;

				for (size_t nErrorIndex = 0; nErrorIndex < nErrorCount; nErrorIndex++) {

					if (nErrorIndex > 0)
						sErrorStream << " | ";

					size_t nBufferSize = 1024;
					std::vector<char> buffer(nBufferSize + 1);
					size_t nErrorCode = 0;

					uint32_t nErrorGetReturn = this->slsc_ctrl_get_error(hHandle, nErrorIndex, nErrorCode, buffer.data(), nBufferSize);
					if (nErrorGetReturn == 0) {
						buffer.at(nBufferSize) = 0;
						sErrorStream << "error #" << std::to_string(nErrorCode) << ": " << std::string(buffer.data ());
					}
					else {
						sErrorStream << "<could not get error message for #" << std::to_string(nErrorIndex) << ">";
					}

				}

				throw std::runtime_error("SMC Error: " + std::to_string(nSMCError) + ", Error Details: " + sErrorStream.str () );

			} else {
				throw std::runtime_error("Could not get error count for: " + std::to_string(nSMCError));

			}

		}
		else {
			throw std::runtime_error("Generic SMC Error: " + std::to_string(nSMCError));
		}
		
	}
}


void CScanLabSMCSDK::resetFunctionPtrs()
{
	slsc_cfg_initialize_from_file = nullptr;
	slsc_cfg_delete = nullptr;
	slsc_job_begin = nullptr;
	slsc_job_end = nullptr;
	slsc_job_jump = nullptr;
	slsc_job_begin_polyline = nullptr;
	slsc_job_end_polyline = nullptr;
	slsc_job_line = nullptr;
	slsc_ctrl_start_execution = nullptr;
	slsc_ctrl_stop = nullptr;
	slsc_ctrl_stop_controlled = nullptr;
	slsc_ctrl_get_exec_state = nullptr;
	slsc_job_set_jump_speed = nullptr;
	slsc_job_set_mark_speed = nullptr;
	slsc_job_set_min_mark_speed = nullptr;
	slsc_job_jump_min_time = nullptr;
	slsc_job_set_corner_tolerance = nullptr;
	slsc_ctrl_get_error = nullptr;
	slsc_ctrl_get_error_count = nullptr;
	slsc_ctrl_get_simulation_filename = nullptr;

}


PScanLabSMCSDK_DLLDirectoryCache CScanLabSMCSDK::cacheDllDirectory()
{
	auto pCache = std::make_shared<CScanLabSMCSDK_DLLDirectoryCache>();

#ifdef _WIN32
	SetDllDirectoryW(m_sDLLDirectoryW.c_str());
	SetCurrentDirectoryW(m_sDLLDirectoryW.c_str());
#endif // _WIN32

	return pCache;

}

