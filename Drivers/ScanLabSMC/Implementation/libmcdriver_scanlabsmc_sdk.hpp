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


#ifndef __LIBMCDRIVER_SCANLABSMC_SDK
#define __LIBMCDRIVER_SCANLABSMC_SDK

#include "libmcdriver_scanlabsmc_types.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"

#include <memory>
#include <string>
#include <mutex>

#ifdef _WIN32

#define SCANLABSMC_CALLINGCONVENTION __stdcall

#else

#define SCANLABSMC_CALLINGCONVENTION 

#endif



namespace LibMCDriver_ScanLabSMC {
	namespace Impl {

		typedef size_t slscHandle;
		typedef size_t slscJobID;
		typedef uint32_t slscReturnValue;

		enum class slsc_PolylineGeometry : uint8_t
		{
			slsc_Polyline_Open = 0,
			slsc_Polyline_Closed = 1
		};
		

		enum class slsc_PolylineProfile : uint8_t
		{
			slsc_Maximize_Velocity = 0,
			slsc_Constant_Velocity = 1
									
		};

		struct _slsc_PolylineOptions
		{
			slsc_PolylineGeometry Geometry;
			slsc_PolylineProfile ProfileType;
		};

		struct _slsc_VersionInfo
		{
			uint32_t m_nMajor;
			uint32_t m_nMinor;
			uint32_t m_nRevision;
		};

		enum class slsc_ExecState : int32_t
		{
			slsc_ExecState_Idle = 0, 
			slsc_ExecState_ReadyForExecution = 1, //!< The RTC6 board is ready to execute jobs.
			slsc_ExecState_Executing = 2,         //!< The RTC6 board is executing a job.
			slsc_ExecState_NotInitOrError = 3,    //!< The RTC6 board is not initialized or an error is present.
		};

		enum class slsc_JobCharacteristic : uint32_t
		{
			slsc_JobCharacteristic_Cart_XPos_ScanAxis = 0, //!< Max. absolute scan head position in X direction. In mm.
			slsc_JobCharacteristic_Cart_YPos_ScanAxis = 1, //!< Max. absolute scan head position in Y direction. In mm.
			slsc_JobCharacteristic_Cart_ZPos_ScanAxis = 2, //!< Max. absolute scan head position in Z direction. In mm.
			slsc_JobCharacteristic_Cart_XPos_StageAxis = 10, //!< Max. absolute stage position in X direction. In mm.
			slsc_JobCharacteristic_Cart_YPos_StageAxis = 11, //!< Max. absolute stage position in Y direction. In mm.
			slsc_JobCharacteristic_Cart_ZPos_StageAxis = 12, //!< Max. absolute stage position in Z direction. In mm.
			slsc_JobCharacteristic_Cart_XVel_ScanAxis = 20, //!< Max. absolute scan head velocity in X direction. In mm/s.
			slsc_JobCharacteristic_Cart_YVel_ScanAxis = 21, //!< Max. absolute scan head velocity in Y direction. In mm/s.
			slsc_JobCharacteristic_Cart_ZVel_ScanAxis = 22, //!< Max. absolute scan head velocity in Z direction. In mm/s.
			slsc_JobCharacteristic_Cart_XVel_StageAxis = 30, //!< Max. absolute stage velocity in X direction. In mm/s.
			slsc_JobCharacteristic_Cart_YVel_StageAxis = 31, //!< Max. absolute stage velocity in Y direction. In mm/s.
			slsc_JobCharacteristic_Cart_ZVel_StageAxis = 32, //!< Max. absolute stage velocity in X direction. In mm/s.
			slsc_JobCharacteristic_Cart_XAcc_ScanAxis = 40, //!< Max. absolute scan head acceleration in X direction. In mm/s^2.
			slsc_JobCharacteristic_Cart_YAcc_ScanAxis = 41, //!< Max. absolute scan head acceleration in Y direction.In mm/s^2.
			slsc_JobCharacteristic_Cart_ZAcc_ScanAxis = 42, //!< Max. absolute scan head acceleration in Zdirection.In mm/s^2.
			slsc_JobCharacteristic_Cart_XAcc_StageAxis = 50, //!< Max. absolute stage acceleration in X direction. In mm/s^2.
			slsc_JobCharacteristic_Cart_YAcc_StageAxis = 51, //!< Max. absolute stage acceleration in Y direction. In mm/s^2.
			slsc_JobCharacteristic_Cart_ZAcc_StageAxis = 52, //!< Max. absolute stage acceleration in Z direction. In mm/s^2.

