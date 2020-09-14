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

#ifndef __AMC_OIE_PACKETTYPES
#define __AMC_OIE_PACKETTYPES

#include <memory>
#include <string>

namespace LibOIE::Impl {

#define OIE_MAXNAMELENGTH 1024
#define OIE_MAXVARIABLEHEADERSIZE (1024 * 1024)
	
#pragma pack(push, 1)
	typedef struct {
		uint32_t m_nTypeID;
		uint64_t m_nVariableHeaderSize;
		//uint64_t m_nPayloadSize;
		//uint32_t m_nCRC32;
		//uint32_t m_nSequenceNumber;
		uint32_t m_nVersion;
	} sOIECommandFixedHeader;
#pragma pack(pop)

	enum class ePacketType {
		UndefinedType = 0,
		LoginRequest = 15,
		LoginReply = 16,
		FirmwareInfoRequest = 17,
		FirmwareInfoReply = 18,
		UpdateFirmwareRequest = 19,
		UpdateFirmwareReply = 20,
		StartFirmwareRequest = 21,
		StartFirmwareReply = 22,
		StopFirmwareRequest = 23,
		StopFirmwareReply = 24,
		ErrorMsg = 27,
		AliveRequest = 29,
		AliveReply = 30,
		RemoteFunctionCall = 31,
		RemoteFunctionCallReturnReceive = 32,
		DeviceStatusRequest = 35,
		DeviceStatusReply = 36,
		PyroDataPackage = 40,
		OctDataPackage = 41
	};

}


#endif //__AMC_OIE_PACKETTYPES

