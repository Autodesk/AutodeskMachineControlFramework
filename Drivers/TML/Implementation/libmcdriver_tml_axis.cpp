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


Abstract: This is a stub class definition of CAxis

*/

#include "libmcdriver_tml_axis.hpp"
#include "libmcdriver_tml_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_TML::Impl;

/*************************************************************************************************************************
 Class definition of CAxis 
**************************************************************************************************************************/

CAxis::CAxis(PTMLInstance pTMLInstance, const std::string& sChannelIdentifier, const std::string& sAxisIdentifier, LibMCEnv::PDriverEnvironment pDriverEnvironment)
    : m_pTMLInstance (pTMLInstance), m_sChannelIdentifier (sChannelIdentifier), m_sAxisIdentifier (sAxisIdentifier), m_pDriverEnvironment(pDriverEnvironment)
{
    if (pTMLInstance.get () == nullptr)
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDPARAM);

    if (!CTMLInstance::checkIdentifierString(sChannelIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDCHANNELIDENTIFIER);
    if (!CTMLInstance::checkIdentifierString(sAxisIdentifier))
        throw ELibMCDriver_TMLInterfaceException(LIBMCDRIVER_TML_ERROR_INVALIDAXISIDENTIFIER);
}

CAxis::~CAxis()
{

}

void CAxis::MoveRelative(const LibMCDriver_TML_double dDistance, const LibMCDriver_TML_double dSpeed, const LibMCDriver_TML_double dAcceleration)
{
    m_pTMLInstance->moveAxisRelative(m_sChannelIdentifier, m_sAxisIdentifier, dDistance, dSpeed, dAcceleration);
}

void CAxis::SetPower(const bool bEnable)
{
    m_pTMLInstance->setAxisPower(m_sChannelIdentifier, m_sAxisIdentifier, bEnable);
}

bool CAxis::CheckPower(void)
{
    LibMCDriver_TML_uint32 register_value = m_pTMLInstance->readAxisStatus(m_sChannelIdentifier, m_sAxisIdentifier, TML_REG_SRL);

    
    if ((register_value & TML_REG_SRL_POWERBIT) != 0) {
        return true;
    }
    else {
        return false;
    }

}


LibMCDriver_TML_uint32 CAxis::ReadRegister(const LibMCDriver_TML_uint32 nRegister)
{
    return m_pTMLInstance->readAxisStatus(m_sChannelIdentifier, m_sAxisIdentifier, nRegister);
}

std::string CAxis::GetIdentifier()
{
    return m_sAxisIdentifier;
}

std::string CAxis::GetChannelIdentifier()
{
    return m_sChannelIdentifier;
}


void CAxis::CallSubroutine(const std::string& sRoutine) {
    return m_pTMLInstance->callSubroutine(m_sChannelIdentifier, m_sAxisIdentifier, sRoutine);
}

void CAxis::MoveAbsolute(const LibMCDriver_TML_double dDistance, const LibMCDriver_TML_double dSpeed, const LibMCDriver_TML_double dAcceleration) {
    m_pTMLInstance->moveAxisAbsolute(m_sChannelIdentifier, m_sAxisIdentifier, dDistance, dSpeed, dAcceleration);
}

LibMCDriver_TML_double CAxis::GetPosition(const LibMCDriver_TML::eReferenceType eReference) {

    if (eReference == LibMCDriver_TML::eReferenceType::Actual_Load)
        return m_pTMLInstance->readPosition(m_sChannelIdentifier, m_sAxisIdentifier, "APOS_LD");

    if (eReference == LibMCDriver_TML::eReferenceType::Actual_Motor)
        return m_pTMLInstance->readPosition(m_sChannelIdentifier, m_sAxisIdentifier, "APOS_MT");

    if (eReference == LibMCDriver_TML::eReferenceType::Error)
        return m_pTMLInstance->readPosition(m_sChannelIdentifier, m_sAxisIdentifier, "POSERR");

    //Default case being target
    return m_pTMLInstance->readPosition(m_sChannelIdentifier, m_sAxisIdentifier, "TPOS");

}
LibMCDriver_TML_double CAxis::GetSpeed(const LibMCDriver_TML::eReferenceType eReference) {

    if (eReference == LibMCDriver_TML::eReferenceType::Actual_Load)
        return m_pTMLInstance->readSpeed(m_sChannelIdentifier, m_sAxisIdentifier, "ASPD_LD");

    if (eReference == LibMCDriver_TML::eReferenceType::Actual_Motor)
        return m_pTMLInstance->readSpeed(m_sChannelIdentifier, m_sAxisIdentifier, "ASPD_MT");

    if (eReference == LibMCDriver_TML::eReferenceType::Error)
        return m_pTMLInstance->readSpeed(m_sChannelIdentifier, m_sAxisIdentifier, "SPDERR");

    //Default case being target
    return m_pTMLInstance->readSpeed(m_sChannelIdentifier, m_sAxisIdentifier, "TSPD");

}

LibMCDriver_TML_int32 CAxis::GetIntVariable(const std::string& sVariableName) {

    return (LibMCDriver_TML_int32)m_pTMLInstance->readIntVariable(m_sChannelIdentifier, m_sAxisIdentifier, sVariableName);

}

bool CAxis::MotionComplete() {


    auto registerContent = m_pTMLInstance->readAxisStatus(m_sChannelIdentifier, m_sAxisIdentifier, TML_REG_SRL);
    if (registerContent & TML_REG_SRL_MOTIONCOMPLETE)
        return true;

    return false;

}

bool CAxis::TargetReached() {

    auto registerContent = m_pTMLInstance->readAxisStatus(m_sChannelIdentifier, m_sAxisIdentifier, TML_REG_SRH);
    if (registerContent & TML_REG_SRH_TARGETREACHED)
        return true;

    return false;

}


bool CAxis::CheckAxisError(LibMCDriver_TML_uint16& nErrorRegister) {

    nErrorRegister = m_pTMLInstance->readAxisStatus(m_sChannelIdentifier, m_sAxisIdentifier, TML_REG_MER);

    uint16_t nMaskRegister = (1 << (int)LibMCDriver_TML::eMERType::LSNST) + (1 << (int)LibMCDriver_TML::eMERType::LSPST);
    uint16_t nNoLimitError =  nErrorRegister & ~nMaskRegister; //ignore the limit switches in any checks

    if (nNoLimitError > 0)
        return true;

    return false;
}

void CAxis::ResetAxis(const bool bForceFull) {
    return m_pTMLInstance->resetAxis(m_sChannelIdentifier, m_sAxisIdentifier, bForceFull);
}