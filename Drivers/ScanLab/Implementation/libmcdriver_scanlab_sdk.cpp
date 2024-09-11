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


CScanLabSDKJournal::CScanLabSDKJournal(const std::string& sDebugFileName)	
{

	m_CStream.open(sDebugFileName, std::ios::out);
	if (!m_CStream.is_open())
		throw std::runtime_error("could not create sdk debug log: " + sDebugFileName);
}

CScanLabSDKJournal::~CScanLabSDKJournal()
{
}

void CScanLabSDKJournal::writeCLine(const std::string& sLine)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	m_CStream << sLine << std::endl;

}


void CScanLabSDKJournal::logCall(const std::string& sFunctionName, const std::string& sCParameters)
{
	writeCLine(sFunctionName + "(" + sCParameters + ");");
}

std::string CScanLabSDKJournal::getVariableSuffix(const std::string& sVariableBaseName)
{
	auto iIter = m_DefinedVariables.find(sVariableBaseName);
	if (iIter == m_DefinedVariables.end()) {
		m_DefinedVariables.insert(std::make_pair (sVariableBaseName, 1));
		return "1";
	}
	else {
		iIter->second++;
		return std::to_string(iIter->second);
	}
}


std::string CScanLabSDKJournal::defineDoubleVariable(const std::string& sVariableBaseName)
{
	std::string sVariableName = sVariableBaseName + getVariableSuffix (sVariableBaseName);

	writeCLine("double " + sVariableName + ";");
	return sVariableName;
}

std::string CScanLabSDKJournal::defineUint32Variable(const std::string& sVariableBaseName)
{
	std::string sVariableName = sVariableBaseName + getVariableSuffix(sVariableBaseName);

	writeCLine("uint32_t " + sVariableName + ";");
	return sVariableName;

}

std::string CScanLabSDKJournal::defineInt32Variable(const std::string& sVariableBaseName)
{
	std::string sVariableName = sVariableBaseName + getVariableSuffix(sVariableBaseName);

	writeCLine("int32_t " + sVariableName + ";");
	return sVariableName;

}

