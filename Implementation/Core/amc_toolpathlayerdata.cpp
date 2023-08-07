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
#include "libmc_exceptiontypes.hpp"

namespace AMC {


	CToolpathCustomSegmentAttribute::CToolpathCustomSegmentAttribute(const std::string& sNameSpace, const std::string& sAttributeName, LibMCEnv::eToolpathAttributeType attributeType)
		: m_sNameSpace (sNameSpace), m_sAttributeName (sAttributeName), m_AttributeType (attributeType), m_nAttributeID (0)
	{

	}

	CToolpathCustomSegmentAttribute::~CToolpathCustomSegmentAttribute()
	{

	}

	uint32_t CToolpathCustomSegmentAttribute::getAttributeID()
	{
		return m_nAttributeID;
	}

	void CToolpathCustomSegmentAttribute::setAttributeID(uint32_t nAttributeID)
	{
		m_nAttributeID = nAttributeID;
	}

	LibMCEnv::eToolpathAttributeType CToolpathCustomSegmentAttribute::getAttributeType()
	{
		return m_AttributeType;
	}

	std::string CToolpathCustomSegmentAttribute::getNameSpace()
	{
		return m_sNameSpace;
	}

	std::string CToolpathCustomSegmentAttribute::getAttributeName()
	{
		return m_sAttributeName;
	}

	CToolpathLayerProfile::CToolpathLayerProfile(const std::string& sUUID)
		: m_sUUID (sUUID)
	{

	}

	CToolpathLayerProfile::~CToolpathLayerProfile()
	{

	}

	std::string CToolpathLayerProfile::getUUID()
	{
		return m_sUUID;
	}

	void CToolpathLayerProfile::addValue(const std::string& sNameSpace, const std::string& sValueName, const std::string& sValue)
	{
		m_ProfileValues.insert(std::make_pair(std::make_pair(sNameSpace, sValueName), sValue));
	}

	bool CToolpathLayerProfile::hasValue(const std::string& sNameSpace, const std::string& sValueName)
	{
		auto iIter = m_ProfileValues.find(std::make_pair(sNameSpace, sValueName));
		return (iIter != m_ProfileValues.end());
	}

	std::string CToolpathLayerProfile::getValue(const std::string& sNameSpace, const std::string& sValueName)
	{
		auto iIter = m_ProfileValues.find(std::make_pair(sNameSpace, sValueName));
		if (iIter == m_ProfileValues.end())
			throw ELibMCCustomException(LIBMC_ERROR_PROFILEVALUENOTFOUND, sNameSpace + "/" + sValueName);

		return iIter->second;
	}

	std::string CToolpathLayerProfile::getValueDef(const std::string& sNameSpace, const std::string& sValueName, const std::string& sDefaultValue)
	{
		auto iIter = m_ProfileValues.find(std::make_pair(sNameSpace, sValueName));
		if (iIter == m_ProfileValues.end())
			return sDefaultValue;

		return iIter->second;

	}

