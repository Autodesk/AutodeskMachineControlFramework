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


Abstract: This is the class declaration of CDeviceConfiguration

*/


#ifndef __LIBMCDRIVER_SCANLABOIE_DATARECORDINGINSTANCE
#define __LIBMCDRIVER_SCANLABOIE_DATARECORDINGINSTANCE

#include "libmcdriver_scanlaboie_interfaces.hpp"
#include "libmcdriver_scanlaboie_sdk.hpp"
#include "libmcdriver_scanlaboie_pagedvector.hpp"

// Parent classes
#include "libmcdriver_scanlaboie_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <vector>
#include <list>
#include <fstream>

namespace LibMCDriver_ScanLabOIE {
namespace Impl {

#define SCANLABOIE_DATARECORDPAGESIZE (1024 * 256)

/*************************************************************************************************************************
 Class declaration of CDeviceConfiguration 
**************************************************************************************************************************/

class CDataRecordingBuffer {
private:
    std::vector<int32_t> m_RawData;
    size_t m_nCurrentPosition;
public:

    CDataRecordingBuffer(size_t nBufferSizeInValues);

    virtual ~CDataRecordingBuffer();

    bool hasSpace(size_t nValueCount);

    int32_t* allocData(size_t nValueCount);

};

typedef std::shared_ptr<CDataRecordingBuffer> PDataRecordingBuffer;

typedef struct _sDataRecordingEntry
{
    uint32_t m_nPacketNumber;
    uint32_t m_nMeasurementTag;
    double m_dX;
    double m_dY;
    int32_t* m_pData;

} sDataRecordingEntry;



class CDataRecordingInstance;

typedef std::shared_ptr<CDataRecordingInstance> PDataRecordingInstance;

class CDataRecordingInstance {
private:

    std::vector<PDataRecordingBuffer> m_Buffers;
    PDataRecordingBuffer m_pCurrentBuffer;

    sDataRecordingEntry m_CurrentEntry;
    uint32_t m_nCurrentEntryDataIndex;

    CPagedVector<sDataRecordingEntry, SCANLABOIE_DATARECORDPAGESIZE> m_Entries;

    uint32_t m_nValuesPerRecord;

    uint32_t m_nFirstRTCValueIndex;
    uint32_t m_nRTCValueCount;
    uint32_t m_nFirstSensorValueIndex;
    uint32_t m_nSensorValueCount;
    uint32_t m_nFirstAdditionalValueIndex;
    uint32_t m_nAdditionalValueCount;

    uint32_t m_nBufferSizeInRecords;
    size_t m_nValueCountPerBuffer;
    
    int32_t* getRTCData (size_t nRecordIndex);
    int32_t* getSensorData(size_t nRecordIndex);
    int32_t* getAdditionalData(size_t nRecordIndex);

public:

    CDataRecordingInstance(uint32_t nSensorValuesPerRecord, uint32_t nRTCValuesPerRecord, uint32_t nAdditionalValuesPerRecord, uint32_t nBufferSizeInRecords);

    virtual ~CDataRecordingInstance();

    void startRecord(uint32_t nPacketNumber, uint32_t nMeasurementTag, double dX, double dY);

    void recordValue (int32_t nValue);

    void finishRecord();

    size_t getRecordCount();

    uint32_t getAdditionalValuesPerRecord ();

    uint32_t getRTCValuesPerRecord();

    uint32_t getSensorValuesPerRecord();

    uint32_t getBufferSizeInRecords ();

    void copyRTCSignals (size_t nRecordIndex, int32_t * pRTCSignalBuffer, size_t nRTCSignalBufferSize);

    void copySensorSignals(size_t nRecordIndex, int32_t* pSensorSignalBuffer, size_t nSensorSignalBufferSize);
    
    void copyAdditionalSignals(size_t nRecordIndex, int32_t* pAdditionalSignalBuffer, size_t nAdditionalSignalBufferSize);

    void copyXCoordinates(double * pCoordinateBuffer, size_t nCoordinateBufferSize);

    void copyYCoordinates(double* pCoordinateBuffer, size_t nCoordinateBufferSize);

    void copyPacketNumbers(uint32_t* pPacketNumberBuffer, size_t nPacketNumberBufferSize);

    void copyMeasurementTags(uint32_t* pMeasurementTagBuffer, size_t nMeasurementTagBufferSize);

    void copyAllRTCSignalsByIndex(uint32_t nRTCIndex, int32_t* pRTCSignalBuffer, size_t nRTCSignalBufferSize);

    void copyAllSensorSignalsByIndex(uint32_t nSensorIndex, int32_t* pSensorSignalBuffer, size_t nSensorSignalBufferSize);

    void copyAllAdditionalSignalsByIndex(uint32_t nSensnAdditionalIndexorIndex, int32_t* pAdditionalSignalBuffer, size_t nAdditionalSignalBufferSize);

    void writeToFile(const std::string & sFileName);

    sDataRecordingEntry* getRecord (uint32_t nIndex);

    PDataRecordingInstance createEmptyDuplicate ();

};


} // namespace Impl
} // namespace LibMCDriver_ScanLabOIE

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABOIE_DATARECORDINGINSTANCE
