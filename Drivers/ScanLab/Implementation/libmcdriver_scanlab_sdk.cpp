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

#include "libmcdriver_scanlab_sdk.hpp"
#include "libmcdriver_scanlab_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32


#include <vector>
#include <iostream>
#include <string>

using namespace LibMCDriver_ScanLab::Impl;


#define SCANLAB_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadScanLabAddress (HMODULE hLibrary, const char * pSymbolName) {
	void * pFuncPtr = (void*) GetProcAddress(hLibrary, pSymbolName);
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string (pSymbolName));

	return pFuncPtr;
}
#else
void* _loadScanLabAddress(void * hLibrary, const char* pSymbolName) {

	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (pFuncPtr == nullptr)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string (pSymbolName));

	return pFuncPtr;
}
#endif


CScanLabSDK::CScanLabSDK(const std::string& sDLLNameUTF8)
	: m_LibraryHandle (nullptr), m_bIsInitialized (false)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > SCANLAB_MAXDLLNAMELENGTH)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_ScanLabInterfaceException (LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADLIBRARY);

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32


	this->init_rtc6_dll = (PScanLabPtr_init_rtc6_dll)_loadScanLabAddress(hLibrary, "init_rtc6_dll");
	this->free_rtc6_dll = (PScanLabPtr_free_rtc6_dll)_loadScanLabAddress(hLibrary, "free_rtc6_dll");
	this->eth_convert_string_to_ip = (PScanLabPtr_eth_convert_string_to_ip)_loadScanLabAddress(hLibrary, "eth_convert_string_to_ip");
	this->eth_set_search_cards_timeout = (PScanLabPtr_eth_set_search_cards_timeout)_loadScanLabAddress(hLibrary, "eth_set_search_cards_timeout");	
	this->eth_search_cards_range = (PScanLabPtr_eth_search_cards_range)_loadScanLabAddress(hLibrary, "eth_search_cards_range");
	this->rtc6_count_cards = (PScanLabPtr_rtc6_count_cards)_loadScanLabAddress(hLibrary, "rtc6_count_cards");
	this->eth_count_cards = (PScanLabPtr_eth_count_cards)_loadScanLabAddress(hLibrary, "eth_count_cards");
	this->eth_found_cards = (PScanLabPtr_eth_found_cards)_loadScanLabAddress(hLibrary, "eth_found_cards");
	this->eth_assign_card = (PScanLabPtr_eth_assign_card)_loadScanLabAddress(hLibrary, "eth_assign_card");
	this->acquire_rtc = (PScanLabPtr_acquire_rtc)_loadScanLabAddress(hLibrary, "acquire_rtc");
	this->release_rtc = (PScanLabPtr_release_rtc)_loadScanLabAddress(hLibrary, "release_rtc");
	this->n_get_serial_number = (PScanLabPtr_n_get_serial_number)_loadScanLabAddress(hLibrary, "n_get_serial_number");
	this->eth_get_serial_search = (PScanLabPtr_eth_get_serial_search)_loadScanLabAddress(hLibrary, "eth_get_serial_search");
	this->eth_search_cards = (PScanLabPtr_eth_search_cards)_loadScanLabAddress(hLibrary, "eth_search_cards");
	this->n_load_correction_file = (PScanLabPtr_n_load_correction_file)_loadScanLabAddress(hLibrary, "n_load_correction_file");
	this->n_select_cor_table = (PScanLabPtr_n_select_cor_table)_loadScanLabAddress(hLibrary, "n_select_cor_table");
	this->n_eth_set_com_timeouts_auto = (PScanLabPtr_n_eth_set_com_timeouts_auto)_loadScanLabAddress(hLibrary, "n_eth_set_com_timeouts_auto");
	this->n_eth_get_com_timeouts_auto = (PScanLabPtr_n_eth_get_com_timeouts_auto)_loadScanLabAddress(hLibrary, "n_eth_get_com_timeouts_auto");

	this->n_config_list = (PScanLabPtr_n_config_list)_loadScanLabAddress(hLibrary, "n_config_list");
	this->n_set_laser_mode = (PScanLabPtr_n_set_laser_mode)_loadScanLabAddress(hLibrary, "n_set_laser_mode");
	this->n_set_laser_control = (PScanLabPtr_n_set_laser_control)_loadScanLabAddress(hLibrary, "n_set_laser_control");
	this->n_set_auto_laser_control = (PScanLabPtr_n_set_auto_laser_control)_loadScanLabAddress(hLibrary, "n_set_auto_laser_control");
	this->n_set_laser_pulses = (PScanLabPtr_n_set_laser_pulses)_loadScanLabAddress(hLibrary, "n_set_laser_pulses");
	this->n_set_standby = (PScanLabPtr_n_set_standby)_loadScanLabAddress(hLibrary, "n_set_standby");
	this->n_get_last_error = (PScanLabPtr_n_get_last_error)_loadScanLabAddress(hLibrary, "n_get_last_error");
	this->get_last_error = (PScanLabPtr_get_last_error)_loadScanLabAddress(hLibrary, "get_last_error");
	this->n_load_program_file = (PScanLabPtr_n_load_program_file)_loadScanLabAddress(hLibrary, "n_load_program_file");
	this->n_get_table_para = (PScanLabPtr_n_get_table_para)_loadScanLabAddress(hLibrary, "n_get_table_para");
	this->n_set_end_of_list = (PScanLabPtr_n_set_end_of_list)_loadScanLabAddress(hLibrary, "n_set_end_of_list");
	this->n_execute_list_pos = (PScanLabPtr_n_execute_list_pos)_loadScanLabAddress(hLibrary, "n_execute_list_pos");
	this->n_auto_change_pos = (PScanLabPtr_n_auto_change_pos)_loadScanLabAddress(hLibrary, "n_auto_change_pos");
	this->n_set_scanner_delays = (PScanLabPtr_n_set_scanner_delays)_loadScanLabAddress(hLibrary, "n_set_scanner_delays");
	this->n_set_mark_speed = (PScanLabPtr_n_set_mark_speed)_loadScanLabAddress(hLibrary, "n_set_mark_speed");
	this->n_set_jump_speed = (PScanLabPtr_n_set_jump_speed)_loadScanLabAddress(hLibrary, "n_set_jump_speed");
	this->n_write_io_port = (PScanLabPtr_n_write_io_port)_loadScanLabAddress(hLibrary, "n_write_io_port");
	this->n_write_8bit_port = (PScanLabPtr_n_write_8bit_port)_loadScanLabAddress(hLibrary, "n_write_8bit_port");
	this->n_write_da_1 = (PScanLabPtr_n_write_da_1)_loadScanLabAddress(hLibrary, "n_write_da_1");
	this->n_write_da_2 = (PScanLabPtr_n_write_da_2)_loadScanLabAddress(hLibrary, "n_write_da_2");
	this->n_write_io_port_list = (PScanLabPtr_n_write_io_port)_loadScanLabAddress(hLibrary, "n_write_io_port_list");
	this->n_write_8bit_port_list = (PScanLabPtr_n_write_8bit_port)_loadScanLabAddress(hLibrary, "n_write_8bit_port_list");
	this->n_write_da_1_list = (PScanLabPtr_n_write_da_1)_loadScanLabAddress(hLibrary, "n_write_da_1_list");
	this->n_write_da_2_list = (PScanLabPtr_n_write_da_2)_loadScanLabAddress(hLibrary, "n_write_da_2_list");
	this->n_jump_abs = (PScanLabPtr_n_jump_abs)_loadScanLabAddress(hLibrary, "n_jump_abs");
	this->n_jump_abs_3d = (PScanLabPtr_n_jump_abs_3d)_loadScanLabAddress(hLibrary, "n_jump_abs_3d");
	this->n_mark_abs = (PScanLabPtr_n_mark_abs)_loadScanLabAddress(hLibrary, "n_mark_abs");
	this->n_mark_abs_3d = (PScanLabPtr_n_mark_abs_3d)_loadScanLabAddress(hLibrary, "n_mark_abs_3d");
	this->n_long_delay = (PScanLabPtr_n_long_delay)_loadScanLabAddress(hLibrary, "n_long_delay");
	this->n_get_status = (PScanLabPtr_n_get_status)_loadScanLabAddress(hLibrary, "n_get_status");
	this->n_get_input_pointer = (PScanLabPtr_n_get_input_pointer)_loadScanLabAddress(hLibrary, "n_get_input_pointer");
	this->n_set_laser_delays = (PScanLabPtr_n_set_laser_delays)_loadScanLabAddress(hLibrary, "n_set_laser_delays");
	this->n_set_start_list_pos = (PScanLabPtr_n_set_start_list_pos)_loadScanLabAddress(hLibrary, "n_set_start_list_pos");
	this->n_set_defocus_list = (PScanLabPtr_n_set_defocus_list)_loadScanLabAddress(hLibrary, "n_set_defocus_list");
	this->n_get_head_status = (PScanLabPtr_n_get_head_status)_loadScanLabAddress(hLibrary, "n_get_head_status");
	this->n_get_value = (PScanLabPtr_n_get_value)_loadScanLabAddress(hLibrary, "n_get_value");
	this->get_dll_version = (PScanLabPtr_get_dll_version)_loadScanLabAddress(hLibrary, "get_dll_version");
	this->n_get_hex_version = (PScanLabPtr_n_get_hex_version)_loadScanLabAddress(hLibrary, "n_get_hex_version");
	this->n_get_bios_version = (PScanLabPtr_n_get_bios_version)_loadScanLabAddress(hLibrary, "n_get_bios_version");
	this->n_get_rtc_version = (PScanLabPtr_n_get_rtc_version)_loadScanLabAddress(hLibrary, "n_get_rtc_version");
	this->n_get_card_type = (PScanLabPtr_n_get_card_type)_loadScanLabAddress(hLibrary, "n_get_card_type");

	m_LibraryHandle = (void*) hLibrary;
}


