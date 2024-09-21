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

void CRaylaseCard::AssignLaserIndex(const LibMCDriver_Raylase_uint32 nLaserIndex)
{
    m_pRaylaseCardImpl->assignLaserIndex(nLaserIndex);
}

LibMCDriver_Raylase_uint32 CRaylaseCard::GetAssignedLaserIndex()
{
    return m_pRaylaseCardImpl->getAssignedLaserIndex();
}

void CRaylaseCard::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex)
{
    if (m_pRaylaseCardImpl->isSimulationMode ())
        return;

    auto pDriverEnvironment = m_pRaylaseCardImpl->getDriverEnvironment();

    auto pToolpathAccessor = pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);
    auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

    auto pList = m_pRaylaseCardImpl->createNewList();
    pList->addLayerToList(pLayer, 0, false);
    pList->executeList(0);
    bool done = false;
    while (!done) {
        pList->waitForExecution(100);
    }
    pList->deleteListListOnCard();
}

bool CRaylaseCard::IsConnected()
{
    return m_pRaylaseCardImpl->IsConnected();
}

void CRaylaseCard::Disconnect()
{
    m_pRaylaseCardImpl->Disconnect();

}