std::string CScanLabSDKJournal::escapeString(const std::string& sString)
{
	return "\"" + sString + "\"";
}


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


	this->ptr_init_rtc6_dll = (PScanLabPtr_init_rtc6_dll)_loadScanLabAddress(hLibrary, "init_rtc6_dll");
	this->ptr_free_rtc6_dll = (PScanLabPtr_free_rtc6_dll)_loadScanLabAddress(hLibrary, "free_rtc6_dll");
	this->ptr_eth_convert_string_to_ip = (PScanLabPtr_eth_convert_string_to_ip)_loadScanLabAddress(hLibrary, "eth_convert_string_to_ip");
	this->ptr_eth_set_search_cards_timeout = (PScanLabPtr_eth_set_search_cards_timeout)_loadScanLabAddress(hLibrary, "eth_set_search_cards_timeout");
	this->ptr_eth_search_cards_range = (PScanLabPtr_eth_search_cards_range)_loadScanLabAddress(hLibrary, "eth_search_cards_range");
	this->ptr_rtc6_count_cards = (PScanLabPtr_rtc6_count_cards)_loadScanLabAddress(hLibrary, "rtc6_count_cards");
	this->ptr_eth_count_cards = (PScanLabPtr_eth_count_cards)_loadScanLabAddress(hLibrary, "eth_count_cards");
	this->ptr_eth_found_cards = (PScanLabPtr_eth_found_cards)_loadScanLabAddress(hLibrary, "eth_found_cards");
	this->ptr_eth_assign_card = (PScanLabPtr_eth_assign_card)_loadScanLabAddress(hLibrary, "eth_assign_card");
	this->ptr_eth_remove_card = (PScanLabPtr_eth_remove_card)_loadScanLabAddress(hLibrary, "eth_remove_card");
	this->ptr_acquire_rtc = (PScanLabPtr_acquire_rtc)_loadScanLabAddress(hLibrary, "acquire_rtc");
	this->ptr_release_rtc = (PScanLabPtr_release_rtc)_loadScanLabAddress(hLibrary, "release_rtc");
	this->ptr_n_get_serial_number = (PScanLabPtr_n_get_serial_number)_loadScanLabAddress(hLibrary, "n_get_serial_number");
	this->ptr_eth_get_serial_search = (PScanLabPtr_eth_get_serial_search)_loadScanLabAddress(hLibrary, "eth_get_serial_search");
	this->ptr_eth_search_cards = (PScanLabPtr_eth_search_cards)_loadScanLabAddress(hLibrary, "eth_search_cards");
	this->ptr_n_load_correction_file = (PScanLabPtr_n_load_correction_file)_loadScanLabAddress(hLibrary, "n_load_correction_file");
	this->ptr_n_select_cor_table = (PScanLabPtr_n_select_cor_table)_loadScanLabAddress(hLibrary, "n_select_cor_table");
	this->ptr_n_eth_set_com_timeouts_auto = (PScanLabPtr_n_eth_set_com_timeouts_auto)_loadScanLabAddress(hLibrary, "n_eth_set_com_timeouts_auto");
	this->ptr_n_eth_get_com_timeouts_auto = (PScanLabPtr_n_eth_get_com_timeouts_auto)_loadScanLabAddress(hLibrary, "n_eth_get_com_timeouts_auto");

	this->ptr_n_config_list = (PScanLabPtr_n_config_list)_loadScanLabAddress(hLibrary, "n_config_list");
	this->ptr_n_set_laser_mode = (PScanLabPtr_n_set_laser_mode)_loadScanLabAddress(hLibrary, "n_set_laser_mode");
	this->ptr_n_set_laser_control = (PScanLabPtr_n_set_laser_control)_loadScanLabAddress(hLibrary, "n_set_laser_control");
	this->ptr_n_set_auto_laser_control = (PScanLabPtr_n_set_auto_laser_control)_loadScanLabAddress(hLibrary, "n_set_auto_laser_control");
	this->ptr_n_set_laser_pulses = (PScanLabPtr_n_set_laser_pulses)_loadScanLabAddress(hLibrary, "n_set_laser_pulses");
	this->ptr_n_set_standby = (PScanLabPtr_n_set_standby)_loadScanLabAddress(hLibrary, "n_set_standby");
	this->ptr_n_get_last_error = (PScanLabPtr_n_get_last_error)_loadScanLabAddress(hLibrary, "n_get_last_error");
	this->ptr_get_last_error = (PScanLabPtr_get_last_error)_loadScanLabAddress(hLibrary, "get_last_error");
	this->ptr_n_load_program_file = (PScanLabPtr_n_load_program_file)_loadScanLabAddress(hLibrary, "n_load_program_file");
	this->ptr_n_get_table_para = (PScanLabPtr_n_get_table_para)_loadScanLabAddress(hLibrary, "n_get_table_para");
	this->ptr_n_set_end_of_list = (PScanLabPtr_n_set_end_of_list)_loadScanLabAddress(hLibrary, "n_set_end_of_list");
	this->ptr_n_execute_list_pos = (PScanLabPtr_n_execute_list_pos)_loadScanLabAddress(hLibrary, "n_execute_list_pos");
	this->ptr_n_auto_change_pos = (PScanLabPtr_n_auto_change_pos)_loadScanLabAddress(hLibrary, "n_auto_change_pos");
	this->ptr_n_set_scanner_delays = (PScanLabPtr_n_set_scanner_delays)_loadScanLabAddress(hLibrary, "n_set_scanner_delays");
	this->ptr_n_set_mark_speed = (PScanLabPtr_n_set_mark_speed)_loadScanLabAddress(hLibrary, "n_set_mark_speed");
	this->ptr_n_set_jump_speed = (PScanLabPtr_n_set_jump_speed)_loadScanLabAddress(hLibrary, "n_set_jump_speed");
	this->ptr_n_write_io_port = (PScanLabPtr_n_write_io_port)_loadScanLabAddress(hLibrary, "n_write_io_port");
	this->ptr_n_write_8bit_port = (PScanLabPtr_n_write_8bit_port)_loadScanLabAddress(hLibrary, "n_write_8bit_port");
	this->ptr_n_write_da_1 = (PScanLabPtr_n_write_da_1)_loadScanLabAddress(hLibrary, "n_write_da_1");
	this->ptr_n_write_da_2 = (PScanLabPtr_n_write_da_2)_loadScanLabAddress(hLibrary, "n_write_da_2");
	this->ptr_n_write_io_port_list = (PScanLabPtr_n_write_io_port_list)_loadScanLabAddress(hLibrary, "n_write_io_port_list");
	this->ptr_n_write_io_port_mask_list = (PScanLabPtr_n_write_io_port_mask_list)_loadScanLabAddress(hLibrary, "n_write_io_port_mask_list");
	this->ptr_n_write_8bit_port_list = (PScanLabPtr_n_write_8bit_port)_loadScanLabAddress(hLibrary, "n_write_8bit_port_list");
	this->ptr_n_write_da_1_list = (PScanLabPtr_n_write_da_1)_loadScanLabAddress(hLibrary, "n_write_da_1_list");
	this->ptr_n_write_da_2_list = (PScanLabPtr_n_write_da_2)_loadScanLabAddress(hLibrary, "n_write_da_2_list");
	this->ptr_n_jump_abs = (PScanLabPtr_n_jump_abs)_loadScanLabAddress(hLibrary, "n_jump_abs");
	this->ptr_n_mark_abs = (PScanLabPtr_n_mark_abs)_loadScanLabAddress(hLibrary, "n_mark_abs");
	this->ptr_n_long_delay = (PScanLabPtr_n_long_delay)_loadScanLabAddress(hLibrary, "n_long_delay");
	this->ptr_n_get_status = (PScanLabPtr_n_get_status)_loadScanLabAddress(hLibrary, "n_get_status");
	this->ptr_n_get_input_pointer = (PScanLabPtr_n_get_input_pointer)_loadScanLabAddress(hLibrary, "n_get_input_pointer");
	this->ptr_n_set_laser_delays = (PScanLabPtr_n_set_laser_delays)_loadScanLabAddress(hLibrary, "n_set_laser_delays");
	this->ptr_n_set_start_list_pos = (PScanLabPtr_n_set_start_list_pos)_loadScanLabAddress(hLibrary, "n_set_start_list_pos");
	this->ptr_n_set_defocus_list = (PScanLabPtr_n_set_defocus_list)_loadScanLabAddress(hLibrary, "n_set_defocus_list");
	this->ptr_n_get_head_status = (PScanLabPtr_n_get_head_status)_loadScanLabAddress(hLibrary, "n_get_head_status");
	this->ptr_n_get_value = (PScanLabPtr_n_get_value)_loadScanLabAddress(hLibrary, "n_get_value");
	this->ptr_get_dll_version = (PScanLabPtr_get_dll_version)_loadScanLabAddress(hLibrary, "get_dll_version");
	this->ptr_n_get_hex_version = (PScanLabPtr_n_get_hex_version)_loadScanLabAddress(hLibrary, "n_get_hex_version");
	this->ptr_n_get_bios_version = (PScanLabPtr_n_get_bios_version)_loadScanLabAddress(hLibrary, "n_get_bios_version");
	this->ptr_n_get_rtc_version = (PScanLabPtr_n_get_rtc_version)_loadScanLabAddress(hLibrary, "n_get_rtc_version");
	this->ptr_n_get_card_type = (PScanLabPtr_n_get_card_type)_loadScanLabAddress(hLibrary, "n_get_card_type");

	this->ptr_n_set_mcbsp_freq = (PScanLabPtr_n_set_mcbsp_freq)_loadScanLabAddress(hLibrary, "n_set_mcbsp_freq");
	this->ptr_n_mcbsp_init = (PScanLabPtr_n_mcbsp_init)_loadScanLabAddress(hLibrary, "n_mcbsp_init");
	this->ptr_n_mcbsp_init_spi = (PScanLabPtr_n_mcbsp_init_spi)_loadScanLabAddress(hLibrary, "n_mcbsp_init_spi");
	this->ptr_n_set_mcbsp_out_ptr = (PScanLabPtr_n_set_mcbsp_out_ptr)_loadScanLabAddress(hLibrary, "n_set_mcbsp_out_ptr");
	this->ptr_n_set_multi_mcbsp_in = (PScanLabPtr_n_set_multi_mcbsp_in)_loadScanLabAddress(hLibrary, "n_set_multi_mcbsp_in");
	this->ptr_n_list_nop = (PScanLabPtr_n_list_nop)_loadScanLabAddress(hLibrary, "n_list_nop");
	this->ptr_n_set_free_variable_list = (PScanLabPtr_n_set_free_variable_list)_loadScanLabAddress(hLibrary, "n_set_free_variable_list");
	this->ptr_n_set_free_variable = (PScanLabPtr_n_set_free_variable)_loadScanLabAddress(hLibrary, "n_set_free_variable");
	this->ptr_n_get_free_variable = (PScanLabPtr_n_get_free_variable)_loadScanLabAddress(hLibrary, "n_get_free_variable");
	this->ptr_n_set_trigger = (PScanLabPtr_n_set_trigger)_loadScanLabAddress(hLibrary, "n_set_trigger");
	this->ptr_n_set_trigger4 = (PScanLabPtr_n_set_trigger4)_loadScanLabAddress(hLibrary, "n_set_trigger4");
	this->ptr_n_set_trigger8 = (PScanLabPtr_n_set_trigger8)_loadScanLabAddress(hLibrary, "n_set_trigger8");
	this->ptr_n_set_control_mode = (PScanLabPtr_n_set_control_mode)_loadScanLabAddress(hLibrary, "n_set_control_mode");
	this->ptr_n_set_laser_pulses_ctrl = (PScanLabPtr_n_set_laser_pulses_ctrl)_loadScanLabAddress(hLibrary, "n_set_laser_pulses_ctrl");
	this->ptr_n_set_mark_speed_ctrl = (PScanLabPtr_n_set_mark_speed_ctrl)_loadScanLabAddress(hLibrary, "n_set_mark_speed_ctrl");
	this->ptr_n_set_jump_speed_ctrl = (PScanLabPtr_n_set_jump_speed_ctrl)_loadScanLabAddress(hLibrary, "n_set_jump_speed_ctrl");
	this->ptr_n_set_firstpulse_killer = (PScanLabPtr_n_set_firstpulse_killer)_loadScanLabAddress(hLibrary, "n_set_firstpulse_killer");
	this->ptr_n_set_firstpulse_killer_list = (PScanLabPtr_n_set_firstpulse_killer_list)_loadScanLabAddress(hLibrary, "n_set_firstpulse_killer_list");
	this->ptr_n_set_qswitch_delay = (PScanLabPtr_n_set_qswitch_delay)_loadScanLabAddress(hLibrary, "n_set_qswitch_delay");
	this->ptr_n_set_qswitch_delay_list = (PScanLabPtr_n_set_qswitch_delay_list)_loadScanLabAddress(hLibrary, "n_set_qswitch_delay_list");
	this->ptr_n_write_da_x = (PScanLabPtr_n_write_da_x)_loadScanLabAddress(hLibrary, "n_write_da_x");
	this->ptr_n_set_laser_pin_out = (PScanLabPtr_n_set_laser_pin_out)_loadScanLabAddress(hLibrary, "n_set_laser_pin_out");
	this->ptr_n_get_laser_pin_in = (PScanLabPtr_n_get_laser_pin_in)_loadScanLabAddress(hLibrary, "n_get_laser_pin_in");
	this->ptr_n_set_laser_pin_out_list = (PScanLabPtr_n_set_laser_pin_out_list)_loadScanLabAddress(hLibrary, "n_set_laser_pin_out_list");
	this->ptr_n_set_sky_writing_para = (PScanLabPtr_n_set_sky_writing_para)_loadScanLabAddress(hLibrary, "n_set_sky_writing_para");
	this->ptr_n_set_sky_writing_limit = (PScanLabPtr_n_set_sky_writing_limit)_loadScanLabAddress(hLibrary, "n_set_sky_writing_limit");
	this->ptr_n_set_sky_writing_mode = (PScanLabPtr_n_set_sky_writing_mode)_loadScanLabAddress(hLibrary, "n_set_sky_writing_mode");
	this->ptr_n_set_sky_writing = (PScanLabPtr_n_set_sky_writing)_loadScanLabAddress(hLibrary, "n_set_sky_writing");
	this->ptr_n_set_sky_writing_para_list = (PScanLabPtr_n_set_sky_writing_para_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_para_list");
	this->ptr_n_set_sky_writing_list = (PScanLabPtr_n_set_sky_writing_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_list");
	this->ptr_n_set_sky_writing_limit_list = (PScanLabPtr_n_set_sky_writing_limit_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_limit_list");
	this->ptr_n_set_sky_writing_mode_list = (PScanLabPtr_n_set_sky_writing_mode_list)_loadScanLabAddress(hLibrary, "n_set_sky_writing_mode_list");
	this->ptr_n_control_command = (PScanLabPtr_n_control_command)_loadScanLabAddress(hLibrary, "n_control_command");
	this->ptr_n_get_scanahead_params = (PScanLabPtr_n_get_scanahead_params)_loadScanLabAddress(hLibrary, "n_get_scanahead_params", false);
	this->ptr_n_activate_scanahead_autodelays = (PScanLabPtr_n_activate_scanahead_autodelays)_loadScanLabAddress(hLibrary, "n_activate_scanahead_autodelays", false);
	this->ptr_n_set_scanahead_laser_shifts = (PScanLabPtr_n_set_scanahead_laser_shifts)_loadScanLabAddress(hLibrary, "n_set_scanahead_laser_shifts", false);
	this->ptr_n_set_scanahead_line_params = (PScanLabPtr_n_set_scanahead_line_params)_loadScanLabAddress(hLibrary, "n_set_scanahead_line_params", false);
	this->ptr_n_set_scanahead_line_params_ex = (PScanLabPtr_n_set_scanahead_line_params_ex)_loadScanLabAddress(hLibrary, "n_set_scanahead_line_params_ex", false);
	this->ptr_n_set_scanahead_params = (PScanLabPtr_n_set_scanahead_params)_loadScanLabAddress(hLibrary, "n_set_scanahead_params", false);
	this->ptr_n_set_scanahead_speed_control = (PScanLabPtr_n_set_scanahead_speed_control)_loadScanLabAddress(hLibrary, "n_set_scanahead_speed_control", false);
	this->ptr_n_micro_vector_abs_3d = (PScanLabPtr_n_micro_vector_abs_3d)_loadScanLabAddress(hLibrary, "n_micro_vector_abs_3d", false);
	this->ptr_n_micro_vector_rel_3d = (PScanLabPtr_n_micro_vector_rel_3d)_loadScanLabAddress(hLibrary, "n_micro_vector_rel_3d", false);
	this->ptr_n_micro_vector_abs = (PScanLabPtr_n_micro_vector_abs)_loadScanLabAddress(hLibrary, "n_micro_vector_abs", false);
	this->ptr_n_micro_vector_rel = (PScanLabPtr_n_micro_vector_rel)_loadScanLabAddress(hLibrary, "n_micro_vector_rel", false);

	this->ptr_n_get_error = (PScanLabPtr_n_get_error)_loadScanLabAddress(hLibrary, "n_get_error");
	this->ptr_n_reset_error = (PScanLabPtr_n_reset_error)_loadScanLabAddress(hLibrary, "n_reset_error");

	this->ptr_n_set_multi_mcbsp_in_list = (PScanLabPtr_n_set_multi_mcbsp_in_list)_loadScanLabAddress(hLibrary, "n_set_multi_mcbsp_in_list");
	this->ptr_n_set_laser_power = (PScanLabPtr_n_set_laser_power)_loadScanLabAddress(hLibrary, "n_set_laser_power");
	this->ptr_n_set_angle = (PScanLabPtr_n_set_angle)_loadScanLabAddress(hLibrary, "n_set_angle");
	this->ptr_n_set_scale = (PScanLabPtr_n_set_scale)_loadScanLabAddress(hLibrary, "n_set_scale");
	this->ptr_n_set_offset = (PScanLabPtr_n_set_offset)_loadScanLabAddress(hLibrary, "n_set_offset");
	this->ptr_n_set_matrix = (PScanLabPtr_n_set_matrix)_loadScanLabAddress(hLibrary, "n_set_matrix");

	this->ptr_n_get_waveform_offset = (PScanLabPtr_n_get_waveform_offset)_loadScanLabAddress(hLibrary, "n_get_waveform_offset");
	this->ptr_n_measurement_status = (PScanLabPtr_n_measurement_status)_loadScanLabAddress(hLibrary, "n_measurement_status");
	this->ptr_transform = (PScanLabPtr_transform)_loadScanLabAddress(hLibrary, "transform");
	this->ptr_n_upload_transform = (PScanLabPtr_n_upload_transform)_loadScanLabAddress(hLibrary, "n_upload_transform");
	this->ptr_n_set_timelag_compensation = (PScanLabPtr_n_set_timelag_compensation)_loadScanLabAddress(hLibrary, "n_set_timelag_compensation");

	this->ptr_n_init_fly_2d = (PScanLabPtr_n_init_fly_2d)_loadScanLabAddress(hLibrary, "n_init_fly_2d");
	this->ptr_n_activate_fly_2d = (PScanLabPtr_n_activate_fly_2d)_loadScanLabAddress(hLibrary, "n_activate_fly_2d");
	this->ptr_n_activate_fly_2d_encoder = (PScanLabPtr_n_activate_fly_2d_encoder)_loadScanLabAddress(hLibrary, "n_activate_fly_2d_encoder");
	this->ptr_n_set_fly_2d = (PScanLabPtr_n_set_fly_2d)_loadScanLabAddress(hLibrary, "n_set_fly_2d");
	this->ptr_n_get_fly_2d_offset = (PScanLabPtr_n_get_fly_2d_offset)_loadScanLabAddress(hLibrary, "n_get_fly_2d_offset");
	this->ptr_n_set_fly_x_pos = (PScanLabPtr_n_set_fly_x_pos)_loadScanLabAddress(hLibrary, "n_set_fly_x_pos");
	this->ptr_n_set_fly_y_pos = (PScanLabPtr_n_set_fly_y_pos)_loadScanLabAddress(hLibrary, "n_set_fly_y_pos");
	this->ptr_n_set_fly_x = (PScanLabPtr_n_set_fly_x)_loadScanLabAddress(hLibrary, "n_set_fly_x");
	this->ptr_n_set_fly_y = (PScanLabPtr_n_set_fly_y)_loadScanLabAddress(hLibrary, "n_set_fly_y");
	this->ptr_n_fly_return = (PScanLabPtr_n_fly_return)_loadScanLabAddress(hLibrary, "n_fly_return");
	this->ptr_n_get_encoder = (PScanLabPtr_n_get_encoder)_loadScanLabAddress(hLibrary, "n_get_encoder");
	this->ptr_n_get_marking_info = (PScanLabPtr_n_get_marking_info)_loadScanLabAddress(hLibrary, "n_get_marking_info");
	this->ptr_n_wait_for_encoder = (PScanLabPtr_n_wait_for_encoder)_loadScanLabAddress(hLibrary, "n_wait_for_encoder");
	this->ptr_n_wait_for_encoder_mode = (PScanLabPtr_n_wait_for_encoder_mode)_loadScanLabAddress(hLibrary, "n_wait_for_encoder_mode");
	this->ptr_n_set_fly_limits = (PScanLabPtr_n_set_fly_limits)_loadScanLabAddress(hLibrary, "n_set_fly_limits");
	this->ptr_n_range_checking = (PScanLabPtr_n_range_checking)_loadScanLabAddress(hLibrary, "n_range_checking");
	this->ptr_n_stop_execution = (PScanLabPtr_n_stop_execution)_loadScanLabAddress(hLibrary, "n_stop_execution");
	this->ptr_n_timed_mark_abs = (PScanLabPtr_n_timed_mark_abs)_loadScanLabAddress(hLibrary, "n_timed_mark_abs");
	this->ptr_n_read_multi_mcbsp = (PScanLabPtr_n_read_multi_mcbsp)_loadScanLabAddress(hLibrary, "n_read_multi_mcbsp");
	this->ptr_n_uart_config = (PScanLabPtr_n_uart_config)_loadScanLabAddress(hLibrary, "n_uart_config");
	this->ptr_n_rs232_write_data = (PScanLabPtr_n_rs232_write_data)_loadScanLabAddress(hLibrary, "n_rs232_write_data");
	this->ptr_n_rs232_read_data = (PScanLabPtr_n_rs232_read_data)_loadScanLabAddress(hLibrary, "n_rs232_read_data");

	this->ptr_n_set_mcbsp_out_oie_ctrl = (PScanLabPtr_n_set_mcbsp_out_oie_ctrl)_loadScanLabAddress(hLibrary, "n_set_mcbsp_out_oie_ctrl", false);
	this->ptr_n_eth_config_waveform_streaming_ctrl = (PScanLabPtr_n_eth_config_waveform_streaming_ctrl)_loadScanLabAddress(hLibrary, "n_eth_config_waveform_streaming_ctrl", false);
	this->ptr_n_eth_set_high_performance_mode = (PScanLabPtr_n_eth_set_high_performance_mode)_loadScanLabAddress(hLibrary, "n_eth_set_high_performance_mode", false);

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

		if (ptr_init_rtc6_dll == nullptr)
			throw std::runtime_error("RTC DLL not loaded");

		ptr_init_rtc6_dll();
		m_bIsInitialized = true;
	
	}
}

void CScanLabSDK::reinitDLL()
{
	if (ptr_free_rtc6_dll == nullptr)
		throw std::runtime_error("RTC DLL not loaded");

	ptr_free_rtc6_dll();
	m_bIsInitialized = false;

	initDLL();

}


void CScanLabSDK::checkError(uint32_t nRTCError)
{
	if (nRTCError != 0)
		throw std::runtime_error("RTC Error: " + std::to_string (nRTCError));
}

void CScanLabSDK::checkLastErrorOfCard(uint32_t nCardNo)
{
	checkError(ptr_n_get_last_error(nCardNo));
}

void CScanLabSDK::checkGlobalErrorOfCard(uint32_t nCardNo)
{
	uint32_t nError = ptr_n_get_error(nCardNo);
	checkError(nError);
}


void CScanLabSDK::resetFunctionPtrs()
{
	ptr_init_rtc6_dll = nullptr;
	ptr_free_rtc6_dll = nullptr;
	ptr_eth_convert_string_to_ip = nullptr;
	ptr_eth_set_search_cards_timeout = nullptr;
	ptr_eth_search_cards_range = nullptr;
	ptr_rtc6_count_cards = nullptr;
	ptr_eth_count_cards = nullptr;
	ptr_eth_found_cards = nullptr;
	ptr_eth_assign_card = nullptr;
	ptr_eth_remove_card = nullptr;
	ptr_acquire_rtc = nullptr;
	ptr_release_rtc = nullptr;
	ptr_n_get_serial_number = nullptr;
	ptr_eth_get_serial_search = nullptr;
	ptr_eth_search_cards = nullptr;
	ptr_n_load_correction_file = nullptr;
	ptr_n_select_cor_table = nullptr;
	ptr_n_eth_set_com_timeouts_auto = nullptr;
	ptr_n_eth_get_com_timeouts_auto = nullptr;
	
	ptr_n_config_list = nullptr;
	ptr_n_set_laser_mode = nullptr;
	ptr_n_set_laser_control = nullptr;
	ptr_n_set_auto_laser_control = nullptr;
	ptr_n_set_laser_pulses = nullptr;
	ptr_n_set_standby = nullptr;
	ptr_n_get_last_error = nullptr;
	ptr_get_last_error = nullptr;
	ptr_n_load_program_file = nullptr;
	ptr_n_get_table_para = nullptr;
	ptr_n_set_end_of_list = nullptr;
	ptr_n_execute_list_pos = nullptr;
	ptr_n_auto_change_pos = nullptr;
	ptr_n_set_scanner_delays = nullptr;
	ptr_n_set_mark_speed = nullptr;
	ptr_n_set_jump_speed = nullptr;
	ptr_n_write_io_port = nullptr;
	ptr_n_write_8bit_port = nullptr;
	ptr_n_write_da_1 = nullptr;
	ptr_n_write_da_2 = nullptr;
	ptr_n_write_io_port_list = nullptr;
	ptr_n_write_io_port_mask_list = nullptr;
	ptr_n_write_8bit_port_list = nullptr;
	ptr_n_write_da_1_list = nullptr;
	ptr_n_write_da_2_list = nullptr;
	ptr_n_jump_abs = nullptr;
	ptr_n_mark_abs = nullptr;
	ptr_n_long_delay = nullptr;
	ptr_n_get_status = nullptr;
	ptr_n_get_input_pointer = nullptr;
	ptr_n_set_laser_delays = nullptr;
	ptr_n_set_start_list_pos = nullptr;
	ptr_n_set_defocus_list = nullptr;
	
	ptr_n_get_head_status = nullptr;
	ptr_n_get_value = nullptr;
	ptr_get_dll_version = nullptr;
	ptr_n_get_hex_version = nullptr;
	ptr_n_get_bios_version = nullptr;
	ptr_n_get_rtc_version = nullptr;
	ptr_n_get_card_type = nullptr;

	ptr_n_set_mcbsp_freq = nullptr;
	ptr_n_mcbsp_init = nullptr;
	ptr_n_mcbsp_init_spi = nullptr;
	ptr_n_set_mcbsp_out_ptr = nullptr;
	ptr_n_set_multi_mcbsp_in = nullptr;
	ptr_n_list_nop = nullptr;
	ptr_n_set_free_variable_list = nullptr;
	ptr_n_set_free_variable = nullptr;
	ptr_n_get_free_variable = nullptr;
	ptr_n_set_trigger = nullptr;
	ptr_n_set_trigger4 = nullptr;
	ptr_n_set_trigger8 = nullptr;
	ptr_n_set_control_mode = nullptr;
	ptr_n_set_laser_pulses_ctrl = nullptr;
	ptr_n_set_mark_speed_ctrl = nullptr;
	ptr_n_set_jump_speed_ctrl = nullptr;
	ptr_n_set_firstpulse_killer = nullptr;
	ptr_n_set_firstpulse_killer_list = nullptr;
	ptr_n_set_qswitch_delay = nullptr;
	ptr_n_set_qswitch_delay_list = nullptr;
	ptr_n_write_da_x = nullptr;
	ptr_n_set_laser_pin_out = nullptr;
	ptr_n_get_laser_pin_in = nullptr;
	ptr_n_set_laser_pin_out_list = nullptr;
	ptr_n_set_sky_writing_para = nullptr;
	ptr_n_set_sky_writing_limit = nullptr;
	ptr_n_set_sky_writing_mode = nullptr;
	ptr_n_set_sky_writing = nullptr;
	ptr_n_set_sky_writing_para_list = nullptr;
	ptr_n_set_sky_writing_list = nullptr;
	ptr_n_set_sky_writing_limit_list = nullptr;
	ptr_n_set_sky_writing_mode_list = nullptr;
	ptr_n_control_command = nullptr;
	ptr_n_get_scanahead_params = nullptr;
	ptr_n_activate_scanahead_autodelays = nullptr;
	ptr_n_set_scanahead_laser_shifts = nullptr;
	ptr_n_set_scanahead_line_params = nullptr;
	ptr_n_set_scanahead_line_params_ex = nullptr;
	ptr_n_set_scanahead_params = nullptr;
	ptr_n_set_scanahead_speed_control = nullptr;
	ptr_n_micro_vector_abs_3d = nullptr;
	ptr_n_micro_vector_rel_3d = nullptr;
	ptr_n_micro_vector_abs = nullptr;
	ptr_n_micro_vector_rel = nullptr;
	ptr_n_get_error = nullptr;
	ptr_n_reset_error = nullptr;
	
	ptr_n_set_multi_mcbsp_in_list = nullptr;
	ptr_n_set_laser_power = nullptr;
	
	ptr_n_set_angle = nullptr;
	ptr_n_set_scale = nullptr;
	ptr_n_set_offset = nullptr;
	ptr_n_set_matrix = nullptr;

	ptr_n_get_waveform_offset = nullptr;
	ptr_n_measurement_status = nullptr;
	ptr_transform = nullptr;
	ptr_n_upload_transform = nullptr;
	ptr_n_set_timelag_compensation = nullptr;

	ptr_n_init_fly_2d = nullptr;
	ptr_n_activate_fly_2d = nullptr;
	ptr_n_set_fly_2d = nullptr;
	ptr_n_activate_fly_2d_encoder = nullptr;
	ptr_n_get_fly_2d_offset = nullptr;

	ptr_n_set_fly_x_pos = nullptr;
	ptr_n_set_fly_y_pos = nullptr;
	ptr_n_set_fly_x = nullptr;
	ptr_n_set_fly_y = nullptr;
	ptr_n_get_encoder = nullptr;
	ptr_n_fly_return = nullptr;

	ptr_n_get_marking_info = nullptr;
	ptr_n_wait_for_encoder = nullptr;
	ptr_n_wait_for_encoder_mode = nullptr;
	ptr_n_set_fly_limits = nullptr;
	ptr_n_range_checking = nullptr;

	ptr_n_stop_execution = nullptr;
	ptr_n_timed_mark_abs = nullptr;
	ptr_n_read_multi_mcbsp = nullptr;

	ptr_n_uart_config = nullptr;
	ptr_n_rs232_write_data = nullptr;
	ptr_n_rs232_read_data = nullptr;

	ptr_n_set_mcbsp_out_oie_ctrl = nullptr;
	ptr_n_eth_config_waveform_streaming_ctrl = nullptr;
	ptr_n_eth_set_high_performance_mode = nullptr;

}



uint32_t CScanLabSDK::init_rtc6_dll()
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("init_rtc6_dll", "");

	return ptr_init_rtc6_dll();
}

