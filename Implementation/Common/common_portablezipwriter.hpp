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

#ifndef __COMMON_PORTABLEZIPWRITER
#define __COMMON_PORTABLEZIPWRITER

#include <string>
#include <list>

#include "common_exportstream.hpp"
#include "common_portablezipwriterentry.hpp"

namespace AMCCommon {

	class CPortableZIPWriter {
	private:
		PExportStream m_pExportStream;
		PPortableZIPWriterEntry m_pCurrentEntry;
		uint32_t m_nCurrentEntryKey;
		uint32_t m_nNextEntryKey;
		bool m_bIsFinished;

		bool m_bWriteZIP64;
		uint16_t m_nVersionMade;
		uint16_t m_nVersionNeeded;

		std::list<PPortableZIPWriterEntry> m_Entries;
		PExportStream m_pCurrentStream;
	public:
		CPortableZIPWriter() = delete;
		CPortableZIPWriter(PExportStream pExportStream, bool bWriteZIP64);
		~CPortableZIPWriter();

		PExportStream createEntry(const std::string sName, uint64_t nUnixTimeStamp);
		void closeEntry();

		void writeDeflatedBuffer(uint32_t nEntryKey, const void * pBuffer, uint32_t cbCompressedBytes);
		void calculateChecksum(uint32_t nEntryKey, const void * pBuffer, uint32_t cbUncompressedBytes);
		uint64_t getCurrentSize(uint32_t nEntryKey);

		void writeDirectory();
	};

	typedef std::shared_ptr <CPortableZIPWriter> PPortableZIPWriter;

}

#endif //__COMMON_PORTABLEZIPWRITER
