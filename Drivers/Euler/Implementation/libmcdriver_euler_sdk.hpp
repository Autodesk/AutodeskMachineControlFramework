/*++

Copyright (C) 2025 Autodesk Inc.

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


#ifndef __LIBMCDRIVER_EULER_SDK
#define __LIBMCDRIVER_EULER_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define EULER_CALLINGCONVENTION __stdcall

#else

#define EULER_CALLINGCONVENTION 

#endif

#define EULERSDK_IMAGETYPE_RECOAT 0
#define EULERSDK_IMAGETYPE_EXPOSURE 1

#define EULERSDK_JOBSTATUS_INPROGRESS 0
#define EULERSDK_JOBSTATUS_COMPLETED 1

namespace LibMCDriver_Euler {
	namespace Impl {

#pragma	pack(push, 1)
		typedef struct _sLibEulerConnectConfig
		{
		  const char *m_pszBaseURL;
		  const char *m_pszAPIKey;
		  const char *m_pszDeviceID;
		  const char *m_pszApplicationName;
		  const char *m_pszApplicationVersion;
		  uint32_t m_bEnabledStdOutLogging;
		} sLibEulerConnectConfig;

		typedef struct _sLibEulerConnectLicenseInfo
		{
		  const char *m_pszBaseURL;
		  const char *m_pszAPIKey;
		  const char *m_pszErrorMessage;
		} sLibEulerConnectLicenseInfo;


#pragma	pack(pop)

		typedef void * libEulerHandle;

		typedef libEulerHandle(EULER_CALLINGCONVENTION* Peuler_connect_new) (const sLibEulerConnectConfig *config);
		typedef bool(EULER_CALLINGCONVENTION* Peuler_connect_start_heartbeat) (libEulerHandle pHandle);
		typedef bool(EULER_CALLINGCONVENTION* Peuler_connect_stop_heartbeat) (libEulerHandle pHandle);
		
		typedef char *(EULER_CALLINGCONVENTION* Peuler_connect_new_build_job) (libEulerHandle pHandle, const char * pszJobName, uint32_t nTotalLayers);
		typedef bool(EULER_CALLINGCONVENTION* Peuler_connect_upload_layer_image) (libEulerHandle pHandle, const char * pszJobID, uint32_t nLayerNumber, uint32_t eImageType, const uint8_t * pData, uintptr_t nDataLength);
		typedef bool(EULER_CALLINGCONVENTION* Peuler_connect_set_job_status) (libEulerHandle pHandle, const char * pszJobID, uint32_t eJobStatus);

		typedef void(EULER_CALLINGCONVENTION* Peuler_connect_free_string) (char * pszString);
		typedef void(EULER_CALLINGCONVENTION* Peuler_connect_destroy) (libEulerHandle pHandle);
		typedef sLibEulerConnectLicenseInfo* (EULER_CALLINGCONVENTION* Peuler_connect_read_license) (const char *pszLicensePath);
		typedef void(EULER_CALLINGCONVENTION* Peuler_connect_free_license_info) (sLibEulerConnectLicenseInfo * pLicenseInfo);



		class CLibEulerSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			Peuler_connect_new euler_connect_new = nullptr;
			Peuler_connect_start_heartbeat euler_connect_start_heartbeat = nullptr;
			Peuler_connect_stop_heartbeat euler_connect_stop_heartbeat = nullptr;
			Peuler_connect_new_build_job euler_connect_new_build_job = nullptr;
			Peuler_connect_upload_layer_image euler_connect_upload_layer_image = nullptr;
			Peuler_connect_set_job_status euler_connect_set_job_status = nullptr;
			Peuler_connect_free_string euler_connect_free_string = nullptr;
			Peuler_connect_destroy euler_connect_destroy = nullptr;
			Peuler_connect_read_license euler_connect_read_license = nullptr;
			Peuler_connect_free_license_info euler_connect_free_license_info = nullptr;

			CLibEulerSDK(const std::string & sDLLNameUTF8);
			~CLibEulerSDK();

			void checkError(int32_t statusCode);

		};


		typedef std::shared_ptr<CLibEulerSDK> PLibEulerSDK;

	} 
} 

#endif // __LIBMCDRIVER_EULER_SDK
