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


Abstract: This is the class declaration of CGPIOSequence

*/


#ifndef __LIBMCDRIVER_SCANLAB_GPIOSEQUENCE
#define __LIBMCDRIVER_SCANLAB_GPIOSEQUENCE

#include "libmcdriver_scanlab_interfaces.hpp"
#include "libmcdriver_scanlab_gpiosequenceinstance.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif


namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CGPIOSequence 
**************************************************************************************************************************/

class CGPIOSequence : public virtual IGPIOSequence, public virtual CBase {
private:

	PGPIOSequenceInstance m_pInstance;

public:

	CGPIOSequence(PGPIOSequenceInstance pInstance);

	virtual ~CGPIOSequence();

	std::string GetIdentifier() override;

	void Clear() override;

	void AddOutput(const LibMCDriver_ScanLab_uint32 nOutputBit, const bool bOutputValue) override;

	void AddDelay(const LibMCDriver_ScanLab_uint32 nDelayInMilliseconds) override;

	void WaitforInput(const LibMCDriver_ScanLab_uint32 nInputBit, const bool bInputValue, const LibMCDriver_ScanLab_uint32 nMaxDelayInMilliseconds) override;

	void AddLabel(const std::string & sLabelName, const LibMCDriver_ScanLab_uint32 nMaxPasses) override;

	void GoToLabel(const std::string & sLabelName) override;

	void ConditionalGoToLabel(const LibMCDriver_ScanLab_uint32 nInputBit, const bool bInputValue, const std::string & sLabelName) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_GPIOSEQUENCE
