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


Abstract: This is the class declaration of CTempStreamWriter

*/


#ifndef __LIBMCENV_TEMPSTREAMWRITER
#define __LIBMCENV_TEMPSTREAMWRITER

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"
#include <mutex>
#include "common_chrono.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CTempStreamWriter 
**************************************************************************************************************************/

class CTempStreamWriter : public virtual ITempStreamWriter, public virtual CBase {
private:

	std::string m_sUUID;
	LibMCData::PDataModel m_pDataModel;
	LibMCData::PStorage m_pStorage;
	std::mutex m_StorageMutex;

	uint64_t m_nWritePosition;

	std::string m_sName;
	std::string m_sMIMEType;
	bool m_bIsFinished;

	AMCCommon::PChrono m_pGlobalChrono;

public:

	CTempStreamWriter(LibMCData::PDataModel pDataModel, const std::string & sName, const std::string & sMIMEType, const std::string & sCurrentUserID, AMCCommon::PChrono pGlobalChrono);

	virtual ~CTempStreamWriter();

	std::string GetUUID() override;

	std::string GetName() override;

	std::string GetMIMEType() override;

	LibMCEnv_uint64 GetSize() override;

	LibMCEnv_uint64 GetWritePosition() override;

	void Seek(const LibMCEnv_uint64 nWritePosition) override;

	bool IsFinished() override;

	void WriteData(const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8 * pDataBuffer) override;

	void WriteString(const std::string & sData) override;

	void WriteLine(const std::string & sLine) override;

	void Finish() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_TEMPSTREAMWRITER
