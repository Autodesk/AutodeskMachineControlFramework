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
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <mutex>

#ifdef _WIN32

#define SCANLAB_CALLINGCONVENTION __stdcall

#else

#define SCANLAB_CALLINGCONVENTION 

#endif

#define SCANLAB_LASERFIELD_MINIMUMUNITS -524288
#define SCANLAB_LASERFIELD_MAXIMUMUNITS 524287


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
		typedef int32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_eth_remove_card) (const uint32_t nCardNo);
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
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_io_port_mask_list) (uint32_t nCardNo, uint32_t nValue, uint32_t nMask);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_8bit_port_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_1_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_write_da_2_list) (uint32_t nCardNo, uint32_t nValue);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_jump_abs) (uint32_t nCardNo, int32_t nX, int32_t nY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_mark_abs) (uint32_t nCardNo, int32_t nX, int32_t nY);
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
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_trigger8) (uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2, uint32_t nSignal3, uint32_t nSignal4, uint32_t nSignal5, uint32_t nSignal6, uint32_t nSignal7, uint32_t nSignal8);

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
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_laser_pin_out_list) (uint32_t nCardNo, uint32_t nPins);
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
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_scanahead_params) (uint32_t nCardNo, uint32_t nMode, uint32_t nHeadNo, uint32_t nTableNo, uint32_t nPreviewTime, uint32_t nVMax, double dAmax);
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
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_matrix) (uint32_t nCardNo, uint32_t nHeadNo, double dM11, double dM12, double dM21, double dM22, uint32_t nAtOnce);

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
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_fly_return) (uint32_t nCardNo, int32_t sX, int32_t nY);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_encoder) (uint32_t nCardNo, const int32_t* pEncoderX, const int32_t* pEncoderY);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_get_marking_info) (uint32_t nCardNo);		
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_wait_for_encoder) (uint32_t nCardNo, int32_t nValue, uint32_t nEncoderNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_wait_for_encoder_mode) (uint32_t nCardNo, int32_t nValue, uint32_t nEncoderNo, int32_t nMode);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_fly_limits) (uint32_t nCardNo, int32_t nXMin, int32_t nXMax, int32_t nYMin, int32_t nYMax);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_range_checking) (uint32_t nCardNo, uint32_t nHeadNo, uint32_t nMode, uint32_t nData);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_stop_execution) (uint32_t nCardNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_timed_mark_abs) (uint32_t nCardNo, int32_t nX, int32_t nY, double dTime);
		typedef int32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_read_multi_mcbsp) (uint32_t nCardNo, uint32_t nRegisterNo);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_uart_config) (uint32_t nCardNo, uint32_t nBaudRate);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_rs232_write_data) (uint32_t nCardNo, uint32_t nData);
		typedef uint32_t(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_rs232_read_data) (uint32_t nCardNo);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_set_mcbsp_out_oie_ctrl) (uint32_t nCardNo, uint32_t nSignalID1, uint32_t nSignalID2);
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_eth_config_waveform_streaming_ctrl) (uint32_t nCardNo, uint32_t nSize, uint32_t nFlags);
			
		typedef void(SCANLAB_CALLINGCONVENTION* PScanLabPtr_n_eth_set_high_performance_mode) (uint32_t nCardNo, uint32_t nMode);

		class CScanLabSDKJournal {
		private:
			std::map<std::string, uint32_t> m_DefinedVariables;
			std::ofstream m_CStream;
			std::mutex m_Mutex;

			void writeCLine(const std::string & sLine);

		public:

			CScanLabSDKJournal(const std::string & sDebugFileName);
			virtual ~CScanLabSDKJournal();

			void logCall (const std::string & sFunctionName, const std::string & sCParameters);

			std::string defineDoubleVariable(const std::string & sVariableBaseName);
			std::string defineUint32Variable(const std::string& sVariableBaseName);
			std::string defineInt32Variable(const std::string& sVariableBaseName);

			std::string getVariableSuffix(const std::string& sVariableBaseName);

			std::string escapeString(const std::string & sString);
		};

		typedef std::shared_ptr<CScanLabSDKJournal> PScanLabSDKJournal;


		class CScanLabSDK {
		private:
			bool m_bIsInitialized;

			PScanLabSDKJournal m_pLogJournal;

			void* m_LibraryHandle;

			PScanLabPtr_init_rtc6_dll ptr_init_rtc6_dll = nullptr;
			PScanLabPtr_free_rtc6_dll ptr_free_rtc6_dll = nullptr;
			PScanLabPtr_eth_convert_string_to_ip ptr_eth_convert_string_to_ip = nullptr;
			PScanLabPtr_eth_set_search_cards_timeout ptr_eth_set_search_cards_timeout = nullptr;
			PScanLabPtr_eth_search_cards_range ptr_eth_search_cards_range = nullptr;
			PScanLabPtr_rtc6_count_cards ptr_rtc6_count_cards = nullptr;
			PScanLabPtr_eth_count_cards ptr_eth_count_cards = nullptr;
			PScanLabPtr_eth_found_cards ptr_eth_found_cards = nullptr;
			PScanLabPtr_eth_assign_card ptr_eth_assign_card = nullptr;
			PScanLabPtr_eth_remove_card ptr_eth_remove_card = nullptr;
			PScanLabPtr_acquire_rtc ptr_acquire_rtc = nullptr;
			PScanLabPtr_release_rtc ptr_release_rtc = nullptr;
			PScanLabPtr_n_get_serial_number ptr_n_get_serial_number = nullptr;
			PScanLabPtr_eth_get_serial_search ptr_eth_get_serial_search = nullptr;
			PScanLabPtr_eth_search_cards ptr_eth_search_cards = nullptr;
			PScanLabPtr_n_load_correction_file ptr_n_load_correction_file = nullptr;
			PScanLabPtr_n_select_cor_table ptr_n_select_cor_table = nullptr;
			PScanLabPtr_n_eth_set_com_timeouts_auto ptr_n_eth_set_com_timeouts_auto = nullptr;
			PScanLabPtr_n_eth_get_com_timeouts_auto ptr_n_eth_get_com_timeouts_auto = nullptr;
			PScanLabPtr_n_config_list ptr_n_config_list = nullptr;
			PScanLabPtr_n_set_laser_mode ptr_n_set_laser_mode = nullptr;
			PScanLabPtr_n_set_laser_control ptr_n_set_laser_control = nullptr;
			PScanLabPtr_n_set_auto_laser_control ptr_n_set_auto_laser_control = nullptr;
			PScanLabPtr_n_set_laser_pulses ptr_n_set_laser_pulses = nullptr;
			PScanLabPtr_n_set_standby ptr_n_set_standby = nullptr;
			PScanLabPtr_n_get_last_error ptr_n_get_last_error = nullptr;
			PScanLabPtr_get_last_error ptr_get_last_error = nullptr;
			PScanLabPtr_n_load_program_file ptr_n_load_program_file = nullptr;
			PScanLabPtr_n_get_table_para ptr_n_get_table_para = nullptr;
			PScanLabPtr_n_set_end_of_list ptr_n_set_end_of_list = nullptr;
			PScanLabPtr_n_execute_list_pos ptr_n_execute_list_pos = nullptr;
			PScanLabPtr_n_auto_change_pos ptr_n_auto_change_pos = nullptr;
			PScanLabPtr_n_set_scanner_delays ptr_n_set_scanner_delays = nullptr;
			PScanLabPtr_n_set_mark_speed ptr_n_set_mark_speed = nullptr;
			PScanLabPtr_n_set_jump_speed ptr_n_set_jump_speed = nullptr;
			PScanLabPtr_n_write_io_port ptr_n_write_io_port = nullptr;
			PScanLabPtr_n_write_8bit_port ptr_n_write_8bit_port = nullptr;
			PScanLabPtr_n_write_da_1 ptr_n_write_da_1 = nullptr;
			PScanLabPtr_n_write_da_2 ptr_n_write_da_2 = nullptr;
			PScanLabPtr_n_write_io_port_list ptr_n_write_io_port_list = nullptr;
			PScanLabPtr_n_write_io_port_mask_list ptr_n_write_io_port_mask_list = nullptr;
			PScanLabPtr_n_write_8bit_port ptr_n_write_8bit_port_list = nullptr;
			PScanLabPtr_n_write_da_1 ptr_n_write_da_1_list = nullptr;
			PScanLabPtr_n_write_da_2 ptr_n_write_da_2_list = nullptr;
			PScanLabPtr_n_jump_abs ptr_n_jump_abs = nullptr;
			PScanLabPtr_n_mark_abs ptr_n_mark_abs = nullptr;
			PScanLabPtr_n_long_delay ptr_n_long_delay = nullptr;
			PScanLabPtr_n_get_status ptr_n_get_status = nullptr;
			PScanLabPtr_n_get_input_pointer ptr_n_get_input_pointer = nullptr;
			PScanLabPtr_n_set_laser_delays ptr_n_set_laser_delays = nullptr;
			PScanLabPtr_n_set_start_list_pos ptr_n_set_start_list_pos = nullptr;
			PScanLabPtr_n_set_defocus_list ptr_n_set_defocus_list = nullptr;
			PScanLabPtr_n_get_head_status ptr_n_get_head_status = nullptr;
			PScanLabPtr_n_get_value ptr_n_get_value = nullptr;
			PScanLabPtr_get_dll_version ptr_get_dll_version = nullptr;
			PScanLabPtr_n_get_hex_version ptr_n_get_hex_version = nullptr;
			PScanLabPtr_n_get_bios_version ptr_n_get_bios_version = nullptr;
			PScanLabPtr_n_get_rtc_version ptr_n_get_rtc_version = nullptr;
			PScanLabPtr_n_get_card_type ptr_n_get_card_type = nullptr;

			PScanLabPtr_n_set_mcbsp_freq ptr_n_set_mcbsp_freq = nullptr;
			PScanLabPtr_n_mcbsp_init ptr_n_mcbsp_init = nullptr;
			PScanLabPtr_n_mcbsp_init_spi ptr_n_mcbsp_init_spi = nullptr;
			PScanLabPtr_n_set_mcbsp_out_ptr ptr_n_set_mcbsp_out_ptr = nullptr;
			PScanLabPtr_n_set_multi_mcbsp_in ptr_n_set_multi_mcbsp_in = nullptr;
			PScanLabPtr_n_list_nop ptr_n_list_nop = nullptr;
			PScanLabPtr_n_set_free_variable_list ptr_n_set_free_variable_list = nullptr;
			PScanLabPtr_n_set_free_variable ptr_n_set_free_variable = nullptr;
			PScanLabPtr_n_get_free_variable ptr_n_get_free_variable = nullptr;
			PScanLabPtr_n_set_trigger ptr_n_set_trigger = nullptr;
			PScanLabPtr_n_set_trigger4 ptr_n_set_trigger4 = nullptr;
			PScanLabPtr_n_set_trigger8 ptr_n_set_trigger8 = nullptr;
			PScanLabPtr_n_set_control_mode ptr_n_set_control_mode = nullptr;
			PScanLabPtr_n_set_laser_pulses_ctrl ptr_n_set_laser_pulses_ctrl = nullptr;
			PScanLabPtr_n_set_mark_speed_ctrl ptr_n_set_mark_speed_ctrl = nullptr;
			PScanLabPtr_n_set_jump_speed_ctrl ptr_n_set_jump_speed_ctrl = nullptr;
			PScanLabPtr_n_set_firstpulse_killer ptr_n_set_firstpulse_killer = nullptr;
			PScanLabPtr_n_set_firstpulse_killer_list ptr_n_set_firstpulse_killer_list = nullptr;
			PScanLabPtr_n_set_qswitch_delay ptr_n_set_qswitch_delay = nullptr;
			PScanLabPtr_n_set_qswitch_delay_list ptr_n_set_qswitch_delay_list = nullptr;
			PScanLabPtr_n_write_da_x ptr_n_write_da_x = nullptr;
			PScanLabPtr_n_set_laser_pin_out ptr_n_set_laser_pin_out = nullptr;
			PScanLabPtr_n_get_laser_pin_in ptr_n_get_laser_pin_in = nullptr;
			PScanLabPtr_n_set_laser_pin_out_list ptr_n_set_laser_pin_out_list = nullptr;
			PScanLabPtr_n_set_sky_writing_para ptr_n_set_sky_writing_para = nullptr;
			PScanLabPtr_n_set_sky_writing_limit ptr_n_set_sky_writing_limit = nullptr;
			PScanLabPtr_n_set_sky_writing_mode ptr_n_set_sky_writing_mode = nullptr;
			PScanLabPtr_n_set_sky_writing ptr_n_set_sky_writing = nullptr;
			PScanLabPtr_n_set_sky_writing_para_list ptr_n_set_sky_writing_para_list = nullptr;
			PScanLabPtr_n_set_sky_writing_list ptr_n_set_sky_writing_list = nullptr;
			PScanLabPtr_n_set_sky_writing_limit_list ptr_n_set_sky_writing_limit_list = nullptr;
			PScanLabPtr_n_set_sky_writing_mode_list ptr_n_set_sky_writing_mode_list = nullptr;
			PScanLabPtr_n_control_command ptr_n_control_command = nullptr;
			PScanLabPtr_n_get_scanahead_params ptr_n_get_scanahead_params = nullptr;
			PScanLabPtr_n_activate_scanahead_autodelays ptr_n_activate_scanahead_autodelays = nullptr;
			PScanLabPtr_n_set_scanahead_laser_shifts ptr_n_set_scanahead_laser_shifts = nullptr;
			PScanLabPtr_n_set_scanahead_line_params ptr_n_set_scanahead_line_params = nullptr;
			PScanLabPtr_n_set_scanahead_line_params_ex ptr_n_set_scanahead_line_params_ex = nullptr;
			PScanLabPtr_n_set_scanahead_params ptr_n_set_scanahead_params = nullptr;
			PScanLabPtr_n_set_scanahead_speed_control ptr_n_set_scanahead_speed_control = nullptr;
			PScanLabPtr_n_micro_vector_abs_3d ptr_n_micro_vector_abs_3d = nullptr;
			PScanLabPtr_n_micro_vector_rel_3d ptr_n_micro_vector_rel_3d = nullptr;
			PScanLabPtr_n_micro_vector_abs ptr_n_micro_vector_abs = nullptr;
			PScanLabPtr_n_micro_vector_rel ptr_n_micro_vector_rel = nullptr;
			PScanLabPtr_n_get_error ptr_n_get_error = nullptr;
			PScanLabPtr_n_reset_error ptr_n_reset_error = nullptr;
			PScanLabPtr_n_set_multi_mcbsp_in_list ptr_n_set_multi_mcbsp_in_list = nullptr;
			PScanLabPtr_n_set_laser_power ptr_n_set_laser_power = nullptr;
			PScanLabPtr_n_set_angle ptr_n_set_angle = nullptr;
			PScanLabPtr_n_set_scale ptr_n_set_scale = nullptr;
			PScanLabPtr_n_set_offset ptr_n_set_offset = nullptr;
			PScanLabPtr_n_set_matrix ptr_n_set_matrix = nullptr;

			PScanLabPtr_n_get_waveform_offset ptr_n_get_waveform_offset = nullptr;
			PScanLabPtr_n_measurement_status ptr_n_measurement_status = nullptr;
			PScanLabPtr_transform ptr_transform = nullptr;
			PScanLabPtr_n_upload_transform ptr_n_upload_transform = nullptr;
			PScanLabPtr_n_set_timelag_compensation ptr_n_set_timelag_compensation = nullptr;

			PScanLabPtr_n_init_fly_2d ptr_n_init_fly_2d = nullptr;
			PScanLabPtr_n_activate_fly_2d ptr_n_activate_fly_2d = nullptr;
			PScanLabPtr_n_activate_fly_2d_encoder ptr_n_activate_fly_2d_encoder = nullptr;
			PScanLabPtr_n_set_fly_2d ptr_n_set_fly_2d = nullptr;
			PScanLabPtr_n_get_fly_2d_offset ptr_n_get_fly_2d_offset = nullptr;
			PScanLabPtr_n_set_fly_x_pos ptr_n_set_fly_x_pos = nullptr;
			PScanLabPtr_n_set_fly_y_pos ptr_n_set_fly_y_pos = nullptr;
			PScanLabPtr_n_set_fly_x ptr_n_set_fly_x = nullptr;
			PScanLabPtr_n_set_fly_y ptr_n_set_fly_y = nullptr;
			PScanLabPtr_n_fly_return ptr_n_fly_return = nullptr;
			PScanLabPtr_n_get_encoder ptr_n_get_encoder = nullptr;
			PScanLabPtr_n_get_marking_info ptr_n_get_marking_info = nullptr;
			PScanLabPtr_n_wait_for_encoder ptr_n_wait_for_encoder = nullptr;
			PScanLabPtr_n_wait_for_encoder_mode ptr_n_wait_for_encoder_mode = nullptr;
			PScanLabPtr_n_set_fly_limits ptr_n_set_fly_limits = nullptr;
			PScanLabPtr_n_range_checking ptr_n_range_checking = nullptr;
			PScanLabPtr_n_stop_execution ptr_n_stop_execution = nullptr;
			PScanLabPtr_n_timed_mark_abs ptr_n_timed_mark_abs = nullptr;
			PScanLabPtr_n_read_multi_mcbsp ptr_n_read_multi_mcbsp = nullptr;
			PScanLabPtr_n_uart_config ptr_n_uart_config = nullptr;
			PScanLabPtr_n_rs232_write_data ptr_n_rs232_write_data = nullptr;
			PScanLabPtr_n_rs232_read_data ptr_n_rs232_read_data = nullptr;
			PScanLabPtr_n_set_mcbsp_out_oie_ctrl ptr_n_set_mcbsp_out_oie_ctrl = nullptr;
			PScanLabPtr_n_eth_config_waveform_streaming_ctrl ptr_n_eth_config_waveform_streaming_ctrl = nullptr;
			PScanLabPtr_n_eth_set_high_performance_mode ptr_n_eth_set_high_performance_mode = nullptr;

			void resetFunctionPtrs ();
		public:

			CScanLabSDK(const std::string & sDLLNameUTF8);
			~CScanLabSDK();

			void setJournal(PScanLabSDKJournal pLogJournal);

			void initDLL();
			void reinitDLL();

			void checkError(uint32_t nRTCError);
			void checkLastErrorOfCard(uint32_t nCardNo);
			void checkGlobalErrorOfCard(uint32_t nCardNo);

			uint32_t init_rtc6_dll ();
			void free_rtc6_dll ();
			uint32_t  eth_convert_string_to_ip (const char* pIPString);
			void eth_set_search_cards_timeout (const uint32_t nTimeOut);
			uint32_t eth_search_cards_range (const uint32_t nStartIP, const uint32_t nEndIP);
			uint32_t rtc6_count_cards ();
			uint32_t eth_count_cards ();
			uint32_t eth_found_cards ();
			int32_t eth_assign_card (const uint32_t nSearchNo, const uint32_t nCardNo);
			int32_t eth_remove_card(const uint32_t nCardNo);
			uint32_t acquire_rtc (uint32_t nCardNo);
			uint32_t release_rtc (uint32_t nCardNo);
			uint32_t n_get_serial_number (uint32_t nCardNo);
			uint32_t eth_get_serial_search (uint32_t nSearchNo);
			uint32_t eth_search_cards (uint32_t nIP, uint32_t n_NetMask);
			uint32_t n_load_correction_file (uint32_t nCardNo, const char* nFileName, uint32_t nNo, uint32_t n_Dimension);

			void n_eth_set_com_timeouts_auto (uint32_t nCardNo, const double dInitialTimeout, const double dMaxTimeout, const double dMultiplier, const uint32_t nMode);
			void n_eth_get_com_timeouts_auto (uint32_t nCardNo, double* pdInitialTimeout, double* pdMaxTimeout, double* pdMultiplier, uint32_t* pnMode);

			void n_select_cor_table (uint32_t nCardNo, uint32_t nHeadA, uint32_t nHeadB);
			void n_config_list (uint32_t nCardNo, uint32_t nMem1, uint32_t nMem2);
			void n_set_laser_mode (uint32_t nCardNo, uint32_t nMode);
			void n_set_laser_control (uint32_t nCardNo, uint32_t nControl);
			void n_set_auto_laser_control (uint32_t nCardNo, uint32_t nControl, uint32_t nValue, uint32_t nMode, uint32_t nMinValue, uint32_t nMaxValue);
			void n_set_laser_pulses (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
			void n_set_standby (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
			uint32_t n_get_last_error (uint32_t nCardNo);
			uint32_t get_last_error ();
			uint32_t n_load_program_file (uint32_t nCardNo, const char* pPath);
			double n_get_table_para (uint32_t nCardNo, uint32_t nTableNo, uint32_t nParaNo);
			void n_set_end_of_list (uint32_t nCardNo);
			void n_execute_list_pos (uint32_t nCardNo, uint32_t nListNo, uint32_t nPos);
			void n_auto_change_pos (uint32_t nCardNo, uint32_t nPos);
			void n_set_scanner_delays (uint32_t nCardNo, uint32_t nJump, uint32_t nMark, uint32_t nPolygon);
			void n_set_mark_speed (uint32_t nCardNo, double dSpeed);
			void n_set_jump_speed (uint32_t nCardNo, double dSpeed);
			void n_write_io_port (uint32_t nCardNo, uint32_t nValue);
			void n_write_8bit_port (uint32_t nCardNo, uint32_t nValue);
			void n_write_da_1 (uint32_t nCardNo, uint32_t nValue);
			void n_write_da_2 (uint32_t nCardNo, uint32_t nValue);
			void n_write_io_port_list (uint32_t nCardNo, uint32_t nValue);
			void n_write_io_port_mask_list(uint32_t nCardNo, uint32_t nValue, uint32_t nMask);			
			void n_write_8bit_port_list (uint32_t nCardNo, uint32_t nValue);
			void n_write_da_1_list (uint32_t nCardNo, uint32_t nValue);
			void n_write_da_2_list (uint32_t nCardNo, uint32_t nValue);
			void n_jump_abs (uint32_t nCardNo, int32_t nX, int32_t nY);
			void n_mark_abs (uint32_t nCardNo, int32_t nX, int32_t nY);
			void n_long_delay (uint32_t nCardNo, uint32_t nDelay);
			void n_get_status (uint32_t nCardNo, uint32_t* pnStatus, uint32_t* pnPos);
			uint32_t n_get_input_pointer (uint32_t nCardNo);
			void n_set_laser_delays (uint32_t nCardNo, int32_t nLaserOnDelay, uint32_t nLaserOffDelay);
			void n_set_start_list_pos (uint32_t nCardNo, uint32_t nListNo, uint32_t nListPos);
			void n_set_defocus_list (uint32_t nCardNo, int32_t nShift);
			uint32_t n_get_head_status (uint32_t nCardNo, uint32_t nHeadNo);
			int32_t n_get_value (uint32_t nCardNo, uint32_t nSignalNo);
			uint32_t get_dll_version ();
			uint32_t n_get_hex_version (uint32_t nCardNo);
			uint32_t n_get_bios_version (uint32_t nCardNo);
			uint32_t n_get_rtc_version (uint32_t nCardNo);
			uint32_t n_get_card_type (uint32_t nCardNo);

			uint32_t n_set_mcbsp_freq (uint32_t nCardNo, uint32_t nFrequency);
			void n_mcbsp_init (uint32_t nCardNo, uint32_t nXDelay, uint32_t nYDelay);
			void n_mcbsp_init_spi (uint32_t nCardNo, uint32_t nClockLevel, uint32_t nClockDelay);
			void n_set_mcbsp_out_ptr (uint32_t nCardNo, uint32_t nNumber, void* pSignalPtr);
			void n_set_multi_mcbsp_in (uint32_t nCardNo, uint32_t nCtrl, uint32_t nP, uint32_t nMode);
			void n_set_multi_mcbsp_in_list (uint32_t nCardNo, uint32_t nCtrl, uint32_t nP, uint32_t nMode);
			void n_set_laser_power (uint32_t nCardNo, uint32_t nPort, uint32_t nPower);
			void n_list_nop (uint32_t nCardNo);

			void n_set_free_variable_list (uint32_t nCardNo, uint32_t nVarNo, uint32_t nValue);
			void n_set_free_variable (uint32_t nCardNo, uint32_t nVarNo, uint32_t nValue);
			uint32_t n_get_free_variable (uint32_t nCardNo, uint32_t nVarNo);

			void n_set_trigger (uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2);
			void n_set_trigger4 (uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2, uint32_t nSignal3, uint32_t nSignal4);
			void n_set_trigger8 (uint32_t nCardNo, uint32_t nPeriod, uint32_t nSignal1, uint32_t nSignal2, uint32_t nSignal3, uint32_t nSignal4, uint32_t nSignal5, uint32_t nSignal6, uint32_t nSignal7, uint32_t nSignal8);

			void n_set_control_mode (uint32_t nCardNo, uint32_t nMode);
			void n_set_laser_pulses_ctrl (uint32_t nCardNo, uint32_t nHalfPeriod, uint32_t nPulseLength);
			void n_set_mark_speed_ctrl (uint32_t nCardNo, double dSpeed);
			void n_set_jump_speed_ctrl (uint32_t nCardNo, double dSpeed);
			void n_set_firstpulse_killer (uint32_t nCardNo, uint32_t nLength);
			void n_set_firstpulse_killer_list (uint32_t nCardNo, uint32_t nLength);
			void n_set_qswitch_delay (uint32_t nCardNo, uint32_t nDelay);
			void n_set_qswitch_delay_list (uint32_t nCardNo, uint32_t nDelay);
			void n_write_da_x (uint32_t nCardNo, uint32_t nX, uint32_t nValue);

			void n_set_laser_pin_out (uint32_t nCardNo, uint32_t nPins);
			uint32_t n_get_laser_pin_in (uint32_t nCardNo);
			void n_set_laser_pin_out_list(uint32_t nCardNo, uint32_t nPins);

			void n_set_sky_writing_para (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift, uint32_t nNprev, uint32_t nNPost);
			void n_set_sky_writing_limit (uint32_t nCardNo, double dCosAngle);
			void n_set_sky_writing_mode (uint32_t nCardNo, uint32_t nMode);
			void n_set_sky_writing (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift);
			void n_set_sky_writing_para_list (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift, uint32_t nNprev, uint32_t nNPost);
			void n_set_sky_writing_list (uint32_t nCardNo, double dTimelag, int32_t nLaserOnShift);
			void n_set_sky_writing_limit_list (uint32_t nCardNo, double dCosAngle);
			void n_set_sky_writing_mode_list (uint32_t nCardNo, uint32_t nMode);

			void n_control_command (uint32_t nCardNo, uint32_t nHead, uint32_t nAxis, uint32_t nData);

			uint32_t n_get_scanahead_params (uint32_t nCardNo, uint32_t nHead, uint32_t* pPreViewTime, uint32_t* Vmax, double* Amax);
			int32_t n_activate_scanahead_autodelays (uint32_t nCardNo, int32_t nMode);
			void n_set_scanahead_laser_shifts (uint32_t nCardNo, int32_t nDLasOn, int32_t nDLasOff);
			void n_set_scanahead_line_params (uint32_t nCardNo, uint32_t nCornerScale, uint32_t nEndScale, uint32_t nAccScale);
			void n_set_scanahead_line_params_ex (uint32_t nCardNo, uint32_t nCornerScale, uint32_t nEndScale, uint32_t nAccScale, uint32_t nJumpScale);
			uint32_t n_set_scanahead_params (uint32_t nCardNo, uint32_t nMode, uint32_t nHeadNo, uint32_t nTableNo, uint32_t nPreviewTime, uint32_t nVMax, double dAmax);
			void n_set_scanahead_speed_control (uint32_t nCardNo, uint32_t nMode);

			/*void n_micro_vector_abs_3d (uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nZ, int32_t nLasOn, int32_t nLasOff);
			void n_micro_vector_rel_3d (uint32_t nCardNo, int32_t ndX, int32_t ndY, int32_t ndZ, int32_t nLasOn, int32_t nLasOff);
			void n_micro_vector_abs (uint32_t nCardNo, int32_t nX, int32_t nY, int32_t nLasOn, int32_t nLasOff);
			void n_micro_vector_rel (uint32_t nCardNo, int32_t ndX, int32_t ndY, int32_t nLasOn, int32_t nLasOff); */

			uint32_t n_get_error (uint32_t nCardNo);
			void n_reset_error (uint32_t nCardNo, uint32_t nCode);

			void n_set_angle (uint32_t nCardNo, uint32_t nHeadNo, double dAngle, uint32_t nAtOnce);
			void n_set_scale (uint32_t nCardNo, uint32_t nHeadNo, double dScaleFactor, uint32_t nAtOnce);
			void n_set_offset (uint32_t nCardNo, uint32_t nHeadNo, int32_t nXOffset, int32_t nYOffset, uint32_t nAtOnce);
			void n_set_matrix (uint32_t nCardNo, uint32_t nHeadNo, double dM11, double dM12, double dM21, double dM22, uint32_t nAtOnce);

			void n_get_waveform_offset (uint32_t nCardNo, uint32_t nChannel, uint32_t nOffset, uint32_t nNumber, int32_t* pPtr);
			void n_measurement_status (uint32_t nCardNo, uint32_t* pnBusy, uint32_t* pnPos);
			uint32_t transform (int32_t* pSignal1, int32_t* pSignal2, uint8_t* pTransform, uint32_t nCode);
			uint32_t n_upload_transform (uint32_t nCardNo, uint32_t nHeadNo, uint8_t* pTransformData);

			void n_set_timelag_compensation (uint32_t nCardNo, uint32_t nHeadNo, uint32_t nTimelagXY, uint32_t nTimelagZ);

			void n_init_fly_2d (uint32_t nCardNo, int32_t nOffsetX, int32_t nOffsetY, uint32_t nNo);
			void n_activate_fly_2d (uint32_t nCardNo, const double ScaleX, const double ScaleY);
			void n_activate_fly_2d_encoder (uint32_t nCardNo, const double ScaleX, const double ScaleY, int32_t nEncoderOffsetX, int32_t nEncoderOffsetY);
			void n_set_fly_2d (uint32_t nCardNo, const double ScaleX, const double ScaleY);
			void n_get_fly_2d_offset (uint32_t nCardNo, const int32_t* pOffsetX, const int32_t* pOffsetY);
			void n_set_fly_x_pos (uint32_t nCardNo, const double ScaleX);
			void n_set_fly_y_pos (uint32_t nCardNo, const double ScaleY);
			void n_set_fly_x (uint32_t nCardNo, const double ScaleX);
			void n_set_fly_y (uint32_t nCardNo, const double ScaleY);
			void n_fly_return (uint32_t nCardNo, int32_t sX, int32_t nY);
			void n_get_encoder (uint32_t nCardNo, const int32_t* pEncoderX, const int32_t* pEncoderY);
			uint32_t n_get_marking_info (uint32_t nCardNo);
			void n_wait_for_encoder (uint32_t nCardNo, int32_t nValue, uint32_t nEncoderNo);
			void n_wait_for_encoder_mode (uint32_t nCardNo, int32_t nValue, uint32_t nEncoderNo, int32_t nMode);
			void n_set_fly_limits (uint32_t nCardNo, int32_t nXMin, int32_t nXMax, int32_t nYMin, int32_t nYMax);
			void n_range_checking (uint32_t nCardNo, uint32_t nHeadNo, uint32_t nMode, uint32_t nData);
			void n_stop_execution (uint32_t nCardNo);
			void n_timed_mark_abs (uint32_t nCardNo, int32_t nX, int32_t nY, double dTime);
			int32_t n_read_multi_mcbsp(uint32_t nCardNo, uint32_t nRegisterNo);
			uint32_t n_uart_config(uint32_t nCardNo, uint32_t nBaudRate);
			void n_rs232_write_data(uint32_t nCardNo, uint32_t nData);
			uint32_t n_rs232_read_data(uint32_t nCardNo);
			void n_set_mcbsp_out_oie_ctrl (uint32_t nCardNo, uint32_t nSignalID1, uint32_t nSignalID2);
			void n_eth_config_waveform_streaming_ctrl(uint32_t nCardNo, uint32_t nSize, uint32_t nFlags);
			void n_eth_set_high_performance_mode(uint32_t nCardNo, uint32_t nMode);

		};


		typedef std::shared_ptr<CScanLabSDK> PScanLabSDK;

	} 
} 

#endif // __LIBMCDRIVER_SCANLAB_SDK
