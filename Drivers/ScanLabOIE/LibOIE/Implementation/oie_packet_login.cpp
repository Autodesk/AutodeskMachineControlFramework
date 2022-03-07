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

#include "oie_packet_login.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;


CPacket_Login::CPacket_Login(uint32_t nSequenceNumber, const std::string& sDeviceName, const std::string& sApplicationName, const std::string& sApplicationVersion, bool bLoginStatus)
	: CPacket (nSequenceNumber), m_sDeviceName (sDeviceName), m_sApplicationName (sApplicationName), m_sApplicationVersion (sApplicationVersion), m_bLoginStatus (bLoginStatus)
{
	if (m_sDeviceName.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sApplicationName.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sApplicationVersion.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
}

CPacket_Login::CPacket_Login(CPacketReader& pReader)
	: CPacket (pReader.getSequenceNumber ()), m_bLoginStatus (false)
{

	pReader.beginVariableHeader();

	auto nVersion = pReader.getPacketVersion();
	switch (nVersion) {
	case 1:
		m_sDeviceName = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sApplicationName = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sApplicationVersion = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_bLoginStatus = pReader.readVariableBoolean();
		pReader.checkNoPayload();
		break;

	default:
		throw ELibOIEInterfaceException(LIBOIE_ERROR_UNSUPPORTEDPACKETVERSION, "Login #" + std::to_string (nVersion));

	}

	pReader.endVariableHeader();


}

CPacket_Login::~CPacket_Login()
{

}


void CPacket_Login::serialize(CPacketWriter& packetWriter)
{	
	packetWriter.setVersion (1);
	packetWriter.beginVariableHeader(OIE_MAXNAMELENGTH * 3 + 1);
	packetWriter.writeVariableString(m_sDeviceName);
	packetWriter.writeVariableString(m_sApplicationName);
	packetWriter.writeVariableString(m_sApplicationVersion);
	packetWriter.writeVariableBoolean(m_bLoginStatus);
	packetWriter.endVariableHeader();
}


CPacket_LoginRequest::CPacket_LoginRequest(CPacketReader& pReader)
	: CPacket_Login (pReader)
{
}

ePacketType CPacket_LoginRequest::getType()
{
	return ePacketType::LoginRequest;
}


CPacket_LoginReply::CPacket_LoginReply(CPacketReader& pReader)
	: CPacket_Login(pReader)
{
}

ePacketType CPacket_LoginReply::getType()
{
	return ePacketType::LoginReply;
}

