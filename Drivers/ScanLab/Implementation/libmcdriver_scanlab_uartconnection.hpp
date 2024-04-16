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


Abstract: This is the class declaration of CUARTConnection

*/


#ifndef __LIBMCDRIVER_SCANLAB_UARTCONNECTION
#define __LIBMCDRIVER_SCANLAB_UARTCONNECTION

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlab_sdk.hpp"

namespace LibMCDriver_ScanLab {
namespace Impl {

#include <vector>

/*************************************************************************************************************************
 Class declaration of CUARTConnection 
**************************************************************************************************************************/

class CUARTConnection : public virtual IUARTConnection, public virtual CBase {
private:

	uint32_t m_nCardNo;
	uint32_t m_nDesiredBaudRate;
	uint32_t m_nActualBaudRate;

	PScanLabSDK m_pScanlabSDK;

	std::vector<uint8_t> m_ReceiveBuffer;

	void pollRTCReceiveBuffer();

public:

	CUARTConnection(PScanLabSDK pScanlabSDK, uint32_t nDesiredBaudRate, uint32_t nCardNo);

	virtual ~CUARTConnection();

	LibMCDriver_ScanLab_uint32 GetBaudRate() override;

	LibMCDriver_ScanLab_uint32 GetConfiguredBaudRate() override;

	void ClearReceiveBuffer() override;

	LibMCDriver_ScanLab_uint32 AvailableBytes() override;

	void WriteString(const std::string & sValue) override;

	void WriteData(const LibMCDriver_ScanLab_uint64 nDataBufferSize, const LibMCDriver_ScanLab_uint8 * pDataBuffer) override;

	void ReadData(const LibMCDriver_ScanLab_uint32 nByteCount, const LibMCDriver_ScanLab_uint32 nTimeOutInMS, LibMCDriver_ScanLab_uint64 nDataBufferSize, LibMCDriver_ScanLab_uint64* pDataNeededCount, LibMCDriver_ScanLab_uint8 * pDataBuffer) override;

	std::string ReadLine(const std::string & sSeparator, const LibMCDriver_ScanLab_uint32 nMaxLineLength, const LibMCDriver_ScanLab_uint32 nTimeOutInMS) override;


};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_UARTCONNECTION
