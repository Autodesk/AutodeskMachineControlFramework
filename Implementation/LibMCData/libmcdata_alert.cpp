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
	: m_pJournal (pJournal), m_sAlertUUID (AMCCommon::CUtils::normalizeUUIDString (sAlertUUID))
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

std::string CAlert::GetUUID()
{
	return m_sAlertUUID;
}

std::string CAlert::GetIdentifier()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

LibMCData::eAlertLevel CAlert::GetLevel()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

std::string CAlert::GetDescription()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

std::string CAlert::GetDescriptionIdentifier()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

std::string CAlert::GetReadableContextInformation()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

bool CAlert::GetNeedsAcknowledgement()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

std::string CAlert::GetTimestampUTC()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

bool CAlert::HasBeenAcknowledged()
{
	throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_NOTIMPLEMENTED);
}

AMCData::PJournal CAlert::getJournal()
{
	return m_pJournal;
}