			slsc_JobCharacteristic_Cart_XJerk_StageAxis = 70, //!< Max. absolute stage jerk in X direction. In mm/s^3.
			slsc_JobCharacteristic_Cart_YJerk_StageAxis = 71, //!< Max. absolute stage jerk in Y direction. In mm/s^3.
			slsc_JobCharacteristic_Cart_ZJerk_StageAxis = 72, //!< Max. absolute stage jerk in Z direction. In mm/s^3.
			slsc_JobCharacteristic_Cart_XPos_ScanAxis_LaserOn = 80, //!< Max. absolute scan head position in X direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_YPos_ScanAxis_LaserOn = 81, //!< Max. absolute scan head position in Y direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_ZPos_ScanAxis_LaserOn = 82, //!< Max. absolute scan head position in Z direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_XPos_StageAxis_LaserOn = 90, //!< Max. abs. stage position in X direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_YPos_StageAxis_LaserOn = 91, //!< Max. abs. stage position in Y direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_ZPos_StageAxis_LaserOn = 92, //!< Max. abs. stage position in Z direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_XPos_ScanAxis_Max = 100, //!< Max. scan head position in X direction. In mm.
			slsc_JobCharacteristic_Cart_YPos_ScanAxis_Max = 101, //!< Max. scan head position in Y direction. In mm.
			slsc_JobCharacteristic_Cart_ZPos_ScanAxis_Max = 102, //!< Max. scan head position in Z direction. In mm.
			slsc_JobCharacteristic_Cart_XPos_ScanAxis_Min = 110, //!< Min. scan head position in X direction. In mm.
			slsc_JobCharacteristic_Cart_YPos_ScanAxis_Min = 111, //!< Min. scan head position in Y direction. In mm.
			slsc_JobCharacteristic_Cart_ZPos_ScanAxis_Min = 112, //!< Min. scan head position in Z direction. In mm.
			slsc_JobCharacteristic_Cart_XPos_ScanAxis_LaserOn_Max = 120, //!< Max. scan head position in X direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_YPos_ScanAxis_LaserOn_Max = 121, //!< Max. scan head position in Y direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_ZPos_ScanAxis_LaserOn_Max = 122, //!< Max. scan head position in Z direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_XPos_ScanAxis_LaserOn_Min = 130, //!< Min. scan head position in X direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_YPos_ScanAxis_LaserOn_Min = 131, //!< Min. scan head position in Y direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_ZPos_ScanAxis_LaserOn_Min = 132, //!< Min. scan head position in Z direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_XPos_StageAxis_Max = 140, //!< Max. stage position in X direction. In mm.
			slsc_JobCharacteristic_Cart_YPos_StageAxis_Max = 141, //!< Max. stage position in Y direction. In mm.
			slsc_JobCharacteristic_Cart_ZPos_StageAxis_Max = 142, //!< Max. stage position in Z direction. In mm.
			slsc_JobCharacteristic_Cart_XPos_StageAxis_Min = 150, //!< Min. stage position in X direction. In mm.
			slsc_JobCharacteristic_Cart_YPos_StageAxis_Min = 151, //!< Min. stage position in Y direction. In mm.
			slsc_JobCharacteristic_Cart_ZPos_StageAxis_Min = 152, //!< Min. stage position in Z direction. In mm.
			slsc_JobCharacteristic_Cart_XPos_StageAxis_LaserOn_Max = 160, //!< Max. stage position in X direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_YPos_StageAxis_LaserOn_Max = 161, //!< Max. stage position in Y direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_ZPos_StageAxis_LaserOn_Max = 162, //!< Max. stage position in Z direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_XPos_StageAxis_LaserOn_Min = 170, //!< Min. stage position in X direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_YPos_StageAxis_LaserOn_Min = 171, //!< Min. stage position in Y direction with active laser. In mm.
			slsc_JobCharacteristic_Cart_ZPos_StageAxis_LaserOn_Min = 172, //!< Min. stage position in Z direction with active laser. In mm.
			slsc_JobCharacteristic_InsertedSkywritings = 180, //!< Number of transitions in the job where a skywriting had to be inserted.
			slsc_JobCharacteristic_MotionMicroSteps = 181, //!< Number of micro vectors that make up the job.
			slsc_JobCharacteristic_MinimalMarkSpeed = 182, //!< Minimum velocity reached by the laser spot during the job.
			slsc_JobCharacteristic_MaximalMarkSpeed = 183, //!< Maximum velocity reached by the laser spot during the job.

		};


		typedef struct _slsc_PolylineOptions slsc_PolylineOptions;
		typedef struct _slsc_VersionInfo slsc_VersionInfo;

