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


#ifndef __LIBMCDRIVER_CIFX_SDK
#define __LIBMCDRIVER_CIFX_SDK

#include <memory>
#include <string>

#ifdef _WIN32

#define CIFX_CALLINGCONVENTION __stdcall

#else

#define CIFX_CALLINGCONVENTION 

#endif

#define CIFX_MAXDATASIZE 1556

#define CIFX_HOST_STATE_NOT_READY 0
#define CIFX_HOST_STATE_READY 1
#define CIFX_HOST_STATE_READ 2

#define CIFX_BUS_STATE_OFF 0
#define CIFX_BUS_STATE_ON 1
#define CIFX_BUS_STATE_GETSTATE 2

namespace LibMCDriver_CifX {
	namespace Impl {

#pragma	pack(push, 1)

		typedef struct _cifxPacketHeader {
			uint32_t   m_nDest;
			uint32_t   m_nSrc;
			uint32_t   m_nDestId;
			uint32_t   m_nSrcId;
			uint32_t   m_nLen;
			uint32_t   m_nId;
			uint32_t   m_nState;
			uint32_t   m_nCmd;
			uint32_t   m_nExt; 
			uint32_t   m_nRout;  
		} cifxPacketHeader;


		typedef struct _cifxPacket {
			cifxPacketHeader m_Header;
			uint8_t m_Data[CIFX_MAXDATASIZE];

		} cifxPacket;


#pragma	pack(pop)

		typedef int32_t cifxError;
		typedef void * cifxHandle;

		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverOpen) (cifxHandle * phDriver);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverClose) (cifxHandle hDriver);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverGetInformation) (cifxHandle hDriver, uint32_t nSize, void * pDriverInfo);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverGetErrorDescription) (cifxError nError, char* pszBuffer, uint32_t nBufferLen);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverEnumBoards) (cifxHandle hDriver, uint32_t nBoard, uint32_t nSize, void * pBoardInfo);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverEnumChannels) (cifxHandle hDriver, uint32_t nBoard, uint32_t nChannel, uint32_t nSize, void * pChannelInfo);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverMemoryPointer) (cifxHandle hDriver, uint32_t nBoard, uint32_t nCommandId, void * pMemoryInfo);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxDriverRestartDevice) (cifxHandle hDriver, const char * pszBoardName, void * pData);
		
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelOpen) (cifxHandle hDriver, const char* pszBoardName, uint32_t nChannel, cifxHandle * phChannel);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelClose) (cifxHandle hChannel);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelHostState) (cifxHandle hChannel, uint32_t nCommand, uint32_t * pnState, uint32_t nTimeout);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelBusState) (cifxHandle hChannel, uint32_t nCommand, uint32_t* pnState, uint32_t nTimeout);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelPutPacket) (cifxHandle hChannel, cifxPacket * pSendPkt, uint32_t nTimeout);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelGetPacket) (cifxHandle hChannel, uint32_t nSize, cifxPacket* pRecvPkt, uint32_t nTimeout);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelIORead) (cifxHandle hChannel, uint32_t nAreaNumber, uint32_t nOffset, uint32_t nDataLen, void* pData, uint32_t nTimeout);
		typedef cifxError(CIFX_CALLINGCONVENTION* PxChannelIOWrite) (cifxHandle hChannel, uint32_t nAreaNumber, uint32_t nOffset, uint32_t nDataLen, void* pData, uint32_t nTimeout);


		class CCifXSDK {
		private:
			void* m_LibraryHandle;
			void resetFunctionPtrs ();
		public:
			
			PxDriverOpen xDriverOpen = nullptr;
			PxDriverClose xDriverClose = nullptr;
			PxDriverGetInformation xDriverGetInformation = nullptr;
			PxDriverGetErrorDescription xDriverGetErrorDescription = nullptr;
			PxDriverEnumBoards xDriverEnumBoards = nullptr;
			PxDriverEnumChannels xDriverEnumChannels = nullptr;
			PxDriverMemoryPointer xDriverMemoryPointer = nullptr;
			PxDriverRestartDevice xDriverRestartDevice = nullptr;

			PxChannelOpen xChannelOpen = nullptr;
			PxChannelClose xChannelClose = nullptr;
			PxChannelHostState xChannelHostState = nullptr;
			PxChannelBusState xChannelBusState = nullptr;
			PxChannelPutPacket xChannelPutPacket = nullptr;
			PxChannelGetPacket xChannelGetPacket = nullptr;
			PxChannelIORead xChannelIORead = nullptr;
			PxChannelIOWrite xChannelIOWrite = nullptr;

			CCifXSDK(const std::string & sDLLNameUTF8);
			~CCifXSDK();

			void checkError(int32_t statusCode);

		};


		typedef std::shared_ptr<CCifXSDK> PCifXSDK;

	} 
} 

#endif // __LIBMCDRIVER_CIFX_SDK
