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


Abstract: This is a stub class definition of CDeviceConfiguration

*/

#include "libmcdriver_scanlaboie_datarecordinginstance.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

#include <stdexcept>
using namespace LibMCDriver_ScanLabOIE::Impl;

#define DATARECORDING_MINBUFFERSIZEINVALUES 256
#define DATARECORDING_MINBUFFERSIZEINRECORDS 256
#define DATARECORDING_MAXBUFFERSIZEINRECORDS (1024 * 1024)


#include <iostream>
#include <cstring>

CDataRecordingBuffer::CDataRecordingBuffer(size_t nBufferSizeInValues)
    : m_nCurrentPosition(0)
{
    m_RawData.resize(nBufferSizeInValues);
}

CDataRecordingBuffer::~CDataRecordingBuffer()
{

}

bool CDataRecordingBuffer::hasSpace(size_t nValueCount)
{
    if (nValueCount == 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    return (m_nCurrentPosition + nValueCount) <= m_RawData.size();
}

int32_t* CDataRecordingBuffer::allocData(size_t nValueCount)
{
    if (nValueCount == 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);
    if ((m_nCurrentPosition + nValueCount) > m_RawData.size())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFEROVERFLOW);

    int32_t * pData = &m_RawData.at(m_nCurrentPosition);
    m_nCurrentPosition += nValueCount;

    return pData;

}


CDataRecordingInstance::CDataRecordingInstance(uint32_t nSensorValuesPerRecord, uint32_t nRTCValuesPerRecord, uint32_t nAdditionalValuesPerRecord, uint32_t nBufferSizeInRecords)
    : m_nBufferSizeInRecords (nBufferSizeInRecords)
{
    if (nSensorValuesPerRecord <= 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDVALUESPERRECORD);
    if (nRTCValuesPerRecord <= 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDVALUESPERRECORD);

    m_nValuesPerRecord = nSensorValuesPerRecord + nRTCValuesPerRecord + nAdditionalValuesPerRecord;
    m_nFirstSensorValueIndex = 0;
    m_nSensorValueCount = nSensorValuesPerRecord;
    m_nFirstRTCValueIndex = m_nFirstSensorValueIndex + m_nSensorValueCount;
    m_nRTCValueCount = nRTCValuesPerRecord;
    m_nFirstAdditionalValueIndex = m_nFirstRTCValueIndex + m_nRTCValueCount;
    m_nAdditionalValueCount = nAdditionalValuesPerRecord;

    if (nBufferSizeInRecords < DATARECORDING_MINBUFFERSIZEINRECORDS)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDBUFFERSIZE);
    if (nBufferSizeInRecords > DATARECORDING_MAXBUFFERSIZEINRECORDS)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDBUFFERSIZE);

    m_nValueCountPerBuffer = (size_t)m_nValuesPerRecord * (size_t)nBufferSizeInRecords;
    memset((void*)&m_CurrentEntry, 0, sizeof(m_CurrentEntry));
}

CDataRecordingInstance::~CDataRecordingInstance()
{

}

void CDataRecordingInstance::startRecord (uint32_t nPacketNumber, uint32_t nMeasurementTag, double dX, double dY)
{
    if (m_pCurrentBuffer.get() == nullptr) {
        m_pCurrentBuffer = std::make_shared<CDataRecordingBuffer>(m_nValueCountPerBuffer);
        m_Buffers.push_back(m_pCurrentBuffer);
    }
    
    m_CurrentEntry.m_dX = dX;
    m_CurrentEntry.m_dY = dY;
    m_CurrentEntry.m_nPacketNumber = nPacketNumber;
    m_CurrentEntry.m_nMeasurementTag = nMeasurementTag;
    m_CurrentEntry.m_pData = m_pCurrentBuffer->allocData (m_nValuesPerRecord);
    m_nCurrentEntryDataIndex = 0;

    // If buffer is full, alloc a new buffer the next iteration
    if (!m_pCurrentBuffer->hasSpace(m_nValuesPerRecord))
        m_pCurrentBuffer = nullptr;
}

