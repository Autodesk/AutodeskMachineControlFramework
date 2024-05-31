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


Abstract: This is a stub class definition of CBuildJobExecutionDataIterator

*/

#include "libmcdata_buildjobexecutiondataiterator.hpp"
#include "libmcdata_interfaceexception.hpp"
#include "libmcdata_buildjobexecutiondata.hpp"

// Include custom headers here.


using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CBuildJobExecutionDataIterator 
**************************************************************************************************************************/

CBuildJobExecutionDataIterator::CBuildJobExecutionDataIterator()
    : CIterator()
{

}

CBuildJobExecutionDataIterator::~CBuildJobExecutionDataIterator()
{

}

IBase* CBuildJobExecutionDataIterator::GetCurrent()
{
    return GetCurrentJobExecutionData();
}

IBuildJobExecutionData* CBuildJobExecutionDataIterator::GetCurrentJobExecutionData()
{
    if ((m_nCurrentIndex < 0) || (m_nCurrentIndex >= m_List.size()))
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDITERATOR);

    auto pBuildJobExecutionData = std::dynamic_pointer_cast<CBuildJobExecutionData> (m_List[m_nCurrentIndex]);
    if (pBuildJobExecutionData.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDCAST);

    return CBuildJobExecutionData::makeFrom(pBuildJobExecutionData.get());
}


IIterator* CBuildJobExecutionDataIterator::Clone()
{
    std::unique_ptr<CBuildJobExecutionDataIterator> pNewIterator(new CBuildJobExecutionDataIterator());

    for (auto pBase : m_List) {
        auto pBuildJobExecutionData = std::dynamic_pointer_cast<CBuildJobExecutionData> (pBase);
        if (pBuildJobExecutionData.get() == nullptr)
            throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDCAST);
        pNewIterator->AddJobExecutionData(CBuildJobExecutionData::makeSharedFrom(pBuildJobExecutionData.get()));
    }

    return pNewIterator.release();
}

void CBuildJobExecutionDataIterator::AddJobExecutionData(std::shared_ptr<CBuildJobExecutionData> pBuildJobExecutionData)
{
    if (pBuildJobExecutionData.get() == nullptr)
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDPARAM);

    m_List.push_back(pBuildJobExecutionData);
}

