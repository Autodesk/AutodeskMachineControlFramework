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


Abstract: This is a stub class definition of CCommandParameters

*/

#include "libs7com_commandparameters.hpp"
#include "libs7com_interfaceexception.hpp"

// Include custom headers here.


using namespace LibS7Com::Impl;

/*************************************************************************************************************************
 Class definition of CCommandParameters 
**************************************************************************************************************************/

CCommandParameters::CCommandParameters(PPLCWriteBuffer pPLCWriteBuffer)
	: m_pPLCWriteBuffer (pPLCWriteBuffer)
{
	if (pPLCWriteBuffer.get() == nullptr)
		throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
}

void CCommandParameters::WriteString(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nMaxLength, const std::string& sValue) 
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

void CCommandParameters::WriteBool(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nBit, const bool bValue)
{
	m_pPLCWriteBuffer->writeBit(nAddress, nBit, bValue);
}

void CCommandParameters::WriteByte(const LibS7Com_uint32 nAddress, const LibS7Com_uint8 nValue)
{
	m_pPLCWriteBuffer->writeUint8(nAddress, nValue);
}

void CCommandParameters::WriteInt16(const LibS7Com_uint32 nAddress, const LibS7Com_int16 nValue)
{
	m_pPLCWriteBuffer->writeInt16(nAddress, nValue);
}

void CCommandParameters::WriteUint16(const LibS7Com_uint32 nAddress, const LibS7Com_uint16 nValue)
{
	m_pPLCWriteBuffer->writeUint16(nAddress, nValue);
}

void CCommandParameters::WriteInt32(const LibS7Com_uint32 nAddress, const LibS7Com_int32 nValue)
{
	m_pPLCWriteBuffer->writeInt32(nAddress, nValue);
}

void CCommandParameters::WriteUint32(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nValue)
{
	m_pPLCWriteBuffer->writeUint32(nAddress, nValue);
}

void CCommandParameters::WriteReal(const LibS7Com_uint32 nAddress, const LibS7Com_double dValue)
{
	m_pPLCWriteBuffer->writeFloat(nAddress, (float) dValue);
}

void CCommandParameters::WriteLReal(const LibS7Com_uint32 nAddress, const LibS7Com_double dValue)
{
	m_pPLCWriteBuffer->writeDouble(nAddress, (double)dValue);
}

PPLCWriteBuffer CCommandParameters::getWriteBuffer()
{
	return m_pPLCWriteBuffer;
}

