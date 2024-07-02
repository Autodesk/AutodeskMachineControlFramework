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

#ifndef __COMMON_PORTABLEZIPWRITERENTRY
#define __COMMON_PORTABLEZIPWRITERENTRY

#include <string>
#include <list>
#include <memory>

namespace AMCCommon {

	class CPortableZIPWriterEntry {
	private:
		std::string m_sUTF8Name;
		uint32_t m_nCRC32;
		uint64_t m_nCompressedSize;
		uint64_t m_nUncompressedSize;
		uint16_t m_nLastModTime;
		uint16_t m_nLastModDate;
		uint64_t m_nFilePosition;
		uint64_t m_nExtInfoPosition;
		uint64_t m_nDataPosition;
	public:
		CPortableZIPWriterEntry(const std::string sUTF8Name, uint16_t nLastModTime, uint16_t nLastModDate, uint64_t nFilePosition, uint64_t nExtInfoPosition, uint64_t nDataPosition);
		std::string getUTF8Name();
		uint32_t getCRC32();
		uint64_t getCompressedSize();
		uint64_t getUncompressedSize();
		uint16_t getLastModTime();
		uint16_t getLastModDate();
		uint64_t getFilePosition();
		uint64_t getExtInfoPosition();
		uint64_t getDataPosition();
		void increaseCompressedSize(uint32_t nCompressedSize);
		void increaseUncompressedSize(uint32_t nUncompressedSize);
		void calculateChecksum(const void * pBuffer, uint32_t cbCount);

	};

	typedef std::shared_ptr <CPortableZIPWriterEntry> PPortableZIPWriterEntry;


}

#endif //__COMMON_PORTABLEZIPWRITERENTRY
