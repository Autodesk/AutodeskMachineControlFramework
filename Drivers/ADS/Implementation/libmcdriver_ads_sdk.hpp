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


#ifndef __LIBMCDRIVER_ADS_SDK
#define __LIBMCDRIVER_ADS_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define ADS_CALLINGCONVENTION __stdcall

#else

#define ADS_CALLINGCONVENTION 

#endif

#define ADSIGRP_SYM_HNDBYNAME 0xF003
#define ADSIGRP_SYM_VALBYNAME 0xF004
#define ADSIGRP_SYM_VALBYHND 0xF005

#define	AMSPORT_R0_PLC_TC3 851

#define ADS_MAXNAMELENGTH 4096


namespace LibMCDriver_ADS {
	namespace Impl {

#pragma	pack(push, 1)
		typedef struct _sAmsNetworkID
		{
			uint8_t m_Data[6];

		} sAmsNetworkID;

		typedef struct _sAmsAddr
		{
			sAmsNetworkID m_NetworkID;
			uint16_t m_Port;
		} sAmsAddr;

		typedef	struct
		{
			uint8_t m_Version;
			uint8_t m_Revision;
			uint16_t m_Build;
		} AdsVersion;
#pragma	pack(pop)

		typedef int32_t AdsInt32;
		typedef uint32_t AdsUint32;
		typedef int32_t AdsError;
		typedef int32_t AdsPort;

		typedef AdsUint32(ADS_CALLINGCONVENTION* PAdsGetDllVersion) ();
		typedef AdsPort(ADS_CALLINGCONVENTION* PAdsPortOpenEx) ();
		typedef AdsError(ADS_CALLINGCONVENTION* PAdsPortCloseEx) (AdsPort nPort);
		typedef AdsError(ADS_CALLINGCONVENTION* PAdsGetLocalAddressEx) (AdsPort nPort, sAmsAddr* pAddr);
		typedef AdsError(ADS_CALLINGCONVENTION* PAdsSyncWriteReqEx) (AdsPort nPort, sAmsAddr* pAddr, AdsUint32 indexGroup, AdsUint32 indexOffset, AdsUint32 length, void* pData);
		typedef AdsError(ADS_CALLINGCONVENTION* PAdsSyncReadReqEx2) (AdsPort nPort, sAmsAddr* pAddr, AdsUint32 indexGroup, AdsUint32 indexOffset, AdsUint32 length, void* pData, AdsUint32 * pBytesRead);
		typedef AdsError(ADS_CALLINGCONVENTION* PAdsSyncReadWriteReqEx2) (AdsPort nPort, sAmsAddr* pAddr, AdsUint32 indexGroup, AdsUint32 indexOffset, AdsUint32 cbReadLength, void* pReadData, AdsUint32 cbWriteLength, const void * pWriteData, AdsUint32* pBytesRead);
		


		class CADSSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:

			PAdsGetDllVersion AdsGetDllVersion = nullptr;
			PAdsPortOpenEx AdsPortOpenEx = nullptr;
			PAdsPortCloseEx AdsPortCloseEx = nullptr;
			PAdsGetLocalAddressEx AdsGetLocalAddressEx = nullptr;
			PAdsSyncWriteReqEx AdsSyncWriteReqEx = nullptr;
			PAdsSyncReadReqEx2 AdsSyncReadReqEx2 = nullptr;
			PAdsSyncReadWriteReqEx2 AdsSyncReadWriteReqEx2 = nullptr;

			CADSSDK(const std::string & sDLLNameUTF8);
			~CADSSDK();

			void checkError(int32_t statusCode);

		};


		typedef std::shared_ptr<CADSSDK> PADSSDK;

	} 
} 

#endif // __LIBMCDRIVER_ADS_SDK
