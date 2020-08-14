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


Abstract: This is a stub class definition of CAPIRequestHandler

*/

#include "libmc_apirequesthandler.hpp"
#include "libmc_interfaceexception.hpp"
#include "API/amc_api_constants.hpp"

// Include custom headers here.


using namespace LibMC::Impl;

/*************************************************************************************************************************
 Class definition of CAPIRequestHandler 
**************************************************************************************************************************/

CAPIRequestHandler::CAPIRequestHandler(AMC::PAPI pAPI, const std::string& sURI, const std::string& sRequestMethod, AMC::PAPIAuth pAuth)
    : m_RequestType(AMC::eAPIRequestType::rtUnknown), m_pAPI (pAPI), m_pAuth (pAuth)
{
    if (pAPI.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pAuth.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    if (sURI.length() > 0) {
        if (sURI.substr(0, 1) == "/")
            m_sURIWithoutLeadingSlash = sURI.substr(1);
        else
            m_sURIWithoutLeadingSlash = sURI;
    }

    if (sRequestMethod == "GET")
        m_RequestType = AMC::eAPIRequestType::rtGet;
    if (sRequestMethod == "POST")
        m_RequestType = AMC::eAPIRequestType::rtPost;

    if (m_RequestType == AMC::eAPIRequestType::rtUnknown)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDAPIREQUESTTYPE);

}




bool CAPIRequestHandler::ExpectsRawBody()
{
    return m_pAPI->expectsRawBody (m_sURIWithoutLeadingSlash, m_RequestType);
}

bool CAPIRequestHandler::ExpectsFormData(LibMC_uint32 & nFieldCount)
{
    nFieldCount = m_pAPI->getFormDataFieldCount(m_sURIWithoutLeadingSlash, m_RequestType);
    return (nFieldCount > 0);
}

void CAPIRequestHandler::GetFormDataDetails(const LibMC_uint32 nFieldIndex, std::string & sName, bool& bIsFile, bool & bMandatory)
{
    auto fieldDetails = m_pAPI->getFormDataFieldDetails(m_sURIWithoutLeadingSlash, m_RequestType, nFieldIndex);
    sName = fieldDetails.m_sFieldName;
    bIsFile = fieldDetails.m_bIsFileData;
    bMandatory = fieldDetails.m_bIsMandatory;
}


void CAPIRequestHandler::SetFormDataField(const std::string & sName, const LibMC_uint64 nBodyBufferSize, const LibMC_uint8 * pBodyBuffer)
{
    auto pData = std::make_shared<std::vector<uint8_t>> ();

    if (pBodyBuffer != nullptr) {
        pData->resize(nBodyBufferSize);
        if (nBodyBufferSize > 0) {
            auto pSrc = pBodyBuffer;
            auto pDst = pData->data();
            for (LibMC_uint64 nIndex = 0; nIndex < nBodyBufferSize; nIndex++) {
                *pDst = *pSrc;
                pDst++;
                pSrc++;
            }
        }
    }
    else {
        if (nBodyBufferSize > 0)
            throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    }

    m_FormFields.addDataField (sName, pData);
}

void CAPIRequestHandler::SetFormStringField(const std::string& sName, const std::string& sString)
{
    m_FormFields.addStringField(sName, sString);
}


void CAPIRequestHandler::Handle(const LibMC_uint64 nRawBodyBufferSize, const LibMC_uint8* pRawBodyBuffer, std::string& sContentType, LibMC_uint32& nHTTPCode)
{

    if (m_pResponse.get() != nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_APIREQUESTALREADYHANDLED);

    m_pResponse = m_pAPI->handleRequest(m_sURIWithoutLeadingSlash, m_RequestType, pRawBodyBuffer, nRawBodyBufferSize, m_FormFields, m_pAuth);

    if (m_pResponse.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INTERNALERROR);

    sContentType = m_pResponse->getContentType();

    nHTTPCode = m_pResponse->getHTTPCode ();
}

void CAPIRequestHandler::GetResultData(LibMC_uint64 nDataBufferSize, LibMC_uint64* pDataNeededCount, LibMC_uint8 * pDataBuffer)
{

    if (m_pResponse.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_APIREQUESTNOTHANDLED);

	uint64_t nStreamSize = (uint64_t)m_pResponse->getStreamSize();

	if (pDataNeededCount != nullptr) {
		*pDataNeededCount = nStreamSize;
	}

	if (pDataBuffer != nullptr) {

		if (nDataBufferSize < nStreamSize)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

		if (m_pResponse.get() != nullptr) {
			const uint8_t* pSrc = m_pResponse->getStreamData();
			uint8_t* pDst = pDataBuffer;

			for (uint64_t nIndex = 0; nIndex < nStreamSize; nIndex++) {
				*pDst = *pSrc; pDst++; pSrc++;
			}
		}
	}

}

