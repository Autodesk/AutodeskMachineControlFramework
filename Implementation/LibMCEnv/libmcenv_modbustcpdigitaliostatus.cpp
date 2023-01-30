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


Abstract: This is a stub class definition of CModbusTCPDigitalIOStatus

*/

#include "libmcenv_modbustcpdigitaliostatus.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CModbusTCPDigitalIOStatus 
**************************************************************************************************************************/

CModbusTCPDigitalIOStatus::CModbusTCPDigitalIOStatus(uint32_t nBaseAddress)
    : m_nBaseAddress (nBaseAddress)
{

}

CModbusTCPDigitalIOStatus::~CModbusTCPDigitalIOStatus()
{

}

std::vector<bool>& CModbusTCPDigitalIOStatus::getInternalData()
{
    return m_Data;
}


LibMCEnv_uint32 CModbusTCPDigitalIOStatus::GetCount()
{
    return (uint32_t) m_Data.size();
}

LibMCEnv_uint32 CModbusTCPDigitalIOStatus::GetBaseAddress()
{
    return m_nBaseAddress;
}

bool CModbusTCPDigitalIOStatus::GetValue(const LibMCEnv_uint32 nIndex)
{
    if (nIndex > m_Data.size())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDMODBUSIOINDEX);

    return m_Data.at(nIndex);
}

void CModbusTCPDigitalIOStatus::GetValues(LibMCEnv_uint64 nStateArrayBufferSize, LibMCEnv_uint64* pStateArrayNeededCount, LibMCEnv_uint8 * pStateArrayBuffer)
{
    if (m_Data.size() == 0)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_MODBUSIOSTATUSEMPTY);

    if (pStateArrayNeededCount != nullptr)
        *pStateArrayNeededCount = m_Data.size();

    if (pStateArrayBuffer != nullptr) {
        if (nStateArrayBufferSize < m_Data.size ())
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

        for (size_t nIndex = 0; nIndex < m_Data.size(); nIndex++)
            pStateArrayBuffer[nIndex] = m_Data.at(nIndex) ? 1 : 0;
    }

}

