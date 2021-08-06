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


Abstract: This is the class declaration of CDataStream

*/


#ifndef __LIBAMCF_DATASTREAM
#define __LIBAMCF_DATASTREAM

#include "libamcf_interfaces.hpp"

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
 Class declaration of CDataStream 
**************************************************************************************************************************/

class CDataStream : public virtual IDataStream, public virtual CBase {
private:


protected:

    std::string m_sUUID;
    std::string m_sContextUUID;
    std::string m_sName;
    std::string m_sMimeType;
    std::string m_sTimestamp;
    std::string m_sSHA256;
    uint64_t m_nSize;

public:

    CDataStream (const std::string & sUUID, const std::string & sContextUUID, const std::string & sName, const std::string & sMimeType, const std::string & sSHA256, uint64_t nSize, const std::string & sTimestamp);

	std::string GetUUID() override;

	std::string GetContextUUID() override;

	std::string GetName() override;

	std::string GetMimeType() override;

    std::string GetSHA256() override;

	LibAMCF_uint64 GetSize() override;

    std::string GetTimestamp() override;

};

} // namespace Impl
} // namespace LibAMCF

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBAMCF_DATASTREAM
