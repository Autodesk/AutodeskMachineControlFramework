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


Abstract: This is the class declaration of CBuildExecution

*/


#ifndef __LIBMCENV_BUILDEXECUTION
#define __LIBMCENV_BUILDEXECUTION

#include "libmcenv_interfaces.hpp"
#include "amc_toolpathhandler.hpp"
#include "amc_meshhandler.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"
#include "common_chrono.hpp"
#include <mutex>

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CBuildExecution 
**************************************************************************************************************************/

class CBuildExecution : public virtual IBuildExecution, public virtual CBase {
private:

	std::string m_sExecutionUUID;
	LibMCData::PBuildJobExecution m_pExecution;
	LibMCData::PDataModel m_pDataModel;
	AMC::PToolpathHandler m_pToolpathHandler;
	AMC::PMeshHandler m_pMeshHandler;
	AMCCommon::PChrono m_pGlobalChrono;
	std::mutex m_Mutex;

public:

	static CBuildExecution* makeFrom (CBuildExecution * pBuildExecution);

	static std::shared_ptr<CBuildExecution> makeSharedFrom(CBuildExecution* pBuildExecution);

	CBuildExecution(LibMCData::PBuildJobExecution pExecution, LibMCData::PDataModel pDataModel, AMC::PToolpathHandler pToolpathHandler, AMC::PMeshHandler pMeshHandler, AMCCommon::PChrono pGlobalChrono);

	virtual ~CBuildExecution();

	std::string GetUUID() override;

	std::string GetBuildUUID() override;

	IBuild * GetBuild() override;

	LibMCEnv::eBuildExecutionStatus GetExecutionStatus() override;

	bool IsInProcess() override;

	bool IsFinished() override;

	bool IsFailed() override;

	void SetStatusToFinished() override;

	void SetStatusToFailed() override;

	std::string GetDescription() override;

	void SetDescription(const std::string & sDescription) override;

	std::string GetJournalUUID() override;

	bool HasAttachedUser() override;

	std::string GetUserUUID() override;

	LibMCEnv_uint64 GetStartTimeStampInMilliseconds() override;

	LibMCEnv_uint64 GetStartTimeStampInMicroseconds() override;

	LibMCEnv_uint64 GetEndTimeStampInMilliseconds() override;

	LibMCEnv_uint64 GetEndTimeStampInMicroseconds() override;

	LibMCEnv_uint64 GetElapsedTimeInMilliseconds() override;

	LibMCEnv_uint64 GetElapsedTimeInMicroseconds() override;

	bool HasAttachment(const std::string& sDataUUID) override;

	bool HasAttachmentIdentifier(const std::string& sIdentifier) override;

	std::string AddBinaryData(const std::string& sIdentifier, const std::string& sName, const std::string& sMIMEType, const std::string& sUserUUID, const LibMCEnv_uint64 nContentBufferSize, const LibMCEnv_uint8* pContentBuffer) override;

	std::string AttachTempStream(const std::string& sIdentifier, const std::string& sName, const std::string& sUserUUID, IBaseTempStreamWriter* pStreamWriterInstance) override;

	IStreamReader* LoadStreamByIdentifier(const std::string& sIdentifier) override;

	IStreamReader* LoadStreamByUUID(const std::string& sDataUUID) override;

	IDiscreteFieldData2D * LoadDiscreteField2DByIdentifier(const std::string & sContextIdentifier) override;

	IDiscreteFieldData2D * LoadDiscreteField2DByUUID(const std::string & sDataUUID) override;

	std::string StoreDiscreteField2D(const std::string & sContextIdentifier, const std::string & sName, IDiscreteFieldData2D* pFieldDataInstance, IDiscreteFieldData2DStoreOptions* pStoreOptions, const std::string& sUserUUID) override;

	IDataTable* LoadDataTableByIdentifier(const std::string& sIdentifier) override;

	IDataTable* LoadDataTableByUUID(const std::string& sDataUUID) override;

	std::string StoreDataTable(const std::string& sIdentifier, const std::string& sName, IDataTable* pFieldDataInstance, IDataTableWriteOptions* pStoreOptions, const std::string& sUserUUID) override;

	IImageData * LoadPNGImageByIdentifier(const std::string& sContextIdentifier, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	IImageData * LoadPNGImageByUUID(const std::string & sDataUUID, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	std::string StorePNGImage(const std::string & sContextIdentifier, const std::string & sName, IImageData* pImageDataInstance, IPNGImageStoreOptions* pStoreOptions, const std::string& sUserUUID) override;

	void StoreMetaDataString(const std::string & sKey, const std::string & sValue) override;

	bool HasMetaDataString(const std::string & sKey) override;

	std::string GetMetaDataString(const std::string & sKey) override;

	IJournalHandler* LoadAttachedJournal() override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_BUILDEXECUTION
