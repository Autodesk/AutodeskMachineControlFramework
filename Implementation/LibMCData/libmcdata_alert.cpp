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


Abstract: This is a stub class definition of CAlert

*/

#include "libmcdata_alert.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CAlert 
**************************************************************************************************************************/

CAlert::CAlert(AMCData::PJournal pJournal, const std::string& sAlertUUID)
	: m_pJournal (pJournal), 
	m_sAlertUUID (AMCCommon::CUtils::normalizeUUIDString (sAlertUUID)), 
	m_bNeedsAcknowledgement (false), 
	m_AlertLevel (LibMCData::eAlertLevel::Unknown),
	m_bNeedsRefresh (true)
{
	if (pJournal.get () == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

}

CAlert::~CAlert()
{

}


CAlert* CAlert::makeFrom(CAlert* pAlert)
{
	if (pAlert == nullptr)
		throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

	return new CAlert(pAlert->getJournal (), pAlert->GetUUID ());
}

std::shared_ptr<CAlert> CAlert::makeSharedFrom(CAlert* pAlert)
{
	return std::shared_ptr<CAlert>(makeFrom(pAlert));
}

void CAlert::refreshIfNeeded()
{
	if (m_bNeedsRefresh)
		forceRefresh();
}

void CAlert::forceRefresh()
{
	m_pJournal->getAlertInformation(m_sAlertUUID, m_sIdentifier, m_AlertLevel, m_sDescription, m_sDescriptionIdentifier, m_sReadableContextInformation, m_bNeedsAcknowledgement, m_sTimestampUTC);
	m_bNeedsRefresh = false;
}


std::string CAlert::GetUUID()
{
	return m_sAlertUUID;
}

std::string CAlert::GetIdentifier()
{
	refreshIfNeeded();
	return m_sIdentifier;
}

bool CAlert::IsActive()
{
	return m_pJournal->alertIsActive(m_sAlertUUID);
}

LibMCData::eAlertLevel CAlert::GetLevel()
{
	refreshIfNeeded();
	return m_AlertLevel;
}

std::string CAlert::GetLevelString()
{
	refreshIfNeeded();
	return AMCData::CJournal::convertAlertLevelToString(m_AlertLevel);

}


std::string CAlert::GetDescription()
{
	refreshIfNeeded();
	return m_sDescription;
}

std::string CAlert::GetDescriptionIdentifier()
{
	refreshIfNeeded();
	return m_sDescriptionIdentifier;
}

std::string CAlert::GetReadableContextInformation()
{
	refreshIfNeeded();
	return m_sReadableContextInformation;
}

bool CAlert::GetNeedsAcknowledgement()
{
	refreshIfNeeded();
	return m_bNeedsAcknowledgement;
}

std::string CAlert::GetTimestampUTC()
{
	refreshIfNeeded();
	return m_sTimestampUTC;
}

bool CAlert::HasBeenAcknowledged()
{
	return m_pJournal->alertHasBeenAcknowledged(m_sAlertUUID);
}

AMCData::PJournal CAlert::getJournal()
{
	return m_pJournal;
}

void CAlert::GetAcknowledgementInformation(std::string& sUserUUID, std::string& sUserComment, std::string& sAckTime)
{
	m_pJournal->getAcknowledgementInformation(m_sAlertUUID, sUserUUID, sUserComment, m_sTimestampUTC);
}

void CAlert::AcknowledgeForUser(const std::string& sUserUUID, const std::string& sUserComment)
{
}

void CAlert::DeactivateAlert()
{

}
