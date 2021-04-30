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

#include "oie_packet_firmwareinfo.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;


CPacket_FirmwareInfo::CPacket_FirmwareInfo(uint32_t nSequenceNumber, const std::string& sDeviceName, const std::string& sFirmwareName, const std::string& sFirmwareVersion, const std::string& sFilename, const uint64_t nFileDate)
	: CPacket (nSequenceNumber), m_sDeviceName (sDeviceName), m_sFirmwareName(sFirmwareName), m_sFirmwareVersion(sFirmwareVersion), m_sFilename(sFilename), m_nFileDate(nFileDate)
{
	if (m_sDeviceName.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sFirmwareName.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sFirmwareVersion.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sFilename.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
}

CPacket_FirmwareInfo::CPacket_FirmwareInfo(CPacketReader& pReader)
	: CPacket (pReader.getSequenceNumber ()), m_nFileDate (0)
{

	pReader.beginVariableHeader();

	auto nVersion = pReader.getPacketVersion();
	switch (nVersion) {
	case 1:
		m_sDeviceName = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sFirmwareName = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sFirmwareVersion = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sFilename = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_nFileDate = pReader.readVariableUInt64(0, 0xFFFFFFFFFFFFFFFF);
		pReader.checkNoPayload();
		break;

	default:
		throw ELibOIEInterfaceException(LIBOIE_ERROR_UNSUPPORTEDPACKETVERSION, "FirmwareInfo #" + std::to_string (nVersion));

	}

	pReader.endVariableHeader();


}

CPacket_FirmwareInfo::~CPacket_FirmwareInfo()
{

}


void CPacket_FirmwareInfo::serialize(CPacketWriter& packetWriter)
{	
	packetWriter.setVersion (1);
	packetWriter.beginVariableHeader(OIE_MAXNAMELENGTH * 3 + 1);
	packetWriter.writeVariableString(m_sDeviceName);
	packetWriter.writeVariableString(m_sFirmwareName);
	packetWriter.writeVariableString(m_sFirmwareVersion);
	packetWriter.writeVariableString(m_sFilename);
	packetWriter.writeVariableUint64(m_nFileDate);
	packetWriter.endVariableHeader();
}


CPacket_FirmwareInfoRequest::CPacket_FirmwareInfoRequest(CPacketReader& pReader)
	: CPacket_FirmwareInfo (pReader)
{

}


ePacketType CPacket_FirmwareInfoRequest::getType()
{
	return ePacketType::FirmwareInfoRequest;
}



CPacket_FirmwareInfoReply::CPacket_FirmwareInfoReply(CPacketReader& pReader)
	: CPacket_FirmwareInfo(pReader)
{

}

ePacketType CPacket_FirmwareInfoReply::getType()
{
	return ePacketType::FirmwareInfoReply;
}

