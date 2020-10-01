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


#include "amc_api_handler_root.hpp"

#include "libmc_interfaceexception.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include "Libraries/libzip/zip.h"
#include "pugixml.hpp"


using namespace AMC;



CAPIHandler_Root::CAPIHandler_Root()
{
	
}

CAPIHandler_Root::~CAPIHandler_Root()
{
	
}
				
std::string CAPIHandler_Root::getBaseURI () 
{
	return "";
}
		
void CAPIHandler_Root::checkAuthorizationMode(const std::string& sURI, const eAPIRequestType requestType, bool& bNeedsToBeAuthorized, bool& bCreateNewSession)
{
	bNeedsToBeAuthorized = false;
	bCreateNewSession = false;
}


PAPIResponse CAPIHandler_Root::handleRequest(const std::string& sURI, const eAPIRequestType requestType, CAPIFormFields & pFormFields, const uint8_t* pBodyData, const size_t nBodyDataSize, PAPIAuth pAuth)
{

	if (requestType == eAPIRequestType::rtGet) {
		auto iIterator = m_FilesToServe.find(sURI);
		if (iIterator != m_FilesToServe.end())
			return iIterator->second;
	}

	return nullptr;
}

void CAPIHandler_Root::LoadClientPackage(PResourcePackage pResourcePackage)
{
	if (pResourcePackage.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

	size_t nCount = pResourcePackage->getEntryCount();
	for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
		auto pEntry = pResourcePackage->getEntry(nIndex);

		auto apiResponse = std::make_shared<CAPIFixedBufferResponse>(pEntry->getContentType());
		pResourcePackage->readEntry(pEntry->getName (), apiResponse->getBuffer());
		m_FilesToServe.insert(std::make_pair(pEntry->getName (), apiResponse));
	}



}
		


