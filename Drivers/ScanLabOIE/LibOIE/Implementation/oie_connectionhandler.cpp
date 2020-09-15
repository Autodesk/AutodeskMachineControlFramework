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

#include "oie_connectionhandler.hpp"
#include "oie_packet.hpp"
#include "oie_packettypes.hpp"
#include "oie_packetfactory.hpp"
#include "oie_packetreader.hpp"
#include "liboie_interfaceexception.hpp"

using namespace LibOIE::Impl;



CConnectionHandler::CConnectionHandler(const uint64_t nID, const std::string& sIPAddress)
	: m_nID (nID), 
	  m_sIPAddress (sIPAddress), 
	  m_bNeedsToTerminate (false),
	  m_CurrentBuffer (m_Buffer1),
	  m_OtherBuffer (m_Buffer2)
{

}

CConnectionHandler::~CConnectionHandler()
{

}

uint64_t CConnectionHandler::getID()
{
	return m_nID;
}

void CConnectionHandler::receivedData(const uint8_t* pData, size_t nSize)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	if (pData == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPARAM);

	if (nSize > 0) {

		const uint8_t * pReadStart;
		size_t nReadSize;
		bool bHasBeenPushed;

		if (m_CurrentBuffer.empty()) { // If we have no buffered memory, just use the passed array.
			pReadStart = pData; 
			nReadSize = nSize;
			bHasBeenPushed = false; // Data has been stored already
		}
		else {
			pushToBuffer(m_CurrentBuffer, pData, nSize);
			pReadStart = m_CurrentBuffer.data();
			nReadSize = m_CurrentBuffer.size();
			bHasBeenPushed = true;
		}

		size_t nPacketSize; 		
		if (predictPacketSize(pReadStart, nReadSize, nPacketSize)) {
			if (nPacketSize > nReadSize)
				throw ELibOIEInterfaceException(LIBOIE_ERROR_INTERNALERROR);

			CPacketReader packetReader(pReadStart, nReadSize);
			auto pPacket = CPacketFactory::makePacket (packetReader);
			handlePacket(pPacket);

			if (nPacketSize == nReadSize) {
				m_CurrentBuffer.clear();
				m_OtherBuffer.clear();
			}
			else {
				// Write Next Packet to Other buffer
				pushToBuffer(m_OtherBuffer, (pData + nPacketSize), nReadSize - nPacketSize); 

				// Switch Buffers
				auto Buffer = m_CurrentBuffer; 
				m_CurrentBuffer = m_OtherBuffer;
				m_OtherBuffer = Buffer;
				m_OtherBuffer.clear();

			}


		}
		else {
			if (!bHasBeenPushed)
				pushToBuffer(m_CurrentBuffer, pData, nSize);

		}

	}

}

void CConnectionHandler::handlePacket(PPacket pPacket)
{
	if (pPacket.get() == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPARAM);


}




bool CConnectionHandler::predictPacketSize(const uint8_t* pReadData, size_t nReadSize, size_t& predictedSize)
{
	if (pReadData == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPARAM);

	predictedSize = OIE_INVALIDPACKETSIZE;

	if (sizeof(sOIECommandFixedHeader) > nReadSize)
		return false;

	const sOIECommandFixedHeader* pHeader = (const sOIECommandFixedHeader*)pReadData;
	ePacketType packetType = (ePacketType)pHeader->m_nTypeID;	

	bool bHasPayload = packetTypeHasPayload(packetType);
	if (bHasPayload) {
		size_t nPayLoadStart = sizeof(sOIECommandFixedHeader) + pHeader->m_nVariableHeaderSize;
		
		if (nReadSize < (nPayLoadStart + sizeof(uint64_t)))
			return false;

		const uint64_t* pPayLoadSize = (const uint64_t *) (pReadData + nPayLoadStart);
		uint64_t nPayLoadSize = *pPayLoadSize;

		if (nPayLoadSize > OIE_MAXPACKETSIZE)
			throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPAYLOADSIZE);
		if (nPayLoadSize < sizeof(uint64_t))
			throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPAYLOADSIZE);

		predictedSize = sizeof(sOIECommandFixedHeader) + pHeader->m_nVariableHeaderSize + nPayLoadSize;
		if (predictedSize > OIE_MAXPACKETSIZE)
			throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPACKETSIZE);

		return (predictedSize <= nReadSize);

	}
	else {
		predictedSize = sizeof(sOIECommandFixedHeader) + pHeader->m_nVariableHeaderSize;
		if (predictedSize > OIE_MAXPACKETSIZE)
			throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPACKETSIZE);

		return (predictedSize <= nReadSize);
	}

}




void CConnectionHandler::pushToBuffer(std::vector<uint8_t>& Buffer, const uint8_t* pData, size_t nSize)
{
	if (pData == nullptr)
		throw ELibOIEInterfaceException(LIBOIE_ERROR_INVALIDPARAM);

	if (nSize > 0) {
		for (size_t nIndex = 0; nIndex < nSize; nIndex++) {
			Buffer.push_back(pData[nIndex]);
		}
	}

}



bool CConnectionHandler::needsToTerminate()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	return m_bNeedsToTerminate;
}

std::string CConnectionHandler::getIPAddress()
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	return std::string (m_sIPAddress.c_str());
}
