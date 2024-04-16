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


Abstract: This is the class declaration of CBuildJobExecution

*/


#ifndef __LIBMCDATA_BUILDJOBEXECUTION
#define __LIBMCDATA_BUILDJOBEXECUTION

#include "libmcdata_interfaces.hpp"
#include "libmcdata_buildjobexecutiondata.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amcdata_sqlhandler.hpp"
#include "amcdata_storagestate.hpp"

namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CBuildJobExecution 
**************************************************************************************************************************/

class CBuildJobExecution : public virtual IBuildJobExecution, public virtual CBase {
private:

	AMCData::PSQLHandler m_pSQLHandler;
	std::string m_sExecutionUUID;
	std::string m_sJobUUID;
	std::string m_sUserUUID;
	std::string m_sJournalUUID;
	uint64_t m_nStartJournalTimestamp;
	AMCData::PStorageState m_pStorageState;

	CBuildJobExecutionData* makeJobExecutionDataEx(AMCData::CSQLStatement* pStatement);
	IBuildJobExecutionDataIterator* listJobExecutionDataEx(AMCData::CSQLStatement* pStatement);


public:

	static CBuildJobExecution* makeFrom(CBuildJobExecution * pBuildJobExecution);

	static std::shared_ptr<CBuildJobExecution> makeSharedFrom(CBuildJobExecution* pBuildJobExecution);

	CBuildJobExecution(AMCData::PSQLHandler pSQLHandler, const std::string & sExecutionUUID, AMCData::PStorageState pStorageState);

	virtual ~CBuildJobExecution();

	std::string GetExecutionUUID() override;

	std::string GetJobUUID() override;

	LibMCData::eBuildJobExecutionStatus GetStatus() override;

	void ChangeStatus(const LibMCData::eBuildJobExecutionStatus eNewExecutionStatus, const LibMCData_uint64 nAbsoluteEndTimeStampInMicrosecondsSince1970) override;

	std::string GetDescription() override;

	void SetDescription(const std::string & sNewDescription) override;

	std::string GetJournalUUID() override;

	std::string GetUserUUID() override;

	LibMCData_uint64 GetStartTimeStampInMicroseconds() override;

	LibMCData_uint64 GetEndTimeStampInMicroseconds() override;

	LibMCData_uint64 ComputeElapsedTimeInMicroseconds(const LibMCData_uint64 nGlobalTimerInMicroseconds) override;

	void AddJobExecutionData(const std::string& sIdentifier, const std::string& sName, IStorageStream* pStream, const LibMCData::eCustomDataType eDataType, const std::string& sUserUUID, const LibMCData_uint64 nAbsoluteTimeStamp) override;

	IBuildJobExecutionDataIterator* ListJobExecutionDataByType(const LibMCData::eCustomDataType eDataType) override;

	IBuildJobExecutionDataIterator* ListJobExecutionData() override;

	IBuildJobExecutionData* RetrieveJobExecutionData(const std::string& sDataUUID) override;

	IBuildJobExecutionData* RetrieveJobExecutionDataByIdentifier(const std::string& sIdentifier) override;

	bool HasJobExecutionDataUUID(const std::string& sUUID) override;

	bool HasJobExecutionDataIdentifier(const std::string& sIdentifier) override;

	void StoreMetaDataString(const std::string& sKey, const std::string& sValue, const LibMCData_uint64 nAbsoluteTimeStamp) override;

	bool HasMetaDataString(const std::string& sKey) override;

	std::string GetMetaDataString(const std::string& sKey) override;

	static std::string convertBuildJobExecutionStatusToString(const LibMCData::eBuildJobExecutionStatus eStatus);
	static LibMCData::eBuildJobExecutionStatus convertStringToBuildJobExecutionStatus(const std::string& sValue);


};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_BUILDJOBEXECUTION