	CToolpathLayerData::CToolpathLayerData(Lib3MF::PToolpath pToolpath, Lib3MF::PToolpathLayerReader p3MFLayer, double dUnits, int32_t nZValue, const std::string& sDebugName, std::vector<CToolpathCustomSegmentAttribute> customSegmentAttributes)
		: m_dUnits (dUnits), m_nZValue (nZValue), m_sDebugName (sDebugName), m_CustomSegmentAttributes (customSegmentAttributes)
	{
		LibMCAssertNotNull(p3MFLayer.get());
		LibMCAssertNotNull(pToolpath.get());

		m_sUUID = p3MFLayer->GetLayerDataUUID();
		uint32_t nSegmentCount = p3MFLayer->GetSegmentCount();
		uint32_t nTotalPointCount = 0;

		for (auto& attribute : m_CustomSegmentAttributes) {
			std::string sNameSpace = attribute.getNameSpace();
			std::string sAttributeName = attribute.getAttributeName();
			attribute.setAttributeID (p3MFLayer->FindAttributeIDByName (sNameSpace, sAttributeName));
			m_CustomSegmentAttributeMap.insert (std::make_pair (std::make_pair (sNameSpace, sAttributeName), &attribute));
		}

		m_SegmentAttributeData.resize((size_t)nSegmentCount * m_CustomSegmentAttributes.size());

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
			if (m_CustomSegmentAttributes.size() > 0) {
				pSegment->m_AttributeData = &m_SegmentAttributeData.at((size_t)nSegmentIndex * m_CustomSegmentAttributes.size());
				int64_t* pAttributeData = pSegment->m_AttributeData;
				for (auto& attribute : m_CustomSegmentAttributes) {
					switch (attribute.getAttributeType()) {
						case LibMCEnv::eToolpathAttributeType::Integer:
							*pAttributeData = p3MFLayer->GetSegmentIntegerAttributeByID(nSegmentIndex, attribute.getAttributeID());
							break;
						case LibMCEnv::eToolpathAttributeType::Double:
							*((double*)pAttributeData) = p3MFLayer->GetSegmentDoubleAttributeByID(nSegmentIndex, attribute.getAttributeID());
							break;
						default:
							throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHATTRIBUTETYPE, m_sDebugName);
					}
					pAttributeData++;
				}

			}
			else {
				pSegment->m_AttributeData = nullptr;
			}

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
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

