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


Abstract: This is the class declaration of CDriver_CifX

*/


#ifndef __LIBMCDRIVER_CIFX_DRIVER_CIFX
#define __LIBMCDRIVER_CIFX_DRIVER_CIFX

#include "libmcdriver_cifx_interfaces.hpp"

// Parent classes
#include "libmcdriver_cifx_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_cifx_sdk.hpp"
#include <map>
#include <pugixml.hpp>
#include <mutex>
#include <thread>

namespace LibMCDriver_CifX {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_CifX 
**************************************************************************************************************************/

	enum class eDriver_CifXParameterType : int32_t {
		CifXParameter_Unknown = 0,
		CifXParameter_BOOL = 1,
		CifXParameter_UINT8 = 2,
		CifXParameter_UINT16 = 3,
		CifXParameter_UINT32 = 4,
		CifXParameter_INT8 = 5,
		CifXParameter_INT16 = 6,
		CifXParameter_INT32 = 7,
		CifXParameter_FLOAT = 8,
		CifXParameter_DOUBLE = 9,
	};

	enum class eDriver_AbstractParameterType : int32_t {
		CifXAbstractParameter_Unknown = 0,
		CifXAbstractParameter_BOOL = 1,
		CifXAbstractParameter_INT = 2,
		CifXAbstractParameter_DOUBLE = 3
	};

	class CDriver_CifXParameter {
	private:
		std::string m_sName;
		std::string m_sDescription;
		eDriver_CifXParameterType m_eType;
		uint32_t m_nAddress;
	public:
		CDriver_CifXParameter(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress);
		virtual ~CDriver_CifXParameter();

		std::string getName();
		std::string getDescription();

		virtual double GetActualDoubleValue();
		virtual int64_t GetActualIntegerValue();
		bool GetActualBoolValue();
		virtual void SetActualDoubleValue(double dValue);
		virtual void SetActualIntegerValue(int64_t nValue);
		void SetActualBoolValue(bool bValue);
		virtual double GetTargetDoubleValue();
		virtual int64_t GetTargetIntegerValue();
		bool GetTargetBoolValue();
		virtual void SetTargetDoubleValue(double dValue);
		virtual void SetTargetIntegerValue(int64_t nValue);
		void SetTargetBoolValue(bool bValue);

		eDriver_CifXParameterType getType();
		eDriver_AbstractParameterType getAbstractType();

	};

	typedef std::shared_ptr<CDriver_CifXParameter> PDriver_CifXParameter;


	class CDriver_CifXParameter_Integer : public CDriver_CifXParameter {
	private:
		int64_t m_nActualValue;
		int64_t m_nTargetValue;

	public:
		CDriver_CifXParameter_Integer(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress);

		virtual ~CDriver_CifXParameter_Integer();

		virtual int64_t GetActualIntegerValue() override;

		virtual void SetActualIntegerValue(int64_t nValue) override;

		virtual int64_t GetTargetIntegerValue() override;

		virtual void SetTargetIntegerValue(int64_t nValue) override;
	};


	class CDriver_CifXParameter_Double : public CDriver_CifXParameter {
	private:
		double m_dActualValue;
		double m_dTargetValue;

	public:
		CDriver_CifXParameter_Double(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress);

		virtual ~CDriver_CifXParameter_Double();

		virtual double GetActualDoubleValue() override;

		virtual void SetActualDoubleValue(double dValue) override;

		virtual double GetTargetDoubleValue() override;

		virtual void SetTargetDoubleValue(double dValue) override;
	};


	class CDriver_CifXParameter_Bool : public CDriver_CifXParameter_Integer {
	private:
		uint32_t m_nBit;

	public:
		CDriver_CifXParameter_Bool(const std::string& sName, const std::string& sDescription, eDriver_CifXParameterType Type, const uint32_t nAddress, const uint32_t nBit);

		virtual ~CDriver_CifXParameter_Bool();

	};


	class CDriver_CifXChannelThreadState {
		private:
			std::vector<uint8_t> m_InputData;
			std::vector<uint8_t> m_OutputData;
			PCifXSDK m_pCifXSDK;
			cifxHandle m_hChannel;

			std::mutex m_Mutex;

			std::atomic<bool> m_CancelFlag;

