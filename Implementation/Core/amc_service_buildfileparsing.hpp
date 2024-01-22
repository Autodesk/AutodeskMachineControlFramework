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


#ifndef __AMC_SERVICE_BUILDFILEPARSING
#define __AMC_SERVICE_BUILDFILEPARSING

#include "amc_servicehandler.hpp"
#include "amc_service.hpp"

#include "libmcdata_dynamic.hpp"
#include "lib3mf/lib3mf_dynamic.hpp"

#include <memory>

namespace AMC {


	class CService_BuildFileParsing : public CService {
	private:
		LibMCData::PDataModel m_pDataModel;
		std::string m_sBuildJobUUID;
		
		Lib3MF::PWrapper m_p3MFWrapper;
		Lib3MF::PModel m_p3MFModel;
		std::string m_sUserID;

	protected:
	public:

		CService_BuildFileParsing(CServiceHandler* pServiceHandler, LibMCData::PDataModel pDataModel, const std::string & sBuildJobUUID, Lib3MF::PWrapper p3MFWrapper, const std::string & sUserID);
		virtual ~CService_BuildFileParsing();

		virtual void executeBlocking() override;

		virtual std::string getName() override;
	};

	
}


#endif //__AMC_SERVICE_BUILDFILEPARSING

