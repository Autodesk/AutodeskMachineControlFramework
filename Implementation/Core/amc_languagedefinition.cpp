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

namespace AMC {


	CLanguageDefinition::CLanguageDefinition(const std::string& sLanguageIdentifier, PLanguageDefinition pParentLanguage)
		: m_sLanguageIdentifier (sLanguageIdentifier), m_pParentLanguage (pParentLanguage)
	{
		if (sLanguageIdentifier.empty())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYLANGUAGEIDENTIFIER);

		if (!AMCCommon::CUtils::stringIsValidLanguageIdentifier (sLanguageIdentifier))
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


	bool CLanguageDefinition::findTranslation(const std::string& sStringIdentifier, std::string& sValue)
	{
		auto iIter = m_TranslationMap.find(sStringIdentifier);
		if (iIter != m_TranslationMap.end()) {
			sValue = iIter->second;
			return true;
		}

		if (m_pParentLanguage.get() != nullptr)
			return m_pParentLanguage->findTranslation(sStringIdentifier, sValue);

		return false;
	}

	std::string CLanguageDefinition::getTranslatedString(const std::string& sStringIdentifier, const std::string& sFallbackValue)
	{
		if (sStringIdentifier.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYLANGUAGESTRINGIDENTIFIER);

		if (!AMCCommon::CUtils::stringIsValidLanguageStringIdentifier(sStringIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER, sStringIdentifier);

		std::string sValue;
		if (findTranslation(sStringIdentifier, sValue))
			return sValue;

		{
			std::lock_guard<std::mutex> lockGuard(m_TranslationMissesMutex);
			m_TranslationMisses.insert(sStringIdentifier);
		}

		if (sFallbackValue.empty())
			return sStringIdentifier;

		return sFallbackValue;
	}

	std::set<std::string> CLanguageDefinition::getTranslationMisses()
	{
		std::lock_guard<std::mutex> lockGuard(m_TranslationMissesMutex);
		return m_TranslationMisses;
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

		if (!AMCCommon::CUtils::stringIsValidLanguageStringIdentifier(sStringIdentifier))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDLANGUAGESTRINGIDENTIFIER, sStringIdentifier);

		m_TranslationMap.insert(std::make_pair (sStringIdentifier, sValue));
	}


}


