/*++

Copyright (C) 2024 ASL Inc.

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


Abstract: This is the class declaration of CDriverContext

*/


#ifndef __LIBMCDRIVER_ASL_DRIVERCONTEXT
#define __LIBMCDRIVER_ASL_DRIVERCONTEXT

#include "libmcdriver_asl_interfaces.hpp"

// Parent classes
#include "libmcdriver_asl_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_asl_drivercontextinstance.hpp"

namespace LibMCDriver_ASL {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriverContext 
**************************************************************************************************************************/

class CDriverContext : public virtual IDriverContext, public virtual CBase {
private:

	std::shared_ptr<CDriverContextInstance> m_pInstance;

public:

	CDriverContext(std::shared_ptr<CDriverContextInstance> pInstance);

	virtual ~CDriverContext();
	
	std::string GetSerialNumber() override;

	void SetPower(const bool bPower) override;

	void SetPrintheadMode(const LibMCDriver_ASL::eBoardMode eMode) override;

	void SetFrequency(const LibMCDriver_ASL_uint32 nFrequency) override;

	void SetTemperature(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_double dTemperature) override;

	void HomeLocation() override;

	void SetPrintStart(const LibMCDriver_ASL_uint32 nStartLocation) override;

	void SendImage(const LibMCDriver_ASL_uint8 nIndex, const LibMCDriver_ASL_uint32 nPadding, LibMCEnv::PImageData pImageObject) override;

	void Poll() override;

	LibMCDriver_ASL_double GetTemperature(const LibMCDriver_ASL_uint8 nIndex, const bool bSet) override;

	LibMCDriver_ASL_double GetPrintCounts(const LibMCDriver_ASL_uint8 nIndex) override;

	LibMCDriver_ASL_double GetImageLength(const LibMCDriver_ASL_uint8 nIndex) override;

	LibMCDriver_ASL_double GetHeadState(const LibMCDriver_ASL_uint8 nIndex) override;

	bool IsHeating(const LibMCDriver_ASL_uint8 nIndex) override;

	bool GetPower() override;

	LibMCDriver_ASL_uint32 GetHeadTimeOn() override;

	bool VerifyImages() override;

};

} // namespace Impl
} // namespace LibMCDriver_ASL

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_ASL_DRIVERCONTEXT
