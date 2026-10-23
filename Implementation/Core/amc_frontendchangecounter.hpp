/*++

Copyright (C) 2026 Autodesk Inc.

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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Abstract: This is the class declaration of CFrontendChangeCounter.
A counter that is bumped whenever a value the frontend status is built from may have
changed (parameter values of frontend-visible groups, state machine states, handled UI
events). It is owned by CStateMachineData. Values that live outside the core (database
heads, logs, data series) do not bump it; status builders pick those up with a heartbeat.

*/


#ifndef __AMC_FRONTENDCHANGECOUNTER
#define __AMC_FRONTENDCHANGECOUNTER

#include <atomic>
#include <cstdint>
#include <memory>

namespace AMC {

	class CFrontendChangeCounter;
	typedef std::shared_ptr<CFrontendChangeCounter> PFrontendChangeCounter;

	class CFrontendChangeCounter {
	private:

		std::atomic<uint64_t> m_nCounter;

	public:

		CFrontendChangeCounter()
			: m_nCounter(1)
		{
		}

		CFrontendChangeCounter(const CFrontendChangeCounter&) = delete;
		CFrontendChangeCounter& operator=(const CFrontendChangeCounter&) = delete;

		void bump()
		{
			m_nCounter.fetch_add(1, std::memory_order_release);
		}

		uint64_t get() const
		{
			return m_nCounter.load(std::memory_order_acquire);
		}

	};

}


#endif //__AMC_FRONTENDCHANGECOUNTER
