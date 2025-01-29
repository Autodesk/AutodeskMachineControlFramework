/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is the class declaration of CSMCJob

*/


#ifndef __LIBMCDRIVER_SCANLABSMC_SMCSIMULATIONPARSER
#define __LIBMCDRIVER_SCANLABSMC_SMCSIMULATIONPARSER

#include "libmcdriver_scanlabsmc_interfaces.hpp"

#include "libmcdriver_scanlabsmc_smccontexthandle.hpp"
#include "libmcdriver_scanlabsmc_sdk.hpp"


namespace LibMCDriver_ScanLabSMC {
namespace Impl {


typedef struct _sSMCSimulationEntry {
	double m_dTimestamp;
	double m_dCoordinates[3];
	int32_t m_dLaserToggle;
	double m_dActiveChannel1;
	double m_dActiveChannel2;
	int32_t m_CommandIndex;
} sSMCSimulationEntry;


class CSMCSimulationParser {
private:

	std::vector<sSMCSimulationEntry> m_Entries;

public:

	CSMCSimulationParser(const std::string & sAbsoluteFileName);

	virtual ~CSMCSimulationParser();

	size_t getCount();

	sSMCSimulationEntry & getEntry(size_t nIndex);

	void readTimestampValues(std::vector<double> & values);
	void readXValues(std::vector<double>& values);
	void readYValues(std::vector<double>& values);

};

typedef std::shared_ptr<CSMCSimulationParser> PSMCSimulationParser;

} // namespace Impl
} // namespace LibMCDriver_ScanLabSMC

#endif // __LIBMCDRIVER_SCANLABSMC_SMCSIMULATIONPARSER