CScanLabSDK::~CScanLabSDK()
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

void CScanLabSDK::initDLL()
{
	if (!m_bIsInitialized) {

		if (init_rtc6_dll == nullptr)
			throw std::runtime_error("RTC DLL not loaded");

		init_rtc6_dll();
		m_bIsInitialized = true;
	}
}

void CScanLabSDK::checkError(uint32_t nRTCError)
{
	if (nRTCError != 0)
		throw std::runtime_error("RTC Error: " + std::to_string (nRTCError));
}

void CScanLabSDK::resetFunctionPtrs()
{
	init_rtc6_dll = nullptr;
	free_rtc6_dll = nullptr;
	eth_convert_string_to_ip = nullptr;
	eth_set_search_cards_timeout = nullptr;
	eth_search_cards_range = nullptr;
	rtc6_count_cards = nullptr;
	eth_count_cards = nullptr;
	eth_found_cards = nullptr;
	eth_assign_card = nullptr;
	acquire_rtc = nullptr;
	release_rtc = nullptr;
	n_get_serial_number = nullptr;
	eth_get_serial_search = nullptr;
	eth_search_cards = nullptr;
	n_load_correction_file = nullptr;
	n_select_cor_table = nullptr;
	n_eth_set_com_timeouts_auto = nullptr;
	n_eth_get_com_timeouts_auto = nullptr;

	n_config_list = nullptr;
	n_set_laser_mode = nullptr;
	n_set_laser_control = nullptr;
	n_set_auto_laser_control = nullptr;
	n_set_laser_pulses = nullptr;
	n_set_standby = nullptr;
	n_get_last_error = nullptr;
	get_last_error = nullptr;
	n_load_program_file = nullptr;
	n_get_table_para = nullptr;
	n_set_end_of_list = nullptr;
	n_execute_list_pos = nullptr;
	n_auto_change_pos = nullptr;
	n_set_scanner_delays = nullptr;
	n_set_mark_speed = nullptr;
	n_set_jump_speed = nullptr;
	n_write_io_port = nullptr;
	n_write_8bit_port = nullptr;
	n_write_da_1 = nullptr;
	n_write_da_2 = nullptr;
	n_write_io_port_list = nullptr;
	n_write_8bit_port_list = nullptr;
	n_write_da_1_list = nullptr;
	n_write_da_2_list = nullptr;
	n_jump_abs = nullptr;
	n_mark_abs = nullptr;
	n_jump_abs_3d = nullptr;
	n_mark_abs_3d = nullptr;
	n_long_delay = nullptr;
	n_get_status = nullptr;
	n_get_input_pointer = nullptr;
	n_set_laser_delays = nullptr;
	n_set_start_list_pos = nullptr;
	n_set_defocus_list = nullptr;
}