void CScanLabSDK::free_rtc6_dll()
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("free_rtc6_dll", "");
	ptr_free_rtc6_dll();
}

uint32_t CScanLabSDK::eth_convert_string_to_ip(const char* pIPString)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_convert_string_to_ip", m_pLogJournal->escapeString (pIPString));

	return ptr_eth_convert_string_to_ip(pIPString);
}

void CScanLabSDK::eth_set_search_cards_timeout(const uint32_t nTimeOut)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_set_search_cards_timeout", std::to_string(nTimeOut));

	ptr_eth_set_search_cards_timeout(nTimeOut);
}

uint32_t CScanLabSDK::eth_search_cards_range(const uint32_t nStartIP, const uint32_t nEndIP)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_search_cards_range", std::to_string(nStartIP) + ", " + std::to_string (nEndIP));

	return ptr_eth_search_cards_range(nStartIP, nEndIP);
}


uint32_t CScanLabSDK::rtc6_count_cards()
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("rtc6_count_cards", "");

	return ptr_rtc6_count_cards();
}

uint32_t CScanLabSDK::eth_count_cards() 
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_count_cards", "");

	return ptr_eth_count_cards();
}

uint32_t CScanLabSDK::eth_found_cards()
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_found_cards", "");

	return ptr_eth_found_cards();
}

