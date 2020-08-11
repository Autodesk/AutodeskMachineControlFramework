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


Abstract: This is a stub class definition of CWorkingFile

*/

#include "libmcdriverenv_workingfile.hpp"
#include "libmcdriverenv_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriverEnv::Impl;

/*************************************************************************************************************************
 Class definition of CWorkingFile 
**************************************************************************************************************************/

std::string CWorkingFile::GetAbsoluteFileName()
{
	throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_NOTIMPLEMENTED);
}

LibMCDriverEnv_uint64 CWorkingFile::GetSize()
{
	throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_NOTIMPLEMENTED);
}

std::string CWorkingFile::CalculateSHA2()
{
	throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_NOTIMPLEMENTED);
}

void CWorkingFile::DeleteFile()
{
	throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_NOTIMPLEMENTED);
}

IWorkingFileExecution * CWorkingFile::ExecuteFile()
{
	throw ELibMCDriverEnvInterfaceException(LIBMCDRIVERENV_ERROR_NOTIMPLEMENTED);
}

