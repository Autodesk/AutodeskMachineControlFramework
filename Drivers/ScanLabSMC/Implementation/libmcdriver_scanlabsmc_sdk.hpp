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

		typedef struct _slsc_PolylineOptions slsc_PolylineOptions;

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

	
		class CScanLabSMCSDK {
		private:
			bool m_bIsInitialized;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

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

			CScanLabSMCSDK(const std::string & sDLLNameUTF8);
			~CScanLabSMCSDK();

			void initDLL();
			void checkError(uint32_t nSMCError);

		};


		typedef std::shared_ptr<CScanLabSMCSDK> PScanLabSMCSDK;

	} 
} 

#endif // __LIBMCDRIVER_SCANLABSMC_SDK