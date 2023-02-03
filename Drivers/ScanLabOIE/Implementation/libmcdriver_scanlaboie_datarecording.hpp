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


#ifndef __LIBMCDRIVER_SCANLABOIE_DATARECORDING
#define __LIBMCDRIVER_SCANLABOIE_DATARECORDING

#include "libmcdriver_scanlaboie_interfaces.hpp"
#include "libmcdriver_scanlaboie_sdk.hpp"

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
    double m_dX;
    double m_dY;
    int32_t* m_pData;

} sDataRecordingEntry;


class CDataRecording {
private:

    std::vector<PDataRecordingBuffer> m_Buffers;
    PDataRecordingBuffer m_pCurrentBuffer;

    std::list<sDataRecordingEntry> m_Entries;
    sDataRecordingEntry m_CurrentEntry;
    uint32_t m_nCurrentEntryDataIndex;

    uint32_t m_nValuesPerPacket;
    uint32_t m_nBufferSizeInPackets;
    size_t m_nValueCountPerBuffer;
    

public:

    CDataRecording(uint32_t nValuesPerPacket, uint32_t nBufferSizeInPackets);

    virtual ~CDataRecording();

    void startRecord(uint32_t nPacketNumber, double dX, double dY);

    void recordValue (int32_t nValue);

    void finishRecord();

    size_t getRecordCount();

    void writeToFile(const std::string & sFileName);

};

typedef std::shared_ptr<CDataRecording> PDataRecording;

} // namespace Impl
} // namespace LibMCDriver_ScanLabOIE

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_SCANLABOIE_DATARECORDING
