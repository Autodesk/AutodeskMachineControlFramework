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


Abstract: This is a stub class definition of CWorkingFileIterator

*/

#include "libmcenv_workingfileiterator.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_workingfile.hpp"

// Include custom headers here.


using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CWorkingFileIterator 
**************************************************************************************************************************/

CWorkingFileIterator::CWorkingFileIterator()
    : CIterator()
{
}

IBase* CWorkingFileIterator::GetCurrent()
{
    return GetCurrentFile();
}

IWorkingFile* CWorkingFileIterator::GetCurrentFile()
{
    if ((m_nCurrentIndex < 0) || (m_nCurrentIndex >= m_List.size()))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDITERATOR);

    auto pWorkingFile = std::dynamic_pointer_cast<CWorkingFile> (m_List[m_nCurrentIndex]);
    if (pWorkingFile.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);

    return CWorkingFile::makeFrom(pWorkingFile.get());

}


IIterator* CWorkingFileIterator::Clone()
{
    auto pNewIterator = std::make_unique<CWorkingFileIterator>();

    for (auto pBase : m_List) {
        auto pWorkingFile = std::dynamic_pointer_cast<CWorkingFile> (pBase);
        if (pWorkingFile.get() == nullptr)
            throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDCAST);
        pNewIterator->AddWorkingFile(CWorkingFile::makeSharedFrom (pWorkingFile.get()));
    }

    return pNewIterator.release();
}

void CWorkingFileIterator::AddWorkingFile(std::shared_ptr<CWorkingFile> pWorkingFile)
{
    if (pWorkingFile.get() == nullptr)
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

    m_List.push_back(pWorkingFile);
}


