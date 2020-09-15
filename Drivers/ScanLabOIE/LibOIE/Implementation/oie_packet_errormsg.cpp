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

#include "oie_packet_errormsg.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;

CPacket_ErrorMsg::CPacket_ErrorMsg(uint32_t nSequenceNumber, const std::string& sErrorMessage)
	: CPacket (nSequenceNumber), m_sErrorMessage (sErrorMessage)
{
	if (m_sErrorMessage.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
}

CPacket_ErrorMsg::CPacket_ErrorMsg(CPacketReader& pReader)
	: CPacket (pReader.getSequenceNumber ())
{

	pReader.beginVariableHeader();

	auto nVersion = pReader.getPacketVersion();
	switch (nVersion) {
	case 1:
		m_sErrorMessage = pReader.readVariableString(OIE_MAXNAMELENGTH);
		pReader.checkNoPayload();
		break;

	default:
		throw ELibOIEInterfaceException(LIBOIE_ERROR_UNSUPPORTEDPACKETVERSION, "ErrorMsg #" + std::to_string (nVersion));

	}

	pReader.endVariableHeader();


}

CPacket_ErrorMsg::~CPacket_ErrorMsg()
{

}


void CPacket_ErrorMsg::serialize(CPacketWriter& packetWriter)
{	
	packetWriter.setVersion (1);
	packetWriter.beginVariableHeader(OIE_MAXNAMELENGTH);
	packetWriter.writeVariableString(m_sErrorMessage);
	packetWriter.endVariableHeader();
}


ePacketType CPacket_ErrorMsg::getType()
{
	return ePacketType::ErrorMsg;
}

