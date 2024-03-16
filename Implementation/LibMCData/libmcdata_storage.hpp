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


Abstract: This is the class declaration of CStorage

*/


#ifndef __LIBMCDATA_STORAGE
#define __LIBMCDATA_STORAGE

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
#include "amcdata_storagewriter.hpp"

#include <thread>
#include <mutex>
#include <map>
#include <set>

namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CStorage 
**************************************************************************************************************************/


class CStorage : public virtual IStorage, public virtual CBase {
private:
    AMCData::PStorageState m_pStorageState;
    AMCData::PSQLHandler m_pSQLHandler;

    void insertDBEntry(const std::string& sUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const LibMCData_uint64 nSize, const std::string& sSHA2, const std::string& sUserID);

protected:

    void FinishPartialStreamEx(const std::string& sUUID, const std::string& sSHA2, const std::string& sBlockwiseSHA2);

public:

    CStorage(AMCData::PSQLHandler pSQLHandler, AMCData::PStorageState pStorageState);

    bool StreamIsReady(const std::string& sUUID) override;

    IStorageStream* RetrieveStream(const std::string& sUUID) override;
    
    void StoreNewStream(const std::string& sUUID, const std::string& sContextUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const LibMCData_uint64 nContentBufferSize, const LibMCData_uint8* pContentBuffer, const std::string& sUserID) override;

    void BeginPartialStream(const std::string& sUUID, const std::string& sContextUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const LibMCData_uint64 nSize, const std::string& sUserID) override;

	void StorePartialStream(const std::string & sUUID, const LibMCData_uint64 nOffset, const LibMCData_uint64 nContentBufferSize, const LibMCData_uint8 * pContentBuffer) override;

	void FinishPartialStream(const std::string & sUUID, const std::string& sSHA2) override;

    void FinishPartialStreamBlockwiseSHA256(const std::string& sUUID, const std::string& sBlockwiseSHA2) override;

	void BeginRandomWriteStream(const std::string& sUUID, const std::string& sContextUUID, const std::string& sContextIdentifier, const std::string& sName, const std::string& sMimeType, const std::string& sUserID) override;

	void StoreRandomWriteStream(const std::string& sUUID, const LibMCData_uint64 nOffset, const LibMCData_uint64 nContentBufferSize, const LibMCData_uint8* pContentBuffer) override;

    LibMCData_uint64 GetRandomWriteStreamSize(const std::string& sUUID) override;

	void FinishRandomWriteStream(const std::string& sUUID) override;

    LibMCData_uint64 GetMaxStreamSize() override;

    bool ContentTypeIsAccepted(const std::string& sContentType) override;

    bool StreamIsImage(const std::string& sUUID) override;


    void CreateDownloadTicket(const std::string& sTicketUUID, const std::string& sStreamUUID, const std::string& sClientFileName, const std::string& sSessionUUID, const std::string& sUserUUID) override;

    void RequestDownloadTicket(const std::string& sTicketUUID, const std::string& sIPAddress, std::string& sStreamUUID, std::string& sClientFileName, std::string& sSessionUUID, std::string& sUserUUID) override;

    void AttachStreamToJournal(const std::string& sStreamUUID, const std::string& sJournalUUID) override;
};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_STORAGE
