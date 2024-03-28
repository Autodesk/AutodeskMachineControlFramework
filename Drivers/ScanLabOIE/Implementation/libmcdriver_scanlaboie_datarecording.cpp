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


Abstract: This is a stub class definition of CDataRecording

*/

#include "libmcdriver_scanlaboie_datarecording.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

// Include custom headers here.

using namespace LibMCDriver_ScanLabOIE::Impl;

/*************************************************************************************************************************
 Class definition of CDataRecording 
**************************************************************************************************************************/

CDataRecording::CDataRecording(PDataRecordingInstance pDataRecordingInstance)
	: m_pDataRecordingInstance (pDataRecordingInstance)
{
	if (pDataRecordingInstance.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);
}

CDataRecording::~CDataRecording()
{

}


LibMCDriver_ScanLabOIE_uint32 CDataRecording::GetRTCSignalCount()
{
	return m_pDataRecordingInstance->getRTCValuesPerRecord();
}

LibMCDriver_ScanLabOIE_uint32 CDataRecording::GetSensorSignalCount()
{
	return m_pDataRecordingInstance->getSensorValuesPerRecord();
}

LibMCDriver_ScanLabOIE_uint32 CDataRecording::GetAdditionalSignalCount()
{
	return m_pDataRecordingInstance->getAdditionalValuesPerRecord();
}


LibMCDriver_ScanLabOIE_uint64 CDataRecording::GetRecordCount()
{
	return m_pDataRecordingInstance->getRecordCount();
}

void CDataRecording::GetRecordInformation(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint32 & nPacketNumber, LibMCDriver_ScanLabOIE_double & dX, LibMCDriver_ScanLabOIE_double & dY)
{
	auto pRecord = m_pDataRecordingInstance->getRecord(nIndex);
	nPacketNumber = pRecord->m_nPacketNumber;
	dX = pRecord->m_dX;
	dY = pRecord->m_dY;
}

LibMCDriver_ScanLabOIE_uint32 CDataRecording::GetMeasurementTag(const LibMCDriver_ScanLabOIE_uint32 nIndex)
{
	auto pRecord = m_pDataRecordingInstance->getRecord(nIndex);
	return pRecord->m_nMeasurementTag;

}


void CDataRecording::GetRTCSignalsOfRecord(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint64 nRTCSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pRTCSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pRTCSignalsBuffer)
{
	if (nIndex >= m_pDataRecordingInstance->getRecordCount())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRECORDINDEX);

	uint32_t nRTCValueCount = m_pDataRecordingInstance->getRTCValuesPerRecord();

	if (pRTCSignalsNeededCount != nullptr)
		*pRTCSignalsNeededCount = nRTCValueCount;

	if (pRTCSignalsBuffer != nullptr) {
		if (nRTCSignalsBufferSize < (size_t) nRTCValueCount)
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

		m_pDataRecordingInstance->copyRTCSignals (nIndex, pRTCSignalsBuffer, nRTCSignalsBufferSize);
	}

}

void CDataRecording::GetSensorSignalsOfRecord(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint64 nSensorSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSensorSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pSensorSignalsBuffer)
{
	if (nIndex >= m_pDataRecordingInstance->getRecordCount())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRECORDINDEX);

	uint32_t nSensorValueCount = m_pDataRecordingInstance->getSensorValuesPerRecord();

	if (pSensorSignalsNeededCount != nullptr)
		*pSensorSignalsNeededCount = nSensorValueCount;

	if (pSensorSignalsBuffer != nullptr) {
		if (nSensorSignalsBufferSize < (size_t)nSensorValueCount)
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

		m_pDataRecordingInstance->copySensorSignals(nIndex, pSensorSignalsBuffer, nSensorSignalsBufferSize);
	}

}

