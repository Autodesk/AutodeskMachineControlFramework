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
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/



#include "common_importstream_native.hpp"
#include "common_utils.hpp"
#include <string>
#include <exception>


namespace AMCCommon {


	CImportStream_Native::CImportStream_Native(const std::string& sUTF8Filename)
	{
#if defined(_WIN32) && !defined(__MINGW32__)
		std::wstring sUTF16FileName = CUtils::UTF8toUTF16(sUTF8Filename);
		m_Stream.open(sUTF16FileName.c_str(), std::ios::in | std::ios::binary);
#else
		m_Stream.open(sUTF8Filename.c_str(), std::ios::in | std::ios::binary);
#endif
		if (m_Stream.fail())
			throw std::runtime_error("could not open file: " + sUTF8Filename);
	}

	CImportStream_Native::~CImportStream_Native()
	{
		if (m_Stream.is_open()) {
			m_Stream.close();
		}
	}

	bool CImportStream_Native::seekPosition(const uint64_t position, const bool bHasToSucceed)
	{
		std::streampos nStreamPos = position;
		m_Stream.seekg(nStreamPos, std::ios_base::beg);

		if (m_Stream.fail()) {
			if (bHasToSucceed)
				throw std::runtime_error("could not seek stream");

			return false;
		}

		return true;
	}

	bool CImportStream_Native::seekForward(const uint64_t bytes, const bool bHasToSucceed)
	{
		std::streampos nStreamOffset = bytes;
		m_Stream.seekg(nStreamOffset, std::ios_base::cur);

		if (m_Stream.fail()) {
			if (bHasToSucceed)
				throw std::runtime_error("could not seek stream");

			return false;
		}

		return true;
	}

	bool CImportStream_Native::seekFromEnd(const uint64_t bytes, const bool bHasToSucceed)
	{
		std::streampos nStreamOffset = 0 - bytes;
		m_Stream.seekg(nStreamOffset, std::ios_base::end);

		if (m_Stream.fail()) {
			if (bHasToSucceed)
				throw std::runtime_error("could not seek stream");

			return false;
		}

		return true;
	}

	uint64_t CImportStream_Native::readBuffer(uint8_t* pBuffer, const uint64_t cbTotalBytesToRead, const bool bNeedsToReadAll)
	{
		if (pBuffer == nullptr)
			throw std::runtime_error("invalid buffer parameter");

		char* pChar = (char*)pBuffer;
		std::streamsize nSize = cbTotalBytesToRead;
		m_Stream.read(pChar, nSize);
		if (m_Stream.fail())
			throw std::runtime_error("could not read from stream");

		std::streamsize nReadBytes = m_Stream.gcount();
		if (nReadBytes != nSize) {
			if (bNeedsToReadAll)
				throw std::runtime_error("could not read full data");
		}

		return nReadBytes;

	}

	uint64_t CImportStream_Native::retrieveSize()
	{
		uint64_t nOrigPosition = getPosition();
		seekFromEnd(0, true);
		uint64_t nSize = getPosition();
		seekPosition(nOrigPosition, true);
		return nSize;

	}

	uint64_t CImportStream_Native::getPosition()
	{
		std::streampos nStreamPosition = m_Stream.tellg();
		if (nStreamPosition < 0)
			throw std::runtime_error("could not get stream position");

		return nStreamPosition;
	}


}
