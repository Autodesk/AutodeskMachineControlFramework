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

*/


#include "amc_scatterplot.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

namespace AMC {

	CScatterplot::CScatterplot(const std::string& sUUID)
		: m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID))
	{

	}

	CScatterplot::~CScatterplot()
	{

	}

	std::string CScatterplot::getUUID()
	{
		return m_sUUID;
	}

	void CScatterplot::clearData()
	{
		m_PointEntries.clear();
	}

	bool CScatterplot::isEmpty()
	{
		return m_PointEntries.empty ();
	}

	uint64_t CScatterplot::getEntryCount()
	{
		return m_PointEntries.size();
	}

	std::vector<sScatterplotEntry>& CScatterplot::getEntries()
	{
		return m_PointEntries;
	}

	void CScatterplot::getBoundaries(double& dMinX, double& dMinY, double& dMaxX, double& dMaxY)
	{
		dMinX = m_dMinX;
		dMinY = m_dMinY;
		dMaxX = m_dMaxX;
		dMaxY = m_dMaxY;
	}

	void CScatterplot::computeBoundaries()
	{
		size_t nPointCount = m_PointEntries.size();
		if (nPointCount > 0) {

			auto& point = m_PointEntries.at(0);
			m_dMinX = point.m_dX;
			m_dMinY = point.m_dY;
			m_dMaxX = point.m_dX;
			m_dMaxY = point.m_dY;

			for (size_t nIndex = 1; nIndex < nPointCount; nIndex++) {
				auto& point = m_PointEntries.at(nIndex);
				if (point.m_dX < m_dMinX)
					m_dMinX = point.m_dX;
				if (point.m_dY < m_dMinY)
					m_dMinY = point.m_dY;
				if (point.m_dX > m_dMaxX)
					m_dMaxX = point.m_dX;
				if (point.m_dY > m_dMaxY)
					m_dMaxY = point.m_dY;
			}


		}
		else {
			m_dMinX = 0.0;
			m_dMinY = 0.0;
			m_dMaxX = 0.0;
			m_dMaxY = 0.0;

		}
	}

}


