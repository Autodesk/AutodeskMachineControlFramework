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


Abstract: This is the class declaration of CSessionMetricsHandler

*/


#ifndef __LIBMCDATA_SESSIONMETRICSHANDLER
#define __LIBMCDATA_SESSIONMETRICSHANDLER

#include "libmcdata_interfaces.hpp"

// Parent classes
#include "libmcdata_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

#include "amcdata_journal.hpp"


namespace LibMCData {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSessionMetricsHandler 
**************************************************************************************************************************/

class CSessionMetricsHandler : public virtual ISessionMetricsHandler, public virtual CBase {

protected:
	AMCData::PJournal m_pJournal;

public:

	CSessionMetricsHandler(AMCData::PJournal pJournal);

	void AddFrontendMetrics(const std::string & sSessionUUID, const std::string & sLabel, const LibMCData_uint64 nIntervalStart, const LibMCData_uint64 nIntervalEnd, const LibMCData_uint32 nRequestCount, const LibMCData_double dSumDurationMS, const LibMCData_double dMinDurationMS, const LibMCData_double dMaxDurationMS, const LibMCData_double dSumSqDurationMS, const LibMCData_uint64 nPayloadSumBytes, const LibMCData_uint64 nPayloadMaxBytes, const LibMCData_double dServerBuildSumMS, const LibMCData_uint64 nAbsoluteTimeStamp) override;

};

} // namespace Impl
} // namespace LibMCData

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDATA_SESSIONMETRICSHANDLER
