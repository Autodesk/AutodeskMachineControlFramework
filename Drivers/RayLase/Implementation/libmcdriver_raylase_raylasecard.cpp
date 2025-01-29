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
#include "libmcdriver_raylase_nlightdriverboard.hpp"

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

void CRaylaseCard::EnableCommandLogging()
{
    m_pRaylaseCardImpl->EnableCommandLogging();
}

void CRaylaseCard::DisableCommandLogging()
{
    m_pRaylaseCardImpl->DisableCommandLogging();
}

IRaylaseCommandLog* CRaylaseCard::RetrieveLatestLog()
{
    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_NOTIMPLEMENTED);
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

void CRaylaseCard::DrawLayer(const std::string & sStreamUUID, const LibMCDriver_Raylase_uint32 nLayerIndex, const LibMCDriver_Raylase_uint32 nScanningTimeoutInMS)
{
    bool bVerbose = true;

    if (m_pRaylaseCardImpl->isSimulationMode ())
        return;

    auto pDriverEnvironment = m_pRaylaseCardImpl->getDriverEnvironment();

    uint64_t nStartTime = pDriverEnvironment->GetGlobalTimerInMilliseconds();

    auto pToolpathAccessor = pDriverEnvironment->CreateToolpathAccessor(sStreamUUID);

	if (bVerbose)
    	pDriverEnvironment->LogMessage("Loading layer");

    auto pLayer = pToolpathAccessor->LoadLayer(nLayerIndex);

    auto pList = m_pRaylaseCardImpl->createNewList();
	if (bVerbose)
    	pDriverEnvironment->LogMessage("Adding Layer to List");

    pList->addLayerToList(pLayer, 0, false);
    
	if (bVerbose)
    	pDriverEnvironment->LogMessage("Set List on Card");

    pList->setListOnCard(0);
	if (bVerbose)
    	pDriverEnvironment->LogMessage("ExecuteList");

    try {

        pList->executeList(0);
        bool done = false;
        while (!done) {
            if (bVerbose)
                pDriverEnvironment->LogMessage("Waiting for execution");

            uint64_t nCurrentTime = pDriverEnvironment->GetGlobalTimerInMilliseconds();
            if (nCurrentTime < nStartTime)
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDSYSTEMTIMING);

            uint64_t nMillisecondsPassed = nCurrentTime - nStartTime;
            if (nMillisecondsPassed > nScanningTimeoutInMS)
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_SCANNINGTIMEOUT);

            done = pList->waitForExecution(100);
            if (bVerbose)
                pDriverEnvironment->LogMessage("Waiting for execution.. Done: " + std::to_string((int)done));

        }

        if (bVerbose)
            pDriverEnvironment->LogMessage("Deleting list from card");

    }
    catch (...)
    {
        // Abort execution, if it is still running
        m_pRaylaseCardImpl->abortListExecution();

        // Always delete list on card
        pList->deleteListListOnCard();

        throw;
    }

    pList->deleteListListOnCard();

	if (bVerbose)
    	pDriverEnvironment->LogMessage("Layer exposure finished");
}

void CRaylaseCard::SetRotationalCoordinateTransform(const LibMCDriver_Raylase_double dM11, const LibMCDriver_Raylase_double dM12, const LibMCDriver_Raylase_double dM21, const LibMCDriver_Raylase_double dM22)
{
    auto pTransform = m_pRaylaseCardImpl->getCoordinateTransform();
    pTransform->setRotationalCoordinateTransform(dM11, dM12, dM21, dM22);
}

void CRaylaseCard::GetRotationalCoordinateTransform(LibMCDriver_Raylase_double& dM11, LibMCDriver_Raylase_double& dM12, LibMCDriver_Raylase_double& dM21, LibMCDriver_Raylase_double& dM22)
{
    auto pTransform = m_pRaylaseCardImpl->getCoordinateTransform();
    pTransform->getRotationalCoordinateTransform(dM11, dM12, dM21, dM22);
}

void CRaylaseCard::SetTranslationalCoordinateTransform(const LibMCDriver_Raylase_double dOffsetX, const LibMCDriver_Raylase_double dOffsetY)
{
    auto pTransform = m_pRaylaseCardImpl->getCoordinateTransform();
    pTransform->setTranslationalCoordinateTransform(dOffsetX, dOffsetY);
}

void CRaylaseCard::GetTranslationalCoordinateTransform(LibMCDriver_Raylase_double& dOffsetX, LibMCDriver_Raylase_double& dOffsetY)
{
    auto pTransform = m_pRaylaseCardImpl->getCoordinateTransform();
    pTransform->getTranslationalCoordinateTransform(dOffsetX, dOffsetY);
}


bool CRaylaseCard::IsConnected()
{
    return m_pRaylaseCardImpl->IsConnected();
}


INLightDriverBoard* CRaylaseCard::GetNLightDriverBoard() 
{
    return new CNLightDriverBoard(m_pRaylaseCardImpl);
}
