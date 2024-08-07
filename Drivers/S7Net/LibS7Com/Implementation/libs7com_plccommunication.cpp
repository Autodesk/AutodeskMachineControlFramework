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
#include "libs7com_commandparameters.hpp"

// Include custom headers here.


using namespace LibS7Com::Impl;



/*************************************************************************************************************************
 Class definition of CPLCCommunication 
**************************************************************************************************************************/

CPLCCommunication::CPLCCommunication()
    : m_nPLCtoAMC_Size(0), 
    m_nAMCtoPLC_Size (0),
    m_nPLCtoAMC_DBNo(0), 
    m_nAMCtoPLC_DBNo (0), 
    m_nCmdCycleCounter (1),
    m_nMajorVersion (0),
    m_nMinorVersion (0),
    m_nPatchVersion (0),
    m_nAMCtoPLC_MajorVersionAddress (0),
    m_nAMCtoPLC_MinorVersionAddress (0),
    m_nAMCtoPLC_PatchVersionAddress (0),
    m_nAMCtoPLC_BuildVersionAddress (0),
    m_nAMCtoPLC_CommandSequenceAddress(0),
    m_nAMCtoPLC_CommandIDAddress(0),
    m_nAMCtoPLC_CommandChecksumAddress(0),
    m_nPLCtoAMC_MajorVersionAddress(0),
    m_nPLCtoAMC_MinorVersionAddress(0),
    m_nPLCtoAMC_PatchVersionAddress(0),
    m_nPLCtoAMC_BuildVersionAddress(0),
    m_nPLCtoAMC_SequenceRunningAddress(0),
    m_nPLCtoAMC_SequenceFinishedAddress(0),
    m_nPLCtoAMC_SequenceStatusAddress(0),
    m_nPLCtoAMC_SequenceErrorAddress(0)

{

}