			if (pSegment->m_PointCount > 0) {

				if ((size_t) pSegment->m_PointStartIndex + pSegment->m_PointCount > m_Points.size())
					throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

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

		uint32_t nCustomDataCount = p3MFLayer->GetCustomDataCount();
		for (uint32_t nCustomDataIndex = 0; nCustomDataIndex < nCustomDataCount; nCustomDataIndex++) {
			auto pCustomData = p3MFLayer->GetCustomData(nCustomDataIndex);
			std::string sNameSpace = pCustomData->GetNameSpace();
			std::string sRootName = pCustomData->GetRootNode()->GetName();
			auto sXMLString = pCustomData->SaveToString();

			m_CustomData.push_back(std::make_pair (std::make_pair (sNameSpace, sRootName), sXMLString));
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
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return m_Segments[nSegmentIndex].m_PointCount;
	}

	LibMCEnv::eToolpathSegmentType CToolpathLayerData::getSegmentType(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return m_Segments[nSegmentIndex].m_Type;
	}

	void CToolpathLayerData::storePointsToBufferInUnits(const uint32_t nSegmentIndex, LibMCEnv::sPosition2D* pPositionData)
	{
		LibMCAssertNotNull(pPositionData);
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

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

	void CToolpathLayerData::storeHatchesToBufferInUnits(const uint32_t nSegmentIndex, LibMCEnv::sHatch2D* pHatchData)
	{
		LibMCAssertNotNull(pHatchData);
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		if (pSegment->m_PointCount > 0) {
			uint32_t nHatchCount = pSegment->m_PointCount / 2;

			auto pSrcPoint = &m_Points[pSegment->m_PointStartIndex];
			auto pDstHatch = pHatchData;
			for (uint32_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
				pDstHatch->m_X1 = pSrcPoint->m_Coordinates[0];
				pDstHatch->m_Y1 = pSrcPoint->m_Coordinates[1];
				pSrcPoint++;
				pDstHatch->m_X2 = pSrcPoint->m_Coordinates[0];
				pDstHatch->m_Y2 = pSrcPoint->m_Coordinates[1];
				pSrcPoint++;
				pDstHatch++;

			}
		}
	}

	void CToolpathLayerData::storePointsToBufferInMM(const uint32_t nSegmentIndex, LibMCEnv::sFloatPosition2D* pPositionData)
	{
		LibMCAssertNotNull(pPositionData);
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		if (pSegment->m_PointCount > 0) {
			auto pSrcPoint = &m_Points[pSegment->m_PointStartIndex];
			auto pDstPoint = pPositionData;
			for (uint32_t nPointIndex = 0; nPointIndex < pSegment->m_PointCount; nPointIndex++) {
				pDstPoint->m_Coordinates[0] = pSrcPoint->m_Coordinates[0] * m_dUnits;
				pDstPoint->m_Coordinates[1] = pSrcPoint->m_Coordinates[1] * m_dUnits;
				pDstPoint++;
				pSrcPoint++;
			}
		}
	}

	void CToolpathLayerData::storeHatchesToBufferInMM(const uint32_t nSegmentIndex, LibMCEnv::sFloatHatch2D* pHatchData)
	{
		LibMCAssertNotNull(pHatchData);
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		if (pSegment->m_PointCount > 0) {
			uint32_t nHatchCount = pSegment->m_PointCount / 2;

			auto pSrcPoint = &m_Points[pSegment->m_PointStartIndex];
			auto pDstHatch = pHatchData;
			for (uint32_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
				pDstHatch->m_X1 = pSrcPoint->m_Coordinates[0] * m_dUnits;
				pDstHatch->m_Y1 = pSrcPoint->m_Coordinates[1] * m_dUnits;
				pSrcPoint++;
				pDstHatch->m_X2 = pSrcPoint->m_Coordinates[0] * m_dUnits;
				pDstHatch->m_Y2 = pSrcPoint->m_Coordinates[1] * m_dUnits;
				pSrcPoint++;
				pDstHatch++;

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
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return m_UUIDs[nID - 1];
	}


	std::string CToolpathLayerData::getSegmentProfileUUID(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return getRegisteredUUID(m_Segments[nSegmentIndex].m_ProfileID);
	}

	std::string CToolpathLayerData::getSegmentPartUUID(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return getRegisteredUUID(m_Segments[nSegmentIndex].m_PartID);
	}

	PToolpathLayerProfile CToolpathLayerData::getSegmentProfile(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto sProfileUUID = getRegisteredUUID(m_Segments[nSegmentIndex].m_ProfileID);
		return retrieveProfileData(sProfileUUID);
	}

	bool CToolpathLayerData::findCustomSegmentAttribute(const std::string& sNameSpace, const std::string& sAttributeName, uint32_t& nAttributeID, LibMCEnv::eToolpathAttributeType& attributeType)
	{
		auto iIter = m_CustomSegmentAttributeMap.find(std::make_pair(sNameSpace, sAttributeName));
		if (iIter != m_CustomSegmentAttributeMap.end()) {
			nAttributeID = iIter->second->getAttributeID ();
			attributeType = iIter->second->getAttributeType();
			return true;
		}
		else {
			nAttributeID = 0;
			attributeType = LibMCEnv::eToolpathAttributeType::Unknown;
			return false;
		}
	}

	int64_t CToolpathLayerData::getSegmentIntegerAttribute(const uint32_t nSegmentIndex, uint32_t nAttributeID)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		if ((nAttributeID < 1) || (nAttributeID > m_CustomSegmentAttributes.size ()))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHATTRIBUTEID, m_sDebugName);

		auto eAttributeType = m_CustomSegmentAttributes.at(nAttributeID - 1).getAttributeType();
		if (eAttributeType != LibMCEnv::eToolpathAttributeType::Integer)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHATTRIBUTETYPE, m_sDebugName);

		return m_Segments[nSegmentIndex].m_AttributeData[nAttributeID - 1];
	}


	double CToolpathLayerData::getSegmentDoubleAttribute(const uint32_t nSegmentIndex, uint32_t nAttributeID)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		if ((nAttributeID < 1) || (nAttributeID > m_CustomSegmentAttributes.size()))
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHATTRIBUTEID, m_sDebugName);

