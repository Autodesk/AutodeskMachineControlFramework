/*++

Copyright (C) 2022 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_TCPIP

*/


#ifndef __LIBMCDRIVER_TCPIP_DRIVER_TCPIP
#define __LIBMCDRIVER_TCPIP_DRIVER_TCPIP

#include "libmcdriver_tcpip_interfaces.hpp"
#include "libmcdriver_tcpip_sockets.hpp"

// Parent classes
#include "libmcdriver_tcpip_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_TCPIP {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_TCPIP 
**************************************************************************************************************************/

class CDriver_TCPIP : public virtual IDriver_TCPIP, public virtual CDriver {
private:

	std::string m_sName;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	bool m_bSimulationMode;

	std::shared_ptr<CDriver_TCPIPSocketConnection> m_pSocketConnection;

protected:

public:

	CDriver_TCPIP(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_TCPIP();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_TCPIP_uint32& nMajor, LibMCDriver_TCPIP_uint32& nMinor, LibMCDriver_TCPIP_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void Connect(const std::string& sIPAddress, const LibMCDriver_TCPIP_uint32 nPort, const LibMCDriver_TCPIP_uint32 nTimeout) override;

	void Disconnect() override;

	void SendBuffer(const LibMCDriver_TCPIP_uint64 nBufferBufferSize, const LibMCDriver_TCPIP_uint8* pBufferBuffer) override;

	bool WaitForData(const LibMCDriver_TCPIP_uint32 nTimeOutInMS) override;

	IDriver_TCPIPPacket* ReceivePacket(const LibMCDriver_TCPIP_uint32 nPacketSize, const LibMCDriver_TCPIP_uint32 nTimeOutInMS) override;

	bool IsConnected() override;
};

} // namespace Impl
} // namespace LibMCDriver_TCPIP

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_TCPIP_DRIVER_TCPIP
