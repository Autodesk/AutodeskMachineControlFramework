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


Abstract: This is a stub class definition of CPLCCommandList

*/

#include "libmcdriver_bur_plccommandlist.hpp"
#include "libmcdriver_bur_plccommand.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"

// Include custom headers here.
#include <iostream>
#include <future>
#define PLC_INVALID_LISTIDENTIFIER 0xFFFFFFFF

using namespace LibMCDriver_BuR::Impl;

/*************************************************************************************************************************
 Class definition of CPLCCommandList 
**************************************************************************************************************************/

CPLCCommandList::CPLCCommandList(PDriver_BuRConnector pConnector, ITimeStampGenerator* pTimeStampGenerator, bool bIsSimulationMode)
    : m_pConnector(pConnector),  m_ListIdentifier(PLC_INVALID_LISTIDENTIFIER),
    m_bIsSimulationMode (bIsSimulationMode), m_pTimeStampGenerator (pTimeStampGenerator)
{
    if (pTimeStampGenerator == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    if (!m_bIsSimulationMode) {

        if (m_pConnector.get() != nullptr) {

            m_pConnector->sendCommandToPLC(BUR_COMMAND_DIRECT_BEGINLIST,
                [this](CDriver_BuRPacket* pPacket) {

                m_ListIdentifier = pPacket->readUInt32(0);
            });

        }
    }

}


CPLCCommandList::~CPLCCommandList()
{  
}

void CPLCCommandList::AddCommand(IPLCCommand* pCommandInstance)
{
    if (pCommandInstance == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    auto pCastedCommand = dynamic_cast<CPLCCommand*> (pCommandInstance);
    if (pCastedCommand == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDCAST);

    auto pDuplicateCommand = pCastedCommand->getCommandDefinition()->duplicate();
    m_pCommandQueue.push_back (pDuplicateCommand);

  /*   */

}



void CPLCCommandList::ExecuteList()
{
    if (m_pConnector == nullptr)
        return;

    if (!m_bIsSimulationMode) {

        std::vector <sAMCFToPLCPacketToSend> packetList;
        packetList.reserve(m_pCommandQueue.size());
        for (auto pCommand : m_pCommandQueue) {
            sAMCFToPLCPacketPayload payLoad;
            for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
                payLoad.m_nData[nIndex] = 0;

            auto pParameterNames = pCommand->getParameterNames();
            for (auto sParameterName : pParameterNames) {
                auto pParameter = pCommand->findParameter(sParameterName);
                pParameter->writeToPayload(payLoad);
            }

            packetList.push_back(m_pConnector->makePacket(pCommand->getCommandID(), payLoad, nullptr));
        }

        packetList.push_back(m_pConnector->makePacket(BUR_COMMAND_DIRECT_FINISHLIST, m_ListIdentifier, nullptr));
        packetList.push_back(m_pConnector->makePacket(BUR_COMMAND_DIRECT_EXECUTELIST, m_ListIdentifier, nullptr));

        m_pConnector->sendCommandsToPLC(packetList);


    }

}


uint8_t CPLCCommandList::receiveListStatus()
{
    uint8_t receivedListStatus = 0;

    if (!m_bIsSimulationMode) {

        if (m_pConnector != nullptr) {
            m_pConnector->sendCommandToPLC(BUR_COMMAND_DIRECT_LISTSTATUS, m_ListIdentifier,
                [&receivedListStatus](CDriver_BuRPacket* pPacket) {
                receivedListStatus = pPacket->readUInt8(0);
            });
        }

        return receivedListStatus;
    }

    return 0;

}

bool CPLCCommandList::WaitForList(const LibMCDriver_BuR_uint32 nReactionTimeInMS, const LibMCDriver_BuR_uint32 nWaitForTimeInMS)
{
    if (m_bIsSimulationMode) {
        return true;
    }

    uint64_t nStartTime = m_pTimeStampGenerator->generateTimeStamp();

    int statusValue = 255;
    while (statusValue != 6) {

        uint64_t nCurrentTime = m_pTimeStampGenerator->generateTimeStamp();
        if (nCurrentTime > nStartTime + nWaitForTimeInMS)
            return false;

        statusValue = receiveListStatus();

    }
    return true;

}

void CPLCCommandList::PauseList()
{
    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOTIMPLEMENTED);

}

void CPLCCommandList::ResumeList()
{
    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_NOTIMPLEMENTED);
}


void CPLCCommandList::FinishList()
{

}

void CPLCCommandList::DeleteList()
{
    if (!m_bIsSimulationMode) {

        if (m_pConnector != nullptr) {
            m_pConnector->sendCommandToPLC(BUR_COMMAND_DIRECT_DELETELIST, m_ListIdentifier,
                [](CDriver_BuRPacket* pPacket) {
            });
        }
    }

}