void CDataRecording::GetAdditionalSignalsOfRecord(const LibMCDriver_ScanLabOIE_uint32 nIndex, LibMCDriver_ScanLabOIE_uint64 nAdditionalSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pAdditionalSignalsNeededCount, LibMCDriver_ScanLabOIE_int32* pAdditionalSignalsBuffer) 
{
	if (nIndex >= m_pDataRecordingInstance->getRecordCount())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRECORDINDEX);

	uint32_t nAdditionalValueCount = m_pDataRecordingInstance->getAdditionalValuesPerRecord();

	if (pAdditionalSignalsNeededCount != nullptr)
		*pAdditionalSignalsNeededCount = nAdditionalValueCount;

	if (pAdditionalSignalsBuffer != nullptr) {
		if (nAdditionalSignalsBufferSize < (size_t)nAdditionalValueCount)
			throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

		m_pDataRecordingInstance->copyAdditionalSignals(nIndex, pAdditionalSignalsBuffer, nAdditionalSignalsBufferSize);
	}

}



void CDataRecording::GetAllCoordinates(LibMCDriver_ScanLabOIE_uint64 nXArrayBufferSize, LibMCDriver_ScanLabOIE_uint64* pXArrayNeededCount, LibMCDriver_ScanLabOIE_double * pXArrayBuffer, LibMCDriver_ScanLabOIE_uint64 nYArrayBufferSize, LibMCDriver_ScanLabOIE_uint64* pYArrayNeededCount, LibMCDriver_ScanLabOIE_double * pYArrayBuffer)
{
	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (pXArrayNeededCount != nullptr)
		*pXArrayNeededCount = nRecordCount;
	if (pYArrayNeededCount != nullptr)
		*pYArrayNeededCount = nRecordCount;

	if (pXArrayBuffer != nullptr)
		m_pDataRecordingInstance->copyXCoordinates(pXArrayBuffer, nXArrayBufferSize);
	if (pYArrayBuffer != nullptr)
		m_pDataRecordingInstance->copyYCoordinates(pYArrayBuffer, nYArrayBufferSize);

}

void CDataRecording::GetAllPacketNumbers(LibMCDriver_ScanLabOIE_uint64 nPacketNumersBufferSize, LibMCDriver_ScanLabOIE_uint64* pPacketNumersNeededCount, LibMCDriver_ScanLabOIE_uint32 * pPacketNumersBuffer)
{
	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (pPacketNumersNeededCount != nullptr)
		*pPacketNumersNeededCount = nRecordCount;

	if (pPacketNumersBuffer != nullptr)
		m_pDataRecordingInstance->copyPacketNumbers(pPacketNumersBuffer, nPacketNumersBufferSize);
}

void CDataRecording::GetAllMeasurementTags(LibMCDriver_ScanLabOIE_uint64 nMeasurementTagsBufferSize, LibMCDriver_ScanLabOIE_uint64* pMeasurementTagsNeededCount, LibMCDriver_ScanLabOIE_uint32* pMeasurementTagsBuffer)
{
	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (pMeasurementTagsNeededCount != nullptr)
		*pMeasurementTagsNeededCount = nRecordCount;

	if (pMeasurementTagsBuffer != nullptr)
		m_pDataRecordingInstance->copyMeasurementTags(pMeasurementTagsBuffer, nMeasurementTagsBufferSize);

}

void CDataRecording::GetAllRTCSignals(const LibMCDriver_ScanLabOIE_uint32 nRTCIndex, LibMCDriver_ScanLabOIE_uint64 nSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pSignalsBuffer)
{
	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (pSignalsNeededCount != nullptr)
		*pSignalsNeededCount = nRecordCount;

	if (pSignalsBuffer != nullptr)
		m_pDataRecordingInstance->copyAllRTCSignalsByIndex(nRTCIndex, pSignalsBuffer, nSignalsBufferSize);
}

