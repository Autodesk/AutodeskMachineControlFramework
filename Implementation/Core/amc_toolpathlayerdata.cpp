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


#include "amc_toolpathlayerdata.hpp"
#include "libmc_interfaceexception.hpp"

namespace AMC {

	CToolpathLayerData::CToolpathLayerData(Lib3MF::PToolpath pToolpath, Lib3MF::PToolpathLayerReader p3MFLayer, double dUnits, int32_t nZValue)
		: m_dUnits (dUnits), m_nZValue (nZValue)
	{
		if (p3MFLayer.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pToolpath.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_sUUID = p3MFLayer->GetLayerDataUUID();
		uint32_t nSegmentCount = p3MFLayer->GetSegmentCount();
		uint32_t nTotalPointCount = 0;

		// Read segment information
		m_Segments.resize(nSegmentCount);
		for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {
			Lib3MF::eToolpathSegmentType eType = Lib3MF::eToolpathSegmentType::Unknown;
			uint32_t nPointCount = 0;
			p3MFLayer->GetSegmentInfo(nSegmentIndex, eType, nPointCount);
			std::string sPartUUID = p3MFLayer->GetSegmentPartUUID(nSegmentIndex);
			std::string sProfileUUID = p3MFLayer->GetSegmentProfileUUID(nSegmentIndex);

			auto pSegment = &m_Segments[nSegmentIndex];
			pSegment->m_PointCount = nPointCount;
			pSegment->m_PointStartIndex = nTotalPointCount;
			pSegment->m_Type = (LibMCEnv::eToolpathSegmentType) eType;
			pSegment->m_ProfileID = registerUUID (sProfileUUID);
			pSegment->m_PartID = registerUUID(sPartUUID);

			storeProfileData (pToolpath, sProfileUUID);

			nTotalPointCount += nPointCount;
		}

		// Read point information
		m_Points.resize(nTotalPointCount);
		for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {
			auto pSegment = &m_Segments[nSegmentIndex];

			std::vector<Lib3MF::sPosition2D> PointData;
			p3MFLayer->GetSegmentPointData(nSegmentIndex, PointData);

			if ((uint32_t)PointData.size() != pSegment->m_PointCount)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPOINTCOUNT);

			if (pSegment->m_PointCount > 0) {

				if (pSegment->m_PointStartIndex + pSegment->m_PointCount > m_Points.size())
					throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPOINTCOUNT);

				auto pSrcPoint = &PointData[0];
				auto pDstPoint = &m_Points[pSegment->m_PointStartIndex];

				for (uint32_t nPointIndex = 0; nPointIndex < pSegment->m_PointCount; nPointIndex++) {
					
					pDstPoint->m_Coordinates[0] = (int) pSrcPoint->m_Coordinates[0];
					pDstPoint->m_Coordinates[1] = (int) pSrcPoint->m_Coordinates[1];
					pSrcPoint++;
					pDstPoint++;
					
				}

			}
		}

		

	}

	CToolpathLayerData::~CToolpathLayerData()
	{

	}

	uint32_t CToolpathLayerData::getSegmentCount()
	{
		return (uint32_t) m_Segments.size();
	}

	std::string CToolpathLayerData::getUUID()
	{
		return m_sUUID;
	}

	uint32_t CToolpathLayerData::getSegmentPointCount(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		return m_Segments[nSegmentIndex].m_PointCount;
	}

	LibMCEnv::eToolpathSegmentType CToolpathLayerData::getSegmentType(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		return m_Segments[nSegmentIndex].m_Type;
	}

	void CToolpathLayerData::storePointsToBuffer(const uint32_t nSegmentIndex, LibMCEnv::sPosition2D* pPositionData)
	{
		if (pPositionData == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto pSegment = &m_Segments[nSegmentIndex];
		if (pSegment->m_PointCount > 0) {
			auto pSrcPoint = &m_Points[pSegment->m_PointStartIndex];
			auto pDstPoint = pPositionData;
			for (uint32_t nPointIndex = 0; nPointIndex < pSegment->m_PointCount; nPointIndex++) {
				*pDstPoint = *pSrcPoint;
				pDstPoint++;
				pSrcPoint++;
			}
		}

	}

	uint32_t CToolpathLayerData::registerUUID(const std::string& sUUID)
	{
		if (sUUID.empty())
			return 0;

		auto iIterator = m_UUIDMap.find(sUUID);
		if (iIterator != m_UUIDMap.end())
			return iIterator->second;

		m_UUIDs.push_back(sUUID);
		uint32_t nNewID = (uint32_t) m_UUIDs.size (); // ID is (index + 1).

		m_UUIDMap.insert(std::make_pair (sUUID, nNewID));
		return nNewID;
	}


	std::string CToolpathLayerData::getRegisteredUUID(const uint32_t nID)
	{
		if (nID == 0)
			return "";

		if (nID > m_UUIDs.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		return m_UUIDs[nID - 1];
	}


	std::string CToolpathLayerData::getSegmentProfileUUID(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		return getRegisteredUUID(m_Segments[nSegmentIndex].m_ProfileID);
	}

	std::string CToolpathLayerData::getSegmentPartUUID(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		return getRegisteredUUID(m_Segments[nSegmentIndex].m_PartID);
	}

	sToolpathLayerProfile CToolpathLayerData::getSegmentProfile(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDINDEX);

		auto sProfileUUID = getRegisteredUUID(m_Segments[nSegmentIndex].m_ProfileID);
		return retrieveProfileData(sProfileUUID);
	}

	double CToolpathLayerData::getUnits()
	{
		return m_dUnits;
	}

	int32_t CToolpathLayerData::getZValue()
	{
		return m_nZValue;
	}

	void CToolpathLayerData::storeProfileData(Lib3MF::PToolpath pToolpath, const std::string & sProfileUUID)
	{
		if (pToolpath.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		auto iIter = m_ProfileMap.find(sProfileUUID);
		if (iIter == m_ProfileMap.end()) {

			auto pProfile = pToolpath->GetProfileUUID(sProfileUUID);
			sToolpathLayerProfile sProfile;

			sProfile.m_dLaserSpeed = pProfile->GetParameterDoubleValue("", "laserspeed");
			sProfile.m_dLaserPower = pProfile->GetParameterDoubleValue("", "laserpower");
			sProfile.m_dLaserFocus = pProfile->GetParameterDoubleValueDef("", "laserfocus", 0.0);
			sProfile.m_dJumpSpeed = pProfile->GetParameterDoubleValueDef("", "jumpspeed", sProfile.m_dLaserSpeed);
			sProfile.m_dExtrusionFactor = pProfile->GetParameterDoubleValueDef("", "extrusionfactor", 0.0);
			sProfile.m_dStartDelay = pProfile->GetParameterDoubleValueDef("", "startdelay", 0.0);
			sProfile.m_dEndDelay = pProfile->GetParameterDoubleValueDef("", "enddelay", 0.0);
			sProfile.m_dPolyDelay = pProfile->GetParameterDoubleValueDef("", "polydelay", 0.0);
			sProfile.m_dJumpDelay = pProfile->GetParameterDoubleValueDef("", "jumpdelay", 0.0);
			sProfile.m_dLaserOnDelay = pProfile->GetParameterDoubleValueDef("", "laserondelay", 0.0);
			sProfile.m_dLaserOffDelay = pProfile->GetParameterDoubleValueDef("", "laseroffdelay", 0.0);

			m_ProfileMap.insert(std::make_pair (sProfileUUID, sProfile));
		}

	}


	sToolpathLayerProfile CToolpathLayerData::retrieveProfileData(const std::string& sProfileUUID)
	{
		auto iIter = m_ProfileMap.find(sProfileUUID);
		if (iIter == m_ProfileMap.end())
			throw ELibMCInterfaceException(LIBMC_ERROR_PROFILENOTFOUND);

		return iIter->second;
	}

}


