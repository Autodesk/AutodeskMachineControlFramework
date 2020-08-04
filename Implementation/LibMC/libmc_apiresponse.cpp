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

#include "libmc_apiresponse.hpp"
#include "libmc_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMC::Impl;

/*************************************************************************************************************************
 Class definition of CAPIResponse 
**************************************************************************************************************************/

CAPIResponse::CAPIResponse(AMC::PAPIResponse pResponse, uint32_t nErrorCode)
	: m_pResponse (pResponse), m_nErrorCode (nErrorCode)
{
	if (pResponse.get() == nullptr)
		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
}


LibMC_uint32 CAPIResponse::GetHTTPCode()
{
	return m_nErrorCode;
}

std::string CAPIResponse::GetContentType()
{
	if (m_pResponse.get() != nullptr)
		return m_pResponse->getContentType();

	return "";
}

void CAPIResponse::GetData(LibMC_uint64 nDataBufferSize, LibMC_uint64* pDataNeededCount, LibMC_uint8 * pDataBuffer)
{
	uint64_t nStreamSize = 0;
	if (m_pResponse.get() != nullptr)
		nStreamSize = (uint64_t)m_pResponse->getStreamSize();

	if (pDataNeededCount != nullptr) {
		*pDataNeededCount = nStreamSize;
	}

	if (pDataBuffer != nullptr) {

		if (nDataBufferSize < nStreamSize)
			throw ELibMCInterfaceException(LIBMC_ERROR_BUFFERTOOSMALL);

		if (m_pResponse.get() != nullptr) {
			const uint8_t * pSrc = m_pResponse->getStreamData();
			uint8_t * pDst = pDataBuffer;

			for (uint64_t nIndex = 0; nIndex < nStreamSize; nIndex++) {
				*pDst = *pSrc; pDst++; pSrc++;
			}
		}
	}
}

