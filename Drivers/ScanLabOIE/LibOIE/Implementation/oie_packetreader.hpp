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

#ifndef __AMC_OIE_PACKETREADER
#define __AMC_OIE_PACKETREADER

#include <memory>
#include <string>

namespace LibOIE::Impl {
	

	class CPacketReader {
	private:
		const uint8_t * m_pVariableHeaderData;
		uint64_t m_nVariableHeaderSize;
		
		const uint8_t * m_pPayloadData;
		uint64_t m_nPayloadDataSize;

		uint32_t m_nPacketType;
		uint32_t m_nPacketVersion;
		uint32_t m_nSequenceNumber;

		uint32_t m_nVariableReadIndex;

		const uint8_t * readVariableBytesSafe (uint32_t nCount);

	public:
	
		CPacketReader (const uint8_t * pPacketData, uint64_t nDataSize);
		virtual ~CPacketReader();
		
		uint32_t getPacketType();
		uint32_t getPacketVersion();
		uint32_t getSequenceNumber();

		void beginVariableHeader();
		void endVariableHeader();

		bool readVariableBoolean();
		uint32_t readVariableUInt32 (const uint32_t nMinValue, const uint32_t nMaxValue);
		int32_t readVariableInt32(const int32_t nMinValue, const int32_t nMaxValue);
		uint64_t readVariableUInt64(const uint64_t nMinValue, const uint64_t nMaxValue);
		int64_t readVariableInt64(const int64_t nMinValue, const int64_t nMaxValue);
		std::string readVariableString(const uint32_t nMaxLength);

		bool hasPayload();
		void checkNoPayload();
	
	};

}


#endif //__AMC_OIE_PACKETREADER

