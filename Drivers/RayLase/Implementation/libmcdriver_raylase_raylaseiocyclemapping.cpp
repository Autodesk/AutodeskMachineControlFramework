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


Abstract: This is the implementation of CRaylaseCardIOCycleMapping

*/

#include "libmcdriver_raylase_raylaseiocyclemapping.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

using namespace LibMCDriver_Raylase::Impl;

CRaylaseCardIOCycleMapping::CRaylaseCardIOCycleMapping()
{
}

CRaylaseCardIOCycleMapping::~CRaylaseCardIOCycleMapping()
{
}

PRaylaseIOCycleImpl CRaylaseCardIOCycleMapping::createIOCycle(uint32_t nCycleID)
{
	if (nCycleID == 0)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDIOCYCLEID);

	auto iIter = m_IOCycles.find(nCycleID);
	if (iIter != m_IOCycles.end())
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_IOCYCLEALREADYEXISTS);

	auto pIOCycle = std::make_shared<CRaylaseIOCycleImpl>(nCycleID);
	m_IOCycles.insert(std::make_pair(nCycleID, pIOCycle));

	return pIOCycle;
}

bool CRaylaseCardIOCycleMapping::ioCycleExists(uint32_t nCycleID) const
{
	auto iIter = m_IOCycles.find(nCycleID);
	return (iIter != m_IOCycles.end());
}

PRaylaseIOCycleImpl CRaylaseCardIOCycleMapping::getIOCycle(uint32_t nCycleID) const
{
	auto iIter = m_IOCycles.find(nCycleID);
	if (iIter == m_IOCycles.end())
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_IOCYCLENOTFOUND);

	return iIter->second;
}

void CRaylaseCardIOCycleMapping::removeIOCycle(uint32_t nCycleID)
{
	m_IOCycles.erase(nCycleID);
}

void CRaylaseCardIOCycleMapping::clearAllIOCycles()
{
	m_IOCycles.clear();
}

size_t CRaylaseCardIOCycleMapping::getIOCycleCount() const
{
	return m_IOCycles.size();
}
