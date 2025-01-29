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


Abstract: This is the class declaration of CJournalChunkIntegerData

*/


#ifndef __LIBMCDATA_JOURNALCHUNKDATAFILE
#define __LIBMCDATA_JOURNALCHUNKDATAFILE

#include "libmcdata_interfaces.hpp"
#include <vector>

namespace AMCData {



#define JOURNALSIGNATURE_INTEGERDATA_V1 0x83AC1001

    typedef struct {
        uint32_t m_nSignature;
        uint32_t m_nMemorySize;
        uint32_t m_nVariableCount;
        uint32_t m_nValueCount;
        uint32_t m_nReserved[3];
    } sJournalChunkHeader;


    class CJournalChunkDataFile
    {
    private:
    public:

        CJournalChunkDataFile();

        virtual ~CJournalChunkDataFile();

        virtual void readBuffer(uint64_t nDataOffset, uint8_t* pBuffer, uint64_t nDataLength) = 0;

        void readJournalChunkIntegerData(size_t nDataOffset, size_t nDataLength, std::vector<LibMCData::sJournalChunkVariableInfo>& variableInfo, std::vector<uint32_t>& timeStampData, std::vector<int64_t>& valueData);

    };



} // namespace AMCData


#endif // __LIBMCDATA_JOURNALCHUNKDATAFILE