		auto eAttributeType = m_CustomSegmentAttributes.at(nAttributeID - 1).getAttributeType();
		if (eAttributeType != LibMCEnv::eToolpathAttributeType::Double)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHATTRIBUTETYPE, m_sDebugName);

		double * pData = (double*) &m_Segments[nSegmentIndex].m_AttributeData[nAttributeID - 1];
		return *pData;
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
		LibMCAssertNotNull(pToolpath.get());

		auto iIter = m_ProfileMap.find(sProfileUUID);
		if (iIter == m_ProfileMap.end()) {

			auto p3MFProfile = pToolpath->GetProfileUUID(sProfileUUID);
			PToolpathLayerProfile pLayerProfile = std::make_shared<CToolpathLayerProfile> (sProfileUUID);

			uint32_t nParameterCount = p3MFProfile->GetParameterCount();
			for (uint32_t nParameterIndex = 0; nParameterIndex < nParameterCount; nParameterIndex++) {
				std::string sParameterName = p3MFProfile->GetParameterName(nParameterIndex);
				std::string sParameterNamespace = p3MFProfile->GetParameterNameSpace(nParameterIndex);
				std::string sParameterValue = p3MFProfile->GetParameterValue(sParameterNamespace, sParameterName);

				pLayerProfile->addValue(sParameterNamespace, sParameterName, sParameterValue);
			}



			m_ProfileMap.insert(std::make_pair (sProfileUUID, pLayerProfile));
		}

	}


	PToolpathLayerProfile CToolpathLayerData::retrieveProfileData(const std::string& sProfileUUID)
	{
		auto iIter = m_ProfileMap.find(sProfileUUID);
		if (iIter == m_ProfileMap.end())
			throw ELibMCCustomException(LIBMC_ERROR_PROFILENOTFOUND, m_sDebugName + "/" + sProfileUUID);

		return iIter->second;
	}

	uint32_t CToolpathLayerData::getMetaDataCount()
	{
		return (uint32_t)m_CustomData.size();
	}

	void CToolpathLayerData::getMetaDataInfo(uint32_t nMetaDataIndex, std::string& sNameSpace, std::string& sName)
	{
		if (nMetaDataIndex >= m_CustomData.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMETADATAINDEX);

		auto& entry = m_CustomData.at(nMetaDataIndex);
		sNameSpace = entry.first.first;
		sName = entry.first.second;
	}

	PXMLDocumentInstance CToolpathLayerData::getMetaData(uint32_t nMetaDataIndex)
	{
		if (nMetaDataIndex >= m_CustomData.size())
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDMETADATAINDEX);

		auto& entry = m_CustomData.at(nMetaDataIndex);

		PXMLDocumentInstance pXMLInstance = std::make_shared <CXMLDocumentInstance>();
		pXMLInstance->parseXMLString(entry.second);

		return pXMLInstance;

	}

	bool CToolpathLayerData::hasUniqueMetaData(const std::string& sNameSpace, const std::string& sName)
	{
		uint32_t nFoundCount = 0;
		for (auto& entry : m_CustomData) {
			if ((entry.first.first == sNameSpace) && (entry.first.second == sName))
				nFoundCount++;
		}

		return (nFoundCount == 1);
	}

	PXMLDocumentInstance CToolpathLayerData::findUniqueMetaData(const std::string& sNameSpace, const std::string& sName)
	{
		uint32_t nFoundCount = 0;
		std::pair<std::pair<std::string, std::string>, std::string> foundEntry;

		for (auto& entry : m_CustomData) {
			if ((entry.first.first == sNameSpace) && (entry.first.second == sName)) {
				foundEntry = entry;
				nFoundCount++;
			}
		}

		if (nFoundCount == 0) 
			throw ELibMCCustomException(LIBMC_ERROR_METADATANOTFOUND, sNameSpace + "/" + sName);
	
		if (nFoundCount > 1)
			throw ELibMCCustomException(LIBMC_ERROR_METADATAISNOTUNIQUE, sNameSpace + "/" + sName);

		PXMLDocumentInstance pXMLInstance = std::make_shared <CXMLDocumentInstance>();
		pXMLInstance->parseXMLString(foundEntry.second);

		return pXMLInstance;

	}

}


