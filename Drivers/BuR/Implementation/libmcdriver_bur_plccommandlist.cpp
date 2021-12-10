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

CPLCCommandList::CPLCCommandList(PDriver_BuRConnector pConnector, ITimeStampGenerator* pTimeStampGenerator)
    : m_pConnector (pConnector), m_pTimeStampGenerator (pTimeStampGenerator), m_ListIdentifier (PLC_INVALID_LISTIDENTIFIER)
{
    if (pTimeStampGenerator == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    if (m_pConnector.get() != nullptr) {

        auto nTimeStamp = m_pTimeStampGenerator->generateTimeStamp();
        auto nSequenceID = m_pConnector->sendSimpleCommandToPLC(BUR_COMMAND_DIRECT_BEGINLIST, nTimeStamp,
            [this](CDriver_BuRSendInfo* pSendInfo, CDriver_BuRPacket* pPacket) {

            m_ListIdentifier = pPacket->readUInt32(0);

            m_CommandSequences.erase(pSendInfo->getSequenceID());

        });

        m_CommandSequences.insert(nSequenceID);

    }

}


CPLCCommandList::~CPLCCommandList()
{
    if (m_pConnector.get() != nullptr) {
        for (auto sequenceID : m_CommandSequences) {
            m_pConnector->unregisterSendCallback(sequenceID);
        }
    }
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

    sAMCFToPLCPacketPayload payLoad;
    for (uint32_t nIndex = 0; nIndex < 24; nIndex++)
        payLoad.m_nData[nIndex] = 0;

    auto pParameterNames = pDuplicateCommand->getParameterNames();
    for (auto sParameterName : pParameterNames) {
        auto pParameter = pDuplicateCommand->findParameter(sParameterName);
        pParameter->writeToPayload (payLoad);               
    }

    auto nTimeStamp = m_pTimeStampGenerator->generateTimeStamp();

    if (m_pConnector.get() != nullptr) {

        auto nSequenceID = m_pConnector->sendCommandToPLC(pDuplicateCommand->getCommandID(), payLoad, nTimeStamp,
            [this](CDriver_BuRSendInfo* pSendInfo, CDriver_BuRPacket* pPacket) {

            m_CommandSequences.erase(pSendInfo->getSequenceID());

        });

        m_CommandSequences.insert(nSequenceID);
    }

}



void CPLCCommandList::ExecuteList()
{

    auto nTimeStamp = m_pTimeStampGenerator->generateTimeStamp();
    auto commandCallback = [this](CDriver_BuRSendInfo* pSendInfo, CDriver_BuRPacket* pPacket) {

        m_CommandSequences.erase(pSendInfo->getSequenceID());

    };

    if (m_pConnector.get() != nullptr) {
        auto nSequenceID = m_pConnector->sendSimpleCommandToPLC(BUR_COMMAND_DIRECT_EXECUTELIST, nTimeStamp, commandCallback, m_ListIdentifier);
        m_CommandSequences.insert(nSequenceID);
    }


}


std::future<uint8_t> CPLCCommandList::receiveListStatus()
{
    std::promise<uint8_t> receivedListStatus;
    auto futureListStatus = receivedListStatus.get_future();
    m_receivedListStatus.push_back(std::move(receivedListStatus));

    auto globalPromiseP = &m_receivedListStatus.back();

    auto nTimeStamp = m_pTimeStampGenerator->generateTimeStamp();
    auto commandCallback = [this, globalPromiseP](CDriver_BuRSendInfo* pSendInfo, CDriver_BuRPacket* pPacket) {

        uint8_t listStatus = pPacket->readUInt8 (0);
        globalPromiseP->set_value(listStatus);

        m_CommandSequences.erase(pSendInfo->getSequenceID());

    };

    if (m_pConnector.get() != nullptr) {
        auto nSequenceID = m_pConnector->sendSimpleCommandToPLC(BUR_COMMAND_DIRECT_LISTSTATUS, nTimeStamp, commandCallback, m_ListIdentifier);
        m_CommandSequences.insert(nSequenceID);
    }

    return futureListStatus;
}

bool CPLCCommandList::WaitForList(const LibMCDriver_BuR_uint32 nReactionTimeInMS, const LibMCDriver_BuR_uint32 nWaitForTimeInMS)
{
    uint64_t nStartTime = m_pTimeStampGenerator->generateTimeStamp();

    int statusValue = 255;
    while (statusValue != 6) {
        auto futureListStatus = receiveListStatus();
        futureListStatus.wait_for(std::chrono::milliseconds(nReactionTimeInMS));
        
        if (futureListStatus.valid())
        {
            statusValue = futureListStatus.get();
        }
        else {
            throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COMMANDREACTIONTIMEOUT);
        }

        uint64_t nCurrentTime = m_pTimeStampGenerator->generateTimeStamp();
        if (nCurrentTime > nStartTime + nWaitForTimeInMS)
            return false;
        

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
    auto nTimeStamp = m_pTimeStampGenerator->generateTimeStamp();
    auto commandCallback = [this](CDriver_BuRSendInfo* pSendInfo, CDriver_BuRPacket* pPacket) {

    

        m_CommandSequences.erase(pSendInfo->getSequenceID());

    };

    if (m_pConnector.get() != nullptr) {
        auto nSequenceID = m_pConnector->sendSimpleCommandToPLC(BUR_COMMAND_DIRECT_FINISHLIST, nTimeStamp, commandCallback, m_ListIdentifier);
        m_CommandSequences.insert(nSequenceID);
    }

}
