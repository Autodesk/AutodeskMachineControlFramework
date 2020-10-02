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

void CPLCCommunication::StartCommunication(LibS7Net::PPLC pPLC)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

void CPLCCommunication::StopCommunication()
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

void CPLCCommunication::GetStatus()
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

std::string CPLCCommunication::LoadProgram(const std::string & sProgram)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

void CPLCCommunication::ExecuteProgram(const std::string & sIdentifier)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

void CPLCCommunication::ClearPrograms()
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

LibS7Com_uint32 CPLCCommunication::GetVariableCount()
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

std::string CPLCCommunication::GetVariableName(const LibS7Com_uint32 nIndex)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

LibS7Com::eVariableType CPLCCommunication::GetVariableType(const LibS7Com_uint32 nIndex)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

std::string CPLCCommunication::GetVariableString(const LibS7Com_uint32 nIndex)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

bool CPLCCommunication::GetVariableBool(const LibS7Com_uint32 nIndex)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

LibS7Com_int64 CPLCCommunication::GetVariableInteger(const LibS7Com_uint32 nIndex)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

LibS7Com_double CPLCCommunication::GetVariableDouble(const LibS7Com_uint32 nIndex)
{
	throw ELibS7ComInterfaceException(LIBS7COM_ERROR_NOTIMPLEMENTED);
}

