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


Abstract: This is the class declaration of CModbusTCPConnection

*/


#ifndef __LIBMCENV_MODBUSTCPCONNECTION
#define __LIBMCENV_MODBUSTCPCONNECTION

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCEnv {
namespace Impl {

/*************************************************************************************************************************
 Class declaration of CModbusTCPConnection 
**************************************************************************************************************************/
class CModbusTCPConnectionInstance;

class CModbusTCPConnection : public virtual IModbusTCPConnection, public virtual CBase {
private:

	std::shared_ptr<CModbusTCPConnectionInstance> m_pModbusConnectionInstance;

public:

	CModbusTCPConnection(const std::string& sIPAddress, uint32_t nPort, uint32_t nTimeOutInMS);

	virtual ~CModbusTCPConnection();

	std::string GetIPAddress() override;

	LibMCEnv_uint32 GetPort() override;

	LibMCEnv_uint32 GetTimeout() override;

	bool IsConnected() override;

	void Disconnect() override;

	void Reconnect() override;

	LibMCEnv_uint16 DiagnosisCall(const LibMCEnv_uint16 nSubFunction, const LibMCEnv_uint16 nDataField) override;

	IModbusTCPDigitalIOStatus* ReadCoilStatus(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nBitCount) override;

	IModbusTCPDigitalIOStatus* ReadInputStatus(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nBitCount) override;

	IModbusTCPRegisterStatus* ReadHoldingRegisters(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nRegisterCount) override;

	IModbusTCPRegisterStatus* ReadInputRegisters(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint32 nRegisterCount) override;

	void ForceMultipleCoils(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint64 nBufferBufferSize, const LibMCEnv_uint8 * pBufferBuffer) override;

	void PresetMultipleRegisters(const LibMCEnv_uint32 nStartAddress, const LibMCEnv_uint64 nBufferBufferSize, const LibMCEnv_uint32 * pBufferBuffer) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_MODBUSTCPCONNECTION
