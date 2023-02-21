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


Abstract: This is the class declaration of CChannelInformation

*/


#ifndef __LIBMCDRIVER_CIFX_CHANNELINFORMATION
#define __LIBMCDRIVER_CIFX_CHANNELINFORMATION

#include "libmcdriver_cifx_interfaces.hpp"

// Parent classes
#include "libmcdriver_cifx_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_cifx_channel.hpp"


namespace LibMCDriver_CifX {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CChannelInformation 
**************************************************************************************************************************/

class CChannelInformation : public virtual IChannelInformation, public virtual CBase {
private:
	PDriver_CifXChannel m_pChannel;

public:

	CChannelInformation(PDriver_CifXChannel pChannel);

	virtual ~CChannelInformation();

	std::string GetBoardName() override;

	LibMCDriver_CifX_uint32 GetChannelIndex() override;

	bool IsConnected() override;

	LibMCDriver_CifX_uint32 GetMillisecondsSinceLastUpdate() override;

	void GetConnectionStatistics(LibMCDriver_CifX_uint32 & nNumberOfSucceededUpdates, LibMCDriver_CifX_uint32 & nNumberOfUpdateErrors, LibMCDriver_CifX_double & dMinimumUpdateDurationInMs, LibMCDriver_CifX_double & dMaximumUpdateDurationInMs, LibMCDriver_CifX_double & dAverageUpdateDurationInMs, LibMCDriver_CifX_double & dUpdateDurationVarianceInMs) override;

	bool ValueExists(const std::string & sName) override;

	void GetValueType(const std::string & sName, LibMCDriver_CifX::eValueType & eValueType, bool & bIsInput, bool & bIsOutput) override;

	void GetIntegerValueRange(const std::string & sName, LibMCDriver_CifX_int64 & nMinValue, LibMCDriver_CifX_int64 & nMaxValue) override;

	LibMCDriver_CifX_uint32 GetInputValueCount() override;

	std::string GetInputValueName(const LibMCDriver_CifX_uint32 nIndex) override;

	LibMCDriver_CifX_uint32 GetOutputValueCount() override;

	std::string GetOutputValueName(const LibMCDriver_CifX_uint32 nIndex) override;

};

} // namespace Impl
} // namespace LibMCDriver_CifX

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_CIFX_CHANNELINFORMATION
