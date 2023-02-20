/*++

Copyright (C) 2023 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
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


#ifndef __LIBMCDRIVER_CIFX_CHANNEL
#define __LIBMCDRIVER_CIFX_CHANNEL

#include "libmcdriver_cifx_parameter.hpp"
#include "libmcdriver_cifx_interfaces.hpp"
#include "libmcdriver_cifx_sdk.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <vector>

#include "pugixml.hpp"


namespace LibMCDriver_CifX {
namespace Impl {



	class CDriver_CifXChannelBuffer {
	private:
		std::vector<uint8_t> m_Data;

	public:
		CDriver_CifXChannelBuffer(uint32_t nSize);
		~CDriver_CifXChannelBuffer();

		void clear();

		uint8_t readUint8(uint32_t nAddress);
		uint16_t readUint16(uint32_t nAddress);
		uint32_t readUint32(uint32_t nAddress);
		int8_t readInt8(uint32_t nAddress);
		int16_t readInt16(uint32_t nAddress);
		int32_t readInt32(uint32_t nAddress);
		bool readBool(uint32_t nAddress, uint32_t nBit);
		float readFloat(uint32_t nAddress);
		double readDouble(uint32_t nAddress);

		void writeUint8(uint32_t nAddress, uint8_t nValue);
		void writeUint16(uint32_t nAddress, uint16_t nValue);
		void writeUint32(uint32_t nAddress, uint32_t nValue);
		void writeInt8(uint32_t nAddress, int8_t nValue);
		void writeInt16(uint32_t nAddress, int16_t nValue);
		void writeInt32(uint32_t nAddress, int32_t nValue);
		void writeBool(uint32_t nAddress, uint32_t nBit, bool bValue);
		void writeFloat(uint32_t nAddress, float fValue);
		void writeDouble(uint32_t nAddress, double dValue);

		std::vector<uint8_t>& getBuffer();
	};


	class CDriver_CifXChannelThreadState {
	private:
		CDriver_CifXChannelBuffer m_InputBuffer;
		CDriver_CifXChannelBuffer m_OutputBuffer;

		PCifXSDK m_pCifXSDK;
		cifxHandle m_hChannel;

		std::mutex m_Mutex;

		std::atomic<bool> m_bCancelFlag;
		std::atomic<bool> m_bThreadIsRunning;

		bool m_bDebugMode;
		std::vector<std::pair<uint32_t, std::string>> m_Exceptions;


	public:

		CDriver_CifXChannelThreadState(PCifXSDK pCifXSDK, uint32_t nInputSize, uint32_t nOutputSize, cifxHandle hChannel);

		virtual ~CDriver_CifXChannelThreadState();

		void executeThread(uint32_t nReadTimeOut, uint32_t nWriteTimeOut);

		void handleException(uint32_t nErrorCode, const std::string& sMessage);

		void stopThread(uint32_t nHostStateTimeOut, uint32_t nBusStateTimeOut);

		bool threadShallBeCanceled();
		void setThreadIsRunning(bool bThreadIsRunning);

		void readInputParameter(CDriver_CifXParameter* pParameter);
		void readOutputParameter(CDriver_CifXParameter* pParameter);
		void writeOutputParameter(CDriver_CifXParameter* pParameter);

		bool isConnected();

	};


	class CDriver_CifXChannel {
	private:
		std::string m_sBoardName;
		uint32_t m_nChannelIndex;
		uint32_t m_nInputSize;
		uint32_t m_nOutputSize;

		uint32_t m_nHostStateTimeOut;
		uint32_t m_nBusStateTimeOut;
		uint32_t m_SyncDelay;

		std::shared_ptr<CDriver_CifXChannelThreadState> m_pThreadState;

		std::thread m_SyncThread;

		std::vector<PDriver_CifXParameter> m_Inputs;
		std::map<std::string, PDriver_CifXParameter> m_InputMap;
		std::vector<PDriver_CifXParameter> m_Outputs;
		std::map<std::string, PDriver_CifXParameter> m_OutputMap;

	public:

		CDriver_CifXChannel(pugi::xml_node& channelNode);
		virtual ~CDriver_CifXChannel();

		std::string getBoardName();
		uint32_t getChannelIndex();

		void RegisterVariables(LibMCEnv::PDriverEnvironment pDriverEnvironment);

		uint32_t getInputCount();
		uint32_t getOutputCount();
		PDriver_CifXParameter getInputByIndex(uint32_t nIndex);
		PDriver_CifXParameter getOutputByIndex(uint32_t nIndex);

		void stopSyncThread();
		void startSyncThread(PCifXSDK pCifXSDK, cifxHandle hDriverHandle);

		bool isConnected();

		PDriver_CifXParameter findInputValue(const std::string& sName);
		PDriver_CifXParameter findOutputValue(const std::string& sName);

	};

	typedef std::shared_ptr<CDriver_CifXChannel> PDriver_CifXChannel;



} // namespace Impl
} // namespace LibMCDriver_CifX

#endif // __LIBMCDRIVER_CIFX_CHANNEL