void CDataRecordingInstance::recordValue(int32_t nValue)
{
    if (m_nCurrentEntryDataIndex >= m_nValuesPerRecord) 
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_TOOMANYVALUESINPACKET);
    if (m_CurrentEntry.m_pData == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_PACKETISNOTRECORDING);

    m_CurrentEntry.m_pData[m_nCurrentEntryDataIndex] = nValue;
    m_nCurrentEntryDataIndex++;

}

void CDataRecordingInstance::finishRecord()
{
    //std::cout << "finishing record: " << m_nCurrentEntryDataIndex << " of " << m_nValuesPerPacket << std::endl;

    if (m_nCurrentEntryDataIndex < m_nValuesPerRecord)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTENOUGHVALUESINPACKET);
    if (m_CurrentEntry.m_pData == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_PACKETISNOTRECORDING);

    size_t nNewIndex = 0;
    auto & newEntry = m_Entries.allocDataRef (nNewIndex);
    newEntry = m_CurrentEntry;

    m_CurrentEntry.m_dX = 0.0;
    m_CurrentEntry.m_dY = 0.0;
    m_CurrentEntry.m_nPacketNumber = 0;
    m_CurrentEntry.m_pData = nullptr;
}

size_t CDataRecordingInstance::getRecordCount()
{
    return m_Entries.getCount();
}


void CDataRecordingInstance::writeToFile(const std::string& sFileName)
{
    std::ofstream fStream;
    fStream.open (sFileName);
    if (!fStream.is_open())
        throw std::runtime_error ("could not write file");

    fStream << "packet number, X, Y, value 0, value 1, ...." << std::endl;

    size_t nCount = m_Entries.getCount();
    for (size_t nIndex = 0; nIndex < nCount; nIndex++) {
        auto & entry = m_Entries.getDataRef(nIndex);
        fStream << entry.m_nPacketNumber << ", " << entry.m_dX << ", " << entry.m_dY;
        for (uint32_t nIndex = 0; nIndex < m_nValuesPerRecord; nIndex++)
            fStream << ", " << entry.m_pData[nIndex];

        fStream << std::endl;

    }


    fStream.close();
}


uint32_t CDataRecordingInstance::getRTCValuesPerRecord()
{
    return m_nRTCValueCount;
}

uint32_t CDataRecordingInstance::getSensorValuesPerRecord()
{
    return m_nSensorValueCount;
}

uint32_t CDataRecordingInstance::getAdditionalValuesPerRecord()
{
    return m_nAdditionalValueCount;
}


uint32_t CDataRecordingInstance::getBufferSizeInRecords()
{
    return m_nBufferSizeInRecords;
}

sDataRecordingEntry* CDataRecordingInstance::getRecord(uint32_t nIndex)
{
    if (nIndex >= m_Entries.getCount())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRECORDINDEX);

    return m_Entries.getData(nIndex);
}

PDataRecordingInstance CDataRecordingInstance::createEmptyDuplicate()
{
    return std::make_shared<CDataRecordingInstance>(m_nSensorValueCount, m_nRTCValueCount, m_nAdditionalValueCount, m_nBufferSizeInRecords);
}

int32_t* CDataRecordingInstance::getRTCData(size_t nRecordIndex)
{
    if (m_nRTCValueCount == 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NORTCVALUESAVAILABLE);

    auto pRecord = m_Entries.getData(nRecordIndex);
    return &pRecord->m_pData[m_nFirstRTCValueIndex];
}

int32_t* CDataRecordingInstance::getSensorData(size_t nRecordIndex)
{
    if (m_nSensorValueCount == 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOSENSORVALUESAVAILABLE);

    auto pRecord = m_Entries.getData(nRecordIndex);
    return &pRecord->m_pData[m_nFirstSensorValueIndex];
}

