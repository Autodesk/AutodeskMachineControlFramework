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


Abstract: This is a stub class definition of CRaylaseIOCycleImpl

*/

#include "libmcdriver_raylase_raylaseiocycleimpl.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

using namespace LibMCDriver_Raylase::Impl;

CRaylaseIOCycleImpl::CRaylaseIOCycleImpl(uint32_t nCycleID)
	: m_nCycleID(nCycleID)
{
	if (nCycleID == 0)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDIOCYCLEID);
}

CRaylaseIOCycleImpl::~CRaylaseIOCycleImpl()
{
}

uint32_t CRaylaseIOCycleImpl::getCycleID() const
{
	return m_nCycleID;
}

void CRaylaseIOCycleImpl::addSignalOut(const eIOPort ePort, const uint32_t nIOPin)
{
	if (ePort == eIOPort::Unknown)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDIOPORT);

	sIOCycleEntry entry;
	entry.m_EntryType = eIOCycleEntryType::SignalOut;
	entry.m_IOPort = ePort;
	entry.m_nIOPin = nIOPin;
	entry.m_nTimeoutOrDelayInMicroseconds = 0;

	m_Entries.push_back(entry);
}

void CRaylaseIOCycleImpl::addWaitForSignal(const eIOPort ePort, const uint32_t nIOPin, const uint32_t nTimeoutInMicroseconds)
{
	if (ePort == eIOPort::Unknown)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDIOPORT);

	sIOCycleEntry entry;
	entry.m_EntryType = eIOCycleEntryType::WaitForSignal;
	entry.m_IOPort = ePort;
	entry.m_nIOPin = nIOPin;
	entry.m_nTimeoutOrDelayInMicroseconds = nTimeoutInMicroseconds;

	m_Entries.push_back(entry);
}

void CRaylaseIOCycleImpl::addDelay(const uint32_t nDelayInMicroseconds)
{
	sIOCycleEntry entry;
	entry.m_EntryType = eIOCycleEntryType::Delay;
	entry.m_IOPort = eIOPort::Unknown;
	entry.m_nIOPin = 0;
	entry.m_nTimeoutOrDelayInMicroseconds = nDelayInMicroseconds;

	m_Entries.push_back(entry);
}

size_t CRaylaseIOCycleImpl::getEntryCount() const
{
	return m_Entries.size();
}

const sIOCycleEntry& CRaylaseIOCycleImpl::getEntry(size_t nIndex) const
{
	if (nIndex >= m_Entries.size())
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);

	return m_Entries.at(nIndex);
}

const std::vector<sIOCycleEntry>& CRaylaseIOCycleImpl::getEntries() const
{
	return m_Entries;
}
