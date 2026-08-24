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


Abstract: This is the class declaration of CTelemetryInterval

*/


#ifndef __LIBMCENV_TELEMETRYINTERVAL
#define __LIBMCENV_TELEMETRYINTERVAL

#include "libmcenv_interfaces.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CTelemetryInterval 
**************************************************************************************************************************/

class CTelemetryInterval : public virtual ITelemetryInterval, public virtual CBase {
private:

	uint64_t m_nMarkerID;
	std::string m_sChannelIdentifier;
	uint64_t m_nStartTimestamp;
	uint64_t m_nEndTimestamp;
	uint64_t m_nContextData;

public:

	CTelemetryInterval(uint64_t nMarkerID, const std::string& sChannelIdentifier, uint64_t nStartTimestamp, uint64_t nEndTimestamp, uint64_t nContextData);

	virtual ~CTelemetryInterval();

	LibMCEnv_uint64 GetMarkerID() override;

	std::string GetChannelIdentifier() override;

	LibMCEnv_uint64 GetStartTimestamp() override;

	LibMCEnv_uint64 GetEndTimestamp() override;

	LibMCEnv_uint64 GetDuration() override;

	LibMCEnv_uint64 GetContextData() override;

};

typedef std::shared_ptr<CTelemetryInterval> PTelemetryInterval;

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_TELEMETRYINTERVAL
