/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CAxis

*/


#ifndef __LIBMCDRIVER_TML_AXIS
#define __LIBMCDRIVER_TML_AXIS

#include "libmcdriver_tml_interfaces.hpp"
#include "libmcdriver_tml_instance.hpp"

// Parent classes
#include "libmcdriver_tml_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_TML {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CAxis 
**************************************************************************************************************************/

class CAxis : public virtual IAxis, public virtual CBase {
private:

    PTMLInstance m_pTMLInstance;
    std::string m_sChannelIdentifier;
    std::string m_sAxisIdentifier;
    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

protected:

public:

    CAxis(PTMLInstance pTMLInstance, const std::string& sChannelIdentifier, const std::string & sAxisIdentifier, LibMCEnv::PDriverEnvironment pDriverEnvironment);

    virtual ~CAxis();

    void SetPower(const bool bEnable) override;
    LibMCDriver_TML_uint32 ReadRegister(const LibMCDriver_TML_uint32 nRegister) override;
    bool CheckPower() override;
    void MoveRelative(const LibMCDriver_TML_double dDistance, const LibMCDriver_TML_double dSpeed, const LibMCDriver_TML_double dAcceleration) override;
    void MoveAbsolute(const LibMCDriver_TML_double dDistance, const LibMCDriver_TML_double dSpeed, const LibMCDriver_TML_double dAcceleration) override;
    std::string GetIdentifier() override;
    void CallSubroutine(const std::string& sRoutine) override;
    LibMCDriver_TML_double GetPosition(const LibMCDriver_TML::eReferenceType eReference) override;
    LibMCDriver_TML_double GetSpeed(const LibMCDriver_TML::eReferenceType eReference) override;
    LibMCDriver_TML_int32 GetIntVariable(const std::string & sVariableName) override;
    bool MotionComplete() override;
    bool TargetReached() override;
    bool CheckAxisError(LibMCDriver_TML_uint16& nErrorRegister) override;
    void ResetAxis(const bool bForceFull) override;

    std::string GetChannelIdentifier() override;

};

} // namespace Impl
} // namespace LibMCDriver_TML

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_TML_AXIS
