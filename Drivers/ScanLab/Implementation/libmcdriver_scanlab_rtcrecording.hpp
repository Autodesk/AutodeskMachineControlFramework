/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CRTCRecording

*/


#ifndef __LIBMCDRIVER_SCANLAB_RTCRECORDING
#define __LIBMCDRIVER_SCANLAB_RTCRECORDING

#include "libmcdriver_scanlab_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlab_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlab_sdk.hpp"
#include "libmcdriver_scanlab_rtcrecordinginstance.hpp"
#include <map>

namespace LibMCDriver_ScanLab {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CRTCRecording 
**************************************************************************************************************************/

class CRTCRecording : public virtual IRTCRecording, public virtual CBase {
private:
	PRTCRecordingInstance m_pInstance;

public:

	CRTCRecording(PRTCRecordingInstance pInstance);

	virtual ~CRTCRecording();

	bool ScanheadConnectionCheckIsEnabled() override;

	void EnableScanheadConnectionCheck() override;

	void DisableScanheadConnectionCheck() override;

	void Clear() override;

	void AddChannel(const std::string & sChannelName, const LibMCDriver_ScanLab::eRTCChannelType eChannelType) override;

	void RemoveChannel(const std::string & sChannelName) override;

	bool HasChannel(const std::string & sChannelName) override;

	LibMCDriver_ScanLab::eRTCChannelType GetChannelType(const std::string & sChannelName) override;

	LibMCDriver_ScanLab_uint64 GetRecordCount(const std::string& sChannelName) override;

	LibMCDriver_ScanLab_int32 GetRecordEntry(const std::string & sChannelName, const LibMCDriver_ScanLab_uint64 nRecordIndex) override;

	void GetAllRecordEntries(const std::string & sChannelName, LibMCDriver_ScanLab_uint64 nValuesBufferSize, LibMCDriver_ScanLab_uint64* pValuesNeededCount, LibMCDriver_ScanLab_int32 * pValuesBuffer) override;

	void EnableRecording(const LibMCDriver_ScanLab::eRTCRecordingFrequency eFrequency) override;

	void DisableRecording() override;

	void ExecuteListWithRecording() override;

	void AddRecordsToDataTable(const std::string & sChannelName, LibMCEnv::PDataTable pDataTable, const std::string & sColumnIdentifier, const std::string & sColumnDescription) override;

	void AddScaledRecordsToDataTable(const std::string & sChannelName, LibMCEnv::PDataTable pDataTable, const std::string & sColumnIdentifier, const std::string & sColumnDescription, const LibMCDriver_ScanLab_double dScaleFactor, const LibMCDriver_ScanLab_double dOffset) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLAB_RTCRECORDING
