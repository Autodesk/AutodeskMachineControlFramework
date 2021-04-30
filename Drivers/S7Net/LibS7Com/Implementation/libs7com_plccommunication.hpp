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


Abstract: This is the class declaration of CPLCCommunication

*/


#ifndef __LIBS7COM_PLCCOMMUNICATION
#define __LIBS7COM_PLCCOMMUNICATION

#include "libs7com_interfaces.hpp"

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
 Class declaration of CPLCCommunication 
**************************************************************************************************************************/

class CPLCCommunication : public virtual IPLCCommunication, public virtual CBase {
private:

	/**
	* Put private members here.
	*/

protected:

	uint32_t m_nPLCtoAMC_Size;
	uint32_t m_nAMCtoPLC_Size;
	uint32_t m_nPLCtoAMC_DBNo;
	uint32_t m_nAMCtoPLC_DBNo;
	uint32_t m_nMajorVersion;
	uint32_t m_nMinorVersion;
	uint32_t m_nPatchVersion;
	LibS7Net::PPLC m_pPLC;

	uint32_t m_nCmdCycleCounter;

	LibS7Com_uint32 m_nAMCtoPLC_MajorVersionAddress;
	LibS7Com_uint32 m_nAMCtoPLC_MinorVersionAddress;
	LibS7Com_uint32 m_nAMCtoPLC_PatchVersionAddress;
	LibS7Com_uint32 m_nAMCtoPLC_BuildVersionAddress;
	LibS7Com_uint32 m_nAMCtoPLC_CommandSequenceAddress;
	LibS7Com_uint32 m_nAMCtoPLC_CommandIDAddress;
	LibS7Com_uint32 m_nAMCtoPLC_CommandChecksumAddress;

	LibS7Com_uint32 m_nPLCtoAMC_MajorVersionAddress;
	LibS7Com_uint32 m_nPLCtoAMC_MinorVersionAddress;
	LibS7Com_uint32 m_nPLCtoAMC_PatchVersionAddress;
	LibS7Com_uint32 m_nPLCtoAMC_BuildVersionAddress;
	LibS7Com_uint32 m_nPLCtoAMC_SequenceRunningAddress;
	LibS7Com_uint32 m_nPLCtoAMC_SequenceFinishedAddress;
	LibS7Com_uint32 m_nPLCtoAMC_SequenceStatusAddress;
	LibS7Com_uint32 m_nPLCtoAMC_SequenceErrorAddress;

	std::vector<uint8_t> m_PLCRecvBuffer;

public:

	CPLCCommunication ();

	void SetProtocolConfiguration(const LibS7Com_uint32 nMajorVersion, const LibS7Com_uint32 nMinorVersion, const LibS7Com_uint32 nPatchVersion, const LibS7Com_uint32 nPLCtoAMC_DBNo, const LibS7Com_uint32 nPLCtoAMC_Size, const LibS7Com_uint32 nAMCtoPLC_DBNo, const LibS7Com_uint32 nAMCtoPLC_Size) override;

	void SetAMCTOPLCOffsets(const LibS7Com_uint32 nMajorVersionAddress, const LibS7Com_uint32 nMinorVersionAddress, const LibS7Com_uint32 nPatchVersionAddress, const LibS7Com_uint32 nBuildVersionAddress, const LibS7Com_uint32 nCommandSequenceAddress, const LibS7Com_uint32 nCommandIDAddress, const LibS7Com_uint32 nCommandChecksumAddress) override;

	void SetPLCToAMCOffsets(const LibS7Com_uint32 nMajorVersionAddress, const LibS7Com_uint32 nMinorVersionAddress, const LibS7Com_uint32 nPatchVersionAddress, const LibS7Com_uint32 nBuildVersionAddress, const LibS7Com_uint32 nSequenceRunningAddress, const LibS7Com_uint32 nSequenceFinishedAddress, const LibS7Com_uint32 nSequenceStatusAddress, const LibS7Com_uint32 nSequenceErrorAddress) override;

	void StartCommunication(LibS7Net::PPLC pPLC) override;

	void RetrieveStatus() override;

	void StopCommunication() override;

	LibS7Com_uint32 ExecuteCommand(const LibS7Com_uint32 nCommandID) override;

	void CheckCommandExecution(const LibS7Com_uint32 nSequenceID, bool& bSequenceIsActive, bool& bSequenceIsFinished, LibS7Com_uint32& nErrorCode) override;

	std::string ReadVariableString(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nMaxLength) override;

	bool ReadVariableBool(const LibS7Com_uint32 nAddress, const LibS7Com_uint32 nBit) override;

	LibS7Com_uint8 ReadVariableByte(const LibS7Com_uint32 nAddress) override;

	LibS7Com_int32 ReadVariableInt32(const LibS7Com_uint32 nAddress) override;

	LibS7Com_uint32 ReadVariableUint32(const LibS7Com_uint32 nAddress) override;

	LibS7Com_int16 ReadVariableInt16(const LibS7Com_uint32 nAddress) override;

	LibS7Com_uint16 ReadVariableUint16(const LibS7Com_uint32 nAddress) override;

	LibS7Com_double ReadVariableReal(const LibS7Com_uint32 nAddress) override;

	LibS7Com_double ReadVariableLReal(const LibS7Com_uint32 nAddress) override;

};

} // namespace Impl
} // namespace LibS7Com

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBS7COM_PLCCOMMUNICATION
