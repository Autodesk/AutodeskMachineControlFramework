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


Abstract: This is a stub class definition of CAlertSession

*/

#include "libmcdata_alertsession.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "libmcdata_alert.hpp"
#include "libmcdata_alertiterator.hpp"

#include "common_utils.hpp"

// Include custom headers here.


using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CAlertSession 
**************************************************************************************************************************/

CAlertSession::CAlertSession(AMCData::PJournal pJournal)
    : m_pJournal(pJournal)
{
    if (pJournal.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

CAlertSession::~CAlertSession()
{

}

IAlert* CAlertSession::AddAlert(const std::string& sUUID, const std::string& sIdentifier, const LibMCData::eAlertLevel eLevel, const std::string& sDescription, const std::string& sDescriptionIdentifier, const std::string& sReadableContextInformation, const bool bNeedsAcknowledgement, const std::string& sTimestampUTC)
{
    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);

    m_pJournal->addAlert(sNormalizedUUID, sIdentifier, eLevel, sDescription, sDescriptionIdentifier, sReadableContextInformation, bNeedsAcknowledgement, sTimestampUTC);

    return new CAlert(m_pJournal, sNormalizedUUID);
}

bool CAlertSession::HasAlert(const std::string& sUUID)
{
    return m_pJournal->hasAlert(sUUID);
}

IAlert* CAlertSession::GetAlertByUUID(const std::string& sUUID)
{
    std::string sNormalizedUUID = AMCCommon::CUtils::normalizeUUIDString(sUUID);
    if (!m_pJournal->hasAlert(sNormalizedUUID))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_ALERTNOTFOUND, "alert not found: " + sNormalizedUUID + " (in CAlertSession::GetAlertByUUID)");

    return new CAlert (m_pJournal, sUUID);
}

IAlertIterator* CAlertSession::RetrieveAlerts(const bool bOnlyActive)
{
    std::unique_ptr<CAlertIterator> pResultIterator (new CAlertIterator());
    std::vector<std::string> alertUUIDs;

    if (bOnlyActive) {
        m_pJournal->retrieveActiveAlerts(alertUUIDs);
    }
    else {
        m_pJournal->retrieveAlerts(alertUUIDs);
    }

    for (auto sUUID : alertUUIDs)
        pResultIterator->AddAlert(std::make_shared<CAlert>(m_pJournal, sUUID));

    return pResultIterator.release ();
}


IAlertIterator* CAlertSession::RetrieveAlertsByType(const std::string& sIdentifier, const bool bOnlyActive)
{
    if (sIdentifier.empty())
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_EMPTYALERTIDENTIFIER);

    std::unique_ptr<CAlertIterator> pResultIterator(new CAlertIterator());
    std::vector<std::string> alertUUIDs;

    if (bOnlyActive) {
        m_pJournal->retrieveActiveAlertsByType(alertUUIDs, sIdentifier);
    }
    else {
        m_pJournal->retrieveAlertsByType(alertUUIDs, sIdentifier);
    }

    for (auto sUUID : alertUUIDs)
        pResultIterator->AddAlert(std::make_shared<CAlert>(m_pJournal, sUUID));

    return pResultIterator.release();

}
