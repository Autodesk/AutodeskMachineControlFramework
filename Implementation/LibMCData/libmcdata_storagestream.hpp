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


Abstract: This is the class declaration of CStorageStream

*/


#ifndef __LIBMCDATA_STORAGESTREAM
#define __LIBMCDATA_STORAGESTREAM

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "common_importstream.hpp"
#include "amcdata_storagepath.hpp"
#include "amcdata_sqlhandler.hpp"

namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CStorageStream 
**************************************************************************************************************************/

#define STORAGESTREAM_RECORDSIGNATURE 0x3AB82D1E1FF1982A

class CStorageStream;

typedef std::shared_ptr<CStorageStream> PStorageStream;

#pragma pack(push, 1)
typedef struct {
	uint64_t m_nStructSign;
	CStorageStream* m_pStream;	
} sStorageStreamCallbackData;
#pragma pack(pop) 


class CStorageStream : public virtual IStorageStream, public virtual CBase {
private:

	std::string m_sUUID;
	std::string m_sContextUUID;
	std::string m_sName;
	uint64_t m_nSize;
	std::string m_sMIMEType;
	std::string m_sSHA2;
	std::string m_sTimeStamp;
	std::string m_sUserID;
	AMCCommon::PImportStream m_pImportStream;		

	sStorageStreamCallbackData m_sStreamCallbackData;

protected:

	CStorageStream(AMCCommon::PImportStream pImportStream, const std::string& sUUID, const std::string& sContextUUID, const std::string& sName, const uint64_t nSize, const std::string& sMIMEType, const std::string& sSHA2, const std::string& sTimeStamp, const std::string& sUserID);

public:

	static CStorageStream* make (AMCCommon::PImportStream pImportStream, const std::string& sUUID, const std::string& sContextUUID, const std::string& sName, const uint64_t nSize, const std::string& sMIMEType, const std::string& sSHA2, const std::string& sTimeStamp, const std::string& sUserID);
	static CStorageStream* makeFromDatabase (const std::string& sStreamUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);
	static PStorageStream makeShared(AMCCommon::PImportStream pImportStream, const std::string& sUUID, const std::string& sContextUUID, const std::string& sName, const uint64_t nSize, const std::string& sMIMEType, const std::string& sSHA2, const std::string& sTimeStamp, const std::string& sUserID);
	static PStorageStream makeSharedFromDatabase(const std::string& sStreamUUID, AMCData::PSQLHandler pSQLHandler, AMCData::PStoragePath pStoragePath);

	~CStorageStream();

	std::string GetUUID() override;

	std::string GetContextUUID() override;

	std::string GetTimeStamp() override;

	std::string GetName() override;

	std::string GetMIMEType() override;

	std::string GetSHA2() override;

	LibMCData_uint64 GetSize() override;

	void GetContent(LibMCData_uint64 nContentBufferSize, LibMCData_uint64* pContentNeededCount, LibMCData_uint8 * pContentBuffer) override;

	void GetCallbacks(LibMCData_pvoid & pTheReadCallback, LibMCData_pvoid & pTheSeekCallback, LibMCData_pvoid & pStreamHandle) override;

	AMCCommon::CImportStream * getImportStream ();

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_STORAGESTREAM