			bool m_bDebugMode;
			std::vector<std::pair<uint32_t, std::string>> m_Exceptions;


		public:
			
			CDriver_CifXChannelThreadState(PCifXSDK pCifXSDK, uint32_t nInputSize, uint32_t nOutputSize, cifxHandle hChannel);

			virtual ~CDriver_CifXChannelThreadState();

			void executeThread(uint32_t nReadTimeOut, uint32_t nWriteTimeOut);

			void handleException(uint32_t nErrorCode, const std::string& sMessage);

			void stopThread(uint32_t nHostStateTimeOut, uint32_t nBusStateTimeOut);

			bool threadShallBeCanceled();

	};


	class CDriver_CifXChannel {
	private:
		std::string m_sBoardName;
		uint32_t m_nChannelIndex;
		uint32_t m_nInputSize;
		uint32_t m_nOutputSize;

		uint32_t m_nHostStateTimeOut;
		uint32_t m_nBusStateTimeOut;

		std::shared_ptr<CDriver_CifXChannelThreadState> m_pThreadState;

		std::thread m_SyncThread;

		std::vector<PDriver_CifXParameter> m_Inputs;
		std::map<std::string, PDriver_CifXParameter> m_InputMap;
		std::vector<PDriver_CifXParameter> m_Outputs;
		std::map<std::string, PDriver_CifXParameter> m_OutputMap;

	public:

		CDriver_CifXChannel(pugi::xml_node& channelNode);
		virtual ~CDriver_CifXChannel ();

		std::string getBoardName(); 
		uint32_t getChannelIndex();

		void RegisterVariables(LibMCEnv::PDriverEnvironment pDriverEnvironment);

		std::vector<PDriver_CifXParameter> getInputs ();
		std::vector<PDriver_CifXParameter> getOutputs();

		void stopSyncThread();
		void startSyncThread(PCifXSDK pCifXSDK, cifxHandle hDriverHandle);
	};

	typedef std::shared_ptr<CDriver_CifXChannel> PDriver_CifXChannel;


class CDriver_CifX : public virtual IDriver_CifX, public virtual CDriver {
private:
	std::string m_sName;

	bool m_bIsSimulationMode;

	std::mutex m_driverEnvironmentMutex;
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pCifXDLLFile;

	std::string m_sCustomSDKResource;

	PCifXSDK m_pCifXSDK;

	cifxHandle m_hDriverHandle;

	std::vector<PDriver_CifXChannel> m_Channels;
	std::map<std::string, PDriver_CifXParameter> m_GlobalInputMap;
	std::map<std::string, PDriver_CifXParameter> m_GlobalOutputMap;

	uint32_t m_nMajorVersion;
	uint32_t m_nMinorVersion;
	uint32_t m_nPatchVersion;

	void LoadSDKIfNeeded();

public:

	CDriver_CifX(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_CifX();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_CifX_uint32& nMajor, LibMCDriver_CifX_uint32& nMinor, LibMCDriver_CifX_uint32& nMicro, std::string& sBuild) override;

	void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

	void QueryParameters() override;

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void SetCustomSDKResource(const std::string & sResourceName) override;

	LibMCDriver_CifX_uint32 EnumerateBoards() override;

	IBoardInformation * GetBoardInformation(const LibMCDriver_CifX_uint32 nBoardIndex) override;

	void Connect() override;

	void Disconnect() override;

	bool IsConnected() override;

	bool ValueExists(const std::string& sName) override;

	void WriteIntegerValue(const std::string& sName, const LibMCDriver_CifX_int64 nValue, const LibMCDriver_CifX_uint32 nTimeOutInMs) override;

	void WriteBoolValue(const std::string& sName, const bool bValue, const LibMCDriver_CifX_uint32 nTimeOutInMs) override;

	void WriteDoubleValue(const std::string& sName, const LibMCDriver_CifX_double dValue, const LibMCDriver_CifX_uint32 nTimeOutInMs) override;

	LibMCDriver_CifX_int64 ReadIntegerValue(const std::string& sName) override;

	bool ReadBoolValue(const std::string& sName) override;

	LibMCDriver_CifX_double ReadDoubleValue(const std::string& sName) override;

};

} // namespace Impl
} // namespace LibMCDriver_CifX

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_CIFX_DRIVER_CIFX
