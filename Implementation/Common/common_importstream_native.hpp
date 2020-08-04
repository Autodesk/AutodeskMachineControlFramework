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


#ifndef __AMCCOMMON_IMPORTSTREAM_NATIVE
#define __AMCCOMMON_IMPORTSTREAM_NATIVE

#include "common_importstream.hpp"
#include <string>
#include <iostream>
#include <fstream>


namespace AMCCommon {

	class CImportStream_Native : public CImportStream {
	private:
		std::ifstream m_Stream;
	public:

		CImportStream_Native(const std::string & sUTF8Filename);
		~CImportStream_Native();

		bool seekPosition(const uint64_t position, const bool bHasToSucceed) override;
		bool seekForward(const uint64_t bytes, const bool bHasToSucceed) override;
		bool seekFromEnd(const uint64_t bytes, const bool bHasToSucceed) override;
		uint64_t readBuffer(uint8_t * pBuffer, const uint64_t cbTotalBytesToRead, const bool bNeedsToReadAll) override;
		uint64_t retrieveSize() override;
		uint64_t getPosition() override;
	};

}

#endif // __AMCCOMMON_IMPORTSTREAM_NATIVE
