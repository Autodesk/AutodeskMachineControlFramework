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
void* _loadScanLabAddress (HMODULE hLibrary, const char * pSymbolName, bool bMandatory = true) {
	void * pFuncPtr = (void*) GetProcAddress(hLibrary, pSymbolName);
	if (bMandatory) {
		if (pFuncPtr == nullptr)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));
	}

	return pFuncPtr;
}
#else
void* _loadScanLabAddress(void * hLibrary, const char* pSymbolName, bool bMandatory = true) {

	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (bMandatory) {
		if (pFuncPtr == nullptr)
			throw ELibMCDriver_ScanLabInterfaceException(LIBMCDRIVER_SCANLAB_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));
	}

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

	this->n_set_mcbsp_freq = (PScanLabPtr_n_set_mcbsp_freq)_loadScanLabAddress(hLibrary, "n_set_mcbsp_freq");
	this->n_mcbsp_init = (PScanLabPtr_n_mcbsp_init)_loadScanLabAddress(hLibrary, "n_mcbsp_init");
	this->n_mcbsp_init_spi = (PScanLabPtr_n_mcbsp_init_spi)_loadScanLabAddress(hLibrary, "n_mcbsp_init_spi");
	this->n_set_mcbsp_out_ptr = (PScanLabPtr_n_set_mcbsp_out_ptr)_loadScanLabAddress(hLibrary, "n_set_mcbsp_out_ptr");
	this->n_set_multi_mcbsp_in = (PScanLabPtr_n_set_multi_mcbsp_in)_loadScanLabAddress(hLibrary, "n_set_multi_mcbsp_in");
	this->n_list_nop = (PScanLabPtr_n_list_nop)_loadScanLabAddress(hLibrary, "n_list_nop");
	this->n_set_free_variable_list = (PScanLabPtr_n_set_free_variable_list)_loadScanLabAddress(hLibrary, "n_set_free_variable_list");
	this->n_set_free_variable = (PScanLabPtr_n_set_free_variable)_loadScanLabAddress(hLibrary, "n_set_free_variable");
	this->n_get_free_variable = (PScanLabPtr_n_get_free_variable)_loadScanLabAddress(hLibrary, "n_get_free_variable");
	this->n_set_trigger = (PScanLabPtr_n_set_trigger)_loadScanLabAddress(hLibrary, "n_set_trigger");
	this->n_set_trigger4 = (PScanLabPtr_n_set_trigger4)_loadScanLabAddress(hLibrary, "n_set_trigger4");
	this->n_set_control_mode = (PScanLabPtr_n_set_control_mode)_loadScanLabAddress(hLibrary, "n_set_control_mode");
	this->n_set_laser_pulses_ctrl = (PScanLabPtr_n_set_laser_pulses_ctrl)_loadScanLabAddress(hLibrary, "n_set_laser_pulses_ctrl");
	this->n_set_mark_speed_ctrl = (PScanLabPtr_n_set_mark_speed_ctrl)_loadScanLabAddress(hLibrary, "n_set_mark_speed_ctrl");
	this->n_set_jump_speed_ctrl = (PScanLabPtr_n_set_jump_speed_ctrl)_loadScanLabAddress(hLibrary, "n_set_jump_speed_ctrl");
	this->n_set_firstpulse_killer = (PScanLabPtr_n_set_firstpulse_killer)_loadScanLabAddress(hLibrary, "n_set_firstpulse_killer");
	this->n_set_firstpulse_killer_list = (PScanLabPtr_n_set_firstpulse_killer_list)_loadScanLabAddress(hLibrary, "n_set_firstpulse_killer_list");
	this->n_set_qswitch_delay = (PScanLabPtr_n_set_qswitch_delay)_loadScanLabAddress(hLibrary, "n_set_qswitch_delay");
	this->n_set_qswitch_delay_list = (PScanLabPtr_n_set_qswitch_delay_list)_loadScanLabAddress(hLibrary, "n_set_qswitch_delay_list");
	this->n_write_da_x = (PScanLabPtr_n_write_da_x)_loadScanLabAddress(hLibrary, "n_write_da_x");
	this->n_set_laser_pin_out = (PScanLabPtr_n_set_laser_pin_out)_loadScanLabAddress(hLibrary, "n_set_laser_pin_out");
	this->n_get_laser_pin_in = (PScanLabPtr_n_get_laser_pin_in)_loadScanLabAddress(hLibrary, "n_get_laser_pin_in");
	this->n_set_sky_writing_para = (PScanLabPtr_n_set_sky_writing_para)_loadScanLabAddress(hLibrary, "n_set_sky_writing_para");
	this->n_set_sky_writing_limit = (PScanLabPtr_n_set_sky_writing_limit)_loadScanLabAddress(hLibrary, "n_set_sky_writing_limit");
	this->n_set_sky_writing_mode = (PScanLabPtr_n_set_sky_writing_mode)_loadScanLabAddress(hLibrary, "n_set_sky_writing_mode");
	this->n_set_sky_writing = (PScanLabPtr_n_set_sky_writing)_loadScanLabAddress(hLibrary, "n_set_sky_writing");
	this->n_set_sky_writing_para_list = (PScanLabPtr_n_set_sky_writing_para_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_para_list");
	this->n_set_sky_writing_list = (PScanLabPtr_n_set_sky_writing_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_list");
	this->n_set_sky_writing_limit_list = (PScanLabPtr_n_set_sky_writing_limit_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_limit_list");
	this->n_set_sky_writing_mode_list = (PScanLabPtr_n_set_sky_writing_mode_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_mode_list");
	this->n_control_command = (PScanLabPtr_n_control_command)_loadScanLabAddress(hLibrary, "n_control_command");
	this->n_get_scanahead_params = (PScanLabPtr_n_get_scanahead_params)_loadScanLabAddress(hLibrary, "n_get_scanahead_params", false);
	this->n_activate_scanahead_autodelays = (PScanLabPtr_n_activate_scanahead_autodelays)_loadScanLabAddress(hLibrary, "n_activate_scanahead_autodelays", false);
	this->n_set_scanahead_laser_shifts = (PScanLabPtr_n_set_scanahead_laser_shifts)_loadScanLabAddress(hLibrary, "n_set_scanahead_laser_shifts", false);
	this->n_set_scanahead_line_params = (PScanLabPtr_n_set_scanahead_line_params)_loadScanLabAddress(hLibrary, "n_set_scanahead_line_params", false);
	this->n_set_scanahead_line_params_ex = (PScanLabPtr_n_set_scanahead_line_params_ex)_loadScanLabAddress(hLibrary, "n_set_scanahead_line_params_ex", false);
	this->n_set_scanahead_params = (PScanLabPtr_n_set_scanahead_params)_loadScanLabAddress(hLibrary, "n_set_scanahead_params", false);
	this->n_set_scanahead_speed_control = (PScanLabPtr_n_set_scanahead_speed_control)_loadScanLabAddress(hLibrary, "n_set_scanahead_speed_control", false);
	this->n_micro_vector_abs_3d = (PScanLabPtr_n_micro_vector_abs_3d)_loadScanLabAddress(hLibrary, "n_micro_vector_abs_3d", false);
	this->n_micro_vector_rel_3d = (PScanLabPtr_n_micro_vector_rel_3d)_loadScanLabAddress(hLibrary, "n_micro_vector_rel_3d", false);
	this->n_micro_vector_abs = (PScanLabPtr_n_micro_vector_abs)_loadScanLabAddress(hLibrary, "n_micro_vector_abs", false);
	this->n_micro_vector_rel = (PScanLabPtr_n_micro_vector_rel)_loadScanLabAddress(hLibrary, "n_micro_vector_rel", false);

	this->n_get_error = (PScanLabPtr_n_get_error)_loadScanLabAddress(hLibrary, "n_get_error");
	this->n_reset_error = (PScanLabPtr_n_reset_error)_loadScanLabAddress(hLibrary, "n_reset_error");

	this->n_set_multi_mcbsp_in_list = (PScanLabPtr_n_set_multi_mcbsp_in_list)_loadScanLabAddress(hLibrary, "n_set_multi_mcbsp_in_list");
	this->n_set_laser_power = (PScanLabPtr_n_set_laser_power)_loadScanLabAddress(hLibrary, "n_set_laser_power");
	this->n_set_angle = (PScanLabPtr_n_set_angle)_loadScanLabAddress(hLibrary, "n_set_angle");
	this->n_set_scale = (PScanLabPtr_n_set_scale)_loadScanLabAddress(hLibrary, "n_set_scale");
	this->n_set_offset = (PScanLabPtr_n_set_offset)_loadScanLabAddress(hLibrary, "n_set_offset");
	this->n_set_matrix = (PScanLabPtr_n_set_matrix)_loadScanLabAddress(hLibrary, "n_set_matrix");

	this->n_get_waveform_offset = (PScanLabPtr_n_get_waveform_offset)_loadScanLabAddress(hLibrary, "n_get_waveform_offset");
	this->n_measurement_status = (PScanLabPtr_n_measurement_status)_loadScanLabAddress(hLibrary, "n_measurement_status");
	this->transform = (PScanLabPtr_transform)_loadScanLabAddress(hLibrary, "transform");
	this->n_upload_transform = (PScanLabPtr_n_upload_transform)_loadScanLabAddress(hLibrary, "n_upload_transform");
	this->n_set_timelag_compensation = (PScanLabPtr_n_set_timelag_compensation)_loadScanLabAddress(hLibrary, "n_set_timelag_compensation");

	this->n_init_fly_2d = (PScanLabPtr_n_init_fly_2d)_loadScanLabAddress(hLibrary, "n_init_fly_2d");
	this->n_activate_fly_2d = (PScanLabPtr_n_activate_fly_2d)_loadScanLabAddress(hLibrary, "n_activate_fly_2d");
	this->n_activate_fly_2d_encoder = (PScanLabPtr_n_activate_fly_2d_encoder)_loadScanLabAddress(hLibrary, "n_activate_fly_2d_encoder");
	this->n_set_fly_2d = (PScanLabPtr_n_set_fly_2d)_loadScanLabAddress(hLibrary, "n_set_fly_2d");
	this->n_get_fly_2d_offset = (PScanLabPtr_n_get_fly_2d_offset)_loadScanLabAddress(hLibrary, "n_get_fly_2d_offset");
	this->n_set_fly_x_pos = (PScanLabPtr_n_set_fly_x_pos)_loadScanLabAddress(hLibrary, "n_set_fly_x_pos");
	this->n_set_fly_y_pos = (PScanLabPtr_n_set_fly_y_pos)_loadScanLabAddress(hLibrary, "n_set_fly_y_pos");
	this->n_set_fly_x = (PScanLabPtr_n_set_fly_x)_loadScanLabAddress(hLibrary, "n_set_fly_x");
	this->n_set_fly_y = (PScanLabPtr_n_set_fly_y)_loadScanLabAddress(hLibrary, "n_set_fly_y");	
	this->n_get_encoder = (PScanLabPtr_n_get_encoder)_loadScanLabAddress(hLibrary, "n_get_encoder");
	this->n_get_marking_info = (PScanLabPtr_n_get_marking_info)_loadScanLabAddress(hLibrary, "n_get_marking_info");
	this->n_wait_for_encoder = (PScanLabPtr_n_wait_for_encoder)_loadScanLabAddress(hLibrary, "n_wait_for_encoder");
	this->n_wait_for_encoder_mode = (PScanLabPtr_n_wait_for_encoder_mode)_loadScanLabAddress(hLibrary, "n_wait_for_encoder_mode");
	this->n_set_fly_limits = (PScanLabPtr_n_set_fly_limits)_loadScanLabAddress(hLibrary, "n_set_fly_limits");
	this->n_range_checking = (PScanLabPtr_n_range_checking)_loadScanLabAddress(hLibrary, "n_range_checking");

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

void CScanLabSDK::checkLastErrorOfCard(uint32_t nCardNo)
{
	checkError(n_get_last_error(nCardNo));
}

void CScanLabSDK::checkGlobalErrorOfCard(uint32_t nCardNo)
{
	uint32_t nError = n_get_error(nCardNo);
	checkError(nError);
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

	n_get_head_status = nullptr;
	n_get_value = nullptr;
	get_dll_version = nullptr;
	n_get_hex_version = nullptr;
	n_get_bios_version = nullptr;
	n_get_rtc_version = nullptr;
	n_get_card_type = nullptr;

	n_set_mcbsp_freq = nullptr;
	n_mcbsp_init = nullptr;
	n_mcbsp_init_spi = nullptr;
	n_set_mcbsp_out_ptr = nullptr;
	n_set_multi_mcbsp_in = nullptr;
	n_list_nop = nullptr;
	n_set_free_variable_list = nullptr;
	n_set_free_variable = nullptr;
	n_get_free_variable = nullptr;
	n_set_trigger = nullptr;
	n_set_trigger4 = nullptr;
	n_set_control_mode = nullptr;
	n_set_laser_pulses_ctrl = nullptr;
	n_set_mark_speed_ctrl = nullptr;
	n_set_jump_speed_ctrl = nullptr;
	n_set_firstpulse_killer = nullptr;
	n_set_firstpulse_killer_list = nullptr;
	n_set_qswitch_delay = nullptr;
	n_set_qswitch_delay_list = nullptr;
	n_write_da_x = nullptr;
	n_set_laser_pin_out = nullptr;
	n_get_laser_pin_in = nullptr;
	n_set_sky_writing_para = nullptr;
	n_set_sky_writing_limit = nullptr;
	n_set_sky_writing_mode = nullptr;
	n_set_sky_writing = nullptr;
	n_set_sky_writing_para_list = nullptr;
	n_set_sky_writing_list = nullptr;
	n_set_sky_writing_limit_list = nullptr;
	n_set_sky_writing_mode_list = nullptr;
	n_control_command = nullptr;
	n_get_scanahead_params = nullptr;
	n_activate_scanahead_autodelays = nullptr;
	n_set_scanahead_laser_shifts = nullptr;
	n_set_scanahead_line_params = nullptr;
	n_set_scanahead_line_params_ex = nullptr;
	n_set_scanahead_params = nullptr;
	n_set_scanahead_speed_control = nullptr;
	n_micro_vector_abs_3d = nullptr;
	n_micro_vector_rel_3d = nullptr;
	n_micro_vector_abs = nullptr;
	n_micro_vector_rel = nullptr;
	n_get_error = nullptr;
	n_reset_error = nullptr;

	n_set_multi_mcbsp_in_list = nullptr;
	n_set_laser_power = nullptr;

	n_set_angle = nullptr;
	n_set_scale = nullptr;
	n_set_offset = nullptr;
	n_set_matrix = nullptr;

	n_get_waveform_offset = nullptr;
	n_measurement_status = nullptr;
	transform = nullptr;
	n_upload_transform = nullptr;
	n_set_timelag_compensation = nullptr;

	n_init_fly_2d = nullptr;
	n_activate_fly_2d = nullptr;
	n_set_fly_2d = nullptr;
	n_activate_fly_2d_encoder = nullptr;
	n_get_fly_2d_offset = nullptr;

	n_set_fly_x_pos = nullptr;
	n_set_fly_y_pos = nullptr;
	n_set_fly_x = nullptr;
	n_set_fly_y = nullptr;
	n_get_encoder = nullptr;

	n_get_marking_info = nullptr;
	n_wait_for_encoder = nullptr;
	n_wait_for_encoder_mode = nullptr;
	n_set_fly_limits = nullptr;
	n_range_checking = nullptr;

}

