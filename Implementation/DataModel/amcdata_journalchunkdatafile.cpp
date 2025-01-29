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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "amcdata_journalchunkdatafile.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "common_utils.hpp"
#include "common_exportstream_native.hpp"


#include <sstream>
#include <iomanip>
#include <cstring>

namespace AMCData {


    CJournalChunkDataFile::CJournalChunkDataFile()
    {

    }

    CJournalChunkDataFile::~CJournalChunkDataFile()
    {

    }


    void CJournalChunkDataFile::readJournalChunkIntegerData(size_t nDataOffset, size_t nDataLength, std::vector<LibMCData::sJournalChunkVariableInfo>& variableInfo, std::vector<uint32_t>& timeStampData, std::vector<int64_t>& valueData)
    {
        AMCData::sJournalChunkHeader chunkHeader;

        readBuffer(nDataOffset, (uint8_t*)&chunkHeader, sizeof(chunkHeader));
        if (chunkHeader.m_nSignature != JOURNALSIGNATURE_INTEGERDATA_V1)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDJOURNALDATASIGNATURE);

        if (chunkHeader.m_nMemorySize != nDataLength)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALMEMORYSIZEMISMATCH);

        if (chunkHeader.m_nVariableCount == 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALMEMORYVARIABLECOUNTISZERO);
        if (chunkHeader.m_nValueCount == 0)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALMEMORYVALUECOUNTISZERO);

        uint64_t nVariableBufferMemSize = (uint64_t)chunkHeader.m_nVariableCount * sizeof(LibMCData::sJournalChunkVariableInfo);
        uint64_t nTimeStampBufferMemSize = (uint64_t)chunkHeader.m_nValueCount * sizeof(uint32_t);
        uint64_t nValueBufferMemSize = (uint64_t)chunkHeader.m_nValueCount * sizeof(int64_t);

        uint64_t nTotalMemSize = sizeof(AMCData::sJournalChunkHeader) + nVariableBufferMemSize + nTimeStampBufferMemSize + nValueBufferMemSize;
        if (nTotalMemSize != nDataLength)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_JOURNALMEMORYSIZEMISMATCH);

        uint64_t nVariableStart = nDataOffset + sizeof(chunkHeader);
        uint64_t nTimeStampStart = nVariableStart + nVariableBufferMemSize;
        uint64_t nValueStart = nTimeStampStart + nTimeStampBufferMemSize;

        variableInfo.resize(chunkHeader.m_nVariableCount);
        timeStampData.resize(chunkHeader.m_nValueCount);
        valueData.resize(chunkHeader.m_nValueCount);

        readBuffer(nVariableStart, (uint8_t*)variableInfo.data(), nVariableBufferMemSize);
        readBuffer(nTimeStampStart, (uint8_t*)timeStampData.data(), nTimeStampBufferMemSize);
        readBuffer(nValueStart, (uint8_t*)valueData.data(), nValueBufferMemSize);

    }



}