void CPLCCommunication::SetProtocolConfiguration(const LibS7Com_uint32 nMajorVersion, const LibS7Com_uint32 nMinorVersion, const LibS7Com_uint32 nPatchVersion, const LibS7Com_uint32 nPLCtoAMC_DBNo, const LibS7Com_uint32 nPLCtoAMC_Size, const LibS7Com_uint32 nAMCtoPLC_DBNo, const LibS7Com_uint32 nAMCtoPLC_Size)
{
    if (nPLCtoAMC_DBNo == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
    if (nPLCtoAMC_Size == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
    if (nAMCtoPLC_DBNo == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
    if (nAMCtoPLC_Size == 0)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

    m_nPLCtoAMC_DBNo = nPLCtoAMC_DBNo;
    m_nPLCtoAMC_Size = nPLCtoAMC_Size;
    m_nAMCtoPLC_DBNo = nAMCtoPLC_DBNo;
    m_nAMCtoPLC_Size = nAMCtoPLC_Size;
    m_nMajorVersion = nMajorVersion;
    m_nMinorVersion = nMinorVersion;
    m_nPatchVersion = nPatchVersion;
}


void CPLCCommunication::SetAMCTOPLCOffsets(const LibS7Com_uint32 nMajorVersionAddress, const LibS7Com_uint32 nMinorVersionAddress, const LibS7Com_uint32 nPatchVersionAddress, const LibS7Com_uint32 nBuildVersionAddress, const LibS7Com_uint32 nCommandSequenceAddress, const LibS7Com_uint32 nCommandIDAddress, const LibS7Com_uint32 nCommandChecksumAddress)
{
    m_nAMCtoPLC_MajorVersionAddress = nMajorVersionAddress;
    m_nAMCtoPLC_MinorVersionAddress = nMinorVersionAddress;
    m_nAMCtoPLC_PatchVersionAddress = nPatchVersionAddress;
    m_nAMCtoPLC_BuildVersionAddress = nBuildVersionAddress;
    m_nAMCtoPLC_CommandSequenceAddress = nCommandSequenceAddress;
    m_nAMCtoPLC_CommandIDAddress = nCommandIDAddress;
    m_nAMCtoPLC_CommandChecksumAddress = nCommandChecksumAddress;
}

void CPLCCommunication::SetPLCToAMCOffsets(const LibS7Com_uint32 nMajorVersionAddress, const LibS7Com_uint32 nMinorVersionAddress, const LibS7Com_uint32 nPatchVersionAddress, const LibS7Com_uint32 nBuildVersionAddress, const LibS7Com_uint32 nSequenceRunningAddress, const LibS7Com_uint32 nSequenceFinishedAddress, const LibS7Com_uint32 nSequenceStatusAddress, const LibS7Com_uint32 nSequenceErrorAddress)
{
    m_nPLCtoAMC_MajorVersionAddress = nMajorVersionAddress;
    m_nPLCtoAMC_MinorVersionAddress = nMinorVersionAddress;
    m_nPLCtoAMC_PatchVersionAddress = nPatchVersionAddress;
    m_nPLCtoAMC_BuildVersionAddress = nBuildVersionAddress;
    m_nPLCtoAMC_SequenceRunningAddress = nSequenceRunningAddress;
    m_nPLCtoAMC_SequenceFinishedAddress = nSequenceFinishedAddress;
    m_nPLCtoAMC_SequenceStatusAddress = nSequenceStatusAddress;
    m_nPLCtoAMC_SequenceErrorAddress = nSequenceErrorAddress;
}

ICommandParameters* CPLCCommunication::PrepareParameters()
{
    return new CCommandParameters(std::make_shared<CPLCWriteBuffer> (m_nAMCtoPLC_DBNo, m_nAMCtoPLC_Size));

}


void CPLCCommunication::StartCommunication(LibS7Net::PPLC pPLC)
{
    if (pPLC.get() == nullptr)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

    m_pPLC = pPLC;
    RetrieveStatus();

    m_nCmdCycleCounter = ReadVariableInt32(m_nPLCtoAMC_SequenceFinishedAddress) + 1;
}

void CPLCCommunication::RetrieveStatus()
{
    m_PLCRecvBuffer.resize(0);

    if ((m_pPLC.get() != nullptr) && (m_nPLCtoAMC_Size != 0)) {
       
        auto pReadData = m_pPLC->ReadBytes(m_nPLCtoAMC_DBNo, 0, m_nPLCtoAMC_Size);
        pReadData->GetData(m_PLCRecvBuffer);

        if ((int32_t)ReadVariableInt16 (m_nPLCtoAMC_MajorVersionAddress) != (int32_t)m_nMajorVersion)
            throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDMAJORVERSION);
        if ((int32_t) ReadVariableInt16(m_nPLCtoAMC_MinorVersionAddress) > (int32_t) m_nMinorVersion)
            throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDMINORVERSION);

    }

}

void CPLCCommunication::StopCommunication()
{
    m_pPLC = nullptr;
}


LibS7Com_uint32 CPLCCommunication::ExecuteCommand(ICommandParameters* pParametersInstance, const LibS7Com_uint32 nCommandID)
{
    if (pParametersInstance == nullptr)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);
    if (m_pPLC.get() == nullptr)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTCONNECTED);

    CCommandParameters* pCastedParameters = dynamic_cast<CCommandParameters*> (pParametersInstance);
    if (pCastedParameters == nullptr)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDCAST);

    uint32_t nCycleID = m_nCmdCycleCounter;
    m_nCmdCycleCounter++;

    auto pWriteBuffer = pCastedParameters->getWriteBuffer();
    pWriteBuffer->writeUint16(m_nAMCtoPLC_MajorVersionAddress, m_nMajorVersion);
    pWriteBuffer->writeUint16(m_nAMCtoPLC_MinorVersionAddress, m_nMinorVersion);
    pWriteBuffer->writeUint16(m_nAMCtoPLC_PatchVersionAddress, m_nPatchVersion);
    pWriteBuffer->writeUint32(m_nAMCtoPLC_CommandIDAddress, nCommandID);
    pWriteBuffer->writeUint32(m_nAMCtoPLC_CommandSequenceAddress, nCycleID);
    pWriteBuffer->send (m_pPLC.get());

/*    if (nCommandID == 21) {
        writeBuffer.writeUint32(48, 100);
    } */

/*    WriteData[262 + 0] = (nCycleID >> 24) & 0xff;
    WriteData[262 + 1] = (nCycleID >> 16) & 0xff;
    WriteData[262 + 2] = (nCycleID >> 8) & 0xff;
    WriteData[262 + 3] = nCycleID & 0xff; */

    //WriteData[266 + 3] = nCommandID;

    /*if (nCommandID == 21) {
        WriteData[270 + 3] = 100; // Schichtdicke in mu
        WriteData[274 + 3] = 1; // Anzahl Rakelhuebe
        WriteData[278 + 2] = 7; // Recoaterspeed in mm/min
        WriteData[278 + 3] = 208;
        WriteData[282 + 3] = 1; // Pulverdosierungen
    }
    m_pPLC->WriteBytes(m_nAMCtoPLC_DBNo, 0, WriteData); */

    return nCycleID;
}

