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
		typedef int32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_assign_card) (const uint32_t nSearchNo, const uint32_t nCardNo);		
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_acquire_rtc) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_release_rtc) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_serial_number) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_get_serial_search) (uint32_t nSearchNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_search_cards) (uint32_t nIP, uint32_t n_NetMask);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_load_correction_file) (uint32_t nCardNo, const char * nFileName, uint32_t nNo, uint32_t n_Dimension);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_eth_set_com_timeouts_auto) (uint32_t nCardNo, const double dInitialTimeout, const double dMaxTimeout, const double dMultiplier, const uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_eth_get_com_timeouts_auto) (uint32_t nCardNo, double * pdInitialTimeout, double * pdMaxTimeout, double * pdMultiplier, uint32_t * pnMode);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_select_cor_table) (uint32_t nCardNo, uint32_t nHeadA, uint32_t nHeadB);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_config_list) (uint32_t nCardNo, uint32_t nMem1, uint32_t nMem2);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_mode) (uint32_t nCardNo, uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_control) (uint32_t nCardNo, uint32_t nControl);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_auto_laser_control) (uint32_t nCardNo, uint32_t nControl, uint32_t nValue, uint32_t nMode, uint32_t nMinValue, uint32_t nMaxValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_pulses) (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
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
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_io_port_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_8bit_port_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_1_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_2_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_jump_abs) (uint32_t nCardNo, int32_t nX, int32_t nY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_mark_abs) (uint32_t nCardNo, int32_t nX, int32_t nY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_jump_abs_3d) (uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nZ);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_mark_abs_3d) (uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nZ);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_long_delay) (uint32_t nCardNo, uint32_t nDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_status) (uint32_t nCardNo, uint32_t * pnStatus, uint32_t * pnPos);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_input_pointer) (uint32_t nCardNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_delays) (uint32_t nCardNo, int32_t nLaserOnDelay, uint32_t nLaserOffDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_start_list_pos) (uint32_t nCardNo, uint32_t nListNo, uint32_t nListPos);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_defocus_list) (uint32_t nCardNo, int32_t nShift);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_head_status) (uint32_t nCardNo, uint32_t nHeadNo);
		typedef int32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_value) (uint32_t nCardNo, uint32_t nSignalNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_get_dll_version) ();
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_hex_version) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_bios_version) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_rtc_version) (uint32_t nCardNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_card_type) (uint32_t nCardNo);

		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_mcbsp_freq) (uint32_t nCardNo, uint32_t nFrequency);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_mcbsp_init) (uint32_t nCardNo, uint32_t nXDelay, uint32_t nYDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_mcbsp_init_spi) (uint32_t nCardNo, uint32_t nClockLevel, uint32_t nClockDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_mcbsp_out_ptr) (uint32_t nCardNo, uint32_t nNumber, void* pSignalPtr);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_multi_mcbsp_in) (uint32_t nCardNo, uint32_t nCtrl, uint32_t nP, uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_multi_mcbsp_in_list) (uint32_t nCardNo, uint32_t nCtrl, uint32_t nP, uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_power) (uint32_t nCardNo, uint32_t nPort, uint32_t nPower);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_list_nop) (uint32_t nCardNo);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_free_variable_list) (uint32_t nCardNo, uint32_t nVarNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_free_variable) (uint32_t nCardNo, uint32_t nVarNo, uint32_t nValue);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_free_variable) (uint32_t nCardNo, uint32_t nVarNo);
		
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_trigger) (uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_trigger4) (uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2, uint32_t nSignal3, uint32_t nSignal4);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_control_mode) (uint32_t nCardNo, uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_pulses_ctrl) (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_mark_speed_ctrl) (uint32_t nCardNo, double dSpeed);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_jump_speed_ctrl) (uint32_t nCardNo, double dSpeed);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_firstpulse_killer) (uint32_t nCardNo, uint32_t nLength);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_firstpulse_killer_list) (uint32_t nCardNo, uint32_t nLength);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_qswitch_delay) (uint32_t nCardNo, uint32_t nDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_qswitch_delay_list) (uint32_t nCardNo, uint32_t nDelay);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_x) (uint32_t nCardNo, uint32_t nX, uint32_t nValue);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_pin_out) (uint32_t nCardNo, uint32_t nPins);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_laser_pin_in) (uint32_t nCardNo);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_para) (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift, uint32_t nNprev, uint32_t nNPost);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_limit) (uint32_t nCardNo, double dCosAngle);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_mode) (uint32_t nCardNo, uint32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing) (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_para_list) (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift, uint32_t nNprev, uint32_t nNPost);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_list) (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_limit_list) (uint32_t nCardNo, double dCosAngle);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_sky_writing_mode_list) (uint32_t nCardNo, uint32_t nMode);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_control_command) (uint32_t nCardNo, uint32_t nHead, uint32_t nAxis, uint32_t nData);

		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_scanahead_params) (uint32_t nCardNo, uint32_t nHead, uint32_t * pPreViewTime, uint32_t * Vmax, double* Amax);
		typedef int32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_activate_scanahead_autodelays) (uint32_t nCardNo, int32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanahead_laser_shifts) (uint32_t nCardNo, int32_t nDLasOn, int32_t nDLasOff);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanahead_line_params) (uint32_t nCardNo, uint32_t nCornerScale, uint32_t nEndScale, uint32_t nAccScale);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanahead_line_params_ex) (uint32_t nCardNo, uint32_t nCornerScale, uint32_t nEndScale, uint32_t nAccScale, uint32_t nJumpScale);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanahead_params) (uint32_t nCardNo, uint32_t nMode, uint32_t nTableNo, uint32_t nPreviewTime, uint32_t nVMax, double dAmax);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanahead_speed_control) (uint32_t nCardNo, uint32_t nMode);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_micro_vector_abs_3d) (uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nZ, int32_t nLasOn, int32_t nLasOff);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_micro_vector_rel_3d) (uint32_t nCardNo, int32_t ndX, int32_t ndY, int32_t ndZ, int32_t nLasOn, int32_t nLasOff);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_micro_vector_abs) (uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nLasOn, int32_t nLasOff);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_micro_vector_rel) (uint32_t nCardNo, int32_t ndX, int32_t ndY, int32_t nLasOn, int32_t nLasOff);

		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_error) (uint32_t nCardNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_reset_error) (uint32_t nCardNo, uint32_t nCode);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_angle) (uint32_t nCardNo, uint32_t nHeadNo, double dAngle, uint32_t nAtOnce);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scale) (uint32_t nCardNo, uint32_t nHeadNo, double dScaleFactor, uint32_t nAtOnce);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_offset) (uint32_t nCardNo, uint32_t nHeadNo, int32_t nXOffset, int32_t nYOffset, uint32_t nAtOnce);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_waveform_offset) (uint32_t nCardNo, uint32_t nChannel, uint32_t nOffset, uint32_t nNumber, int32_t* pPtr);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_measurement_status) (uint32_t nCardNo, uint32_t* pnBusy, uint32_t* pnPos);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_transform) (int32_t * pSignal1, int32_t * pSignal2, uint8_t * pTransform, uint32_t nCode);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_upload_transform) (uint32_t nCardNo, uint32_t nHeadNo, uint8_t * pTransformData);

		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_timelag_compensation) (uint32_t nCardNo, uint32_t nHeadNo, uint32_t nTimelagXY, uint32_t nTimelagZ);
		
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_init_fly_2d) (uint32_t nCardNo, int32_t nOffsetX, int32_t nOffsetY, uint32_t nNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_activate_fly_2d) (uint32_t nCardNo, const double ScaleX, const double ScaleY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_activate_fly_2d_encoder) (uint32_t nCardNo, const double ScaleX, const double ScaleY, int32_t nEncoderOffsetX, int32_t nEncoderOffsetY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_fly_2d) (uint32_t nCardNo, const double ScaleX, const double ScaleY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_fly_2d_offset) (uint32_t nCardNo, const int32_t * pOffsetX, const int32_t* pOffsetY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_fly_x_pos) (uint32_t nCardNo, const double ScaleX);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_fly_y_pos) (uint32_t nCardNo, const double ScaleY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_fly_x) (uint32_t nCardNo, const double ScaleX);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_fly_y) (uint32_t nCardNo, const double ScaleY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_encoder) (uint32_t nCardNo, const int32_t* pEncoderX, const int32_t* pEncoderY);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_marking_info) (uint32_t nCardNo);


		class CScanLabSDK {
		private:
			bool m_bIsInitialized;

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
			PScanLabPtr_n_eth_set_com_timeouts_auto n_eth_set_com_timeouts_auto = nullptr;
			PScanLabPtr_n_eth_get_com_timeouts_auto n_eth_get_com_timeouts_auto = nullptr;
			PScanLabPtr_n_config_list n_config_list = nullptr;
			PScanLabPtr_n_set_laser_mode n_set_laser_mode = nullptr;
			PScanLabPtr_n_set_laser_control n_set_laser_control = nullptr;
			PScanLabPtr_n_set_auto_laser_control n_set_auto_laser_control = nullptr;
			PScanLabPtr_n_set_laser_pulses n_set_laser_pulses = nullptr;
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
			PScanLabPtr_n_write_io_port n_write_io_port_list = nullptr;
			PScanLabPtr_n_write_8bit_port n_write_8bit_port_list = nullptr;
			PScanLabPtr_n_write_da_1 n_write_da_1_list = nullptr;
			PScanLabPtr_n_write_da_2 n_write_da_2_list = nullptr;
			PScanLabPtr_n_jump_abs n_jump_abs = nullptr;
			PScanLabPtr_n_mark_abs n_mark_abs = nullptr;
			PScanLabPtr_n_jump_abs_3d n_jump_abs_3d = nullptr;
			PScanLabPtr_n_mark_abs_3d n_mark_abs_3d = nullptr;
			PScanLabPtr_n_long_delay n_long_delay = nullptr;
			PScanLabPtr_n_get_status n_get_status = nullptr;
			PScanLabPtr_n_get_input_pointer n_get_input_pointer = nullptr;
			PScanLabPtr_n_set_laser_delays n_set_laser_delays = nullptr;
			PScanLabPtr_n_set_start_list_pos n_set_start_list_pos = nullptr;		
			PScanLabPtr_n_set_defocus_list n_set_defocus_list = nullptr;
			PScanLabPtr_n_get_head_status n_get_head_status = nullptr;
			PScanLabPtr_n_get_value n_get_value = nullptr;
			PScanLabPtr_get_dll_version get_dll_version = nullptr;
			PScanLabPtr_n_get_hex_version n_get_hex_version = nullptr;
			PScanLabPtr_n_get_bios_version n_get_bios_version = nullptr;
			PScanLabPtr_n_get_rtc_version n_get_rtc_version = nullptr;
			PScanLabPtr_n_get_card_type n_get_card_type = nullptr;

			PScanLabPtr_n_set_mcbsp_freq n_set_mcbsp_freq = nullptr;
			PScanLabPtr_n_mcbsp_init n_mcbsp_init = nullptr;
			PScanLabPtr_n_mcbsp_init_spi n_mcbsp_init_spi = nullptr;
			PScanLabPtr_n_set_mcbsp_out_ptr n_set_mcbsp_out_ptr = nullptr;
			PScanLabPtr_n_set_multi_mcbsp_in n_set_multi_mcbsp_in = nullptr;
			PScanLabPtr_n_list_nop n_list_nop = nullptr;
			PScanLabPtr_n_set_free_variable_list n_set_free_variable_list  = nullptr;
			PScanLabPtr_n_set_free_variable n_set_free_variable = nullptr;
			PScanLabPtr_n_get_free_variable n_get_free_variable = nullptr;
			PScanLabPtr_n_set_trigger n_set_trigger = nullptr;
			PScanLabPtr_n_set_trigger4 n_set_trigger4 = nullptr;
			PScanLabPtr_n_set_control_mode n_set_control_mode = nullptr;
			PScanLabPtr_n_set_laser_pulses_ctrl n_set_laser_pulses_ctrl = nullptr;
			PScanLabPtr_n_set_mark_speed_ctrl n_set_mark_speed_ctrl = nullptr;
			PScanLabPtr_n_set_jump_speed_ctrl n_set_jump_speed_ctrl = nullptr;
			PScanLabPtr_n_set_firstpulse_killer n_set_firstpulse_killer = nullptr;
			PScanLabPtr_n_set_firstpulse_killer_list n_set_firstpulse_killer_list = nullptr;
			PScanLabPtr_n_set_qswitch_delay n_set_qswitch_delay = nullptr;
			PScanLabPtr_n_set_qswitch_delay_list n_set_qswitch_delay_list = nullptr;
			PScanLabPtr_n_write_da_x n_write_da_x = nullptr;
			PScanLabPtr_n_set_laser_pin_out n_set_laser_pin_out = nullptr;
			PScanLabPtr_n_get_laser_pin_in n_get_laser_pin_in = nullptr;
			PScanLabPtr_n_set_sky_writing_para n_set_sky_writing_para = nullptr;
			PScanLabPtr_n_set_sky_writing_limit n_set_sky_writing_limit = nullptr;
			PScanLabPtr_n_set_sky_writing_mode n_set_sky_writing_mode = nullptr;
			PScanLabPtr_n_set_sky_writing n_set_sky_writing = nullptr;
			PScanLabPtr_n_set_sky_writing_para_list n_set_sky_writing_para_list = nullptr;
			PScanLabPtr_n_set_sky_writing_list n_set_sky_writing_list = nullptr;
			PScanLabPtr_n_set_sky_writing_limit_list n_set_sky_writing_limit_list = nullptr;
			PScanLabPtr_n_set_sky_writing_mode_list n_set_sky_writing_mode_list = nullptr;
			PScanLabPtr_n_control_command n_control_command = nullptr;
			PScanLabPtr_n_get_scanahead_params n_get_scanahead_params = nullptr;
			PScanLabPtr_n_activate_scanahead_autodelays n_activate_scanahead_autodelays = nullptr;
			PScanLabPtr_n_set_scanahead_laser_shifts n_set_scanahead_laser_shifts = nullptr;
			PScanLabPtr_n_set_scanahead_line_params n_set_scanahead_line_params = nullptr;
			PScanLabPtr_n_set_scanahead_line_params_ex n_set_scanahead_line_params_ex = nullptr;
			PScanLabPtr_n_set_scanahead_params n_set_scanahead_params = nullptr;
			PScanLabPtr_n_set_scanahead_speed_control n_set_scanahead_speed_control = nullptr;
			PScanLabPtr_n_micro_vector_abs_3d n_micro_vector_abs_3d = nullptr;
			PScanLabPtr_n_micro_vector_rel_3d n_micro_vector_rel_3d = nullptr;
			PScanLabPtr_n_micro_vector_abs n_micro_vector_abs = nullptr;
			PScanLabPtr_n_micro_vector_rel n_micro_vector_rel = nullptr;
			PScanLabPtr_n_get_error n_get_error = nullptr;
			PScanLabPtr_n_reset_error n_reset_error = nullptr;
			PScanLabPtr_n_set_multi_mcbsp_in_list n_set_multi_mcbsp_in_list = nullptr;
			PScanLabPtr_n_set_laser_power n_set_laser_power = nullptr;
			PScanLabPtr_n_set_angle n_set_angle = nullptr;
			PScanLabPtr_n_set_scale n_set_scale = nullptr;
			PScanLabPtr_n_set_offset n_set_offset = nullptr;

			PScanLabPtr_n_get_waveform_offset n_get_waveform_offset = nullptr;
			PScanLabPtr_n_measurement_status n_measurement_status = nullptr;
			PScanLabPtr_transform transform = nullptr;
			PScanLabPtr_n_upload_transform n_upload_transform = nullptr;
			PScanLabPtr_n_set_timelag_compensation n_set_timelag_compensation = nullptr;

			PScanLabPtr_n_init_fly_2d n_init_fly_2d = nullptr;
			PScanLabPtr_n_activate_fly_2d n_activate_fly_2d = nullptr;
			PScanLabPtr_n_activate_fly_2d_encoder n_activate_fly_2d_encoder = nullptr;
			PScanLabPtr_n_set_fly_2d n_set_fly_2d = nullptr;
			PScanLabPtr_n_get_fly_2d_offset n_get_fly_2d_offset = nullptr;
			PScanLabPtr_n_set_fly_x_pos n_set_fly_x_pos = nullptr;
			PScanLabPtr_n_set_fly_y_pos n_set_fly_y_pos = nullptr;
			PScanLabPtr_n_set_fly_x n_set_fly_x = nullptr;
			PScanLabPtr_n_set_fly_y n_set_fly_y = nullptr;
			PScanLabPtr_n_get_encoder n_get_encoder = nullptr;
			PScanLabPtr_n_get_marking_info n_get_marking_info = nullptr;

			CScanLabSDK(const std::string & sDLLNameUTF8);
			~CScanLabSDK();

			void initDLL();
			void checkError(uint32_t nRTCError);
			void checkLastErrorOfCard(uint32_t nCardNo);
			void checkGlobalErrorOfCard(uint32_t nCardNo);

		};


		typedef std::shared_ptr<CScanLabSDK> PScanLabSDK;

	} 
} 

#endif // __LIBMCDRIVER_SCANLAB_SDK
