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

#include "oie_packet_devicestatus.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;


CPacket_DeviceStatus::CPacket_DeviceStatus(uint32_t nSequenceNumber, const std::string& sDeviceType, const std::string& sDeviceName, const std::string& sDeviceVersion, const std::string& sDeviceStatus)
	: CPacket (nSequenceNumber), m_sDeviceType(sDeviceType), m_sDeviceName (sDeviceName), m_sDeviceVersion(sDeviceVersion), m_sDeviceStatus(sDeviceStatus)
{
	if (m_sDeviceType.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sDeviceName.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sDeviceVersion.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
	if (m_sDeviceStatus.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);
}

CPacket_DeviceStatus::CPacket_DeviceStatus(CPacketReader& pReader)
	: CPacket (pReader.getSequenceNumber ())
{

	pReader.beginVariableHeader();

	auto nVersion = pReader.getPacketVersion();
	switch (nVersion) {
	case 1:
		m_sDeviceType = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sDeviceName = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sDeviceVersion = pReader.readVariableString(OIE_MAXNAMELENGTH);
		m_sDeviceStatus = pReader.readVariableString(OIE_MAXNAMELENGTH);
		pReader.checkNoPayload();
		break;

	default:
		throw ELibOIEInterfaceException(LIBOIE_ERROR_UNSUPPORTEDPACKETVERSION, "DeviceStatus #" + std::to_string (nVersion));

	}

	pReader.endVariableHeader();


}

CPacket_DeviceStatus::~CPacket_DeviceStatus()
{

}


void CPacket_DeviceStatus::serialize(CPacketWriter& packetWriter)
{	
	packetWriter.setVersion (1);
	packetWriter.beginVariableHeader(OIE_MAXNAMELENGTH * 4);
	packetWriter.writeVariableString(m_sDeviceType);
	packetWriter.writeVariableString(m_sDeviceName);
	packetWriter.writeVariableString(m_sDeviceVersion);
	packetWriter.writeVariableString(m_sDeviceStatus);
	packetWriter.endVariableHeader();
}


CPacket_DeviceStatusRequest::CPacket_DeviceStatusRequest(CPacketReader& pReader)
	: CPacket_DeviceStatus (pReader)
{

}

ePacketType CPacket_DeviceStatusRequest::getType()
{
	return ePacketType::DeviceStatusRequest;
}

CPacket_DeviceStatusReply::CPacket_DeviceStatusReply(CPacketReader& pReader)
	: CPacket_DeviceStatus(pReader)
{

}

ePacketType CPacket_DeviceStatusReply::getType()
{
	return ePacketType::DeviceStatusReply;
}

