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


#ifndef __AMCCOMMON_IMPORTSTREAM
#define __AMCCOMMON_IMPORTSTREAM

#include <vector>
#include <string>
#include <memory>

namespace AMCCommon {

	class CImportStream;
	typedef std::shared_ptr <CImportStream> PImportStream;

	class CImportStream {
	private:
	public:
		CImportStream()
		{
		}

		virtual ~CImportStream()
		{

		}

		virtual bool seekPosition(const uint64_t position, const bool bHasToSucceed) = 0;
		virtual bool seekForward(const uint64_t bytes, const bool bHasToSucceed) = 0;
		virtual bool seekFromEnd(const uint64_t bytes, const bool bHasToSucceed) = 0;
		virtual uint64_t readBuffer(uint8_t * pBuffer, const uint64_t cbTotalBytesToRead, const bool bNeedsToReadAll) = 0;
		virtual uint64_t retrieveSize() = 0;
		virtual uint64_t getPosition() = 0;

		virtual void readIntoMemory(std::vector<uint8_t>& Buffer) {
			uint64_t nSize = retrieveSize();
			Buffer.resize(nSize);
			if (nSize > 0) {
				seekPosition(0, true);
				readBuffer (Buffer.data(), nSize, true);
			}
		}

		virtual std::string readAsString()
		{
			std::vector<uint8_t> Buffer;
			uint64_t nSize = retrieveSize();
			Buffer.resize(nSize + 1);
			if (nSize > 0) {
				seekPosition(0, true);
				readBuffer(Buffer.data(), nSize, true);
			}

			Buffer[nSize] = 0;
			return (char*) Buffer.data();
		}

	};

}

#endif // __AMCCOMMON_IMPORTSTREAM
