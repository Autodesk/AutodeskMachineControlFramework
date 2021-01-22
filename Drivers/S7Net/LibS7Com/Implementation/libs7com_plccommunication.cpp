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


Abstract: This is a stub class definition of CPLCCommunication

*/

#include "libs7com_plccommunication.hpp"
#include "libs7com_interfaceexception.hpp"

// Include custom headers here.


using namespace LibS7Com::Impl;

/*************************************************************************************************************************
 Class definition of CPLCCommunication 
**************************************************************************************************************************/

CPLCCommunication::CPLCCommunication()
    : m_nPLCtoAMC_Size(0), m_nPLCtoAMC_DBNo(0), m_nAMCtoPLC_DBNo (0)
{

}


void CPLCCommunication::SetProtocolConfiguration(const LibS7Com_uint32 nPLCtoAMC_DBNo, const LibS7Com_uint32 nPLCtoAMC_Size, const LibS7Com_uint32 nAMCtoPLC_DBNo)
{
    if (nPLCtoAMC_DBNo == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
    if (nPLCtoAMC_Size == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
    if (nAMCtoPLC_DBNo == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

    m_nPLCtoAMC_DBNo = nPLCtoAMC_DBNo;
    m_nPLCtoAMC_Size = nPLCtoAMC_Size;
    m_nAMCtoPLC_DBNo = nAMCtoPLC_DBNo;
}

void CPLCCommunication::StartCommunication(LibS7Net::PPLC pPLC)
{
    if (pPLC.get() == nullptr)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

    m_pPLC = pPLC;
}

void CPLCCommunication::RetrieveStatus()
{
    if ((m_pPLC.get() != nullptr) && (m_nPLCtoAMC_Size != 0)) {

        m_PLCRecvBuffer.resize(0);
       
        auto pReadData = m_pPLC->ReadBytes(m_nPLCtoAMC_DBNo, 0, m_nPLCtoAMC_Size);
        pReadData->GetData(m_PLCRecvBuffer);


    }

}

void CPLCCommunication::StopCommunication()
{
    m_pPLC = nullptr;
}

std::string CPLCCommunication::LoadProgram(const std::string& sProgram)
{
    return "";
}

void CPLCCommunication::ExecuteProgram(const std::string& sIdentifier)
{

}

void CPLCCommunication::ClearPrograms()
{

}

std::string CPLCCommunication::ReadVariableString(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nMaxLength)
{
    return "";
}

bool CPLCCommunication::ReadVariableBool(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nBit)
{
    if (nBit >= 8)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

    auto nValue = ReadVariableByte(nAddress);
    return ((nValue & (1UL << nBit)) != 0);
}

LibS7Com_uint8 CPLCCommunication::ReadVariableByte(const LibS7Com_uint32 nAddress)
{
    if (nAddress >= m_PLCRecvBuffer.size ())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    return m_PLCRecvBuffer.at(nAddress);
}

LibS7Com_int32 CPLCCommunication::ReadVariableInt32(const LibS7Com_uint32 nAddress)
{    
    if (((uint64_t) nAddress + 3) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[4];
    for (int j = 0; j < 4; j++)
        nValue[3 - j] = m_PLCRecvBuffer.at(nAddress + j);

    return *((int32_t*) &nValue[0]);
}

LibS7Com_int32 CPLCCommunication::ReadVariableUint32(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 3) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[4];
    for (int j = 0; j < 4; j++)
        nValue[3 - j] = m_PLCRecvBuffer.at(nAddress + j);

    return *((uint32_t*)&nValue[0]);
}

LibS7Com_double CPLCCommunication::ReadVariableReal(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 3) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[4];
    for (int j = 0; j < 4; j++)
        nValue[3 - j] = m_PLCRecvBuffer.at(nAddress + j);

    return *((float*)&nValue[0]);

}

