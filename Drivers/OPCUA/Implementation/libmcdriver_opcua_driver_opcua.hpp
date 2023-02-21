/*++

Copyright (C) 2020 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_OPCUA

*/


#ifndef __LIBMCDRIVER_OPCUA_DRIVER_OPCUA
#define __LIBMCDRIVER_OPCUA_DRIVER_OPCUA

#include "libmcdriver_opcua_interfaces.hpp"
#include "libmcdriver_open62541_client.hpp"

// Parent classes
#include "libmcdriver_opcua_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_OPCUA {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_OPCUA 
**************************************************************************************************************************/

class CDriver_OPCUA : public virtual IDriver_OPCUA, public virtual CDriver, public virtual COpen62541ClientLogger {
private:


protected:

	std::string m_sName;
	bool m_bSimulationMode;

	uint32_t m_nMajorVersion;
	uint32_t m_nMinorVersion;
	uint32_t m_nPatchVersion;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	LibMCEnv::PWorkingDirectory m_pWorkingDirectory;
	LibMCEnv::PWorkingFile m_pOpen62541DLL;

	POpen62541Client m_pClient;

public:

	CDriver_OPCUA(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);
	virtual ~CDriver_OPCUA();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_OPCUA_uint32& nMajor, LibMCDriver_OPCUA_uint32& nMinor, LibMCDriver_OPCUA_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void Connect(const std::string & sIPAddress, const LibMCDriver_OPCUA_uint32 nPort, const LibMCDriver_OPCUA_uint32 nTimeout) override;

	void Disconnect() override;

	IPLCCommandList * CreateCommandList() override;

	IPLCCommand * CreateCommand(const std::string & sCommandName) override;

	void StartJournaling() override;

	void StopJournaling() override;

	void RefreshJournal() override;

	virtual void onLog(const std::string& sMessage, opcUA_LogLevel level, opcUA_LogCategory category) override;

};

} // namespace Impl
} // namespace LibMCDriver_OPCUA

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_OPCUA_DRIVER_OPCUA