int32_t CScanLabSDK::eth_assign_card(const uint32_t nSearchNo, const uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_assign_card", std::to_string (nSearchNo) + ", " + std::to_string (nCardNo));

	return ptr_eth_assign_card(nSearchNo, nCardNo);
}

int32_t CScanLabSDK::eth_remove_card(const uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_remove_card", std::to_string(nCardNo));

	return ptr_eth_remove_card(nCardNo);
}

uint32_t CScanLabSDK::acquire_rtc(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("acquire_rtc", std::to_string (nCardNo));

	return ptr_acquire_rtc(nCardNo);
}

uint32_t CScanLabSDK::release_rtc(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("release_rtc", std::to_string(nCardNo));

	return ptr_release_rtc(nCardNo);
}

uint32_t CScanLabSDK::n_get_serial_number(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_serial_number", std::to_string(nCardNo));

	return ptr_n_get_serial_number(nCardNo);
}

uint32_t CScanLabSDK::eth_get_serial_search(uint32_t nSearchNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_get_serial_search", std::to_string(nSearchNo));

	return ptr_eth_get_serial_search(nSearchNo);
}

uint32_t CScanLabSDK::eth_search_cards(uint32_t nIP, uint32_t n_NetMask)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("eth_search_cards", std::to_string(nIP) + ", " + std::to_string (n_NetMask));

	return ptr_eth_search_cards(nIP, n_NetMask);
}

uint32_t CScanLabSDK::n_load_correction_file(uint32_t nCardNo, const char* pFileName, uint32_t nNo, uint32_t n_Dimension)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_load_correction_file", std::to_string(nCardNo) + ", " + m_pLogJournal->escapeString (pFileName) + ", " + std::to_string (nNo) + ", " + std::to_string (n_Dimension));

	return ptr_n_load_correction_file(nCardNo, pFileName, nNo, n_Dimension);
}

void CScanLabSDK::n_eth_set_com_timeouts_auto(uint32_t nCardNo, const double dInitialTimeout, const double dMaxTimeout, const double dMultiplier, const uint32_t nMode) 
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_eth_set_com_timeouts_auto", std::to_string(nCardNo) + ", " + std::to_string(dInitialTimeout) + ", " + std::to_string(dMaxTimeout) + ", " + std::to_string(dMultiplier) + ", " + std::to_string(nMode));

	ptr_n_eth_set_com_timeouts_auto(nCardNo, dInitialTimeout, dMaxTimeout, dMultiplier, nMode);
}

void CScanLabSDK::n_eth_get_com_timeouts_auto(uint32_t nCardNo, double* pdInitialTimeout, double* pdMaxTimeout, double* pdMultiplier, uint32_t* pnMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_eth_get_com_timeouts_auto", std::to_string(nCardNo) + ", &" + m_pLogJournal->defineDoubleVariable ("dInitialTimeout") + ", &" + m_pLogJournal->defineDoubleVariable("dMultiplier") + ", &" + m_pLogJournal->defineUint32Variable("nMode"));

	ptr_n_eth_get_com_timeouts_auto(nCardNo, pdInitialTimeout, pdMaxTimeout, pdMultiplier, pnMode);
}


