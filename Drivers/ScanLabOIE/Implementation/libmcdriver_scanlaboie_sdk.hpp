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


#ifndef __LIBMCDRIVER_SCANLABOIE_SDK
#define __LIBMCDRIVER_SCANLABOIE_SDK

#include "libmcdriver_scanlaboie_types.hpp"

#include <memory>
#include <string>

#ifdef _WIN32

#define SCANLABOIE_CALLINGCONVENTION __stdcall

#else

#define SCANLABOIE_CALLINGCONVENTION 

#endif


namespace LibMCDriver_ScanLabOIE {
	namespace Impl {

		typedef void* oie_instance;
		typedef void* oie_device;
		typedef uint32_t oie_error;


#pragma pack(push, 1)
		typedef struct _oie_appinfo {
			char m_Name[128]; 
			uint32_t m_nMajorVersion;
			uint32_t m_nMinorVersion;
			uint32_t m_nPatchVersion;
		} oie_appinfo;

		typedef struct _oie_pkt {
			uint32_t pktNr; 
			uint32_t id; 
			uint8_t flags; 
			const void* data;
		} oie_pkt;
#pragma pack(pop)

		typedef void (*oie_pkt_listener)(oie_device device, const oie_pkt* pkt, void* userData);
		typedef void (*oie_err_listener)(oie_device device, oie_error error, int32_t value, void* userData);

