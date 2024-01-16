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


#include "amc_languagedefinition.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"

#define LANGUAGE_MAXSTRINGIDENTIFIERLENGTH 64

namespace AMC {


	CLanguageDefinition::CLanguageDefinition(const std::string& sLanguageIdentifier, PLanguageDefinition pParentLanguage)
		: m_sLanguageIdentifier (sLanguageIdentifier), m_pParentLanguage (pParentLanguage)
	{
		if (sLanguageIdentifier.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYLANGUAGEIDENTIFIER);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString (sLanguageIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDLANGUAGEIDENTIFIER, sLanguageIdentifier);


	}

	CLanguageDefinition::~CLanguageDefinition()
	{

	}

	std::string CLanguageDefinition::getLanguageIdentifier()
	{
		return m_sLanguageIdentifier;
	}

	PLanguageDefinition CLanguageDefinition::getParentLanguage()
	{
		return m_pParentLanguage;
	}


	std::string CLanguageDefinition::getTranslatedString(const std::string& sStringIdentifier)
	{
		if (sStringIdentifier.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYLANGUAGESTRINGIDENTIFIER);

		if (sStringIdentifier.size() >= LANGUAGE_MAXSTRINGIDENTIFIERLENGTH)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER);

		auto iIter = m_TranslationMap.find(sStringIdentifier);
		if (iIter != m_TranslationMap.end())
			return iIter->second;

		if (m_pParentLanguage.get() != nullptr)
			return m_pParentLanguage->getTranslatedString(sStringIdentifier);

		m_TranslationMisses.insert(sStringIdentifier);

		return "";
	}

	bool CLanguageDefinition::stringExists(const std::string& sStringIdentifier)
	{
		auto iIter = m_TranslationMap.find(sStringIdentifier);
		if (iIter != m_TranslationMap.end())
			return true;

		if (m_pParentLanguage.get() != nullptr)
			return m_pParentLanguage->stringExists(sStringIdentifier);

		return false;
	}

	void CLanguageDefinition::addTranslation(const std::string& sStringIdentifier, const std::string& sValue)
	{
		if (sStringIdentifier.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYLANGUAGESTRINGIDENTIFIER);

		if (sStringIdentifier.size() >= LANGUAGE_MAXSTRINGIDENTIFIERLENGTH)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sStringIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER, sStringIdentifier);

		m_TranslationMap.insert(std::make_pair (sStringIdentifier, sValue));
	}


}