void CScanLabSDK::n_select_cor_table(uint32_t nCardNo, uint32_t nHeadA, uint32_t nHeadB)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_select_cor_table", std::to_string(nCardNo) + ", " + std::to_string(nHeadA) + ", " + std::to_string(nHeadB));

	ptr_n_select_cor_table(nCardNo, nHeadA, nHeadB);
}

void CScanLabSDK::n_config_list(uint32_t nCardNo, uint32_t nMem1, uint32_t nMem2)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_config_list", std::to_string(nCardNo) + ", " + std::to_string(nMem1) + ", " + std::to_string(nMem2));

	ptr_n_config_list(nCardNo, nMem1, nMem2);
}

void CScanLabSDK::n_set_laser_mode(uint32_t nCardNo, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_mode", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	ptr_n_set_laser_mode(nCardNo, nMode);
}

void CScanLabSDK::n_set_laser_control(uint32_t nCardNo, uint32_t nControl)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_control", std::to_string(nCardNo) + ", " + std::to_string(nControl));

	ptr_n_set_laser_control(nCardNo, nControl);
}

void CScanLabSDK::n_set_auto_laser_control(uint32_t nCardNo, uint32_t nControl, uint32_t nValue, uint32_t nMode, uint32_t nMinValue, uint32_t nMaxValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_auto_laser_control", std::to_string(nCardNo) + ", " + std::to_string(nControl) + ", " + std::to_string(nValue) + ", " + std::to_string(nMode) + ", " + std::to_string(nMinValue) + ", " + std::to_string(nMaxValue));

	ptr_n_set_auto_laser_control(nCardNo, nControl, nValue, nMode, nMinValue, nMaxValue);
}

void CScanLabSDK::n_set_laser_pulses(uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_pulses", std::to_string(nCardNo) + ", " + std::to_string(nHalfPeriod) + ", " + std::to_string(nPulseLength));

	ptr_n_set_laser_pulses(nCardNo, nHalfPeriod, nPulseLength);
}

void CScanLabSDK::n_set_standby(uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_standby", std::to_string(nCardNo) + ", " + std::to_string(nHalfPeriod) + ", " + std::to_string(nPulseLength));

	ptr_n_set_standby(nCardNo, nHalfPeriod, nPulseLength);
}

uint32_t CScanLabSDK::n_get_last_error(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_last_error", std::to_string(nCardNo));

	return ptr_n_get_last_error(nCardNo);
}

uint32_t CScanLabSDK::get_last_error()
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("get_last_error", "");

	return ptr_get_last_error();
}

uint32_t CScanLabSDK::n_load_program_file(uint32_t nCardNo, const char* pPath)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_load_program_file", std::to_string (nCardNo) + ", " + m_pLogJournal->escapeString(pPath));

	return ptr_n_load_program_file(nCardNo, pPath);
}

double CScanLabSDK::n_get_table_para(uint32_t nCardNo, uint32_t nTableNo, uint32_t nParaNo)
{

	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_table_para", std::to_string(nCardNo) + ", " + std::to_string(nTableNo) + ", " + std::to_string(nParaNo));

	return ptr_n_get_table_para(nCardNo, nTableNo, nParaNo);
}

void CScanLabSDK::n_set_end_of_list(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_end_of_list", "");

	ptr_n_set_end_of_list(nCardNo);
}

void CScanLabSDK::n_execute_list_pos(uint32_t nCardNo, uint32_t nListNo, uint32_t nPos)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_execute_list_pos", std::to_string (nListNo) + ", " + std::to_string (nPos));

	ptr_n_execute_list_pos(nCardNo, nListNo, nPos);
}

void CScanLabSDK::n_auto_change_pos(uint32_t nCardNo, uint32_t nPos)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_auto_change_pos", std::to_string(nCardNo) + ", " + std::to_string(nPos));

	ptr_n_auto_change_pos(nCardNo, nPos);
}

void CScanLabSDK::n_set_scanner_delays(uint32_t nCardNo, uint32_t nJump, uint32_t nMark, uint32_t nPolygon)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scanner_delays", std::to_string(nCardNo) + ", " + std::to_string(nJump) + ", " + std::to_string(nMark) + ", " + std::to_string(nPolygon));

	ptr_n_set_scanner_delays(nCardNo, nJump, nMark, nPolygon);
}

void CScanLabSDK::n_set_mark_speed(uint32_t nCardNo, double dSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_mark_speed", std::to_string(nCardNo) + ", " + std::to_string(dSpeed));

	ptr_n_set_mark_speed(nCardNo, dSpeed);
}

void CScanLabSDK::n_set_jump_speed(uint32_t nCardNo, double dSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_jump_speed", std::to_string(nCardNo) + ", " + std::to_string(dSpeed));

	ptr_n_set_jump_speed(nCardNo, dSpeed);
}

void CScanLabSDK::n_write_io_port(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_io_port", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_io_port(nCardNo, nValue);
}

void CScanLabSDK::n_write_8bit_port(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_8bit_port", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_8bit_port(nCardNo, nValue);
}

void CScanLabSDK::n_write_da_1(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_da_1", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_da_1(nCardNo, nValue);
}

void CScanLabSDK::n_write_da_2(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("ptr_n_write_da_2", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_da_2(nCardNo, nValue);
}

void CScanLabSDK::n_write_io_port_list(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("ptr_n_write_io_port_list", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_io_port_list(nCardNo, nValue);
}

void CScanLabSDK::n_write_io_port_mask_list(uint32_t nCardNo, uint32_t nValue, uint32_t nMask)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("ptr_n_write_io_port_mask_list", std::to_string(nCardNo) + ", " + std::to_string(nValue) + ", " + std::to_string(nMask));

	ptr_n_write_io_port_mask_list(nCardNo, nValue, nMask);
}


void CScanLabSDK::n_write_8bit_port_list(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_8bit_port_list", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_8bit_port_list(nCardNo, nValue);
}

void CScanLabSDK::n_write_da_1_list(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_da_1_list", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_da_1_list(nCardNo, nValue);
}

void CScanLabSDK::n_write_da_2_list(uint32_t nCardNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_da_2_list", std::to_string(nCardNo) + ", " + std::to_string(nValue));

	ptr_n_write_da_2_list(nCardNo, nValue);
}

void CScanLabSDK::n_jump_abs(uint32_t nCardNo, int32_t nX, int32_t nY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_jump_abs", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nY));

	ptr_n_jump_abs(nCardNo, nX, nY);
}

void CScanLabSDK::n_mark_abs(uint32_t nCardNo, int32_t nX, int32_t nY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_mark_abs", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nY));

	ptr_n_mark_abs(nCardNo, nX, nY);
}

void CScanLabSDK::n_long_delay(uint32_t nCardNo, uint32_t nDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_long_delay", std::to_string(nCardNo) + ", " + std::to_string(nDelay));

	ptr_n_long_delay(nCardNo, nDelay);
}

void CScanLabSDK::n_get_status(uint32_t nCardNo, uint32_t* pnStatus, uint32_t* pnPos)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_status", std::to_string(nCardNo) + ", &" + m_pLogJournal->defineUint32Variable("nStatus") + ", &" + m_pLogJournal->defineUint32Variable("nPos"));

	ptr_n_get_status(nCardNo, pnStatus, pnPos);
}

uint32_t CScanLabSDK::n_get_input_pointer(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_input_pointer", std::to_string(nCardNo));

	return ptr_n_get_input_pointer(nCardNo);
}

void CScanLabSDK::n_set_laser_delays(uint32_t nCardNo, int32_t nLaserOnDelay, uint32_t nLaserOffDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_delays", std::to_string(nCardNo) + ", " + std::to_string (nLaserOnDelay) + ", " + std::to_string(nLaserOffDelay));

	ptr_n_set_laser_delays(nCardNo, nLaserOnDelay, nLaserOffDelay);
}

void CScanLabSDK::n_set_start_list_pos(uint32_t nCardNo, uint32_t nListNo, uint32_t nListPos)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_start_list_pos", std::to_string(nCardNo) + ", " + std::to_string(nListNo) + ", " + std::to_string(nListPos));

	ptr_n_set_start_list_pos(nCardNo, nListNo, nListPos);
}

void CScanLabSDK::n_set_defocus_list(uint32_t nCardNo, int32_t nShift)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_defocus_list", std::to_string(nCardNo) + ", " + std::to_string(nShift));

	ptr_n_set_defocus_list(nCardNo, nShift);
}

uint32_t CScanLabSDK::n_get_head_status(uint32_t nCardNo, uint32_t nHeadNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_head_status", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo));

	return ptr_n_get_head_status(nCardNo, nHeadNo);
}

int32_t CScanLabSDK::n_get_value(uint32_t nCardNo, uint32_t nSignalNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_value", std::to_string(nCardNo) + ", " + std::to_string(nSignalNo));

	return ptr_n_get_value(nCardNo, nSignalNo);
}

uint32_t CScanLabSDK::get_dll_version()
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("get_dll_version", "");

	return ptr_get_dll_version();
}

uint32_t CScanLabSDK::n_get_hex_version(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_hex_version", std::to_string(nCardNo));

	return ptr_n_get_hex_version(nCardNo);
}

uint32_t CScanLabSDK::n_get_bios_version(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_bios_version", std::to_string(nCardNo));

	return ptr_n_get_bios_version(nCardNo);
}

uint32_t CScanLabSDK::n_get_rtc_version(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_rtc_version", std::to_string(nCardNo));

	return ptr_n_get_rtc_version(nCardNo);
}

uint32_t CScanLabSDK::n_get_card_type(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_card_type", std::to_string(nCardNo));

	return ptr_n_get_card_type(nCardNo);
}


