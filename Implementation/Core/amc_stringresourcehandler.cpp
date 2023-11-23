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


#include "amc_stringresourcehandler.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"


namespace AMC {
	
	CStringResourceHandler::CStringResourceHandler()
		: m_nIDCounter (1)
	{
	}

	CStringResourceHandler::~CStringResourceHandler()
	{
	}

	CStringResource CStringResourceHandler::registerString(const std::string& sResourceName, const std::string& sDefaultValue)
	{
		std::lock_guard<std::mutex> lockGuard (m_Mutex);

		if (sResourceName.empty ())
			throw ELibMCInterfaceException(LIBMC_ERROR_EMPTYSTRINGRESOURCENAME);

		auto iNameIter = m_ResourceNameMap.find(sResourceName);
		if (iNameIter != m_ResourceNameMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_DUPLICATESTRINGRESOURCENAME, sResourceName);

		if (!AMCCommon::CUtils::stringIsValidAlphanumericNameString(sResourceName))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSTRINGRESOURCENAMEIDENTIFIER, sResourceName);

		StringResourceID newID = m_nIDCounter;
		m_nIDCounter++;

		m_ResourceNameMap.insert(std::make_pair (sResourceName, newID));
		m_ResourceIDMap.insert(std::make_pair(newID, sResourceName));


		return CStringResource (shared_from_this (), newID);
			
	}

	std::string CStringResourceHandler::retrieveStringValue(const StringLanguageID nLanguageID, const StringResourceID nResourceID)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		std::string sString;

		auto iIter = m_pLanguageResourceTables.find(nLanguageID);
		if (iIter == m_pLanguageResourceTables.end()) {
			throw ELibMCCustomException(LIBMC_ERROR_LANGUAGEIDNOTFOUND, "language #" + std::to_string (nLanguageID));
		}

		if (!iIter->second->getEntry(nResourceID, sString)) {

			if (!m_pDefaultResourceTable->getEntry(nResourceID, sString)) {

				auto iResourceNameIter = m_ResourceIDMap.find(nResourceID);
				if (iResourceNameIter == m_ResourceIDMap.end())
					throw ELibMCInterfaceException(LIBMC_ERROR_STRINGRESOURCENOTFOUND);

				throw ELibMCCustomException(LIBMC_ERROR_STRINGRESOURCENOTFOUND, iResourceNameIter->second);
			}

		}

		return sString;
			
	};

}