void CDataRecording::GetAllSensorSignals(const LibMCDriver_ScanLabOIE_uint32 nSignalIndex, LibMCDriver_ScanLabOIE_uint64 nSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalsNeededCount, LibMCDriver_ScanLabOIE_int32 * pSignalsBuffer)
{
	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (pSignalsNeededCount != nullptr)
		*pSignalsNeededCount = nRecordCount;

	if (pSignalsBuffer != nullptr)
		m_pDataRecordingInstance->copyAllSensorSignalsByIndex(nSignalIndex, pSignalsBuffer, nSignalsBufferSize);
}

void CDataRecording::GetAllAdditionalSignals(const LibMCDriver_ScanLabOIE_uint32 nAdditionalIndex, LibMCDriver_ScanLabOIE_uint64 nSignalsBufferSize, LibMCDriver_ScanLabOIE_uint64* pSignalsNeededCount, LibMCDriver_ScanLabOIE_int32* pSignalsBuffer)
{
	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (pSignalsNeededCount != nullptr)
		*pSignalsNeededCount = nRecordCount;

	if (pSignalsBuffer != nullptr)
		m_pDataRecordingInstance->copyAllAdditionalSignalsByIndex(nAdditionalIndex, pSignalsBuffer, nSignalsBufferSize);

}

void CDataRecording::AddPacketNumbersToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::Uint32Column);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<uint32_t> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyPacketNumbers(buffer.data (), buffer.size ());
		pDataTable->SetUint32ColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddXCoordinatesToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::DoubleColumn);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<double> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyXCoordinates(buffer.data(), buffer.size());
		pDataTable->SetDoubleColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddYCoordinatesToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::DoubleColumn);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<double> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyYCoordinates(buffer.data(), buffer.size());
		pDataTable->SetDoubleColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddMeasurementTagsToDataTable(LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::Uint32Column);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<uint32_t> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyMeasurementTags(buffer.data(), buffer.size());
		pDataTable->SetUint32ColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddRTCSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nRTCIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	if (nRTCIndex >= GetRTCSignalCount ())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRTCINDEX);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::Int32Column);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<int32_t> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyRTCSignals(nRTCIndex, buffer.data(), buffer.size());
		pDataTable->SetInt32ColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddSensorSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nSignalIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	if (nSignalIndex >= GetSensorSignalCount())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSIGNALINDEX);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::Int32Column);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<int32_t> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copySensorSignals(nSignalIndex, buffer.data(), buffer.size());
		pDataTable->SetInt32ColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddScaledSensorSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nSignalIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription, const LibMCDriver_ScanLabOIE_double dScaleFactor, const LibMCDriver_ScanLabOIE_double dOffset)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	if (nSignalIndex >= GetSensorSignalCount())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSIGNALINDEX);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::DoubleColumn);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<double> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyScaledSensorSignals(nSignalIndex, buffer.data(), buffer.size(), dScaleFactor, dOffset);
		pDataTable->SetDoubleColumnValues(sColumnIdentifier, buffer);
	}

}

void CDataRecording::AddAdditionalSignalsToDataTable(const LibMCDriver_ScanLabOIE_uint32 nAdditionalIndex, LibMCEnv::PDataTable pDataTable, const std::string& sColumnIdentifier, const std::string& sColumnDescription)
{
	if (pDataTable.get() == nullptr)
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

	if (nAdditionalIndex >= GetAdditionalSignalCount())
		throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDADDITIONALINDEX);

	pDataTable->AddColumn(sColumnIdentifier, sColumnDescription, LibMCEnv::eDataTableColumnType::Int32Column);

	size_t nRecordCount = m_pDataRecordingInstance->getRecordCount();
	if (nRecordCount > 0) {
		std::vector<int32_t> buffer;
		buffer.resize(nRecordCount);
		m_pDataRecordingInstance->copyAdditionalSignals(nAdditionalIndex, buffer.data(), buffer.size());
		pDataTable->SetInt32ColumnValues(sColumnIdentifier, buffer);
	}

}