uint32_t CScanLabSDK::n_set_mcbsp_freq(uint32_t nCardNo, uint32_t nFrequency)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_mcbsp_freq", std::to_string(nCardNo) + ", " + std::to_string(nFrequency));

	return ptr_n_set_mcbsp_freq(nCardNo, nFrequency);
}

void CScanLabSDK::n_mcbsp_init(uint32_t nCardNo, uint32_t nXDelay, uint32_t nYDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_mcbsp_init", std::to_string(nCardNo) + ", " + std::to_string(nXDelay) + ", " + std::to_string(nYDelay));

	ptr_n_mcbsp_init(nCardNo, nXDelay, nYDelay);
}

void CScanLabSDK::n_mcbsp_init_spi(uint32_t nCardNo, uint32_t nClockLevel, uint32_t nClockDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_mcbsp_init_spi", std::to_string(nCardNo) + ", " + std::to_string(nClockLevel) + ", " + std::to_string(nClockDelay));

	ptr_n_mcbsp_init_spi(nCardNo, nClockLevel, nClockDelay);
}

void CScanLabSDK::n_set_mcbsp_out_ptr(uint32_t nCardNo, uint32_t nNumber, void* pSignalPtr)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_mcbsp_out_ptr", std::to_string(nCardNo) + ", " + std::to_string(nNumber));

	ptr_n_set_mcbsp_out_ptr(nCardNo, nNumber, pSignalPtr);
}

void CScanLabSDK::n_set_multi_mcbsp_in(uint32_t nCardNo, uint32_t nCtrl, uint32_t nP, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_multi_mcbsp_in", std::to_string(nCardNo) + ", " + std::to_string(nCtrl) + ", " + std::to_string(nP) + ", " + std::to_string(nMode));

	ptr_n_set_multi_mcbsp_in(nCardNo, nCtrl, nP, nMode);
}

void CScanLabSDK::n_set_multi_mcbsp_in_list(uint32_t nCardNo, uint32_t nCtrl, uint32_t nP, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_multi_mcbsp_in_list", std::to_string(nCardNo) + ", " + std::to_string(nCtrl) + ", " + std::to_string(nP) + ", " + std::to_string(nMode));

	ptr_n_set_multi_mcbsp_in_list(nCardNo, nCtrl, nP, nMode);
}

void CScanLabSDK::n_set_laser_power(uint32_t nCardNo, uint32_t nPort, uint32_t nPower)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_power", std::to_string(nCardNo) + ", " + std::to_string(nPort) + ", " + std::to_string(nPower));

	ptr_n_set_laser_power(nCardNo, nPort, nPower);
}

void CScanLabSDK::n_list_nop(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_list_nop", std::to_string(nCardNo));

	ptr_n_list_nop(nCardNo);
}


void CScanLabSDK::n_set_free_variable_list(uint32_t nCardNo, uint32_t nVarNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_free_variable_list", std::to_string(nCardNo) + ", " + std::to_string (nVarNo) + ", " + std::to_string (nValue));

	ptr_n_set_free_variable_list(nCardNo, nVarNo, nValue);
}

void CScanLabSDK::n_set_free_variable(uint32_t nCardNo, uint32_t nVarNo, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_free_variable", std::to_string(nCardNo) + ", " + std::to_string(nVarNo) + ", " + std::to_string(nValue));

	ptr_n_set_free_variable(nCardNo, nVarNo, nValue);
}

uint32_t CScanLabSDK::n_get_free_variable(uint32_t nCardNo, uint32_t nVarNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("ptr_n_get_free_variable", std::to_string(nCardNo) + ", " + std::to_string(nVarNo));

	return ptr_n_get_free_variable(nCardNo, nVarNo);
}


void CScanLabSDK::n_set_trigger(uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_trigger", std::to_string(nCardNo) + ", " + std::to_string(nPeriod) + ", " + std::to_string(nSignal1) + ", " + std::to_string(nSignal2));

	ptr_n_set_trigger(nCardNo, nPeriod, nSignal1, nSignal2);
}

void CScanLabSDK::n_set_trigger4(uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2, uint32_t nSignal3, uint32_t nSignal4)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_trigger4", std::to_string(nCardNo) + ", " + std::to_string(nPeriod) + ", " + std::to_string(nSignal1) + ", " + std::to_string(nSignal2) + ", " + std::to_string(nSignal3) + ", " + std::to_string(nSignal4));

	ptr_n_set_trigger4(nCardNo, nPeriod, nSignal1, nSignal2, nSignal3, nSignal4);
}

void CScanLabSDK::n_set_trigger8(uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2, uint32_t nSignal3, uint32_t nSignal4, uint32_t nSignal5, uint32_t nSignal6, uint32_t nSignal7, uint32_t nSignal8)

{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_trigger8", std::to_string(nCardNo) + ", " + std::to_string(nPeriod) + ", " + std::to_string(nSignal1) + ", " + std::to_string(nSignal2) + ", " + std::to_string(nSignal3) + ", " + std::to_string(nSignal4) + ", " + std::to_string(nSignal5) + ", " + std::to_string(nSignal6) + ", " + std::to_string(nSignal7) + ", " + std::to_string(nSignal8));

	ptr_n_set_trigger8(nCardNo, nPeriod, nSignal1, nSignal2, nSignal3, nSignal4, nSignal5, nSignal6, nSignal7, nSignal8);

}

void CScanLabSDK::n_set_control_mode(uint32_t nCardNo, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_control_mode", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	ptr_n_set_control_mode(nCardNo, nMode);
}

void CScanLabSDK::n_set_laser_pulses_ctrl(uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_pulses_ctrl", std::to_string(nCardNo) + ", " + std::to_string(nHalfPeriod) + ", " + std::to_string(nPulseLength));

	ptr_n_set_laser_pulses_ctrl(nCardNo, nHalfPeriod, nPulseLength);
}

void CScanLabSDK::n_set_mark_speed_ctrl(uint32_t nCardNo, double dSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_mark_speed_ctrl", std::to_string(nCardNo) + ", " + std::to_string(dSpeed));

	ptr_n_set_mark_speed_ctrl(nCardNo, dSpeed);
}

void CScanLabSDK::n_set_jump_speed_ctrl(uint32_t nCardNo, double dSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_jump_speed_ctrl", std::to_string(nCardNo) + ", " + std::to_string(dSpeed));

	ptr_n_set_jump_speed_ctrl(nCardNo, dSpeed);
}

void CScanLabSDK::n_set_firstpulse_killer(uint32_t nCardNo, uint32_t nLength)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_firstpulse_killer", std::to_string(nCardNo) + ", " + std::to_string(nLength));

	ptr_n_set_firstpulse_killer(nCardNo, nLength);
}

void CScanLabSDK::n_set_firstpulse_killer_list(uint32_t nCardNo, uint32_t nLength)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_firstpulse_killer_list", std::to_string(nCardNo) + ", " + std::to_string(nLength));

	ptr_n_set_firstpulse_killer_list(nCardNo, nLength);
}

void CScanLabSDK::n_set_qswitch_delay(uint32_t nCardNo, uint32_t nDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_qswitch_delay", std::to_string(nCardNo) + ", " + std::to_string(nDelay));

	ptr_n_set_qswitch_delay(nCardNo, nDelay);
}

void CScanLabSDK::n_set_qswitch_delay_list(uint32_t nCardNo, uint32_t nDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_qswitch_delay_list", std::to_string(nCardNo) + ", " + std::to_string(nDelay));

	ptr_n_set_qswitch_delay_list(nCardNo, nDelay);
}

void CScanLabSDK::n_write_da_x(uint32_t nCardNo, uint32_t nX, uint32_t nValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_write_da_x", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nValue));

	ptr_n_write_da_x(nCardNo, nX, nValue);
}


void CScanLabSDK::n_set_laser_pin_out(uint32_t nCardNo, uint32_t nPins)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_pin_out", std::to_string(nCardNo) + ", " + std::to_string(nPins));

	ptr_n_set_laser_pin_out(nCardNo, nPins);
}

void CScanLabSDK::n_set_laser_pin_out_list(uint32_t nCardNo, uint32_t nPins)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_laser_pin_out_list", std::to_string(nCardNo) + ", " + std::to_string(nPins));

	ptr_n_set_laser_pin_out_list(nCardNo, nPins);
}

uint32_t CScanLabSDK::n_get_laser_pin_in(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_laser_pin_in", std::to_string(nCardNo));

	return ptr_n_get_laser_pin_in(nCardNo);
}


void CScanLabSDK::n_set_sky_writing_para(uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift, uint32_t nNprev, uint32_t nNPost)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_para", std::to_string(nCardNo) + ", " + std::to_string(dTimelag) + ", " + std::to_string(nLaserOnShift) + ", " + std::to_string(nNprev) + ", " + std::to_string(nNPost));

	ptr_n_set_sky_writing_para(nCardNo, dTimelag, nLaserOnShift, nNprev, nNPost);
}

void CScanLabSDK::n_set_sky_writing_limit(uint32_t nCardNo, double dCosAngle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_limit", std::to_string(nCardNo) + ", " + std::to_string (dCosAngle));

	ptr_n_set_sky_writing_limit(nCardNo, dCosAngle);
}

void CScanLabSDK::n_set_sky_writing_mode(uint32_t nCardNo, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_mode", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	ptr_n_set_sky_writing_mode(nCardNo, nMode);
}

void CScanLabSDK::n_set_sky_writing(uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing", std::to_string(nCardNo) + ", " + std::to_string(dTimelag) + ", " + std::to_string(nLaserOnShift));

	ptr_n_set_sky_writing(nCardNo, dTimelag, nLaserOnShift);
}

