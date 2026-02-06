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


Abstract: This is the class declaration of CRaylaseIOCycleImpl

*/


#ifndef __LIBMCDRIVER_RAYLASE_RAYLASEIOCYCLEIMPL
#define __LIBMCDRIVER_RAYLASE_RAYLASEIOCYCLEIMPL

#include "libmcdriver_raylase_interfaces.hpp"
#include <vector>
#include <memory>

namespace LibMCDriver_Raylase {
namespace Impl {

enum class eIOCycleEntryType : uint32_t {
	Unknown = 0,
	SignalOut = 1,
	WaitForSignal = 2,
	Delay = 3
};

struct sIOCycleEntry {
	eIOCycleEntryType m_EntryType;
	eIOPort m_IOPort;
	uint32_t m_nIOPin;
	uint32_t m_nTimeoutOrDelayInMicroseconds;

	sIOCycleEntry()
		: m_EntryType(eIOCycleEntryType::Unknown),
		  m_IOPort(eIOPort::Unknown),
		  m_nIOPin(0),
		  m_nTimeoutOrDelayInMicroseconds(0)
	{
	}
};

class CRaylaseIOCycleImpl;
typedef std::shared_ptr<CRaylaseIOCycleImpl> PRaylaseIOCycleImpl;

class CRaylaseIOCycleImpl {
private:
	uint32_t m_nCycleID;
	std::vector<sIOCycleEntry> m_Entries;

public:
	CRaylaseIOCycleImpl(uint32_t nCycleID);

	virtual ~CRaylaseIOCycleImpl();

	uint32_t getCycleID() const;

	void addSignalOut(const eIOPort ePort, const uint32_t nIOPin);

	void addWaitForSignal(const eIOPort ePort, const uint32_t nIOPin, const uint32_t nTimeoutInMicroseconds);

	void addDelay(const uint32_t nDelayInMicroseconds);

	size_t getEntryCount() const;

	const sIOCycleEntry& getEntry(size_t nIndex) const;

	const std::vector<sIOCycleEntry>& getEntries() const;

};

} // namespace Impl
} // namespace LibMCDriver_Raylase

#endif // __LIBMCDRIVER_RAYLASE_RAYLASEIOCYCLEIMPL
