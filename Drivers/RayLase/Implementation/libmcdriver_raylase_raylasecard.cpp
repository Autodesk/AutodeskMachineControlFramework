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


Abstract: This is a stub class definition of CRaylaseCard

*/

#include "libmcdriver_raylase_raylasecard.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"


using namespace LibMCDriver_Raylase::Impl;

PRaylaseCardImpl CRaylaseCardImpl::connectByIP(PRaylaseSDK pSDK, const std::string& sCardName, const std::string& sCardIP, uint32_t nPort)
{
    return std::make_shared<CRaylaseCardImpl>(pSDK, sCardName, sCardIP, nPort);
}

CRaylaseCardImpl::CRaylaseCardImpl(PRaylaseSDK pSDK, const std::string& sCardName, const std::string& sCardIP, uint32_t nPort)
    : m_pSDK (pSDK), m_sCardName (sCardName), m_sCardIP (sCardIP), m_nPort (nPort), m_Handle (0)
{
    if (pSDK.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_SDKNOTINITIALIZED);

    m_Handle = m_pSDK->rlConnect(sCardIP.c_str(), nPort);
    if (m_Handle < 0)
        m_pSDK->checkError (m_Handle);
}

CRaylaseCardImpl::~CRaylaseCardImpl()
{
    if (m_pSDK.get() != nullptr) {
        if (m_Handle > 0)
            m_pSDK->rlDisconnect(m_Handle);
    }

}

void CRaylaseCardImpl::ResetToSystemDefaults()
{
    m_pSDK->checkError(m_pSDK->rlSystemResetToDefaults(m_Handle));
}

void CRaylaseCardImpl::LaserOn()
{
    m_pSDK->checkError(m_pSDK->rlLaserLaserOn(m_Handle));
}

void CRaylaseCardImpl::LaserOff()
{
    m_pSDK->checkError(m_pSDK->rlLaserLaserOff(m_Handle));
}

void CRaylaseCardImpl::ArmLaser(const bool bShallBeArmed)
{
    m_pSDK->checkError(m_pSDK->rlLaserArmLaser(m_Handle, bShallBeArmed));

}

bool CRaylaseCardImpl::IsLaserArmed()
{
    bool bIsArmed = false;
    m_pSDK->checkError(m_pSDK->rlLaserArmLaser(m_Handle, bIsArmed));

    return bIsArmed;
}

void CRaylaseCardImpl::EnablePilot(const bool bShallBeEnabled)
{
    m_pSDK->checkError(m_pSDK->rlLaserEnablePilot(m_Handle, bShallBeEnabled));

}

bool CRaylaseCardImpl::PilotIsEnabled()
{
    bool bPilotIsEnabled = false;
    m_pSDK->checkError(m_pSDK->rlLaserIsPilotEnabled(m_Handle, bPilotIsEnabled));

    return bPilotIsEnabled;
}

void CRaylaseCardImpl::GetLaserStatus(bool& bPilotIsEnabled, bool& bLaserIsArmed, bool& bLaserAlarm)
{

    uint32_t nStatusFlag = 0;
    m_pSDK->checkError(m_pSDK->rlLaserReadLaserStatus(m_Handle, nStatusFlag));

    bPilotIsEnabled = (nStatusFlag & (uint32_t)eRLLaserStatus::lsPilot) != 0;
    bLaserIsArmed = (nStatusFlag & (uint32_t)eRLLaserStatus::lsArmed) != 0;
    bLaserAlarm = (nStatusFlag & (uint32_t)eRLLaserStatus::lsAlarm) != 0;

}

void CRaylaseCardImpl::DrawLayer(const std::string& sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex)
{
    double squareSize = 50000.0;

    for (uint32_t nIndex = 0; nIndex < 100; nIndex++) {

        rlListHandle listHandle = m_pSDK->rlListAllocate(m_Handle);
        m_pSDK->checkError(m_pSDK->rlListAppendJumpSpeed(listHandle, 1.0));
        m_pSDK->checkError(m_pSDK->rlListAppendMarkSpeed(listHandle, 0.05));
        m_pSDK->checkError(m_pSDK->rlListAppendJumpAbs2D(listHandle, -squareSize, -squareSize));
        m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(listHandle, squareSize, -squareSize));
        m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(listHandle, squareSize, squareSize));
        m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(listHandle, -squareSize, squareSize));
        m_pSDK->checkError(m_pSDK->rlListAppendMarkAbs2D(listHandle, -squareSize, -squareSize));
        m_pSDK->checkError(m_pSDK->rlListAppendLaserOff(listHandle));
        m_pSDK->checkError(m_pSDK->rlListSet(m_Handle, 0, listHandle, false, -1));
        m_pSDK->checkError(m_pSDK->rlListExecute(m_Handle, 0));
        int timeoutMs = 30000;
        bool done = false;
        int32_t listID = 0;
        m_pSDK->checkError(m_pSDK->rlListWaitForListDone(m_Handle, timeoutMs, done, listID));
        m_pSDK->checkError(m_pSDK->rlListDelete(m_Handle, 0, true));
        m_pSDK->checkError(m_pSDK->rlListReleaseHandle(listHandle));

    }


}


/*************************************************************************************************************************
 Class definition of CRaylaseCard 
**************************************************************************************************************************/

CRaylaseCard::CRaylaseCard(PRaylaseCardImpl pRaylaseCardImpl)
    : m_pRaylaseCardImpl (pRaylaseCardImpl)
{
    if (pRaylaseCardImpl.get() == nullptr)
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

}

CRaylaseCard::~CRaylaseCard()
{

}


void CRaylaseCard::ResetToSystemDefaults()
{
    m_pRaylaseCardImpl->ResetToSystemDefaults ();
}

void CRaylaseCard::LaserOn()
{
    m_pRaylaseCardImpl->LaserOn();

}

void CRaylaseCard::LaserOff()
{
    m_pRaylaseCardImpl->LaserOff();
}

void CRaylaseCard::ArmLaser(const bool bShallBeArmed)
{
    m_pRaylaseCardImpl->ArmLaser(bShallBeArmed);
}

bool CRaylaseCard::IsLaserArmed()
{
    return m_pRaylaseCardImpl->IsLaserArmed ();
}

void CRaylaseCard::EnablePilot(const bool bShallBeEnabled)
{
    m_pRaylaseCardImpl->EnablePilot(bShallBeEnabled);
}

bool CRaylaseCard::PilotIsEnabled()
{
    return m_pRaylaseCardImpl->PilotIsEnabled();
}

void CRaylaseCard::GetLaserStatus(bool & bPilotIsEnabled, bool & bLaserIsArmed, bool & bLaserAlarm)
{
    m_pRaylaseCardImpl->GetLaserStatus(bPilotIsEnabled, bLaserIsArmed, bLaserAlarm);
}

void CRaylaseCard::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex)
{
    m_pRaylaseCardImpl->DrawLayer(sStreamUUID, nLayerIndex);
}