void CScanLabSDK::n_set_sky_writing_para_list(uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift, uint32_t nNprev, uint32_t nNPost)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_para_list", std::to_string(nCardNo) + ", " + std::to_string(dTimelag) + ", " + std::to_string(nLaserOnShift) + ", " + std::to_string(nNprev) + ", " + std::to_string(nNPost));

	ptr_n_set_sky_writing_para_list(nCardNo, dTimelag, nLaserOnShift, nNprev, nNPost);
}

void CScanLabSDK::n_set_sky_writing_list(uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_list", std::to_string(nCardNo) + ", " + std::to_string(dTimelag) + ", " + std::to_string(nLaserOnShift));

	ptr_n_set_sky_writing_list(nCardNo, dTimelag, nLaserOnShift);
}

void CScanLabSDK::n_set_sky_writing_limit_list(uint32_t nCardNo, double dCosAngle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_limit_list", std::to_string(nCardNo) + ", " + std::to_string(dCosAngle));

	ptr_n_set_sky_writing_limit_list(nCardNo, dCosAngle);
}

void CScanLabSDK::n_set_sky_writing_mode_list(uint32_t nCardNo, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_sky_writing_mode_list", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	ptr_n_set_sky_writing_mode_list(nCardNo, nMode);
}


void CScanLabSDK::n_control_command(uint32_t nCardNo, uint32_t nHead, uint32_t nAxis, uint32_t nData)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_control_command", std::to_string(nCardNo) + ", " + std::to_string(nHead) + ", " + std::to_string(nAxis) + ", " + std::to_string(nData));

	ptr_n_control_command(nCardNo, nHead, nAxis, nData);
}


uint32_t CScanLabSDK::n_get_scanahead_params(uint32_t nCardNo, uint32_t nHead, uint32_t* pPreViewTime, uint32_t* Vmax, double* Amax)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_scanahead_params", std::to_string(nCardNo) + ", " + std::to_string(nHead) + ", " + m_pLogJournal->defineUint32Variable ("nPreViewTime") + ", " + m_pLogJournal->defineUint32Variable("nVmax") + ", " + m_pLogJournal->defineDoubleVariable ("dAmax"));

	return ptr_n_get_scanahead_params(nCardNo, nHead, pPreViewTime, Vmax, Amax);
}

int32_t CScanLabSDK::n_activate_scanahead_autodelays(uint32_t nCardNo, int32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_activate_scanahead_autodelays", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	return ptr_n_activate_scanahead_autodelays(nCardNo, nMode);
}

void CScanLabSDK::n_set_scanahead_laser_shifts(uint32_t nCardNo, int32_t nDLasOn, int32_t nDLasOff)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scanahead_laser_shifts", std::to_string(nCardNo) + ", " + std::to_string(nDLasOn) + ", " + std::to_string(nDLasOff));

	ptr_n_set_scanahead_laser_shifts(nCardNo, nDLasOn, nDLasOff);
}

void CScanLabSDK::n_set_scanahead_line_params(uint32_t nCardNo, uint32_t nCornerScale, uint32_t nEndScale, uint32_t nAccScale)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scanahead_line_params", std::to_string(nCardNo) + ", " + std::to_string(nCornerScale) + ", " + std::to_string(nEndScale) + ", " + std::to_string(nAccScale));

	ptr_n_set_scanahead_line_params(nCardNo, nCornerScale, nEndScale, nAccScale);
}

void CScanLabSDK::n_set_scanahead_line_params_ex(uint32_t nCardNo, uint32_t nCornerScale, uint32_t nEndScale, uint32_t nAccScale, uint32_t nJumpScale)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scanahead_line_params_ex", std::to_string(nCardNo) + ", " + std::to_string(nCornerScale) + ", " + std::to_string(nEndScale) + ", " + std::to_string(nAccScale) + ", " + std::to_string(nJumpScale));

	ptr_n_set_scanahead_line_params_ex(nCardNo, nCornerScale, nEndScale, nAccScale, nJumpScale);
}

uint32_t CScanLabSDK::n_set_scanahead_params(uint32_t nCardNo, uint32_t nMode, uint32_t nHeadNo, uint32_t nTableNo, uint32_t nPreviewTime, uint32_t nVMax, double dAmax)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scanahead_params", std::to_string(nCardNo) + ", " + std::to_string(nMode) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(nTableNo) + ", " + std::to_string(nPreviewTime) + ", " + std::to_string(nVMax) + ", " + std::to_string(dAmax));

	return ptr_n_set_scanahead_params(nCardNo, nMode, nHeadNo, nTableNo, nPreviewTime, nVMax, dAmax);
}

void CScanLabSDK::n_set_scanahead_speed_control(uint32_t nCardNo, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scanahead_speed_control", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	ptr_n_set_scanahead_speed_control(nCardNo, nMode);
}


/*void CScanLabSDK::n_micro_vector_abs_3d(uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nZ, int32_t nLasOn, int32_t nLasOff)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_micro_vector_abs_3d", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nY) + ", " + std::to_string(nZ) + ", " + std::to_string(nLasOn) + ", " + std::to_string(nLasOff));

	ptr_n_micro_vector_abs_3d(nCardNo, nX, nY, nZ, nLasOn, nLasOff);
}

void CScanLabSDK::n_micro_vector_rel_3d(uint32_t nCardNo, int32_t ndX, int32_t ndY, int32_t ndZ, int32_t nLasOn, int32_t nLasOff)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_micro_vector_rel_3d", std::to_string(nCardNo) + ", " + std::to_string(ndX) + ", " + std::to_string(ndY) + ", " + std::to_string(ndZ) + ", " + std::to_string(nLasOn) + ", " + std::to_string(nLasOff));

	ptr_n_micro_vector_rel_3d(nCardNo, ndX, ndY, ndZ, nLasOn, nLasOff);
}

void CScanLabSDK::n_micro_vector_abs(uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nLasOn, int32_t nLasOff)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_micro_vector_abs", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nY) + ", " + std::to_string(nLasOn) + ", " + std::to_string(nLasOff));

	ptr_n_micro_vector_abs(nCardNo, nX, nY, nLasOn, nLasOff);
}

void CScanLabSDK::n_micro_vector_rel(uint32_t nCardNo, int32_t ndX, int32_t ndY, int32_t nLasOn, int32_t nLasOff)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_micro_vector_rel", std::to_string(nCardNo) + ", " + std::to_string(ndX) + ", " + std::to_string(ndY) + ", " + std::to_string(nLasOn) + ", " + std::to_string(nLasOff));

	ptr_n_micro_vector_rel(nCardNo, ndX, ndY, nLasOn, nLasOff);
} */


uint32_t CScanLabSDK::n_get_error(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_error", std::to_string(nCardNo));

	return ptr_n_get_error(nCardNo);
}

void CScanLabSDK::n_reset_error(uint32_t nCardNo, uint32_t nCode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_reset_error", std::to_string(nCardNo) + ", " + std::to_string(nCode));

	ptr_n_reset_error(nCardNo, nCode);
}


void CScanLabSDK::n_set_angle(uint32_t nCardNo, uint32_t nHeadNo, double dAngle, uint32_t nAtOnce)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_angle", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(dAngle) + ", " + std::to_string(nAtOnce));

	ptr_n_set_angle(nCardNo, nHeadNo, dAngle, nAtOnce);
}

void CScanLabSDK::n_set_scale(uint32_t nCardNo, uint32_t nHeadNo, double dScaleFactor, uint32_t nAtOnce)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_scale", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(dScaleFactor) + ", " + std::to_string(nAtOnce));

	ptr_n_set_scale(nCardNo, nHeadNo, dScaleFactor, nAtOnce);
}

void CScanLabSDK::n_set_offset(uint32_t nCardNo, uint32_t nHeadNo, int32_t nXOffset, int32_t nYOffset, uint32_t nAtOnce)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_offset", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(nXOffset) + ", " + std::to_string(nYOffset) + ", " + std::to_string(nAtOnce));

	ptr_n_set_offset(nCardNo, nHeadNo, nXOffset, nYOffset, nAtOnce);
}

void CScanLabSDK::n_set_matrix(uint32_t nCardNo, uint32_t nHeadNo, double dM11, double dM12, double dM21, double dM22, uint32_t nAtOnce)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_matrix", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(dM11) + ", " + std::to_string(dM12) + ", " + std::to_string(dM21) + ", " + std::to_string(dM22) + ", " + std::to_string(nAtOnce));

	ptr_n_set_matrix(nCardNo, nHeadNo, dM11, dM12, dM21, dM22, nAtOnce);
}


void CScanLabSDK::n_get_waveform_offset(uint32_t nCardNo, uint32_t nChannel, uint32_t nOffset, uint32_t nNumber, int32_t* pPtr)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_waveform_offset", std::to_string(nCardNo) + ", " + std::to_string(nChannel) + ", " + std::to_string(nOffset) + ", " + std::to_string(nNumber) + ", " + m_pLogJournal->defineInt32Variable("nWaveFormOffset") );

	ptr_n_get_waveform_offset(nCardNo, nChannel, nOffset, nNumber, pPtr);
}

void CScanLabSDK::n_measurement_status(uint32_t nCardNo, uint32_t* pnBusy, uint32_t* pnPos)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_measurement_status", std::to_string(nCardNo) + ", " + m_pLogJournal->defineUint32Variable ("nBusy") + ", " + m_pLogJournal->defineUint32Variable("nPos"));

	ptr_n_measurement_status(nCardNo, pnBusy, pnPos);
}

uint32_t CScanLabSDK::transform(int32_t* pSignal1, int32_t* pSignal2, uint8_t* pTransform, uint32_t nCode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("transform", ""); // TODO

	return ptr_transform(pSignal1, pSignal2, pTransform, nCode);
}

