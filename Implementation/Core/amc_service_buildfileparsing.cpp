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


#include "amc_service_buildfileparsing.hpp"
#include "amc_servicehandler.hpp"

#include "common_utils.hpp"
#include "libmc_exceptiontypes.hpp"

#include "amc_systemstate.hpp"
#include "amc_toolpathentity.hpp"


namespace AMC {
	
	
	CService_BuildFileParsing::CService_BuildFileParsing(CServiceHandler* pServiceHandler, LibMCData::PDataModel pDataModel, const std::string& sBuildJobUUID, Lib3MF::PWrapper p3MFWrapper, const std::string& sUserID)
		: CService (pServiceHandler), m_pDataModel (pDataModel), m_p3MFWrapper (p3MFWrapper), m_sUserID (sUserID), m_sBuildJobUUID (AMCCommon::CUtils::normalizeUUIDString (sBuildJobUUID))
	{
		LibMCAssertNotNull(pDataModel.get());
		LibMCAssertNotNull(p3MFWrapper.get());

	}
	
	
	CService_BuildFileParsing::~CService_BuildFileParsing()
	{
		
	}

	
	void CService_BuildFileParsing::executeBlocking()
	{
		
		auto pBuildJobHandler = m_pDataModel->CreateBuildJobHandler();
		auto pBuildJob = pBuildJobHandler->RetrieveJob(m_sBuildJobUUID);
		auto pStorageStream = pBuildJob->GetStorageStream();
		
		// TODO: Check Toolpath Entity Integrity
		CToolpathEntity toolpathEntity (m_pDataModel, pStorageStream->GetUUID(), m_p3MFWrapper, pBuildJob->GetName ());		
		pBuildJob->FinishValidating (toolpathEntity.getLayerCount ());
		pBuildJob->AddJobData(pStorageStream->GetContextIdentifier (),  pStorageStream->GetName(), pStorageStream, LibMCData::eBuildJobDataType::Toolpath, m_sUserID);
		

	}
		

	std::string CService_BuildFileParsing::getName()
	{
		return "build file parsing";
	}
	

}


