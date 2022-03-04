/*++

Copyright (C) 2022 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Abstract: This is a stub class definition of CDriver_TCPIPPacket

*/

#include "libmcdriver_tcpip_driver_tcpippacket.hpp"
#include "libmcdriver_tcpip_interfaceexception.hpp"

// Include custom headers here.
#define TCPIPDRIVER_MAXPACKETSIZE (1024UL * 1024UL * 1024UL)

using namespace LibMCDriver_TCPIP::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_TCPIPPacket 
**************************************************************************************************************************/
CDriver_TCPIPPacket::CDriver_TCPIPPacket()
{

}

CDriver_TCPIPPacket::~CDriver_TCPIPPacket()
{

}

LibMCDriver_TCPIP_uint32 CDriver_TCPIPPacket::GetSize()
{
    if (m_BufferData.size() > TCPIPDRIVER_MAXPACKETSIZE)
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_RECEIVECOUNTEXCEEDSMAXIMUM);

    return (uint32_t) m_BufferData.size();
}

void CDriver_TCPIPPacket::GetData(LibMCDriver_TCPIP_uint64 nBufferBufferSize, LibMCDriver_TCPIP_uint64* pBufferNeededCount, LibMCDriver_TCPIP_uint8 * pBufferBuffer)
{
    uint64_t nPacketSize = m_BufferData.size();
    if (nPacketSize > TCPIPDRIVER_MAXPACKETSIZE)
        throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_RECEIVECOUNTEXCEEDSMAXIMUM);


    if (pBufferNeededCount != nullptr)
        *pBufferNeededCount = nPacketSize;

    if (pBufferBuffer != nullptr) {

        if (nBufferBufferSize < nPacketSize)
            throw ELibMCDriver_TCPIPInterfaceException(LIBMCDRIVER_TCPIP_ERROR_BUFFERTOOSMALL);

        
        if (nPacketSize > 0) {
            uint8_t* pSrc = m_BufferData.data();
            uint8_t* pDst = pBufferBuffer;
            for (uint64_t nIndex = 0; nIndex < nPacketSize; nIndex++) {
                *pDst = *pSrc;
                pDst++;
                pSrc++;
            }
        }
    }
}

std::vector<uint8_t>& CDriver_TCPIPPacket::getBufferDataReference()
{
    return m_BufferData;
}
