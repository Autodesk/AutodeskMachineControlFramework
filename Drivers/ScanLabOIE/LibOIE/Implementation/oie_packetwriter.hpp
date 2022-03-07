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

#ifndef __AMC_OIE_PACKETWRITER
#define __AMC_OIE_PACKETWRITER

#include <memory>
#include <string>
#include <vector>

namespace LibOIE::Impl {
	

	class CPacketWriter {
	private:
		uint32_t m_nVersion;
		uint32_t m_nType;

		std::vector<uint8_t> m_VariableHeader;

	public:

		CPacketWriter(uint32_t nType);

		void setVersion(uint32_t nVersion);

		void beginVariableHeader(const size_t nExpectedBufferSize);
		void endVariableHeader();
		void writeVariableString(const std::string sValue);
		void writeVariableBoolean(const bool bValue);
		void writeVariableUint32(const uint32_t nValue);
		void writeVariableInt32(const int32_t nValue);
		void writeVariableUint64(const uint64_t nValue);
		void writeVariableInt64(const int64_t nValue);

	};

}


#endif //__AMC_OIE_PACKETWRITER