int32_t* CDataRecordingInstance::getAdditionalData(size_t nRecordIndex)
{
    if (m_nAdditionalValueCount == 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOADDITIONALVALUESAVAILABLE);

    auto pRecord = m_Entries.getData(nRecordIndex);
    return &pRecord->m_pData[m_nFirstAdditionalValueIndex];
}



void CDataRecordingInstance::copyRTCSignals(size_t nRecordIndex, int32_t* pRTCSignalBuffer, size_t nRTCSignalBufferSize)
{
    uint32_t nRTCValuesPerRecord = getRTCValuesPerRecord();
    int32_t* pRTCData = getRTCData(nRecordIndex);

    if (nRTCSignalBufferSize < (size_t)nRTCValuesPerRecord)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    for (uint32_t nIndex = 0; nIndex < nRTCValuesPerRecord; nIndex++)
        pRTCSignalBuffer[nIndex] = pRTCData[nIndex];

}


void CDataRecordingInstance::copySensorSignals(size_t nRecordIndex, int32_t* pSensorSignalBuffer, size_t nSensorSignalBufferSize)
{
    uint32_t nSensorValuesPerRecord = getSensorValuesPerRecord();
    int32_t* pSensorData = getSensorData(nRecordIndex);

    if (nSensorSignalBufferSize < (size_t)nSensorValuesPerRecord)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    for (uint32_t nIndex = 0; nIndex < nSensorValuesPerRecord; nIndex++)
        pSensorSignalBuffer[nIndex] = pSensorData[nIndex];

}


void CDataRecordingInstance::copyAdditionalSignals(size_t nRecordIndex, int32_t* pAdditionalSignalBuffer, size_t nAdditionalSignalBufferSize)
{
    uint32_t nAdditionalValuesPerRecord = getAdditionalValuesPerRecord();
    int32_t* pAdditionalData = getAdditionalData(nRecordIndex);

    if (nAdditionalSignalBufferSize < (size_t)nAdditionalValuesPerRecord)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    for (uint32_t nIndex = 0; nIndex < nAdditionalValuesPerRecord; nIndex++)
        pAdditionalSignalBuffer[nIndex] = pAdditionalData[nIndex];

}

void CDataRecordingInstance::copyAllXCoordinates(double* pCoordinateBuffer, size_t nCoordinateBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (pCoordinateBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nCoordinateBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    double * pTarget = pCoordinateBuffer;
    for (size_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
        auto pRecord = m_Entries.getData(nIndex);
        *pTarget = pRecord->m_dX;
        pTarget++;
    }

}

void CDataRecordingInstance::copyAllYCoordinates(double* pCoordinateBuffer, size_t nCoordinateBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (pCoordinateBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nCoordinateBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    double* pTarget = pCoordinateBuffer;
    for (size_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
        auto pRecord = m_Entries.getData(nIndex);
        *pTarget = pRecord->m_dY;
        pTarget++;
    }

}

void CDataRecordingInstance::copyAllMeasurementTags(uint32_t* pMeasurementTagBuffer, size_t nMeasurementTagBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (pMeasurementTagBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nMeasurementTagBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    uint32_t* pTarget = pMeasurementTagBuffer;
    for (size_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
        auto pRecord = m_Entries.getData(nIndex);
        *pTarget = pRecord->m_nMeasurementTag;
        pTarget++;
    }

}

void CDataRecordingInstance::copyAllPacketNumbers(uint32_t * pPacketNumberBuffer, size_t nPacketNumberBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (pPacketNumberBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nPacketNumberBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    uint32_t* pTarget = pPacketNumberBuffer;
    for (size_t nIndex = 0; nIndex < nRecordCount; nIndex++) {
        auto pRecord = m_Entries.getData(nIndex);
        *pTarget = pRecord->m_nPacketNumber;
        pTarget++;
    }

}

void CDataRecordingInstance::copyAllRTCSignalsByIndex(uint32_t nRTCIndex, int32_t* pRTCSignalBuffer, size_t nRTCSignalBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (nRTCIndex >= getRTCValuesPerRecord())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRTCINDEX);

    if (pRTCSignalBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nRTCSignalBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    int32_t* pTarget = pRTCSignalBuffer;
    for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
        int32_t* pRTCData = getRTCData(nRecordIndex);
        *pTarget = pRTCData[nRTCIndex];
        pTarget++;
    }
}

void CDataRecordingInstance::copyAllScaledRTCSignalsByIndex(uint32_t nRTCIndex, double* pRTCSignalBuffer, size_t nRTCSignalBufferSize, double dScaleFactor, double dOffset)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (nRTCIndex >= getRTCValuesPerRecord())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDRTCINDEX);

    if (pRTCSignalBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nRTCSignalBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    double* pTarget = pRTCSignalBuffer;
    for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
        int32_t* pRTCData = getRTCData(nRecordIndex);
        *pTarget = pRTCData[nRTCIndex] * dScaleFactor + dOffset;
        pTarget++;
    }
}


void CDataRecordingInstance::copyAllSensorSignalsByIndex(uint32_t nSensorIndex, int32_t* pSensorSignalBuffer, size_t nSensorSignalBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (nSensorIndex >= getSensorValuesPerRecord ())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSENSORINDEX);

    if (pSensorSignalBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nSensorSignalBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    int32_t* pTarget = pSensorSignalBuffer;
    for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
        int32_t* pSensorData = getSensorData(nRecordIndex);
        *pTarget = pSensorData[nSensorIndex];
        pTarget++;
    }

}


