/*++

Copyright (C) 2023 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include "amc_alertdefinition.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"



namespace AMC {

	CAlertDefinition::CAlertDefinition(const std::string& sIdentifier, LibMCData::eAlertLevel alertLevel, const CLanguageString& sDescription, bool bNeedsAcknowledgement)
		: m_sIdentifier (sIdentifier), m_AlertLevel (alertLevel), m_sDescription (sDescription), m_bNeedAcknowledgement (bNeedsAcknowledgement)
	{

	}

	CAlertDefinition::~CAlertDefinition()
	{

	}

	std::string CAlertDefinition::getIdentifier()
	{
		return m_sIdentifier;
	}

	LibMCData::eAlertLevel CAlertDefinition::getAlertLevel()
	{
		return m_AlertLevel;
	}

	CLanguageString CAlertDefinition::getDescription()
	{
		return m_sDescription;
	}

	std::string CAlertDefinition::getTranslatedDescription(CLanguageDefinition* pLanguage)
	{
		return m_sDescription.getTranslatedString(pLanguage);
	}

	bool CAlertDefinition::needsAcknowledgement()
	{
		return m_bNeedAcknowledgement;
	}

	void CAlertDefinition::setAckPermissionIdentifier(std::string& sAckPermissionIdentifier)
	{
		m_sAckPermissionIdentifier = sAckPermissionIdentifier;
	}

	std::string CAlertDefinition::getAckPermissionIdentifier()
	{
		return m_sAckPermissionIdentifier;
	}


}