		typedef slsc_VersionInfo(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_cfg_get_scanmotioncontrol_version) (void);

		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION *PScanLabSMCPtr_slsc_cfg_initialize_from_file) (slscHandle * Handle, const char* XmlConfigFileName);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_cfg_delete) (size_t Handle);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_begin) (size_t Handle, size_t* JobID);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_end) (size_t Handle);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_jump) (size_t Handle, const double* Target);

		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_begin_polyline) (size_t Handle, const slsc_PolylineOptions Options);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_slsc_job_end_polyline) (size_t Handle);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_line) (size_t Handle, const double* Target);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_start_execution) (size_t Handle);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_stop) (size_t Handle);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_stop_controlled) (size_t Handle);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_get_exec_state) (size_t Handle, slsc_ExecState * execState);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_get_error) (size_t Handle, size_t ErrorNr, uint64_t & nErrorCode, char * pErrorMsg, size_t nBufSize);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_get_error_count) (size_t Handle, size_t & nErrorCount);

		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_set_jump_speed) (size_t Handle, double dJumpSpeed);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_set_mark_speed) (size_t Handle, double dMarkSpeed);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_set_min_mark_speed) (size_t Handle, double dMinimalMarkSpeed);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_jump_min_time) (size_t Handle, const double* Target, double dMinimalJumpTime);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_job_set_corner_tolerance) (size_t Handle, const double* Target, double dCornerTolerance);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_get_simulation_filename) (size_t Handle, size_t nJobID, char * pszBuffer, size_t nBufferSize);
		typedef slscReturnValue(SCANLABSMC_CALLINGCONVENTION* PScanLabSMCPtr_slsc_ctrl_get_job_characteristic) (size_t Handle, size_t nJobID, slsc_JobCharacteristic eKey, double * pdValue);
		
		class CScanLabSMCSDK_DLLDirectoryCache {
		private:
#ifdef _WIN32
			std::wstring m_sCachedDLLDirectoryW;
#endif // _WIN32

		public:
			CScanLabSMCSDK_DLLDirectoryCache();
			virtual ~CScanLabSMCSDK_DLLDirectoryCache();
		};

		typedef std::shared_ptr<CScanLabSMCSDK_DLLDirectoryCache> PScanLabSMCSDK_DLLDirectoryCache;

		class CScanLabSMCSDK {
		private:
			bool m_bIsInitialized;
			std::wstring m_sDLLDirectoryW;

			std::mutex m_ListErrorMutex;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();

			PScanLabSMCSDK_DLLDirectoryCache cacheDllDirectory();

		public:

			PScanLabSMCPtr_slsc_cfg_get_scanmotioncontrol_version slsc_cfg_get_scanmotioncontrol_version = nullptr;
			PScanLabSMCPtr_slsc_cfg_initialize_from_file slsc_cfg_initialize_from_file = nullptr;
			PScanLabSMCPtr_slsc_cfg_delete slsc_cfg_delete = nullptr;
			PScanLabSMCPtr_slsc_job_begin slsc_job_begin = nullptr;
			PScanLabSMCPtr_slsc_job_end slsc_job_end = nullptr;
			PScanLabSMCPtr_slsc_job_jump slsc_job_jump = nullptr;
			PScanLabSMCPtr_slsc_job_begin_polyline slsc_job_begin_polyline = nullptr;
			PScanLabSMCPtr_slsc_slsc_job_end_polyline slsc_job_end_polyline = nullptr;
			PScanLabSMCPtr_slsc_job_line slsc_job_line = nullptr;
			PScanLabSMCPtr_slsc_ctrl_start_execution slsc_ctrl_start_execution = nullptr;
			PScanLabSMCPtr_slsc_ctrl_stop slsc_ctrl_stop = nullptr;
			PScanLabSMCPtr_slsc_ctrl_stop_controlled slsc_ctrl_stop_controlled = nullptr;
			PScanLabSMCPtr_slsc_ctrl_get_exec_state slsc_ctrl_get_exec_state = nullptr;
			PScanLabSMCPtr_slsc_job_set_jump_speed slsc_job_set_jump_speed = nullptr;
			PScanLabSMCPtr_slsc_job_set_mark_speed slsc_job_set_mark_speed = nullptr;
			PScanLabSMCPtr_slsc_job_set_min_mark_speed slsc_job_set_min_mark_speed = nullptr;
			PScanLabSMCPtr_slsc_job_jump_min_time slsc_job_jump_min_time = nullptr;
			PScanLabSMCPtr_slsc_job_set_corner_tolerance slsc_job_set_corner_tolerance = nullptr;		
			PScanLabSMCPtr_slsc_ctrl_get_error slsc_ctrl_get_error = nullptr;
			PScanLabSMCPtr_slsc_ctrl_get_error_count slsc_ctrl_get_error_count = nullptr;
			PScanLabSMCPtr_slsc_ctrl_get_simulation_filename slsc_ctrl_get_simulation_filename = nullptr;
			PScanLabSMCPtr_slsc_ctrl_get_job_characteristic slsc_ctrl_get_job_characteristic = nullptr;

			CScanLabSMCSDK(const std::string & sDLLNameUTF8, const std::string& sDLLDirectoryUTF8);
			~CScanLabSMCSDK();

			void initDLL();
			void checkError(size_t hHandle, uint32_t nSMCError);

		};


		typedef std::shared_ptr<CScanLabSMCSDK> PScanLabSMCSDK;

	} 
} 

#endif // __LIBMCDRIVER_SCANLABSMC_SDK