void CDataRecordingInstance::copyAllScaledSensorSignalsByIndex(uint32_t nSensorIndex, double* pSensorSignalBuffer, size_t nSensorSignalBufferSize, double dScaleFactor, double dOffset)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (nSensorIndex >= getSensorValuesPerRecord ())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDSENSORINDEX);

    if (pSensorSignalBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nSensorSignalBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    double* pTarget = pSensorSignalBuffer;
    for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
        int32_t* pSensorData = getSensorData(nRecordIndex);
        *pTarget = pSensorData[nSensorIndex] * dScaleFactor + dOffset;
        pTarget++;
    }

}

void CDataRecordingInstance::copyAllAdditionalSignalsByIndex(uint32_t nAdditionalIndex, int32_t* pAdditionalSignalBuffer, size_t nAdditionalSignalBufferSize)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (nAdditionalIndex >= getAdditionalValuesPerRecord())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDADDITIONALINDEX);

    if (pAdditionalSignalBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nAdditionalSignalBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    int32_t* pTarget = pAdditionalSignalBuffer;
    for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
        int32_t* pAdditionalData = getAdditionalData(nRecordIndex);
        *pTarget = pAdditionalData[nAdditionalIndex];
        pTarget++;
    }

}

void CDataRecordingInstance::copyAllScaledAdditionalSignalsByIndex(uint32_t nAdditionalIndex, double* pAdditionalSignalBuffer, size_t nAdditionalSignalBufferSize, double dScaleFactor, double dOffset)
{
    size_t nRecordCount = getRecordCount();
    if (nRecordCount == 0)
        return;

    if (nAdditionalIndex >= getAdditionalValuesPerRecord())
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDADDITIONALINDEX);

    if (pAdditionalSignalBuffer == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDPARAM);

    if (nAdditionalSignalBufferSize < nRecordCount)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_BUFFERTOOSMALL);

    double* pTarget = pAdditionalSignalBuffer;
    for (size_t nRecordIndex = 0; nRecordIndex < nRecordCount; nRecordIndex++) {
        int32_t* pAdditionalData = getAdditionalData(nRecordIndex);
        *pTarget = pAdditionalData[nAdditionalIndex] * dScaleFactor + dOffset;
        pTarget++;
    }

}

