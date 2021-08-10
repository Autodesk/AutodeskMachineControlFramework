/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is the class declaration of CStreamUpload

*/


#ifndef __LIBAMCF_STREAMUPLOAD
#define __LIBAMCF_STREAMUPLOAD

#include "libamcf_interfaces.hpp"
#include "libamcf_connectionstate.hpp"

// Parent classes
#include "libamcf_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibAMCF {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CStreamUpload 
**************************************************************************************************************************/

class CStreamUpload : public virtual IStreamUpload, public virtual CBase {
private:
    std::string m_sStreamUUID;
    std::string m_sContextUUID;
    std::string m_sCalculatedSHA256;
    std::string m_sUploadTimestamp;

    std::string m_sName;
    std::string m_sMimeType;
    LibAMCF::eStreamContextType m_StreamContext;

    PConnectionState m_pConnectionState;

    uint64_t m_nTotalUploadSize;
    uint64_t m_nCurrentUploadSize;

    std::mutex m_UploadResultMutex;
    uint64_t m_nFinishedBytes;

    uint64_t m_nHashBlockSize;
    std::vector<std::string> m_HashBlockSHA256Sums;

    bool m_bUploadFinished;

public:

    CStreamUpload(PConnectionState pConnectionState, const std::string& sName, const std::string& sMimeType, const LibAMCF::eStreamContextType StreamContext);

	std::string GetName() override;

	std::string GetMimeType() override;

    LibAMCF::eStreamContextType GetContextType() override;

    IOperationResult* UploadData(const LibAMCF_uint64 nDataBufferSize, const LibAMCF_uint8* pDataBuffer, const LibAMCF_uint32 nChunkSize, const LibAMCF_uint32 nThreadCount) override;

    IOperationResult* UploadFile(const std::string& sFileName, const LibAMCF_uint32 nChunkSize, const LibAMCF_uint32 nThreadCount) override;

    IOperationResult* BeginChunking(const LibAMCF_uint64 nDataSize) override;

	IOperationResult * UploadChunk(const LibAMCF_uint64 nDataBufferSize, const LibAMCF_uint8 * pDataBuffer) override;

	IOperationResult * FinishChunking() override;

    void GetStatus(LibAMCF_uint64& nUploadSize, LibAMCF_uint64& nFinishedSize, LibAMCF_uint64& nInProgressSize, bool& bFinished) override;

	IDataStream * GetDataStream() override;

};

} // namespace Impl
} // namespace LibAMCF

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBAMCF_STREAMUPLOAD
