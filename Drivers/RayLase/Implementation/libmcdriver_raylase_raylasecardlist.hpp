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


Abstract: This is the class declaration of CRaylaseCard

*/


#ifndef __LIBMCDRIVER_RAYLASE_RAYLASECARDLIST
#define __LIBMCDRIVER_RAYLASE_RAYLASECARDLIST

#include "libmcdriver_raylase_interfaces.hpp"
#include "libmcdriver_raylase_sdk.hpp"

#include <map>

namespace LibMCDriver_Raylase {
namespace Impl {

#define RAYLASE_LISTONCARDNOTSET 0xffffffff
#define RAYLASE_MAXLISTONCARDID 256

	class CRaylaseCoordinateTransform
	{
	private:
		double m_dM11;
		double m_dM12;
		double m_dM21;
		double m_dM22;
		double m_dOffsetX;
		double m_dOffsetY;

	public:

		CRaylaseCoordinateTransform();

		virtual ~CRaylaseCoordinateTransform();

		void setRotationalCoordinateTransform(const double dM11, const double dM12, const double dM21, const double dM22);

		void getRotationalCoordinateTransform(double& dM11, double& dM12, double& dM21, double& dM22);

		void setTranslationalCoordinateTransform(const double dOffsetX, const double dOffsetY);

		void getTranslationalCoordinateTransform(double& dOffsetX, double& dOffsetY);

		void applyTransform (double & dX, double & dY);


	};

	typedef std::shared_ptr <CRaylaseCoordinateTransform> PRaylaseCoordinateTransform;

	enum class eRaylasePartIgnoreState : uint32_t
	{
		pisDoNotIgnore = 0,
		pisSkipPart = 1,
		pisNoPower = 2
	};

class CRaylaseCardList
{
	private:
		PRaylaseSDK m_pSDK;
		rlHandle m_CardHandle;
		rlListHandle m_ListHandle;
		double m_dMaxLaserPowerInWatts;
		uint32_t m_nListIDOnCard;
		PRaylaseCoordinateTransform m_pCoordinateTransform;

		std::map<std::string, eRaylasePartIgnoreState> m_IgnorePartMap;

	public:

		CRaylaseCardList(PRaylaseSDK pSDK, rlHandle cardHandle, double dMaxLaserPowerInWatts, PRaylaseCoordinateTransform pCoordinateTransform);

		virtual ~CRaylaseCardList();

		// Laser Index Filter of 0 means no filter.
		void addLayerToList (LibMCEnv::PToolpathLayer pLayer, uint32_t nLaserIndexFilter, bool bFailIfNonAssignedDataExists);

		void appendPowerInWatts(double dPowerInWatts);

		void setListOnCard(uint32_t nListIDOnCard);

		void deleteListListOnCard();

		void executeList(uint32_t nListIDOnCard);

		bool waitForExecution(uint32_t nTimeOutInMS);

		void setPartIgnoreState(const std::string & sUUID, eRaylasePartIgnoreState ignoreState);

		void clearPartIgnoreStates ();
};

typedef std::shared_ptr<CRaylaseCardList> PRaylaseCardList;

} // namespace Impl
} // namespace LibMCDriver_Raylase


#endif // __LIBMCDRIVER_RAYLASE_RAYLASECARDLIST