void CPLCCommunication::CheckCommandExecution(const LibS7Com_uint32 nSequenceID, bool& bSequenceIsActive, bool& bSequenceIsFinished, LibS7Com_uint32& nErrorCode)
{
    if (m_pPLC.get() == nullptr)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTCONNECTED);

    RetrieveStatus();

    bSequenceIsFinished = false;
    nErrorCode = 0;

    int32_t varSEQUENCE_CURRENT = ReadVariableInt32(m_nPLCtoAMC_SequenceRunningAddress);

    bSequenceIsActive = (varSEQUENCE_CURRENT == (int32_t) nSequenceID);
    if (bSequenceIsActive) {
        int32_t varSEQUENCE_STATUS = ReadVariableInt32(m_nPLCtoAMC_SequenceStatusAddress);
        int32_t varSEQUENCE_ERRORCODE = ReadVariableInt32(m_nPLCtoAMC_SequenceErrorAddress);
        bSequenceIsFinished = (varSEQUENCE_STATUS == 0);

        if (!bSequenceIsFinished)
            nErrorCode = (uint32_t)varSEQUENCE_ERRORCODE;
    }

}

std::string CPLCCommunication::ReadVariableString(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nMaxLength)
{
    if (nMaxLength >= 256)
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDPARAM);

    if (((uint64_t)nAddress + nMaxLength + 2) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    auto nLength = ReadVariableByte(nAddress);
    if (nLength > nMaxLength)
        nLength = nMaxLength;

    std::vector<uint8_t> Bytes;
    Bytes.resize((uint64_t) nMaxLength + 1);

    for (uint32_t j = 0; j < nLength; j++) {
        Bytes[j] = ReadVariableByte(nAddress + 2 + j);
    }
    Bytes[nMaxLength] = 0;

    return std::string ((char*) Bytes.data());
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


LibS7Com_int16 CPLCCommunication::ReadVariableInt16(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 1) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[2];
    for (int j = 0; j < 2; j++)
        nValue[1 - j] = m_PLCRecvBuffer.at((uint64_t)nAddress + j);

    return *((int16_t*)&nValue[0]);
}


LibS7Com_uint16 CPLCCommunication::ReadVariableUint16(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 1) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[2];
    for (int j = 0; j < 2; j++)
        nValue[1 - j] = m_PLCRecvBuffer.at((uint64_t)nAddress + j);

    return *((uint16_t*)&nValue[0]);
}


LibS7Com_int32 CPLCCommunication::ReadVariableInt32(const LibS7Com_uint32 nAddress)
{    
    if (((uint64_t) nAddress + 3) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[4];
    for (int j = 0; j < 4; j++)
        nValue[3 - j] = m_PLCRecvBuffer.at((uint64_t)nAddress + j);

    return *((int32_t*) &nValue[0]);
}

LibS7Com_uint32 CPLCCommunication::ReadVariableUint32(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 3) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[4];
    for (int j = 0; j < 4; j++)
        nValue[3 - j] = m_PLCRecvBuffer.at((uint64_t)nAddress + j);

    return *((uint32_t*)&nValue[0]);
}

LibS7Com_double CPLCCommunication::ReadVariableReal(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 3) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[4];
    for (int j = 0; j < 4; j++)
        nValue[3 - j] = m_PLCRecvBuffer.at((uint64_t)nAddress + j);

    return *((float*)&nValue[0]);

}

LibS7Com_double CPLCCommunication::ReadVariableLReal(const LibS7Com_uint32 nAddress)
{
    if (((uint64_t)nAddress + 7) >= m_PLCRecvBuffer.size())
        throw ELibS7ComInterfaceException(LIBS7COM_ERROR_INVALIDREADADDRESS);

    uint8_t nValue[8];
    for (int j = 0; j < 8; j++)
        nValue[7 - j] = m_PLCRecvBuffer.at((uint64_t)nAddress + j);

    return *((double*)&nValue[0]);

}