uint32_t CScanLabSDK::n_upload_transform(uint32_t nCardNo, uint32_t nHeadNo, uint8_t* pTransformData)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_upload_transform", std::to_string (nCardNo) + ", " + std::to_string (nHeadNo)); // TODO

	return ptr_n_upload_transform(nCardNo, nHeadNo, pTransformData);
}


void CScanLabSDK::n_set_timelag_compensation(uint32_t nCardNo, uint32_t nHeadNo, uint32_t nTimelagXY, uint32_t nTimelagZ)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_timelag_compensation", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(nTimelagXY) + ", " + std::to_string(nTimelagZ));

	ptr_n_set_timelag_compensation(nCardNo, nHeadNo, nTimelagXY, nTimelagZ);
}


void CScanLabSDK::n_init_fly_2d(uint32_t nCardNo, int32_t nOffsetX, int32_t nOffsetY, uint32_t nNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_init_fly_2d", std::to_string(nCardNo) + ", " + std::to_string(nOffsetX) + ", " + std::to_string(nOffsetY) + ", " + std::to_string(nNo));

	ptr_n_init_fly_2d(nCardNo, nOffsetX, nOffsetY, nNo);
}

void CScanLabSDK::n_activate_fly_2d(uint32_t nCardNo, const double ScaleX, const double ScaleY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_activate_fly_2d", std::to_string(nCardNo) + ", " + std::to_string(ScaleX) + ", " + std::to_string(ScaleY));

	ptr_n_activate_fly_2d(nCardNo, ScaleX, ScaleY);
}

void CScanLabSDK::n_activate_fly_2d_encoder(uint32_t nCardNo, const double ScaleX, const double ScaleY, int32_t nEncoderOffsetX, int32_t nEncoderOffsetY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_activate_fly_2d_encoder", std::to_string(nCardNo) + ", " + std::to_string(ScaleX) + ", " + std::to_string(ScaleY) + ", " + std::to_string(nEncoderOffsetX) + ", " + std::to_string(nEncoderOffsetY));

	ptr_n_activate_fly_2d_encoder(nCardNo, ScaleY, ScaleY, nEncoderOffsetX, nEncoderOffsetY);
}

void CScanLabSDK::n_set_fly_2d(uint32_t nCardNo, const double ScaleX, const double ScaleY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_fly_2d", std::to_string(nCardNo) + ", " + std::to_string(ScaleX) + ", " + std::to_string(ScaleY));

	ptr_n_set_fly_2d(nCardNo, ScaleX, ScaleY);
}

void CScanLabSDK::n_get_fly_2d_offset(uint32_t nCardNo, const int32_t* pOffsetX, const int32_t* pOffsetY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_fly_2d_offset", std::to_string(nCardNo) + ", " + m_pLogJournal->defineInt32Variable ("nOffsetX") + ", " + m_pLogJournal->defineInt32Variable("nOffsetY"));

	ptr_n_get_fly_2d_offset(nCardNo, pOffsetX, pOffsetY);
}

void CScanLabSDK::n_set_fly_x_pos(uint32_t nCardNo, const double ScaleX)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_fly_x_pos", std::to_string(nCardNo) + ", " + std::to_string(ScaleX));

	ptr_n_set_fly_x_pos(nCardNo, ScaleX);
}

void CScanLabSDK::n_set_fly_y_pos(uint32_t nCardNo, const double ScaleY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_fly_y_pos", std::to_string(nCardNo) + ", " + std::to_string(ScaleY));

	ptr_n_set_fly_y_pos(nCardNo, ScaleY);
}

void CScanLabSDK::n_set_fly_x(uint32_t nCardNo, const double ScaleX)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_fly_x", std::to_string(nCardNo) + ", " + std::to_string(ScaleX));

	ptr_n_set_fly_x(nCardNo, ScaleX);
}

void CScanLabSDK::n_set_fly_y(uint32_t nCardNo, const double ScaleY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_fly_y", std::to_string(nCardNo) + ", " + std::to_string(ScaleY));

	ptr_n_set_fly_y(nCardNo, ScaleY);
}

void CScanLabSDK::n_fly_return(uint32_t nCardNo, int32_t nX, int32_t nY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_fly_return", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nY));

	ptr_n_fly_return(nCardNo, nX, nY);
}


void CScanLabSDK::n_get_encoder(uint32_t nCardNo, const int32_t* pEncoderX, const int32_t* pEncoderY)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_encoder", std::to_string(nCardNo) + ", " + m_pLogJournal->defineInt32Variable ("nEncoderX") + ", " + m_pLogJournal->defineInt32Variable("nEncoderY"));

	ptr_n_get_encoder(nCardNo, pEncoderX, pEncoderY);
}

uint32_t CScanLabSDK::n_get_marking_info(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_get_marking_info", std::to_string(nCardNo));

	return ptr_n_get_marking_info(nCardNo);
}

void CScanLabSDK::n_wait_for_encoder(uint32_t nCardNo, int32_t nValue, uint32_t nEncoderNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_wait_for_encoder", std::to_string(nCardNo) + ", " + std::to_string (nValue) + ", " + std::to_string (nEncoderNo));

	return ptr_n_wait_for_encoder(nCardNo, nValue, nEncoderNo);
}

void CScanLabSDK::n_wait_for_encoder_mode(uint32_t nCardNo, int32_t nValue, uint32_t nEncoderNo, int32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_wait_for_encoder_mode", std::to_string(nCardNo) + ", " + std::to_string(nValue) + ", " + std::to_string(nEncoderNo) + ", " + std::to_string(nMode));

	return ptr_n_wait_for_encoder_mode(nCardNo, nValue, nEncoderNo, nMode);
}

void CScanLabSDK::n_set_fly_limits(uint32_t nCardNo, int32_t nXMin, int32_t nXMax, int32_t nYMin, int32_t nYMax)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_fly_limits", std::to_string(nCardNo) + ", " + std::to_string(nXMin) + ", " + std::to_string(nXMax) + ", " + std::to_string(nYMin) + ", " + std::to_string(nYMax));

	ptr_n_set_fly_limits(nCardNo, nXMin, nXMax, nYMin, nYMax);
}

void CScanLabSDK::n_range_checking(uint32_t nCardNo, uint32_t nHeadNo, uint32_t nMode, uint32_t nData)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_range_checking", std::to_string(nCardNo) + ", " + std::to_string(nHeadNo) + ", " + std::to_string(nMode) + ", " + std::to_string(nData));

	ptr_n_range_checking(nCardNo, nHeadNo, nMode, nData);
}

void CScanLabSDK::n_stop_execution(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_stop_execution", std::to_string(nCardNo));

	ptr_n_stop_execution(nCardNo);
}

void CScanLabSDK::n_timed_mark_abs(uint32_t nCardNo, int32_t nX, int32_t nY, double dTime)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_timed_mark_abs", std::to_string(nCardNo) + ", " + std::to_string(nX) + ", " + std::to_string(nY) + ", " + std::to_string(dTime));

	ptr_n_timed_mark_abs(nCardNo, nX, nY, dTime);
}

int32_t CScanLabSDK::n_read_multi_mcbsp(uint32_t nCardNo, uint32_t nRegisterNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_read_multi_mcbsp", std::to_string(nCardNo) + ", " + std::to_string(nRegisterNo));

	return ptr_n_read_multi_mcbsp(nCardNo, nRegisterNo);

}

uint32_t CScanLabSDK::n_uart_config(uint32_t nCardNo, uint32_t nBaudRate)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_uart_config", std::to_string(nCardNo) + ", " + std::to_string(nBaudRate));

	return ptr_n_uart_config(nCardNo, nBaudRate);

}

void CScanLabSDK::n_rs232_write_data(uint32_t nCardNo, uint32_t nData)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_rs232_write_data", std::to_string(nCardNo) + ", " + std::to_string(nData));

	ptr_n_rs232_write_data(nCardNo, nData);
}

uint32_t CScanLabSDK::n_rs232_read_data(uint32_t nCardNo)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_rs232_read_data", std::to_string(nCardNo));

	return ptr_n_rs232_read_data(nCardNo);
}


void CScanLabSDK::n_set_mcbsp_out_oie_ctrl(uint32_t nCardNo, uint32_t nSignalID1, uint32_t nSignalID2)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_set_mcbsp_out_oie_ctrl", std::to_string(nCardNo) + ", " + std::to_string (nSignalID1) + ", " + std::to_string (nSignalID2));

	if (ptr_n_set_mcbsp_out_oie_ctrl != nullptr)
		ptr_n_set_mcbsp_out_oie_ctrl(nCardNo, nSignalID1, nSignalID2);
}

void CScanLabSDK::n_eth_config_waveform_streaming_ctrl(uint32_t nCardNo, uint32_t nSize, uint32_t nFlags)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_eth_config_waveform_streaming_ctrl", std::to_string(nCardNo) + ", " + std::to_string(nSize) + ", " + std::to_string(nFlags));

	if (ptr_n_eth_config_waveform_streaming_ctrl != nullptr)
		ptr_n_eth_config_waveform_streaming_ctrl(nCardNo, nSize, nFlags);

}

void CScanLabSDK::n_eth_set_high_performance_mode(uint32_t nCardNo, uint32_t nMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("n_eth_set_high_performance_mode", std::to_string(nCardNo) + ", " + std::to_string(nMode));

	if (ptr_n_eth_set_high_performance_mode != nullptr)
		ptr_n_eth_set_high_performance_mode(nCardNo, nMode);

}


void CScanLabSDK::setJournal(PScanLabSDKJournal pLogJournal)
{
	m_pLogJournal = pLogJournal;
}
