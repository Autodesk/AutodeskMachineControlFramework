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


Abstract: This is the class declaration of CBuildJob

*/


#ifndef __LIBMCDATA_BUILDJOB
#define __LIBMCDATA_BUILDJOB

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amcdata_storagepath.hpp"
#include "amcdata_sqlhandler.hpp"

namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CBuildJob 
**************************************************************************************************************************/

class CBuildJob;
typedef std::shared_ptr<CBuildJob> PBuildJob;

class CBuildJobData;
typedef std::shared_ptr<CBuildJobData> PBuildJobData;

class CBuildJob : public virtual IBuildJob, public virtual CBase {
private:

    std::string m_sUUID;
    std::string m_sName;
    LibMCData::eBuildJobStatus m_eJobStatus;
    std::string m_sTimeStamp;
    std::string m_sStorageStreamUUID;
    std::string m_sUserID;
    uint32_t m_nLayerCount;
    AMCData::PStoragePath m_pStoragePath;
    AMCData::PSQLHandler m_pSQLHandler;

    void ensureUpdate (const std::string sUpdateUUID, uint32_t nErrorCode);

    CBuildJobData * makeJobDataEx(AMCData::CSQLStatement* pStatement);
    IBuildJobDataIterator* listJobDataEx(AMCData::CSQLStatement * pStatement);

protected:

    CBuildJob(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, uint32_t nLayerCount, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);

public:

    static CBuildJob* make(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, uint32_t nLayerCount, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);
    static CBuildJob* makeFromDatabase(const std::string& sJobUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);
    static CBuildJob* makeFrom(CBuildJob* pBuildJob);

    static PBuildJob makeShared(const std::string& sUUID, const std::string sName, LibMCData::eBuildJobStatus eJobStatus, std::string sTimeStamp, std::string sStorageStreamUUID, std::string sUserID, uint32_t nLayerCount, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);
    static PBuildJob makeSharedFromDatabase(const std::string& sJobUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);
    static PBuildJob makeSharedFrom(CBuildJob* pBuildJob);

    std::string GetUUID() override;

	std::string GetName() override;

    LibMCData_uint32 GetLayerCount() override;

	LibMCData::eBuildJobStatus GetStatus() override;

	std::string GetTimeStamp() override;

	IStorageStream * GetStorageStream() override;

    std::string GetStorageStreamUUID() override;

    ILogSession * GetBuildJobLogger() override;

    void StartValidating() override;

    void FinishValidating(const LibMCData_uint32 nLayerCount) override;

    void ArchiveJob() override;

    void UnArchiveJob() override;

    void DeleteJob() override;
    
    bool JobCanBeArchived() override;

    void AddJobData(const std::string& sName, IStorageStream* pStream, const LibMCData::eBuildJobDataType eDataType, const std::string& sUserID) override;

    IBuildJobDataIterator* ListJobDataByType(const LibMCData::eBuildJobDataType eDataType) override;

    IBuildJobDataIterator* ListJobData() override;

    IBuildJobData* RetrieveJobData(const std::string& sDataUUID) override;

    static std::string convertBuildJobStatusToString(const LibMCData::eBuildJobStatus eStatus);
    static LibMCData::eBuildJobStatus convertStringToBuildJobStatus(const std::string& sValue);

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_BUILDJOB
