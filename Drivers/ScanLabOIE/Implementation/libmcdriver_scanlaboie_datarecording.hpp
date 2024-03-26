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


Abstract: This is the class declaration of CDataRecording

*/


#ifndef __LIBMCDRIVER_SCANLABOIE_DATARECORDING
#define __LIBMCDRIVER_SCANLABOIE_DATARECORDING

#include "libmcdriver_scanlaboie_interfaces.hpp"

// Parent classes
#include "libmcdriver_scanlaboie_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_scanlaboie_datarecordinginstance.hpp"

namespace LibMCDriver_ScanLabOIE {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDataRecording 
**************************************************************************************************************************/

class CDataRecording : public virtual IDataRecording, public virtual CBase {
private:

	PDataRecordingInstance m_pDataRecordingInstance;

public:
	CDataRecording (PDataRecordingInstance pDataRecordingInstance);

	virtual ~CDataRecording();

	LibMCDriver_ScanLabOIE_uint32 GetRTCSignalCount() override;

	LibMCDriver_ScanLabOIE_uint32 GetSensorSignalCount() override;

	LibMCDriver_ScanLabOIE_uint32 GetAdditionalSignalCount() override;

	LibMCDriver_ScanLabOIE_uint64 GetRecordCount() override;

	void GetRecordInformation(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32 & nPacketNumber, LibMCDriver_ScanLabOIE_double & dX, LibMCDriver_ScanLabOIE_double & dY) override;

	LibMCDriver_ScanLabOIE_uint32 GetMeasurementTag(const LibMCDriver_ScanLabOIE_uint32 nIndex) override;

	void GetRTCSignalsOfRecord(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint64 nRTCSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pRTCSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pRTCSignalsBuffer) override;

	void GetSensorSignalsOfRecord(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint64 nSensorSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSensorSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pSensorSignalsBuffer) override;

	void GetAdditionalSignalsOfRecord(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint64 nAdditionalSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pAdditionalSignalsNeededCount, LibMCDriver_ScanLabOIE_int32* pAdditionalSignalsBuffer) override;

	void GetAllCoordinates(LibMCDriver_ScanLabOIE_uint64 nXArrayBufferSize, LibMCDriver_ScanLabOIE_uint64* pXArrayNeededCount, LibMCDriver_ScanLabOIE_double * pXArrayBuffer, LibMCDriver_ScanLabOIE_uint64 nYArrayBufferSize, LibMCDriver_ScanLabOIE_uint64* pYArrayNeededCount, LibMCDriver_ScanLabOIE_double * pYArrayBuffer) override;

	void GetAllPacketNumbers(LibMCDriver_ScanLabOIE_uint64 nPacketNumersBufferSize, LibMCDriver_ScanLabOIE_uint64* pPacketNumersNeededCount, LibMCDriver_ScanLabOIE_uint32 * pPacketNumersBuffer) override;

	void GetAllMeasurementTags(LibMCDriver_ScanLabOIE_uint64 nMeasurementTagsBufferSize, LibMCDriver_ScanLabOIE_uint64* pMeasurementTagsNeededCount, LibMCDriver_ScanLabOIE_uint32* pMeasurementTagsBuffer) override;

	void GetAllRTCSignals(const LibMCDriver_ScanLabOIE_uint32 nRTCIndex, LibMCDriver_ScanLabOIE_uint64 nSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pSignalsBuffer) override;

	void GetAllSensorSignals(const LibMCDriver_ScanLabOIE_uint32 nSignalIndex, LibMCDriver_ScanLabOIE_uint64 nSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pSignalsBuffer) override;

	void GetAllAdditionalSignals(const LibMCDriver_ScanLabOIE_uint32 nAdditionalIndex, LibMCDriver_ScanLabOIE_uint64 nSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalsNeededCount, LibMCDriver_ScanLabOIE_int32* pSignalsBuffer) override;

	void AddPacketNumbersToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

	void AddXCoordinatesToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

	void AddYCoordinatesToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

	void AddMeasurementTagsToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

	void AddRTCSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nRTCIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

	void AddSensorSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nSignalIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

	void AddAdditionalSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nAdditionalIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription) override;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLabOIE

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABOIE_DATARECORDING
