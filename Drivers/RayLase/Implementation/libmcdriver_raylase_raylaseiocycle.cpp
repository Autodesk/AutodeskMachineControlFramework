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


Abstract: This is a stub class definition of CRaylaseIOCycle

*/

#include "libmcdriver_raylase_raylaseiocycle.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"

using namespace LibMCDriver_Raylase::Impl;

CRaylaseIOCycle::CRaylaseIOCycle(PRaylaseIOCycleImpl pIOCycleImpl)
	: m_pIOCycleImpl(pIOCycleImpl)
{
	if (pIOCycleImpl.get() == nullptr)
		throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDPARAM);
}

CRaylaseIOCycle::~CRaylaseIOCycle()
{
}

LibMCDriver_Raylase_uint32 CRaylaseIOCycle::GetCycleID()
{
	return m_pIOCycleImpl->getCycleID();
}

void CRaylaseIOCycle::AddSignalOut(const LibMCDriver_Raylase::eIOPort eIOPort, const LibMCDriver_Raylase_uint32 nIOPin)
{
	m_pIOCycleImpl->addSignalOut(eIOPort, nIOPin);
}

void CRaylaseIOCycle::AddWaitForSignal(const LibMCDriver_Raylase::eIOPort eIOPort, const LibMCDriver_Raylase_uint32 nIOPin, const LibMCDriver_Raylase_uint32 nTimeoutInMicroseconds)
{
	m_pIOCycleImpl->addWaitForSignal(eIOPort, nIOPin, nTimeoutInMicroseconds);
}

void CRaylaseIOCycle::AddDelay(const LibMCDriver_Raylase_uint32 nDelayInMicroseconds)
{
	m_pIOCycleImpl->addDelay(nDelayInMicroseconds);
}
