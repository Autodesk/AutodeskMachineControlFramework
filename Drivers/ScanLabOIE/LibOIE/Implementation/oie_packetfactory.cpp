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

#include "oie_packetfactory.hpp"
#include "liboie_interfaceexception.hpp"

#include "oie_packet_login.hpp"
#include "oie_packet_alive.hpp"
#include "oie_packet_firmwareinfo.hpp"
#include "oie_packet_startfirmware.hpp"
#include "oie_packet_stopfirmware.hpp"
#include "oie_packet_devicestatus.hpp"
#include "oie_packet_errormsg.hpp"

using namespace LibOIE::Impl;

PPacket CPacketFactory::makePacket(CPacketReader& packetReader)
{
	ePacketType packetType = (ePacketType)packetReader.getPacketType();
	switch (packetType) {
		case ePacketType::LoginRequest: return std::make_shared<CPacket_LoginRequest> (packetReader);
		case ePacketType::LoginReply: return std::make_shared<CPacket_LoginReply>(packetReader);
		case ePacketType::FirmwareInfoRequest: return std::make_shared<CPacket_FirmwareInfoRequest>(packetReader);
		case ePacketType::FirmwareInfoReply: return std::make_shared<CPacket_FirmwareInfoReply>(packetReader);
		case ePacketType::StartFirmwareRequest: return std::make_shared<CPacket_StartFirmwareRequest>(packetReader);
		case ePacketType::StartFirmwareReply: return std::make_shared<CPacket_StartFirmwareReply>(packetReader);
		case ePacketType::StopFirmwareRequest: return std::make_shared<CPacket_StopFirmwareRequest>(packetReader);
		case ePacketType::StopFirmwareReply: return std::make_shared<CPacket_StopFirmwareReply>(packetReader);
		case ePacketType::AliveRequest: return std::make_shared<CPacket_AliveRequest>(packetReader);
		case ePacketType::AliveReply: return std::make_shared<CPacket_AliveReply>(packetReader);
		case ePacketType::DeviceStatusRequest: return std::make_shared<CPacket_DeviceStatusRequest>(packetReader);
		case ePacketType::DeviceStatusReply: return std::make_shared<CPacket_DeviceStatusReply>(packetReader);
		case ePacketType::ErrorMsg: return std::make_shared<CPacket_ErrorMsg>(packetReader);

	}

	throw ELibOIEInterfaceException (LIBOIE_ERROR_UNKNOWNPACKETTYPE);
}

