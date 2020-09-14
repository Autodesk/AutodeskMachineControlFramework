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

#include "oie_packetreader.hpp"
#include "oie_packettypes.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;

#define OIE_VARIABLEREADINDEX_INVALID 0xffffffff

CPacketReader::CPacketReader(const uint8_t* pPacketData, uint64_t nDataSize)
  :	m_nPacketType (0), 
	m_nPacketVersion (0), 
	m_nSequenceNumber (0), 
	m_pVariableHeaderData (nullptr),
	m_nVariableHeaderSize (0),
	m_pPayloadData (nullptr),
	m_nPayloadDataSize (0),
	m_nVariableReadIndex (OIE_VARIABLEREADINDEX_INVALID)

{
	if (pPacketData == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPARAM);

	uint64_t nHeaderSize = sizeof(sOIECommandFixedHeader);
	if (nDataSize < nHeaderSize)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INSUFFICIENTPACKETDATA);

	const sOIECommandFixedHeader* pHeader = (const sOIECommandFixedHeader*)pPacketData;
	
	m_nPacketType = pHeader->m_nTypeID;
	m_nPacketVersion = pHeader->m_nVersion;
	// TODO: m_nSequenceNumber = pHeader->m_nSequenceNumber;
	
	uint64_t nVariableSize = (nDataSize - nHeaderSize);
	if (pHeader->m_nVariableHeaderSize < nVariableSize)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INSUFFICIENTPACKETDATA);

	if (nVariableSize > 0) {
		m_nVariableHeaderSize = pHeader->m_nVariableHeaderSize;
		m_nPayloadDataSize = nVariableSize - pHeader->m_nVariableHeaderSize;

		if (m_nVariableHeaderSize > OIE_MAXVARIABLEHEADERSIZE)
			throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDVARIABLEHEADERSIZE);

		if (nDataSize != (nHeaderSize + m_nVariableHeaderSize + m_nPayloadDataSize))
			throw ELibOIEInterfaceException(LIBOIE_ERROR_INTERNALERROR);

		if (m_nVariableHeaderSize > 0)
			m_pVariableHeaderData = &pPacketData[nHeaderSize];

		if (m_nPayloadDataSize > 0)
			m_pPayloadData = &pPacketData[nHeaderSize + m_nVariableHeaderSize];

	}

}


CPacketReader::~CPacketReader()
{
}


uint32_t CPacketReader::getPacketType()
{
	return m_nPacketType;
}

uint32_t CPacketReader::getPacketVersion()
{
	return m_nPacketVersion;
}

uint32_t CPacketReader::getSequenceNumber()
{
	return m_nSequenceNumber;
}

void CPacketReader::beginVariableHeader()
{
	if (m_nVariableReadIndex != OIE_VARIABLEREADINDEX_INVALID)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_READINGPACKETHEADERALREADY);

	if (m_nVariableHeaderSize == 0)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEHEADERREADERROR);
	if (m_pVariableHeaderData == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEHEADERREADERROR);

	m_nVariableReadIndex = 0;
}

void CPacketReader::endVariableHeader()
{
	if ((uint64_t) m_nVariableReadIndex != m_nVariableHeaderSize)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEDATANOTFULLYREAD);
	m_nVariableReadIndex = OIE_VARIABLEREADINDEX_INVALID;
}

bool CPacketReader::readVariableBoolean()
{
	const uint8_t* pValue = readVariableBytesSafe(1);
	if (pValue == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INTERNALERROR);
	uint8_t nValue = *pValue;

	if (nValue == 1)
		return true;
	if (nValue == 0)
		return true;

	throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEDATAINVALIDBOOLEANVALUE);
}

uint32_t CPacketReader::readVariableUInt32(const uint32_t nMinValue, const uint32_t nMaxValue)
{
	const uint32_t* pValue = (const uint32_t *) readVariableBytesSafe(4);
	if (pValue == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INTERNALERROR);
	uint32_t nValue = *pValue;

	if (*pValue < nMinValue)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEVALUEEXCEEDSMINIMUM);
	if (*pValue > nMaxValue)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEVALUEEXCEEDSMAXIMUM);

	return nValue;
}

int32_t CPacketReader::readVariableInt32(const int32_t nMinValue, const int32_t nMaxValue)
{
	const int32_t* pValue = (const int32_t*)readVariableBytesSafe(4);
	if (pValue == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INTERNALERROR);
	int32_t nValue = *pValue;

	if (*pValue < nMinValue)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEVALUEEXCEEDSMINIMUM);
	if (*pValue > nMaxValue)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEVALUEEXCEEDSMAXIMUM);

	return nValue;
}


std::string CPacketReader::readVariableString(const uint32_t nMaxLength)
{
	const uint32_t* pLength = (const uint32_t*)readVariableBytesSafe(4);
	if (pLength == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INTERNALERROR);
	uint32_t nLength = *pLength;

	if (nLength > nMaxLength)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_STRINGEXCEEDSCHARACTERLIMIT);

	auto iStrBegin = static_cast<const char *> ((const char* ) readVariableBytesSafe(nLength));
	auto iStrEnd = iStrBegin + nLength;

	return std::string(iStrBegin, iStrEnd);
}

bool CPacketReader::hasPayload()
{
	return (m_nPayloadDataSize > 0);
}

void CPacketReader::checkNoPayload()
{
	if (hasPayload ())
		throw ELibOIEInterfaceException(LIBOIE_ERROR_PACKETSHOULDNOTHAVEPAYLOAD);
}

const uint8_t* CPacketReader::readVariableBytesSafe(uint32_t nCount)
{
	if (m_nVariableReadIndex >= m_nVariableHeaderSize)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEHEADERREADERROR);

	if ((((uint64_t)m_nVariableReadIndex) + nCount) > m_nVariableHeaderSize)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEHEADERREADERROR);
	if (m_pVariableHeaderData == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_VARIABLEHEADERREADERROR);

	const uint8_t* pData = &m_pVariableHeaderData[m_nVariableReadIndex];
	m_nVariableReadIndex += nCount;

	return pData;
}

