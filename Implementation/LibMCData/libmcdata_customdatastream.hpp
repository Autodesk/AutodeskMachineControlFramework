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


Abstract: This is the class declaration of CCustomDataStream

*/


#ifndef __LIBMCDATA_CUSTOMDATASTREAM
#define __LIBMCDATA_CUSTOMDATASTREAM

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CCustomDataStream 
**************************************************************************************************************************/

class CCustomDataStream : public virtual ICustomDataStream, public virtual CBase {
private:

	std::string m_sDataUUID;
	std::string m_sIdentifier;
	std::string m_sName;
	LibMCData::eCustomDataType m_eDataType;
	std::string m_sTimeStamp;
	std::string m_sStorageStreamUUID;
	std::string m_sUserUUID;
	std::string m_sSHA256;
	uint64_t m_nStreamSize;

public:

	CCustomDataStream(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, LibMCData::eCustomDataType eDataType, const std::string& sTimeStamp, const std::string& sStorageStreamUUID, const std::string& sUserUUID, const  std::string& sSHA2, uint64_t nStreamSize);

	virtual ~CCustomDataStream();

	std::string GetDataUUID() override;

	std::string GetName() override;

	std::string GetIdentifier() override;

	std::string GetTimeStamp() override;

	std::string GetStorageStreamUUID() override;

	std::string GetStorageStreamSHA2() override;

	LibMCData_uint64 GetStorageStreamSize() override;

	std::string GetUserUUID() override;

	LibMCData::eCustomDataType GetDataType() override;

	std::string GetDataTypeAsString() override;

	static std::string convertCustomDataTypeToString(const LibMCData::eCustomDataType dataType);
	static LibMCData::eCustomDataType convertStringToCustomDataType(const std::string& sValue);

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_CUSTOMDATASTREAM
