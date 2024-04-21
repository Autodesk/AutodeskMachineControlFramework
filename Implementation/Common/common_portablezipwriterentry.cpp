/*++

Copyright (C) 2019 3MF Consortium

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

--*/

#include "common_portablezipwriterentry.hpp"
#include "common_utils.hpp"
#include "zlib.h"

namespace AMCCommon {

	CPortableZIPWriterEntry::CPortableZIPWriterEntry(const std::string sUTF8Name, uint16_t nLastModTime, uint16_t nLastModDate, uint64_t nFilePosition, uint64_t nExtInfoPosition, uint64_t nDataPosition)
	{
		m_sUTF8Name = sUTF8Name;
		m_nCRC32 = 0;
		m_nCompressedSize = 0;
		m_nUncompressedSize = 0;
		m_nLastModTime = nLastModTime;
		m_nLastModDate = nLastModDate;
		m_nFilePosition = nFilePosition;
		m_nExtInfoPosition = nExtInfoPosition;
		m_nDataPosition = nDataPosition;
	}

	std::string CPortableZIPWriterEntry::getUTF8Name()
	{
		return m_sUTF8Name;
	}

	uint32_t CPortableZIPWriterEntry::getCRC32()
	{
		return m_nCRC32;
	}

	uint64_t CPortableZIPWriterEntry::getCompressedSize()
	{
		return m_nCompressedSize;
	}

	uint64_t CPortableZIPWriterEntry::getUncompressedSize()
	{
		return m_nUncompressedSize;
	}

	uint16_t CPortableZIPWriterEntry::getLastModTime()
	{
		return m_nLastModTime;
	}

	uint16_t CPortableZIPWriterEntry::getLastModDate()
	{
		return m_nLastModDate;
	}

	uint64_t CPortableZIPWriterEntry::getFilePosition()
	{
		return m_nFilePosition;
	}
		
	uint64_t CPortableZIPWriterEntry::getExtInfoPosition()
	{
		return m_nExtInfoPosition;
	}

	uint64_t CPortableZIPWriterEntry::getDataPosition()
	{
		return m_nDataPosition;
	}

	void CPortableZIPWriterEntry::increaseCompressedSize(uint32_t nCompressedSize)
	{
		m_nCompressedSize += nCompressedSize;
	}

	void CPortableZIPWriterEntry::increaseUncompressedSize(uint32_t nUncompressedSize)
	{
		m_nUncompressedSize += nUncompressedSize;
	}

	void CPortableZIPWriterEntry::calculateChecksum(const void * pBuffer, uint32_t cbCount)
	{
		m_nCRC32 = crc32(m_nCRC32, (Bytef*) pBuffer, cbCount);
	}

}
