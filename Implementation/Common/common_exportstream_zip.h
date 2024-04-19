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

Abstract:

NMR_PortableZIPWriter.h defines a portable stream to write into ZIP files

--*/

#ifndef __COMMON_EXPORTSTREAM_ZIP
#define __COMMON_EXPORTSTREAM_ZIP

#include "common_exportstream.hpp"
#include "common_portablezipwriter.hpp"
#include "Libraries/zlib/zlib.h"

#include <array>

#define ZIPEXPORTBUFFERSIZE 65536
#define ZIPEXPORTWRITECHUNKSIZE 1048576

namespace AMCCommon {

	class CExportStream_ZIP : public CExportStream {
	private:
		CPortableZIPWriter * m_pZIPWriter;
		uint32_t m_nEntryKey;
		z_stream m_pStream;
		std::array<uint8_t, ZIPEXPORTBUFFERSIZE> m_nOutBuffer;

		bool m_bIsInitialized;

		uint32_t writeChunk(_In_ const uint8_t * pData, uint32_t cbCount);
		void finishDeflate();
	public:
		CExportStream_ZIP() = delete;
		CExportStream_ZIP(_In_ CPortableZIPWriter * pZIPWriter, uint32_t nEntryKey);
		~CExportStream_ZIP();

		virtual bool seekPosition(_In_ uint64_t position, _In_ bool bHasToSucceed);
		virtual bool seekForward(_In_ uint64_t bytes, _In_ bool bHasToSucceed);
		virtual bool seekFromEnd(_In_ uint64_t bytes, _In_ bool bHasToSucceed);
		virtual uint64_t getPosition();
		virtual uint64_t writeBuffer(_In_ const void * pBuffer, _In_ uint64_t cbTotalBytesToWrite);

		void flushZIPStream();

		virtual void writeZeros(uint64_t bytes) override;

	};

	typedef std::shared_ptr <CExportStream_ZIP> PExportStream_ZIP;

}

#endif // __COMMON_EXPORTSTREAM_ZIP
