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


Abstract: This is the class declaration of CDriver_BK9xxx

*/


#ifndef __LIBMCDRIVER_BK9XXX_DRIVER_BK9XXX
#define __LIBMCDRIVER_BK9XXX_DRIVER_BK9XXX

#include "libmcdriver_bk9xxx_interfaces.hpp"

// Parent classes
#include "libmcdriver_bk9xxx_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>
#include <set>

#include "pugixml.hpp"
#include <mutex>
#include "libmcdriver_bk9xxx_digitalio.hpp"
#include "libmcdriver_bk9xxx_analogio.hpp"



namespace LibMCDriver_BK9xxx {
	namespace Impl {


		/*************************************************************************************************************************
		 Class declaration of CDriver_BK9xxx
		**************************************************************************************************************************/

		class CDriver_BK9xxxThreadState {
		public:
			bool m_bDebugMode;

			std::mutex m_ModBusConnectionMutex;
			std::atomic<bool> m_ModBusConnectionThreadShallFinish;
			LibMCEnv::PModbusTCPConnection m_pModBusTCPConnection;
			std::vector<std::pair<uint32_t, std::string>> m_Exceptions;

			CDriver_BK9xxxThreadState (LibMCEnv::PModbusTCPConnection pModBusTCPConnection);
			virtual ~CDriver_BK9xxxThreadState();

			void disconnect();
			bool isConnected();

			void handleException(uint32_t nErrorCode, const std::string & sMessage);

		};

		class CDriver_BK9xxx : public virtual IDriver_BK9xxx, public virtual CDriver {
		protected:

			std::string m_sName;

			bool m_bIsInSimulationMode;
			std::string m_sIPAddress;
			uint32_t m_nPort;
			uint32_t m_nTimeOutInMs;

			uint32_t m_nMajorVersion;
			uint32_t m_nMinorVersion;
			uint32_t m_nPatchVersion;

			LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

			std::thread m_ModBusConnectionThread;
			std::shared_ptr<CDriver_BK9xxxThreadState> m_ModBusConnectionThreadState;
			
			

			std::set<std::string> m_ReservedNames;
			std::vector<PDriver_BK9xxx_DigitalInputsDefinition> m_pDigitalInputBlocks;
			std::vector <PDriver_BK9xxx_DigitalOutputsDefinition> m_pDigitalOutputBlocks;
			std::map<std::string, PDriver_BK9xxx_DigitalIODefinition> m_DigitalInputIOMap;
			std::map<std::string, PDriver_BK9xxx_DigitalIODefinition> m_DigitalOutputIOMap;
			std::vector<PDriver_BK9xxx_AnalogInputsDefinition> m_pAnalogInputBlocks;
			std::vector <PDriver_BK9xxx_AnalogOutputsDefinition> m_pAnalogOutputBlocks;
			std::map<std::string, PDriver_BK9xxx_AnalogIODefinition> m_AnalogInputIOMap;
			std::map<std::string, PDriver_BK9xxx_AnalogIODefinition> m_AnalogOutputIOMap;


		public:

			CDriver_BK9xxx(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

			virtual ~CDriver_BK9xxx();

			void Configure(const std::string& sConfigurationString) override;

			std::string GetName() override;

			void GetVersion(LibMCDriver_BK9xxx_uint32& nMajor, LibMCDriver_BK9xxx_uint32& nMinor, LibMCDriver_BK9xxx_uint32& nMicro, std::string& sBuild) override;

			void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

			void QueryParameters() override;

			void SetToSimulationMode() override;

			bool IsSimulationMode() override;

			void Connect(const std::string& sIPAddress, const LibMCDriver_BK9xxx_uint32 nPort, const LibMCDriver_BK9xxx_uint32 nTimeOutInMs) override;

			std::string GetIPAddress() override;

			LibMCDriver_BK9xxx_uint32 GetPort() override;

			LibMCDriver_BK9xxx_uint32 GetTimeOut() override;

			void SetErrorRecoveryMode(const LibMCDriver_BK9xxx_uint32 nReconnectionDelayInMs, const LibMCDriver_BK9xxx_uint32 nReconnectionCount) override;

			void GetErrorRecoveryMode(LibMCDriver_BK9xxx_uint32& nReconnectionDelayInMs, const LibMCDriver_BK9xxx_uint32 nReconnectionCount) override;

			void Disconnect() override;

			void ResetDevice() override;

			void Reconnect() override;

			bool IsConnected() override;

			LibMCDriver_BK9xxx::eErrorState GetErrorState() override;

			bool VariableExists(const std::string& sName) override;

			LibMCDriver_BK9xxx::eVariableType GetVariableType(const std::string& sName) override;

			bool DigitalInputExists(const std::string& sName) override;

			bool DigitalOutputExists(const std::string& sName) override;

			bool AnalogInputExists(const std::string& sName) override;

			bool AnalogOutputExists(const std::string& sName) override;

			bool GetDigitalInput(const std::string& sVariableName) override;

			bool GetDigitalOutput(const std::string& sVariableName) override;

			LibMCDriver_BK9xxx_uint32 GetAnalogInputRaw(const std::string& sVariableName) override;

			LibMCDriver_BK9xxx_uint32 GetAnalogOutputRaw(const std::string& sVariableName) override;

			LibMCDriver_BK9xxx_double GetAnalogInput(const std::string& sVariableName) override;

			LibMCDriver_BK9xxx_double GetAnalogOutput(const std::string& sVariableName) override;

			void SetDigitalOutput(const std::string& sVariableName, const bool bValue, const LibMCDriver_BK9xxx_uint32 nTimeOutInMs) override;

			void SetAnalogOutputRaw(const std::string& sVariableName, const LibMCDriver_BK9xxx_uint32 nValue, const LibMCDriver_BK9xxx_uint32 nTimeOutInMs) override;

			void SetAnalogOutput(const std::string& sVariableName, const LibMCDriver_BK9xxx_double dValue, const LibMCDriver_BK9xxx_uint32 nTimeOutInMs) override;

		};


		class CDriver_BK9000 : public virtual CDriver_BK9xxx {
		public:

			CDriver_BK9000(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

			virtual ~CDriver_BK9000();


			std::string GetType() override;

		};

		class CDriver_BK9050 : public virtual CDriver_BK9xxx {
		public:

			CDriver_BK9050(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

			virtual ~CDriver_BK9050();


			std::string GetType() override;

		};


		class CDriver_BK9100 : public virtual CDriver_BK9xxx {
		public:

			CDriver_BK9100(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

			virtual ~CDriver_BK9100();

			std::string GetType() override;

		};

	} // namespace Impl
} // namespace LibMCDriver_BK9xxx

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // __LIBMCDRIVER_BK9XXX_DRIVER_BK9XXX
