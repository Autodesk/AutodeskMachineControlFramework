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

#include "libmcdriver_scanlaboie_sdk.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32


#include <vector>
#include <iostream>

using namespace LibMCDriver_ScanLabOIE::Impl;

#define SCANLABOIE_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadScanLabOIEAddress(HMODULE hLibrary, const char* pSymbolName) {
	void* pFuncPtr = (void*)GetProcAddress(hLibrary, pSymbolName);
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));

	return pFuncPtr;
}
#else
void* _loadScanLabOIEAddress(void* hLibrary, const char* pSymbolName) {

	void* pFuncPtr = (void*)dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));

	return pFuncPtr;
}
#endif



CScanLabOIESDK::CScanLabOIESDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr), m_bIsInitialized (false)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > SCANLABOIE_MAXDLLNAMELENGTH)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_ScanLabOIEInterfaceException (LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32


	this->oie_get_version = (PScanLabOIEPtr_oie_get_version)_loadScanLabOIEAddress(hLibrary, "oie_get_version");
	this->oie_create = (PScanLabOIEPtr_oie_create)_loadScanLabOIEAddress(hLibrary, "oie_create");
	this->oie_destroy = (PScanLabOIEPtr_oie_destroy)_loadScanLabOIEAddress(hLibrary, "oie_destroy");
	this->oie_get_error = (PScanLabOIEPtr_oie_get_error)_loadScanLabOIEAddress(hLibrary, "oie_get_error");
	this->oie_add_device = (PScanLabOIEPtr_oie_add_device)_loadScanLabOIEAddress(hLibrary, "oie_add_device");
	this->oie_remove_device = (PScanLabOIEPtr_oie_remove_device)_loadScanLabOIEAddress(hLibrary, "oie_remove_device");
	this->oie_get_device_id = (PScanLabOIEPtr_oie_get_device_id)_loadScanLabOIEAddress(hLibrary, "oie_get_device_id");
	this->oie_set_reply_timeout = (PScanLabOIEPtr_oie_set_reply_timeout)_loadScanLabOIEAddress(hLibrary, "oie_set_reply_timeout");
	this->oie_add_device = (PScanLabOIEPtr_oie_add_device)_loadScanLabOIEAddress(hLibrary, "oie_add_device");
	this->oie_connect = (PScanLabOIEPtr_oie_connect)_loadScanLabOIEAddress(hLibrary, "oie_connect");
	this->oie_is_connected = (PScanLabOIEPtr_oie_is_connected)_loadScanLabOIEAddress(hLibrary, "oie_is_connected");
	this->oie_disconnect = (PScanLabOIEPtr_oie_disconnect)_loadScanLabOIEAddress(hLibrary, "oie_disconnect");
	this->oie_device_login = (PScanLabOIEPtr_oie_device_login)_loadScanLabOIEAddress(hLibrary, "oie_device_login");
	this->oie_device_list_apps = (PScanLabOIEPtr_oie_device_list_apps)_loadScanLabOIEAddress(hLibrary, "oie_device_list_apps");
	this->oie_device_start_app = (PScanLabOIEPtr_oie_device_start_app)_loadScanLabOIEAddress(hLibrary, "oie_device_start_app");
	this->oie_device_start_app_ver = (PScanLabOIEPtr_oie_device_start_app_ver)_loadScanLabOIEAddress(hLibrary, "oie_device_start_app_ver");
	this->oie_device_stop_app = (PScanLabOIEPtr_oie_device_stop_app)_loadScanLabOIEAddress(hLibrary, "oie_device_stop_app");
	this->oie_device_is_app_running = (PScanLabOIEPtr_oie_device_is_app_running)_loadScanLabOIEAddress(hLibrary, "oie_device_is_app_running");
	this->oie_device_get_appinfo = (PScanLabOIEPtr_oie_device_get_appinfo)_loadScanLabOIEAddress(hLibrary, "oie_device_get_appinfo");
	this->oie_device_install_app = (PScanLabOIEPtr_oie_device_install_app)_loadScanLabOIEAddress(hLibrary, "oie_device_install_app");
	this->oie_device_uninstall_app = (PScanLabOIEPtr_oie_device_uninstall_app)_loadScanLabOIEAddress(hLibrary, "oie_device_uninstall_app");
	this->oie_device_uninstall_app_ver = (PScanLabOIEPtr_oie_device_uninstall_app_ver)_loadScanLabOIEAddress(hLibrary, "oie_device_uninstall_app_ver");
	this->oie_device_unlock = (PScanLabOIEPtr_oie_device_unlock)_loadScanLabOIEAddress(hLibrary, "oie_device_unlock");
	this->oie_device_is_unlocked = (PScanLabOIEPtr_oie_device_is_unlocked)_loadScanLabOIEAddress(hLibrary, "oie_device_is_unlocked");
	this->oie_device_get_log = (PScanLabOIEPtr_oie_device_get_log)_loadScanLabOIEAddress(hLibrary, "oie_device_get_log");
	this->oie_device_get_board_version = (PScanLabOIEPtr_oie_device_get_board_version)_loadScanLabOIEAddress(hLibrary, "oie_device_get_board_version");
	this->oie_get_rtc_type = (PScanLabOIEPtr_oie_get_rtc_type)_loadScanLabOIEAddress(hLibrary, "oie_get_rtc_type");
	this->oie_get_rtc_signals = (PScanLabOIEPtr_oie_get_rtc_signals)_loadScanLabOIEAddress(hLibrary, "oie_get_rtc_signals");
	this->oie_get_sensor_signals = (PScanLabOIEPtr_oie_get_sensor_signals)_loadScanLabOIEAddress(hLibrary, "oie_get_sensor_signals");


	m_LibraryHandle = (void*) hLibrary;
}


CScanLabOIESDK::~CScanLabOIESDK()
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

void CScanLabOIESDK::initDLL()
{
	if (!m_bIsInitialized) {

		if (oie_get_version == nullptr)
			throw std::runtime_error("RTC DLL not loaded");

		m_bIsInitialized = true;
	}
}

void CScanLabOIESDK::checkError(uint32_t nSDKError)
{
	if (nSDKError != 0)
		throw std::runtime_error("Scanlab OIE Error: " + std::to_string (nSDKError));
}

void CScanLabOIESDK::resetFunctionPtrs()
{
	oie_get_version = nullptr;
	oie_create = nullptr;
	oie_destroy = nullptr;
	oie_get_error = nullptr;
	oie_add_device = nullptr;
	oie_remove_device = nullptr;
	oie_get_device_id = nullptr;
	oie_set_reply_timeout = nullptr;
	oie_add_device = nullptr;
	oie_connect = nullptr;
	oie_is_connected = nullptr;
	oie_disconnect = nullptr;
	oie_device_login = nullptr;
	oie_device_list_apps = nullptr;
	oie_device_start_app = nullptr;
	oie_device_start_app_ver = nullptr;
	oie_device_stop_app = nullptr;
	oie_device_is_app_running = nullptr;
	oie_device_get_appinfo = nullptr;
	oie_device_install_app = nullptr;
	oie_device_uninstall_app = nullptr;
	oie_device_uninstall_app_ver = nullptr;
	oie_device_unlock = nullptr;
	oie_device_is_unlocked = nullptr;
	oie_device_get_log = nullptr;
	oie_device_get_board_version = nullptr;
	oie_get_rtc_type = nullptr;
	oie_get_rtc_signals = nullptr;
	oie_get_sensor_signals = nullptr;
}
