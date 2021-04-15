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


Abstract: This is the class declaration of CCommandParameters

*/


#ifndef __LIBS7COM_COMMANDPARAMETERS
#define __LIBS7COM_COMMANDPARAMETERS

#include "libs7com_interfaces.hpp"
#include "libs7com_interfaceexception.hpp"

// Parent classes
#include "libs7com_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibS7Com {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CCommandParameters 
**************************************************************************************************************************/

    class CPLCWriteBuffer {
    private:
        uint32_t m_nDBNo;
        uint32_t m_nDBSize;
        std::vector <uint8_t> m_Buffer;

    public:
        CPLCWriteBuffer(const uint32_t nDBNo, const uint32_t nDBSize)
            : m_nDBNo(nDBNo), m_nDBSize(nDBSize)
        {
            m_Buffer.resize(nDBSize);
        }

        void send(LibS7Net::CPLC* pPLC)
        {
            if (pPLC == nullptr)
                throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

            pPLC->WriteBytes(m_nDBNo, 0, m_Buffer);
        }

        void writeBit(const uint32_t nAddress, const uint32_t nBit, const bool bValue)
        {
            if (((uint64_t)nAddress < m_Buffer.size()) && (nBit < 8)) {
                if (bValue) {
                    m_Buffer[nAddress] |= (1UL << nBit);
                }
                else {
                    m_Buffer[nAddress] &= ((1UL << nBit) ^ 0xff);
                }
            }
        }


        void writeUint8(const uint32_t nAddress, const uint8_t nValue)
        {
            if ((uint64_t)nAddress < m_Buffer.size()) {
                m_Buffer[(uint64_t)nAddress + 0] = nValue;
            }
        }

        void writeInt16(const uint32_t nAddress, const int16_t nValue)
        {
            writeUint16(nAddress, (uint16_t)nValue);
        }

        void writeUint16(const uint32_t nAddress, const uint16_t nValue)
        {
            if ((uint64_t)nAddress + 2 <= m_Buffer.size()) {
                m_Buffer[(uint64_t)nAddress + 0] = (nValue >> 8) & 0xff;
                m_Buffer[(uint64_t)nAddress + 1] = nValue & 0xff;
            }
        }

        void writeInt32(const uint32_t nAddress, const int32_t nValue)
        {
            writeUint32(nAddress, (uint32_t)nValue);
        }

        void writeUint32(const uint32_t nAddress, const uint32_t nValue)
        {
            if ((uint64_t)nAddress + 4 <= m_Buffer.size()) {
                m_Buffer[nAddress + 0] = (nValue >> 24) & 0xff;
                m_Buffer[(uint64_t)nAddress + 1] = (nValue >> 16) & 0xff;
                m_Buffer[(uint64_t)nAddress + 2] = (nValue >> 8) & 0xff;
                m_Buffer[(uint64_t)nAddress + 3] = nValue & 0xff;
            }
        }

        void writeFloat(const uint32_t nAddress, float fValue)
        {
            uint32_t nValue = *((uint32_t*)((void*)&fValue));

            if ((uint64_t)nAddress + 4 <= m_Buffer.size()) {
                m_Buffer[nAddress + 0] = (nValue >> 24) & 0xff;
                m_Buffer[(uint64_t)nAddress + 1] = (nValue >> 16) & 0xff;
                m_Buffer[(uint64_t)nAddress + 2] = (nValue >> 8) & 0xff;
                m_Buffer[(uint64_t)nAddress + 3] = nValue & 0xff;
            }
        }

        void writeDouble(const uint32_t nAddress, double dValue)
        {
            uint64_t nValue = *((uint64_t*)((void*)&dValue));

            if ((uint64_t)nAddress + 8 <= m_Buffer.size()) {
                m_Buffer[(uint64_t)nAddress + 0] = (nValue >> 56) & 0xff;
                m_Buffer[(uint64_t)nAddress + 1] = (nValue >> 48) & 0xff;
                m_Buffer[(uint64_t)nAddress + 2] = (nValue >> 40) & 0xff;
                m_Buffer[(uint64_t)nAddress + 3] = (nValue >> 32) & 0xff;
                m_Buffer[(uint64_t)nAddress + 4] = (nValue >> 24) & 0xff;
                m_Buffer[(uint64_t)nAddress + 5] = (nValue >> 16) & 0xff;
                m_Buffer[(uint64_t)nAddress + 6] = (nValue >> 8) & 0xff;
                m_Buffer[(uint64_t)nAddress + 7] = nValue & 0xff;
            }
        }

    };


    typedef std::shared_ptr <CPLCWriteBuffer> PPLCWriteBuffer;


class CCommandParameters : public virtual ICommandParameters, public virtual CBase {
private:

    PPLCWriteBuffer m_pPLCWriteBuffer;

protected:

public:

    CCommandParameters (PPLCWriteBuffer pPLCWriteBuffer);

    void WriteString(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nMaxLength, const std::string& sValue) override;

	void WriteBool(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nBit, const bool bValue) override;

	void WriteByte(const LibS7Com_uint32 nAddress, const LibS7Com_uint8 nValue) override;

	void WriteInt16(const LibS7Com_uint32 nAddress, const LibS7Com_int16 nValue) override;

	void WriteUint16(const LibS7Com_uint32 nAddress, const LibS7Com_uint16 nValue) override;

	void WriteInt32(const LibS7Com_uint32 nAddress, const LibS7Com_int32 nValue) override;

	void WriteUint32(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nValue) override;

	void WriteReal(const LibS7Com_uint32 nAddress, const LibS7Com_double dValue) override;

	void WriteLReal(const LibS7Com_uint32 nAddress, const LibS7Com_double dValue) override;

    PPLCWriteBuffer getWriteBuffer ();

};

} // namespace Impl
} // namespace LibS7Com

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBS7COM_COMMANDPARAMETERS
