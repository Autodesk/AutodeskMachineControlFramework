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


Abstract: This is the class declaration of CAPIRequestHandler

*/


#ifndef __LIBMC_APIREQUESTHANDLER
#define __LIBMC_APIREQUESTHANDLER

#include "libmc_interfaces.hpp"
#include "API/amc_api_handler.hpp"
#include "API/amc_api_response.hpp"
#include "API/amc_api.hpp"

#include "amc_logger.hpp"

// Parent classes
#include "libmc_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMC {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CAPIRequestHandler 
**************************************************************************************************************************/

class CAPIRequestHandler : public virtual IAPIRequestHandler, public virtual CBase {
private:


protected:

	AMC::PAPI m_pAPI;
	AMC::PAPIResponse m_pResponse;

	std::string m_sURIWithoutLeadingSlash;
	AMC::eAPIRequestType m_RequestType;

	AMC::CAPIFormFields m_FormFields;
	AMC::PAPIAuth m_pAuth;

	AMC::PLogger m_pLogger;

public:

	CAPIRequestHandler(AMC::PAPI pAPI, const std::string& sURI, const AMC::eAPIRequestType eRequestType, AMC::PAPIAuth pAuth, AMC::PLogger pLogger);

	bool ExpectsRawBody() override;

	bool ExpectsFormData(LibMC_uint32 & nFieldCount) override;

	void GetFormDataDetails(const LibMC_uint32 nFieldIndex, std::string & sName, bool& bIsFile, bool & bMandatory) override;

	void SetFormDataField(const std::string & sName, const LibMC_uint64 nBodyBufferSize, const LibMC_uint8 * pBodyBuffer) override;

	void SetFormStringField(const std::string& sName, const std::string& sString) override;

	void Handle(const LibMC_uint64 nRawBodyBufferSize, const LibMC_uint8* pRawBodyBuffer, std::string& sContentType, LibMC_uint32& nHTTPCode) override;

	void GetResultData(LibMC_uint64 nDataBufferSize, LibMC_uint64* pDataNeededCount, LibMC_uint8 * pDataBuffer) override;

	std::string GetContentDispositionName() override;
	
};

} // namespace Impl
} // namespace LibMC

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMC_APIREQUESTHANDLER
