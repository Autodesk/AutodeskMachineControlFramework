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


Abstract: This is the class declaration of CBuildJobData

*/


#ifndef __LIBMCDATA_BUILDJOBDATA
#define __LIBMCDATA_BUILDJOBDATA

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "amcdata_storagestate.hpp"
#include "amcdata_sqlhandler.hpp"
#include "libmcdata_customdatastream.hpp"


namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CBuildJobData 
**************************************************************************************************************************/

class CBuildJobData;

typedef std::shared_ptr<CBuildJobData> PBuildJobData;

class CBuildJobData : public virtual IBuildJobData, public virtual CCustomDataStream {
private:

    std::string m_sJobUUID;

    AMCData::PSQLHandler m_pSQLHandler;
    AMCData::PStorageState m_pStorageState;

protected:

    CBuildJobData(const std::string& sDataUUID, const std::string& sIdentifier, const std::string & sName, const std::string& sJobUUID, LibMCData::eCustomDataType eDataType, const std::string & sTimeStamp, const std::string & sStorageStreamUUID, const std::string & sUserID, const std::string & sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState);

public:

    ~CBuildJobData();

    static CBuildJobData* make(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eCustomDataType eDataType, const std::string& sTimeStamp, const std::string& sStorageStreamUUID, const std::string& sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState);
    static CBuildJobData* makeFrom(CBuildJobData* pBuildJobData);

    static PBuildJobData makeShared(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, const std::string& sJobUUID, LibMCData::eCustomDataType eDataType, const std::string& sTimeStamp, const std::string& sStorageStreamUUID, const std::string& sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState);
    static PBuildJobData makeSharedFrom(CBuildJobData* pBuildJobData);    

    static CBuildJobData* createInDatabase(const std::string& sIdentifier, const std::string sName, const std::string & sJobUUID, LibMCData::eCustomDataType eDataType, const std::string & sTimeStamp, const std::string & sStorageStreamUUID, const std::string & sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState);
    static PBuildJobData createSharedInDatabase(const std::string& sIdentifier, const std::string & sName,  const std::string & sJobUUID, LibMCData::eCustomDataType eDataType, const std::string & sTimeStamp, const std::string & sStorageStreamUUID, const std::string & sUserID, const std::string& sSHA2, uint64_t nStreamSize, AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState);

	IStorageStream * GetStorageStream() override;

    std::string GetJobUUID() override;

	std::string GetMIMEType() override;

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_BUILDJOBDATA
