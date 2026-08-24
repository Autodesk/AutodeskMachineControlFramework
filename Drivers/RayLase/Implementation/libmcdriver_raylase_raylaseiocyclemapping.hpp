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


Abstract: This is the class declaration of CRaylaseCardIOCycleMapping

*/


#ifndef __LIBMCDRIVER_RAYLASE_RAYLASEIOCYCLEMAPPING
#define __LIBMCDRIVER_RAYLASE_RAYLASEIOCYCLEMAPPING

#include "libmcdriver_raylase_raylaseiocycleimpl.hpp"
#include <map>
#include <memory>

namespace LibMCDriver_Raylase {
namespace Impl {


class CRaylaseCardIOCycleMapping;
typedef std::shared_ptr<CRaylaseCardIOCycleMapping> PRaylaseCardIOCycleMapping;

class CRaylaseCardIOCycleMapping {
private:
	std::map<uint32_t, PRaylaseIOCycleImpl> m_IOCycles;

public:
	CRaylaseCardIOCycleMapping();

	virtual ~CRaylaseCardIOCycleMapping();

	// Creates a new IO cycle with the given ID. Fails if the cycle already exists.
	PRaylaseIOCycleImpl createIOCycle(uint32_t nCycleID);

	// Returns true if an IO cycle with the given ID exists.
	bool ioCycleExists(uint32_t nCycleID) const;

	// Returns the IO cycle with the given ID. Fails if the cycle does not exist.
	PRaylaseIOCycleImpl getIOCycle(uint32_t nCycleID) const;

	// Removes the IO cycle with the given ID. Does nothing if the cycle does not exist.
	void removeIOCycle(uint32_t nCycleID);

	// Clears all IO cycles.
	void clearAllIOCycles();

	// Returns the number of IO cycles.
	size_t getIOCycleCount() const;

};

} // namespace Impl
} // namespace LibMCDriver_Raylase

#endif // __LIBMCDRIVER_RAYLASE_RAYLASEIOCYCLEMAPPING
