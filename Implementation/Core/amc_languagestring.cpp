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


#include "amc_languagestring.hpp"
#include "amc_languagedefinition.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"

#include "PugiXML/pugixml.hpp"

namespace AMC {


	CLanguageString::CLanguageString(const std::string& sStringIdentifier, const std::string& sCustomValue)
		: m_sStringIdentifier (sStringIdentifier), m_sCustomValue (sCustomValue)
	{
		if (!sStringIdentifier.empty()) {
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sStringIdentifier))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER, sStringIdentifier);

		}


	}


	CLanguageString::CLanguageString(pugi::xml_node & xmlNode, const std::string & sAttributeName)
	{
		std::string sStringIdentifier = xmlNode.attribute("i18n:description").as_string ();

		if (!sStringIdentifier.empty()) {
			if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sStringIdentifier))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER, sStringIdentifier);

			m_sStringIdentifier = sStringIdentifier;
		}

		m_sCustomValue = xmlNode.attribute("description").as_string ();

	}


	CLanguageString::~CLanguageString()
	{

	}

	std::string CLanguageString::getStringIdentifier()
	{
		return m_sStringIdentifier;
	}

	std::string CLanguageString::getCustomValue()
	{
		return m_sCustomValue;
	}

	std::string CLanguageString::getTranslatedString(CLanguageDefinition* pLanguageDefinition)
	{
		if (pLanguageDefinition == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDLANGUAGEDEFINITION);

		if (m_sStringIdentifier.empty())
			return m_sCustomValue;

		return pLanguageDefinition->getTranslatedString(m_sStringIdentifier);
	}

	std::string CLanguageString::getTranslatedString(PLanguageDefinition pLanguageDefinition)
	{
		return getTranslatedString(pLanguageDefinition.get ());
	}
}


