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

#include "libmcdriver_scanlaboie_datarecording.hpp"
#include "libmcdriver_scanlaboie_interfaceexception.hpp"

#include <stdexcept>
using namespace LibMCDriver_ScanLabOIE::Impl;

#define DATARECORDING_MINBUFFERSIZEINVALUES 256
#define DATARECORDING_MINBUFFERSIZEINPACKETS 256
#define DATARECORDING_MAXBUFFERSIZEINPACKETS (1024 * 1024)

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


CDataRecording::CDataRecording(uint32_t nValuesPerPacket, uint32_t nBufferSizeInPackets)
    : m_nValuesPerPacket (nValuesPerPacket), m_nBufferSizeInPackets (nBufferSizeInPackets)
{
    if (nValuesPerPacket <= 0)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDVALUESPERPACKET);
    if (nBufferSizeInPackets < DATARECORDING_MINBUFFERSIZEINPACKETS)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDBUFFERSIZE);
    if (nBufferSizeInPackets > DATARECORDING_MAXBUFFERSIZEINPACKETS)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_INVALIDBUFFERSIZE);

    m_nValueCountPerBuffer = (size_t)nValuesPerPacket * (size_t)nBufferSizeInPackets;
    memset((void*)&m_CurrentEntry, 0, sizeof(m_CurrentEntry));
}

CDataRecording::~CDataRecording()
{

}

void CDataRecording::startRecord(uint32_t nPacketNumber, double dX, double dY)
{
    if (m_pCurrentBuffer.get() == nullptr) {
        m_pCurrentBuffer = std::make_shared<CDataRecordingBuffer>(m_nValueCountPerBuffer);
        m_Buffers.push_back(m_pCurrentBuffer);
    }
    
    m_CurrentEntry.m_dX = dX;
    m_CurrentEntry.m_dY = dY;
    m_CurrentEntry.m_nPacketNumber = nPacketNumber;
    m_CurrentEntry.m_pData = m_pCurrentBuffer->allocData (m_nValuesPerPacket);
    m_nCurrentEntryDataIndex = 0;

    // If buffer is full, alloc a new buffer the next iteration
    if (!m_pCurrentBuffer->hasSpace(m_nValuesPerPacket))
        m_pCurrentBuffer = nullptr;
}

void CDataRecording::recordValue(int32_t nValue)
{
    if (m_nCurrentEntryDataIndex >= m_nValuesPerPacket) 
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_TOOMANYVALUESINPACKET);
    if (m_CurrentEntry.m_pData == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_PACKETISNOTRECORDING);

    m_CurrentEntry.m_pData[m_nCurrentEntryDataIndex] = nValue;
    m_nCurrentEntryDataIndex++;

}

void CDataRecording::finishRecord()
{
    //std::cout << "finishing record: " << m_nCurrentEntryDataIndex << " of " << m_nValuesPerPacket << std::endl;

    //if (m_nCurrentEntryDataIndex < m_nValuesPerPacket)
        //throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_NOTENOUGHVALUESINPACKET);
    if (m_CurrentEntry.m_pData == nullptr)
        throw ELibMCDriver_ScanLabOIEInterfaceException(LIBMCDRIVER_SCANLABOIE_ERROR_PACKETISNOTRECORDING);

    m_Entries.push_back(m_CurrentEntry);
    m_CurrentEntry.m_pData = nullptr;
}

size_t CDataRecording::getRecordCount()
{
    return m_Entries.size();
}


void CDataRecording::writeToFile(const std::string& sFileName)
{
    std::ofstream fStream;
    fStream.open (sFileName);
    if (!fStream.is_open())
        throw std::runtime_error ("could not write file");

    fStream << "packet number, X, Y, value 0, value 1, ...." << std::endl;

    for (auto & entry : m_Entries) {
        fStream << entry.m_nPacketNumber << ", " << entry.m_dX << ", " << entry.m_dY;
        for (uint32_t nIndex = 0; nIndex < m_nValuesPerPacket; nIndex++)
            fStream << ", " << entry.m_pData[nIndex];

        fStream << std::endl;

    }


    fStream.close();
}
