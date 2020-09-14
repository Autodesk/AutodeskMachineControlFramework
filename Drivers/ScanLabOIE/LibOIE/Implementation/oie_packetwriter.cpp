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

#include "oie_packetwriter.hpp"
#include "oie_packettypes.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;


CPacketWriter::CPacketWriter()
	: m_nVersion (0)
{
	
}

void CPacketWriter::setVersion(uint32_t nVersion)
{
	if (nVersion == 0)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPACKETVERSION);

	if (m_nVersion != 0)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_PACKETVERSIONALREADYWRITTEN);

	m_nVersion = nVersion;
}

void CPacketWriter::beginVariableHeader(const size_t nExpectedBufferSize)
{
	if (m_VariableHeader.size() != 0)
		throw ELibOIEInterfaceException (LIBOIE_ERROR_VARIABLEHEADERALREADYWRITTEN);
	m_VariableHeader.reserve(nExpectedBufferSize);
}

void CPacketWriter::endVariableHeader()
{
	if (m_VariableHeader.size() == 0)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_NOVARIABLEHEADERCONTENT);
}

void CPacketWriter::writeVariableString(const std::string sValue)
{
	if (sValue.length() > OIE_MAXNAMELENGTH)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);

	writeVariableUint32((uint32_t)sValue.length());
	for (const char & c : sValue) 
		m_VariableHeader.push_back((uint8_t) c);
}

void CPacketWriter::writeVariableBoolean(const bool bValue)
{
	if (bValue)
		m_VariableHeader.push_back(1);
	else
		m_VariableHeader.push_back(0);

}

void CPacketWriter::writeVariableUint32(const uint32_t nValue)
{
	const uint8_t* pValue = (const uint8_t*)&nValue;
	for (uint32_t nIndex = 0; nIndex < 4; nIndex++) {
		m_VariableHeader.push_back(*pValue);
		pValue++;
	}
}

void CPacketWriter::writeVariableInt32(const int32_t nValue)
{
	const uint8_t* pValue = (const uint8_t*)&nValue;
	for (uint32_t nIndex = 0; nIndex < 4; nIndex++) {
		m_VariableHeader.push_back(*pValue);
		pValue++;
	}
}

