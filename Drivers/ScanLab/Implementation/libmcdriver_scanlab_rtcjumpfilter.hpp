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


Abstract: This is the class declaration of CRTCJumpFilter

*/


#ifndef __LIBMCDRIVER_SCANLAB_RTCJUMPFILTER
#define __LIBMCDRIVER_SCANLAB_RTCJUMPFILTER

#include <memory>
#include <cstdint>
#include <cmath>

// Header only and without driver dependencies, so that amc_unittest can test it.

#define RTCJUMPFILTER_MAX_MINIMUMJUMPDISTANCE 1.0

#define RTCJUMPFILTER_PROFILENAMESPACE "http://schemas.scanlab.com/jump/2026/10"
#define RTCJUMPFILTER_PROFILEVALUE_MM "minimumdistance"
#define RTCJUMPFILTER_PROFILEVALUE_BITS "minimumdistance_bits"

namespace LibMCDriver_ScanLab {
namespace Impl {

enum class eRTCJumpFilterOverride : int32_t {
	NoOverride = 0,
	Valid = 1,
	Invalid = 2,
	ProvidedTwice = 3
};


/*************************************************************************************************************************
 Class declaration of CRTCJumpFilter
**************************************************************************************************************************/

// Decides whether a jump has to be written to the RTC list. A jump to the current scan position is never written.
// With a minimum jump distance, shorter jumps are dropped as well: the following mark then starts at the current
// scan position, so the start point of that mark is off by less than the minimum distance. The distance is always
// measured from the actual scan position, so dropped jumps never accumulate.
//
// The minimum distance is the context setting, unless a segment override is active.
class CRTCJumpFilter
{
private:

	double m_dMinimumJumpDistanceInMM;

	bool m_bHasSegmentOverride;
	double m_dSegmentMinimumJumpDistanceInMM;

public:

	CRTCJumpFilter()
		: m_dMinimumJumpDistanceInMM(0.0), m_bHasSegmentOverride(false), m_dSegmentMinimumJumpDistanceInMM(0.0)
	{
	}

	virtual ~CRTCJumpFilter()
	{
	}

	static bool isValidMinimumJumpDistance(double dMinimumJumpDistanceInMM)
	{
		// Written as a positive range check, so that NaN is rejected.
		return (dMinimumJumpDistanceInMM >= 0.0) && (dMinimumJumpDistanceInMM <= RTCJUMPFILTER_MAX_MINIMUMJUMPDISTANCE);
	}

	// Returns false and keeps the current setting if the distance is not valid. 0 disables the distance check.
	bool setMinimumJumpDistance(double dMinimumJumpDistanceInMM)
	{
		if (!isValidMinimumJumpDistance(dMinimumJumpDistanceInMM))
			return false;

		m_dMinimumJumpDistanceInMM = dMinimumJumpDistanceInMM;
		return true;
	}

	double getMinimumJumpDistance() const
	{
		return m_dMinimumJumpDistanceInMM;
	}

	// Returns false and keeps the current state if the distance is not valid.
	bool setSegmentOverride(double dMinimumJumpDistanceInMM)
	{
		if (!isValidMinimumJumpDistance(dMinimumJumpDistanceInMM))
			return false;

		m_bHasSegmentOverride = true;
		m_dSegmentMinimumJumpDistanceInMM = dMinimumJumpDistanceInMM;
		return true;
	}

	void clearSegmentOverride()
	{
		m_bHasSegmentOverride = false;
		m_dSegmentMinimumJumpDistanceInMM = 0.0;
	}

	bool hasSegmentOverride() const
	{
		return m_bHasSegmentOverride;
	}

	double getEffectiveMinimumJumpDistance() const
	{
		if (m_bHasSegmentOverride)
			return m_dSegmentMinimumJumpDistanceInMM;

		return m_dMinimumJumpDistanceInMM;
	}

	// Positions are in RTC bits, dBitsPerMM is the XY correction factor of the context.
	bool jumpIsNeeded(int32_t nCurrentX, int32_t nCurrentY, int32_t nTargetX, int32_t nTargetY, double dBitsPerMM) const
	{
		if ((nCurrentX == nTargetX) && (nCurrentY == nTargetY))
			return false;

		double dMinimumJumpDistanceInMM = getEffectiveMinimumJumpDistance();
		if (dMinimumJumpDistanceInMM <= 0.0)
			return true;

		double dDeltaX = (double)((int64_t)nTargetX - (int64_t)nCurrentX);
		double dDeltaY = (double)((int64_t)nTargetY - (int64_t)nCurrentY);
		double dMinimumDistanceInBits = dMinimumJumpDistanceInMM * std::fabs(dBitsPerMM);

		return ((dDeltaX * dDeltaX + dDeltaY * dDeltaY) >= (dMinimumDistanceInBits * dMinimumDistanceInBits));
	}

	// Resolves the profile values of a segment. The distance may be given in mm or in RTC bits, but not both.
	static eRTCJumpFilterOverride resolveSegmentOverride(bool bHasValueInMM, double dValueInMM, bool bHasValueInBits, double dValueInBits, double dBitsPerMM, double& dMinimumJumpDistanceInMM)
	{
		dMinimumJumpDistanceInMM = 0.0;

		if (bHasValueInMM && bHasValueInBits)
			return eRTCJumpFilterOverride::ProvidedTwice;

		if (bHasValueInMM) {
			if (!isValidMinimumJumpDistance(dValueInMM))
				return eRTCJumpFilterOverride::Invalid;

			dMinimumJumpDistanceInMM = dValueInMM;
			return eRTCJumpFilterOverride::Valid;
		}

		if (bHasValueInBits) {
			double dAbsBitsPerMM = std::fabs(dBitsPerMM);
			if (!(dValueInBits >= 0.0) || !(dAbsBitsPerMM > 0.0))
				return eRTCJumpFilterOverride::Invalid;

			double dValueInMMFromBits = dValueInBits / dAbsBitsPerMM;
			if (!isValidMinimumJumpDistance(dValueInMMFromBits))
				return eRTCJumpFilterOverride::Invalid;

			dMinimumJumpDistanceInMM = dValueInMMFromBits;
			return eRTCJumpFilterOverride::Valid;
		}

		return eRTCJumpFilterOverride::NoOverride;
	}

};

typedef std::shared_ptr<CRTCJumpFilter> PRTCJumpFilter;


// Clears the segment override when it goes out of scope, also if writing a layer fails.
class CRTCJumpFilterSegmentScope
{
private:

	CRTCJumpFilter* m_pJumpFilter;

public:

	CRTCJumpFilterSegmentScope(CRTCJumpFilter* pJumpFilter)
		: m_pJumpFilter(pJumpFilter)
	{
	}

	~CRTCJumpFilterSegmentScope()
	{
		if (m_pJumpFilter != nullptr)
			m_pJumpFilter->clearSegmentOverride();
	}

	CRTCJumpFilterSegmentScope(const CRTCJumpFilterSegmentScope&) = delete;
	CRTCJumpFilterSegmentScope& operator=(const CRTCJumpFilterSegmentScope&) = delete;

};

} // namespace Impl
} // namespace LibMCDriver_ScanLab

#endif // __LIBMCDRIVER_SCANLAB_RTCJUMPFILTER
