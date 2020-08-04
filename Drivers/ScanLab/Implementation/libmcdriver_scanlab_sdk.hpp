/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef __LIBMCDRIVER_SCANLAB_SDK
#define __LIBMCDRIVER_SCANLAB_SDK

#include "libmcdriver_scanlab_types.hpp"

#include <memory>
#include <string>

#ifdef _WIN32

#define SCANLAB_CALLINGCONVENTION __stdcall

#else

#define SCANLAB_CALLINGCONVENTION 

#endif


namespace LibMCDriver_ScanLab {
	namespace Impl {


		typedef uint32_t(SCANLAB_CALLINGCONVENTION *PScanLabPtr_init_rtc6_dll) ();
		typedef void(SCANLAB_CALLINGCONVENTION *PScanLabPtr_free_rtc6_dll) ();
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_convert_string_to_ip) (const char * pIPString);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_set_search_cards_timeout) (const uint32_t nTimeOut);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_search_cards_range) (const uint32_t nStartIP, const uint32_t nEndIP);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_rtc6_count_cards) ();
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_count_cards) ();
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_found_cards) ();
		typedef int32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_assign_card) (uint32_t nSearchNo, uint32_t nCardNo);		
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_acquire_rtc) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_release_rtc) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_serial_number) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_get_serial_search) (uint32_t nSearchNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_search_cards) (uint32_t nIP, uint32_t n_NetMask);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_load_correction_file) (uint32_t nCardNo, const char * nFileName, uint32_t nNo, uint32_t n_Dimension);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_select_cor_table) (uint32_t nCardNo, uint32_t nHeadA, uint32_t nHeadB);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_config_list) (uint32_t nCardNo, uint32_t nMem1, uint32_t nMem2);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_mode) (uint32_t nCardNo, uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_control) (uint32_t nCardNo, uint32_t nControl);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_pulses_ctrl) (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_standby) (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_last_error) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_get_last_error) ();

		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_load_program_file) (uint32_t nCardNo, const char * pPath);
		typedef double (SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_table_para) (uint32_t nCardNo, uint32_t nTableNo, uint32_t nParaNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_end_of_list) (uint32_t nCardNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_execute_list_pos) (uint32_t nCardNo, uint32_t nListNo, uint32_t nPos);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_auto_change_pos) (uint32_t nCardNo, uint32_t nPos);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanner_delays) (uint32_t nCardNo, uint32_t nJump, uint32_t nMark, uint32_t nPolygon);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_mark_speed) (uint32_t nCardNo, double dSpeed);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_jump_speed) (uint32_t nCardNo, double dSpeed);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_io_port) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_8bit_port) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_1) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_2) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_jump_abs) (uint32_t nCardNo, int32_t nX, int32_t nY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_mark_abs) (uint32_t nCardNo, int32_t nX, int32_t nY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_long_delay) (uint32_t nCardNo, uint32_t nDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_status) (uint32_t nCardNo, uint32_t * pnStatus, uint32_t * pnPos);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_input_pointer) (uint32_t nCardNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_delays) (uint32_t nCardNo, int32_t nLaserOnDelay, uint32_t nLaserOffDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_start_list_pos) (uint32_t nCardNo, uint32_t nListNo, uint32_t nListPos);
		

		class CScanLabSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			PScanLabPtr_init_rtc6_dll init_rtc6_dll = nullptr;
			PScanLabPtr_free_rtc6_dll free_rtc6_dll = nullptr;
			PScanLabPtr_eth_convert_string_to_ip eth_convert_string_to_ip = nullptr;
			PScanLabPtr_eth_set_search_cards_timeout eth_set_search_cards_timeout = nullptr;
			PScanLabPtr_eth_search_cards_range eth_search_cards_range = nullptr;
			PScanLabPtr_rtc6_count_cards rtc6_count_cards = nullptr;
			PScanLabPtr_eth_count_cards eth_count_cards = nullptr;
			PScanLabPtr_eth_found_cards eth_found_cards = nullptr;
			PScanLabPtr_eth_assign_card eth_assign_card = nullptr;
			PScanLabPtr_acquire_rtc acquire_rtc = nullptr;
			PScanLabPtr_release_rtc release_rtc = nullptr;
			PScanLabPtr_n_get_serial_number n_get_serial_number = nullptr;
			PScanLabPtr_eth_get_serial_search eth_get_serial_search = nullptr;
			PScanLabPtr_eth_search_cards eth_search_cards = nullptr;
			PScanLabPtr_n_load_correction_file n_load_correction_file = nullptr;
			PScanLabPtr_n_select_cor_table n_select_cor_table = nullptr;
			PScanLabPtr_n_config_list n_config_list = nullptr;
			PScanLabPtr_n_set_laser_mode n_set_laser_mode = nullptr;
			PScanLabPtr_n_set_laser_control n_set_laser_control = nullptr;
			PScanLabPtr_n_set_laser_pulses_ctrl n_set_laser_pulses_ctrl = nullptr;
			PScanLabPtr_n_set_standby n_set_standby = nullptr;
			PScanLabPtr_n_get_last_error n_get_last_error  = nullptr;
			PScanLabPtr_get_last_error get_last_error = nullptr;
			PScanLabPtr_n_load_program_file n_load_program_file = nullptr;
			PScanLabPtr_n_get_table_para n_get_table_para = nullptr;
			PScanLabPtr_n_set_end_of_list n_set_end_of_list = nullptr;
			PScanLabPtr_n_execute_list_pos n_execute_list_pos = nullptr;
			PScanLabPtr_n_auto_change_pos n_auto_change_pos = nullptr;
			PScanLabPtr_n_set_scanner_delays n_set_scanner_delays = nullptr;
			PScanLabPtr_n_set_mark_speed n_set_mark_speed = nullptr;
			PScanLabPtr_n_set_jump_speed n_set_jump_speed = nullptr;
			PScanLabPtr_n_write_io_port n_write_io_port = nullptr;
			PScanLabPtr_n_write_8bit_port n_write_8bit_port = nullptr;
			PScanLabPtr_n_write_da_1 n_write_da_1 = nullptr;
			PScanLabPtr_n_write_da_2 n_write_da_2 = nullptr;
			PScanLabPtr_n_jump_abs n_jump_abs = nullptr;
			PScanLabPtr_n_mark_abs n_mark_abs = nullptr;
			PScanLabPtr_n_long_delay n_long_delay = nullptr;
			PScanLabPtr_n_get_status n_get_status = nullptr;
			PScanLabPtr_n_get_input_pointer n_get_input_pointer = nullptr;
			PScanLabPtr_n_set_laser_delays n_set_laser_delays = nullptr;
			PScanLabPtr_n_set_start_list_pos n_set_start_list_pos = nullptr;

			CScanLabSDK(const std::string & sDLLNameUTF8);
			~CScanLabSDK();

			void initDLL();
			void checkError(uint32_t nRTCError);

		};


		typedef std::shared_ptr<CScanLabSDK> PScanLabSDK;

	} 
} 

#endif // __LIBMCDRIVER_SCANLAB_SDK
