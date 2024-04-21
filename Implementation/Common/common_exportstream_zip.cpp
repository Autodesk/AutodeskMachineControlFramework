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

#include "common_exportstream_zip.h"
#include <stdexcept> 
#include <vector>

namespace AMCCommon {

	CExportStream_ZIP::CExportStream_ZIP(CPortableZIPWriter * pZIPWriter, uint32_t nEntryKey)
	{
		m_bIsInitialized = false;

		if (pZIPWriter == nullptr)
			throw std::runtime_error("invalid param");
		if (nEntryKey == 0)
			throw std::runtime_error("invalid param");

		m_pZIPWriter = pZIPWriter;
		m_nEntryKey = nEntryKey;

		m_pStream.next_in = nullptr;
		m_pStream.avail_in = 0;
		m_pStream.total_in = 0;

		m_pStream.msg = nullptr;
		m_pStream.state = nullptr;

		m_pStream.zalloc = nullptr;
		m_pStream.zfree = nullptr;
		m_pStream.opaque = nullptr;

		m_pStream.data_type = 0;
		m_pStream.adler = 0;
		m_pStream.reserved = 0;

		m_pStream.next_out = &m_nOutBuffer[0];
		m_pStream.avail_out = ZIPEXPORTBUFFERSIZE;
		m_pStream.total_out = 0;

		int32_t nResult = deflateInit2(&m_pStream, Z_BEST_SPEED, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
		if (nResult < 0)
			throw std::runtime_error("deflate init failed");

		m_bIsInitialized = true;
	}

	CExportStream_ZIP::~CExportStream_ZIP()
	{
		if (m_bIsInitialized) {
			finishDeflate();
		}
	}

	bool CExportStream_ZIP::seekPosition(uint64_t position, bool bHasToSucceed)
	{
		if (bHasToSucceed)
			throw std::runtime_error("zip stream cannot seek");
		return false;
	}

	bool CExportStream_ZIP::seekForward(uint64_t bytes, bool bHasToSucceed)
	{
		if (bHasToSucceed)
			throw std::runtime_error("zip stream cannot seek");
		return false;
	}

	bool CExportStream_ZIP::seekFromEnd(uint64_t bytes, bool bHasToSucceed)
	{
		if (bHasToSucceed)
			throw std::runtime_error("zip stream cannot seek");
		return false;
	}

	uint64_t CExportStream_ZIP::getPosition()
	{
		return m_pZIPWriter->getCurrentSize(m_nEntryKey);
	}

	uint64_t CExportStream_ZIP::writeBuffer(const void * pBuffer, uint64_t cbTotalBytesToWrite)
	{
		if (!m_bIsInitialized)
			throw std::runtime_error("zip stream already finished");

		uint64_t cbCount = cbTotalBytesToWrite;
		const uint8_t * pByte = (const uint8_t*)pBuffer;

		while (cbCount > 0) {
			uint32_t cbBytesWritten;
			if (cbCount < ZIPEXPORTWRITECHUNKSIZE)
				cbBytesWritten = writeChunk(pByte, (uint32_t)cbCount);
			else
				cbBytesWritten = writeChunk(pByte, ZIPEXPORTWRITECHUNKSIZE);

			if (cbBytesWritten == 0)
				throw std::runtime_error("zip stream could not deflate");

			cbCount -= cbBytesWritten;
		}

		return cbTotalBytesToWrite;
	}

	uint32_t CExportStream_ZIP::writeChunk(const uint8_t * pData, uint32_t cbCount)
	{
		if ((pData == nullptr) || (cbCount == 0) || (cbCount > ZIPEXPORTWRITECHUNKSIZE))
			throw std::runtime_error("invalid param");

		m_pStream.next_in = (Bytef *) pData;
		m_pStream.avail_in = cbCount;

		m_pZIPWriter->calculateChecksum(m_nEntryKey, pData, cbCount);

		while (m_pStream.avail_in > 0) {
			int32_t nResult = deflate(&m_pStream, 0);
			if (nResult < 0)
				throw std::runtime_error("zip stream could not deflate");

			if (m_pStream.avail_out == 0) {
				m_pZIPWriter->writeDeflatedBuffer(m_nEntryKey, &m_nOutBuffer[0], ZIPEXPORTBUFFERSIZE);

				m_pStream.next_out = &m_nOutBuffer[0];
				m_pStream.avail_out = ZIPEXPORTBUFFERSIZE;
			}
		}

		return cbCount;

	}


	void CExportStream_ZIP::finishDeflate()
	{
		if (!m_bIsInitialized)
			throw std::runtime_error("zip stream already finished");

		m_pStream.next_in = nullptr;
		m_pStream.avail_in = 0;

		bool bContinue = true;
		while (bContinue) {
			int32_t nResult = deflate(&m_pStream, Z_FINISH);
			if (nResult < 0)
				throw std::runtime_error("zip stream could not deflate");

			if ((nResult != Z_STREAM_END) && (m_pStream.avail_out == 0)) {
				m_pZIPWriter->writeDeflatedBuffer(m_nEntryKey, &m_nOutBuffer[0], ZIPEXPORTBUFFERSIZE);

				m_pStream.next_out = &m_nOutBuffer[0];
				m_pStream.avail_out = ZIPEXPORTBUFFERSIZE;
			}
			else
				bContinue = false;
		}

		if (m_pStream.avail_out < ZIPEXPORTBUFFERSIZE) {
			m_pZIPWriter->writeDeflatedBuffer(m_nEntryKey, &m_nOutBuffer[0], ZIPEXPORTBUFFERSIZE - m_pStream.avail_out);
		}

		deflateEnd(&m_pStream);
		m_bIsInitialized = false;
	}

	void CExportStream_ZIP::flushZIPStream()
	{
		finishDeflate();
	}

	void CExportStream_ZIP::writeZeros(uint64_t bytes)
	{

		if (bytes > 0) {
			uint32_t bufferSize = 1024 * 1024;
			std::vector <uint64_t> Buffer;
			Buffer.resize(bufferSize / sizeof(uint64_t));

			for (auto iter = Buffer.begin(); iter != Buffer.end(); iter++)
				*iter = 0;

			while (bytes > 0) {
				auto bytesToWrite = bytes;
				if (bytes > bufferSize)
					bytes = bufferSize;

				writeBuffer(Buffer.data(), bytesToWrite);
				bytes -= bytesToWrite;

			}
		}

	}
}
