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


Abstract: This is the class declaration of CDriver_UART

*/


#ifndef __LIBMCDRIVER_UART_DRIVER_UART
#define __LIBMCDRIVER_UART_DRIVER_UART

#include "libmcdriver_uart_interfaces.hpp"

// Parent classes
#include "libmcdriver_uart_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <string>

namespace serial {
	class Serial;
}

namespace LibMCDriver_UART {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_UART 
**************************************************************************************************************************/

class CDriver_UART : public virtual IDriver_UART, public virtual CDriver {
private:

	std::string m_sName;

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	bool m_bSimulationMode;

	std::unique_ptr<serial::Serial> m_pConnection;

	LibMCDriver_UART::eUARTParity m_Parity;
	LibMCDriver_UART::eUARTStopBits m_StopBits;
	LibMCDriver_UART::eUARTByteSize m_ByteSize;
	LibMCDriver_UART::eUARTFlowControl m_FlowControl;

protected:

public:

	CDriver_UART(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_UART();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_UART_uint32& nMajor, LibMCDriver_UART_uint32& nMinor, LibMCDriver_UART_uint32& nMicro, std::string& sBuild) override;

	void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

	void QueryParameters() override;

	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void Connect(const std::string& sDeviceAddress, const LibMCDriver_UART_uint32 nBaudRate, const LibMCDriver_UART_uint32 nTimeout) override;

	void Disconnect() override;

	bool IsConnected() override;

	std::string SendLine(const std::string& sLineToSend, const LibMCDriver_UART_uint32 nTimeout) override;

	void SetParity(const LibMCDriver_UART::eUARTParity eParity) override;

	LibMCDriver_UART::eUARTParity GetParity() override;

	void SetStopBits(const LibMCDriver_UART::eUARTStopBits eStopBits) override;

	LibMCDriver_UART::eUARTStopBits GetStopBits() override;

	void SetByteSize(const LibMCDriver_UART::eUARTByteSize eByteSize) override;

	LibMCDriver_UART::eUARTByteSize GetByteSize() override;

	void SetFlowControl(const LibMCDriver_UART::eUARTFlowControl eFlowControl) override;

	LibMCDriver_UART::eUARTFlowControl GetFlowControl() override;


};

} // namespace Impl
} // namespace LibMCDriver_UART

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_UART_DRIVER_UART
