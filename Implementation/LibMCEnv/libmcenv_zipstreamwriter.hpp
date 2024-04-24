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


Abstract: This is the class declaration of CZIPStreamWriter

*/


#ifndef __LIBMCENV_ZIPSTREAMWRITER
#define __LIBMCENV_ZIPSTREAMWRITER

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"
#include "common_utils.hpp"
#include "common_chrono.hpp"

#include <mutex>

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CZIPStreamWriter 
**************************************************************************************************************************/


	class CZIPEntryStreamWriter : public virtual ITempStreamWriter, public virtual CBase {
	private:

		LibMCData::PStorageZIPWriter m_pZIPWriter;
		uint32_t m_nEntryID;
		std::string m_sUUID;
		std::string m_sName;

	public:

		CZIPEntryStreamWriter(LibMCData::PStorageZIPWriter pZIPWriter, uint32_t nEntryID, const std::string & sUUID, const std::string & sName);

		virtual ~CZIPEntryStreamWriter();

		std::string GetUUID() override;

		std::string GetName() override;

		std::string GetMIMEType() override;

		LibMCEnv_uint64 GetSize() override;

		LibMCEnv_uint64 GetWritePosition() override;

		void Seek(const LibMCEnv_uint64 nWritePosition) override;

		bool IsFinished() override;

		void WriteData(const LibMCEnv_uint64 nDataBufferSize, const LibMCEnv_uint8* pDataBuffer) override;

		void WriteString(const std::string& sData) override;

		void WriteLine(const std::string& sLine) override;

		void Finish() override;

		void CopyFrom(IStreamReader* pStreamReader) override;

	};


class CZIPStreamWriter : public virtual IZIPStreamWriter, public virtual CBase {
protected:
    LibMCData::PDataModel m_pDataModel;
	LibMCData::PStorageZIPWriter m_pZIPWriter;

	std::string m_sUUID;
	std::string m_sName;
	AMCCommon::PChrono m_pGlobalChrono;

public:

    CZIPStreamWriter (LibMCData::PDataModel pDataModel, LibMCData::PStorageZIPWriter pZIPWriter, const std::string & sUUID, const std::string& sName, AMCCommon::PChrono pGlobalChrono);

    virtual ~CZIPStreamWriter();

	ITempStreamWriter * CreateZIPEntry(const std::string & sFileName) override;

    void CreateZIPEntryFromStream(const std::string& sFileName, IStreamReader* pStreamReader) override;

	void Finish() override;

	std::string GetUUID() override;

	std::string GetName() override;

	std::string GetMIMEType() override;

	LibMCEnv_uint64 GetSize() override;

    bool IsFinished() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_ZIPSTREAMWRITER