		typedef void(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_get_version) (uint32_t* majorVersion, uint32_t* minorVersion, uint32_t* patchVersion);
		typedef oie_instance(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_create) ();
		typedef void(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_destroy) (oie_instance pInstance);
		typedef char *(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_get_error) (oie_error nError, char * pBuffer, int32_t nLen);
		typedef oie_device(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_add_device) (oie_instance pInstance);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_remove_device) (oie_instance pInstance, oie_device pDevice);
		typedef uint32_t(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_get_device_id) (oie_device pDevice);
		typedef void(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_set_reply_timeout) (oie_device pDevice, int32_t nTimeOutInMs);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_connect) (oie_device pDevice, const char * pHostName, uint16_t nPort);
		typedef int(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_is_connected) (oie_device pDevice);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_disconnect) (oie_device pDevice);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_login) (oie_device pDevice, const char * pUserName, const char * pPassword);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_list_apps) (oie_device pDevice, oie_appinfo * pAppBuffer, int32_t nLen, int32_t * pCount);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_start_app) (oie_device pDevice, const char* pAppName, const char* pDeviceConfigFileName);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_start_app_ver) (oie_device pDevice, const char* pAppName, int32_t nMajorVersion, int32_t nMinorVersion, const char* pDeviceConfigFileName);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_stop_app) (oie_device pDevice);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_is_app_running) (oie_device pDevice, uint32_t* pState);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_get_appinfo) (oie_device pDevice, oie_appinfo* pInfo);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_install_app) (oie_device pDevice, const char * pEfwFileName);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_uninstall_app) (oie_device pDevice, const char* pAppName);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_uninstall_app_ver) (oie_device pDevice, const char* pAppName, int32_t nMajorVersion, int32_t nMinorVersion);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_unlock) (oie_device pDevice, uint32_t nFeature, uint64_t nCode);
		typedef int(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_is_unlocked) (oie_device pDevice, uint32_t nFeature);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_get_log) (oie_device pDevice, char * buffer, uint32_t nLength);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_device_get_board_version) (oie_device pDevice, uint32_t * pVersion);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_get_rtc_type) (const char * pDeviceConfigFileName, int32_t * pRTCType);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_get_rtc_signals) (const char* pDeviceConfigFileName, uint32_t* pSignals, uint32_t * pNumberOfSignals);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_get_sensor_signals) (const char* pDeviceConfigFileName, uint32_t* pSignals, uint32_t* pNumberOfSignals);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_set_packet_listener) (oie_device pDevice, oie_pkt_listener pListener, void * pUserData);
		typedef oie_error(SCANLABOIE_CALLINGCONVENTION* PScanLabOIEPtr_oie_set_runtime_error_listener) (oie_device pDevice, oie_err_listener pListener, void* pUserData);

		class CScanLabOIESDK_DLLDirectoryCache {
		private:
			std::wstring m_sCachedDLLDirectoryW;

		public:
			CScanLabOIESDK_DLLDirectoryCache();
			virtual ~CScanLabOIESDK_DLLDirectoryCache();

		};

		typedef std::shared_ptr<CScanLabOIESDK_DLLDirectoryCache> PScanLabOIESDK_DLLDirectoryCache;


		class CScanLabOIESDK {
		private:
			std::wstring m_sDLLDirectoryW;

			bool m_bIsInitialized;

			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			PScanLabOIEPtr_oie_get_version oie_get_version = nullptr;
			PScanLabOIEPtr_oie_create oie_create = nullptr;
			PScanLabOIEPtr_oie_destroy oie_destroy = nullptr;
			PScanLabOIEPtr_oie_get_error oie_get_error = nullptr;
			PScanLabOIEPtr_oie_add_device oie_add_device = nullptr;
			PScanLabOIEPtr_oie_remove_device oie_remove_device = nullptr;
			PScanLabOIEPtr_oie_get_device_id oie_get_device_id = nullptr;
			PScanLabOIEPtr_oie_set_reply_timeout oie_set_reply_timeout = nullptr;
			PScanLabOIEPtr_oie_connect oie_connect = nullptr;
			PScanLabOIEPtr_oie_is_connected oie_is_connected = nullptr;
			PScanLabOIEPtr_oie_disconnect oie_disconnect = nullptr;
			PScanLabOIEPtr_oie_device_login oie_device_login = nullptr;
			PScanLabOIEPtr_oie_device_list_apps oie_device_list_apps = nullptr;
			PScanLabOIEPtr_oie_device_start_app oie_device_start_app = nullptr;
			PScanLabOIEPtr_oie_device_start_app_ver oie_device_start_app_ver = nullptr;
			PScanLabOIEPtr_oie_device_stop_app oie_device_stop_app = nullptr;
			PScanLabOIEPtr_oie_device_is_app_running oie_device_is_app_running = nullptr;
			PScanLabOIEPtr_oie_device_get_appinfo oie_device_get_appinfo = nullptr;
			PScanLabOIEPtr_oie_device_install_app oie_device_install_app = nullptr;
			PScanLabOIEPtr_oie_device_uninstall_app oie_device_uninstall_app = nullptr;
			PScanLabOIEPtr_oie_device_uninstall_app_ver oie_device_uninstall_app_ver = nullptr;
			PScanLabOIEPtr_oie_device_unlock oie_device_unlock = nullptr;
			PScanLabOIEPtr_oie_device_is_unlocked oie_device_is_unlocked = nullptr;
			PScanLabOIEPtr_oie_device_get_log oie_device_get_log = nullptr;
			PScanLabOIEPtr_oie_device_get_board_version oie_device_get_board_version = nullptr;
			PScanLabOIEPtr_oie_get_rtc_type oie_get_rtc_type = nullptr;
			PScanLabOIEPtr_oie_get_rtc_signals oie_get_rtc_signals = nullptr;
			PScanLabOIEPtr_oie_get_sensor_signals oie_get_sensor_signals = nullptr;
			PScanLabOIEPtr_oie_set_packet_listener oie_set_packet_listener = nullptr;
			PScanLabOIEPtr_oie_set_runtime_error_listener oie_set_runtime_error_listener = nullptr;

			CScanLabOIESDK(const std::string & sDLLNameUTF8, const std::string & sDLLDirectoryUTF8);
			~CScanLabOIESDK();

			void initDLL();
			void checkError(uint32_t nSDKError);

			PScanLabOIESDK_DLLDirectoryCache cacheDllDirectory ();

		};


		typedef std::shared_ptr<CScanLabOIESDK> PScanLabOIESDK;

	} 
} 

#endif // __LIBMCDRIVER_SCANLABOIE_SDK
