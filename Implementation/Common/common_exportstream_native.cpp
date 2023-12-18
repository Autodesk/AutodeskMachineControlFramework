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



#include "common_exportstream_native.hpp"
#include "common_utils.hpp"

#include <string>
#include <vector>

namespace AMCCommon {

	CExportStream_Native::CExportStream_Native(const std::string& sFileName)
	{

#if defined(_WIN32) && !defined(__MINGW32__)
		std::wstring sUTF16FileName = AMCCommon::CUtils::UTF8toUTF16 (sFileName); 
		m_Stream.open(sUTF16FileName.c_str(), std::ios::out | std::ios::binary);
#else 
		m_Stream.open(sFileName.c_str(), std::ios::out | std::ios::binary);
#endif
		if (m_Stream.fail())
			throw std::runtime_error ("could not create file");

	}

	CExportStream_Native::~CExportStream_Native()
	{
		if (m_Stream.is_open()) { 
			m_Stream.close();
		}
	}


	bool CExportStream_Native::seekPosition(uint64_t position, bool bHasToSucceed)
	{
		std::streampos nStreamPos = position;
		m_Stream.seekp(nStreamPos, std::ios_base::beg);

		if (m_Stream.fail()) {
			if (bHasToSucceed)
				throw std::runtime_error("could not seek stream");

			return false;
		}

		return true;
	}

	bool CExportStream_Native::seekForward(uint64_t bytes, bool bHasToSucceed)
	{
		std::streampos nStreamOffset = bytes;
		m_Stream.seekp(nStreamOffset, std::ios_base::cur);

		if (m_Stream.fail()) {
			if (bHasToSucceed)
				throw std::runtime_error("could not seek stream");

			return false;
		}

		return true;
	}

	bool CExportStream_Native::seekFromEnd(uint64_t bytes, bool bHasToSucceed)
	{
		std::streampos nStreamOffset = 0-bytes;
		m_Stream.seekp(nStreamOffset, std::ios_base::end);

		if (m_Stream.fail()) {
			if (bHasToSucceed)
				throw std::runtime_error("could not seek stream");

			return false;
		}

		return true;
	}

	uint64_t CExportStream_Native::getPosition()
	{
		std::streampos nStreamPosition = m_Stream.tellp();
		if (nStreamPosition < 0)
			throw std::runtime_error("could not get stream position");

		return nStreamPosition;
	}

	uint64_t CExportStream_Native::writeBuffer (const void * pBuffer, uint64_t cbTotalBytesToWrite)
	{
		if (pBuffer == nullptr)
			throw std::runtime_error("invalid buffer parameter");

		const char * pChar = (const char*)pBuffer;
		std::streamsize nSize = cbTotalBytesToWrite;
        m_Stream.write(pChar, nSize);
		if (m_Stream.fail())
			throw std::runtime_error("could not write to stream");

        return nSize;
	}


	void CExportStream_Native::writeZeros(uint64_t bytes)
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

	void CExportStream_Native::flushStream()
	{
		m_Stream.flush ();
		if (m_Stream.fail())
			throw std::runtime_error("could not flush stream");
	}

		
}

