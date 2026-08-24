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

#include "common_utils.hpp"

#include "amc_parametertype.hpp"

namespace AMC {


	CToolpathCustomSegmentAttribute::CToolpathCustomSegmentAttribute(uint32_t nInternalAttributeID, const std::string& sNameSpace, const std::string& sAttributeName, LibMCEnv::eToolpathAttributeType attributeType)
		: m_sNameSpace (sNameSpace), m_sAttributeName (sAttributeName), m_AttributeType (attributeType), m_n3MFAttributeID (0), m_nInternalAttributeID (nInternalAttributeID)
	{

	}

	CToolpathCustomSegmentAttribute::~CToolpathCustomSegmentAttribute()
	{

	}

	uint32_t CToolpathCustomSegmentAttribute::getInternalAttributeID()
	{
		return m_nInternalAttributeID;
	}

	uint32_t CToolpathCustomSegmentAttribute::get3MFAttributeID()
	{
		return m_n3MFAttributeID;
	}

	void CToolpathCustomSegmentAttribute::set3MFAttributeID(uint32_t nAttributeID)
	{
		m_n3MFAttributeID = nAttributeID;
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


	CToolpathLayerProfileModifier::CToolpathLayerProfileModifier(LibMCEnv::eToolpathProfileModificationType modificationType, LibMCEnv::eToolpathProfileModificationFactor modificationFactor, double dMinValue, double dMaxValue)
		: m_ModificationType (modificationType), m_ModificationFactor (modificationFactor), m_dMinValue (dMinValue), m_dMaxValue (dMaxValue)
	{

	}

	CToolpathLayerProfileModifier::~CToolpathLayerProfileModifier()
	{

	}

	LibMCEnv::eToolpathProfileModificationType CToolpathLayerProfileModifier::getModificationType()
	{
		return m_ModificationType;
	}

	LibMCEnv::eToolpathProfileModificationFactor CToolpathLayerProfileModifier::getModificationFactor()
	{
		return m_ModificationFactor;
	}

	double CToolpathLayerProfileModifier::getMinValue()
	{
		return m_dMinValue;
	}

	double CToolpathLayerProfileModifier::getMaxValue()
	{
		return m_dMaxValue;
	}


	CToolpathLayerProfile::CToolpathLayerProfile(const uint32_t nProfileIndex, const std::string& sUUID, const std::string& sName)
		: m_sUUID (sUUID), m_sName (sName), m_nProfileIndex (nProfileIndex)
	{

	}

	CToolpathLayerProfile::~CToolpathLayerProfile()
	{

	}

	std::string CToolpathLayerProfile::getUUID()
	{
		return m_sUUID;
	}

	std::string CToolpathLayerProfile::getName()
	{
		return m_sName;
	}


	void CToolpathLayerProfile::addValue(const std::string& sNameSpace, const std::string& sValueName, const std::string& sValue)
	{
		m_ProfileValues.insert(std::make_pair(std::make_pair(sNameSpace, sValueName), sValue));
	}

	void CToolpathLayerProfile::addModifier(const std::string& sNameSpace, const std::string& sValueName, LibMCEnv::eToolpathProfileModificationType modificationType, LibMCEnv::eToolpathProfileModificationFactor modificationFactor, double dMinValue, double dMaxValue)
	{
		m_ProfileModifiers.insert(std::make_pair (std::make_pair(sNameSpace, sValueName), CToolpathLayerProfileModifier (modificationType, modificationFactor, dMinValue, dMaxValue)));
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

	double CToolpathLayerProfile::getDoubleValue(const std::string& sNameSpace, const std::string& sValueName)
	{
		return AMCCommon::CUtils::stringToDouble(getValue(sNameSpace, sValueName));
	}

	double CToolpathLayerProfile::getDoubleValueDef(const std::string& sNameSpace, const std::string& sValueName, double dDefaultValue)
	{
		return AMCCommon::CUtils::stringToDouble(getValueDef(sNameSpace, sValueName, std::to_string(dDefaultValue)));
	}

	int64_t CToolpathLayerProfile::getIntegerValue(const std::string& sNameSpace, const std::string& sValueName)
	{
		return AMCCommon::CUtils::stringToIntegerWithAccuracy(getValue(sNameSpace, sValueName), PARAMETER_INTEGERACCURACY);
	}

	int64_t CToolpathLayerProfile::getIntegerValueDef(const std::string& sNameSpace, const std::string& sValueName, int64_t nDefaultValue)
	{
		return AMCCommon::CUtils::stringToIntegerWithAccuracy(getValueDef(sNameSpace, sValueName, std::to_string (nDefaultValue)), PARAMETER_INTEGERACCURACY);
	}

	uint32_t CToolpathLayerProfile::getProfileIndex()
	{
		return m_nProfileIndex;
	}

	LibMCEnv::eToolpathProfileModificationType CToolpathLayerProfile::getModificationType(const std::string& sNameSpace, const std::string& sValueName)
	{
		auto iIter = m_ProfileModifiers.find(std::make_pair(sNameSpace, sValueName));
		if (iIter == m_ProfileModifiers.end ())
			return LibMCEnv::eToolpathProfileModificationType::NoModification;

		return iIter->second.getModificationType();
	}

	void CToolpathLayerProfile::getModificationInformation(const std::string& sNameSpace, const std::string& sValueName, LibMCEnv::eToolpathProfileModificationFactor& modificationFactor, double& dMinValue, double& dMaxValue)
	{
		auto iIter = m_ProfileModifiers.find(std::make_pair(sNameSpace, sValueName));
		if (iIter == m_ProfileModifiers.end()) {
			modificationFactor = LibMCEnv::eToolpathProfileModificationFactor::Unknown;
			dMinValue = 0.0;
			dMaxValue = 0.0;

		}
		else {
			modificationFactor = iIter->second.getModificationFactor();
			dMinValue = iIter->second.getMinValue();
			dMaxValue = iIter->second.getMaxValue();

		}			

	}



	CToolpathLayerData::CToolpathLayerData(Lib3MF::PToolpath pToolpath, Lib3MF::PToolpathLayerReader p3MFLayer, double dUnits, int32_t nZValue, const std::string& sDebugName, std::vector<PToolpathCustomSegmentAttribute> customSegmentAttributes)
		: m_dUnits (dUnits), m_nZValue (nZValue), m_sDebugName (sDebugName), m_CustomSegmentAttributes (customSegmentAttributes)
	{
		LibMCAssertNotNull(p3MFLayer.get());
		LibMCAssertNotNull(pToolpath.get());

		m_sUUID = p3MFLayer->GetLayerDataUUID();
		uint32_t nSegmentCount = p3MFLayer->GetSegmentCount();
		uint32_t nTotalPointCount = 0;

		for (auto& attribute : m_CustomSegmentAttributes) {
			std::string sNameSpace = attribute->getNameSpace();
			std::string sAttributeName = attribute->getAttributeName();
			attribute->set3MFAttributeID (p3MFLayer->FindSegmentAttributeIDByName (sNameSpace, sAttributeName));
			m_CustomSegmentAttributeMap.insert (std::make_pair (std::make_pair (sNameSpace, sAttributeName), attribute));
		}

		m_Segments.reserve(nSegmentCount);

		m_SegmentAttributeData.resize((size_t)nSegmentCount * m_CustomSegmentAttributes.size());

		// Read segment information		
		for (uint32_t nSegmentIndex = 0; nSegmentIndex < nSegmentCount; nSegmentIndex++) {
			Lib3MF::eToolpathSegmentType eType = Lib3MF::eToolpathSegmentType::Unknown;
			uint32_t nPointCount = 0;
			p3MFLayer->GetSegmentInfo(nSegmentIndex, eType, nPointCount);
			std::string sBuildItemUUID = p3MFLayer->GetSegmentBuildItemUUID(nSegmentIndex);
			std::string sProfileUUID = p3MFLayer->GetSegmentDefaultProfileUUID(nSegmentIndex);
			uint32_t nLocalPartID = p3MFLayer->GetSegmentPartID(nSegmentIndex);
			uint32_t nLaserIndex = 0;

			sToolpathLayerSegment segment;

			switch (eType) {
				case Lib3MF::eToolpathSegmentType::Polyline:
					segment.m_Type = LibMCEnv::eToolpathSegmentType::Polyline;
					segment.m_PointCount = nPointCount;
					segment.m_PointStartIndex = nTotalPointCount;				
					nTotalPointCount += nPointCount;
					break;

				case Lib3MF::eToolpathSegmentType::Loop:
					segment.m_Type = LibMCEnv::eToolpathSegmentType::Polyline;
					segment.m_PointCount = nPointCount + 1;
					segment.m_PointStartIndex = nTotalPointCount;
					nTotalPointCount += (nPointCount + 1);
					break;

				case Lib3MF::eToolpathSegmentType::Hatch:
					segment.m_Type = LibMCEnv::eToolpathSegmentType::Hatch;
					segment.m_PointCount = nPointCount;
					segment.m_PointStartIndex = nTotalPointCount;
					nTotalPointCount += nPointCount;
					break;

				default:
					segment.m_Type = LibMCEnv::eToolpathSegmentType::Unknown;
			}

			if (segment.m_Type != LibMCEnv::eToolpathSegmentType::Unknown) {

				segment.m_3MFSegmentIndex = nSegmentIndex;
				segment.m_ProfileUUID = registerUUID(sProfileUUID);
				segment.m_PartUUID = registerUUID(sBuildItemUUID);
				segment.m_LocalPartID = nLocalPartID;
				segment.m_LaserIndex = 0;
				segment.m_TotalSubinterpolationCount = 0;
				segment.m_HasOverrideFactors = 0;
				if (m_CustomSegmentAttributes.size() > 0) {
					segment.m_AttributeData = &m_SegmentAttributeData.at((size_t)nSegmentIndex * m_CustomSegmentAttributes.size());
					int64_t* pAttributeData = segment.m_AttributeData;

					for (auto& attribute : m_CustomSegmentAttributes) {
						switch (attribute->getAttributeType()) {
						case LibMCEnv::eToolpathAttributeType::Integer:
							*pAttributeData = p3MFLayer->GetSegmentIntegerAttributeByID(nSegmentIndex, attribute->get3MFAttributeID());
							break;
						case LibMCEnv::eToolpathAttributeType::Double:
							*((double*)pAttributeData) = p3MFLayer->GetSegmentDoubleAttributeByID(nSegmentIndex, attribute->get3MFAttributeID());
							break;
						default:
							throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHATTRIBUTETYPE, m_sDebugName);
						}
						pAttributeData++;
					}

				}
				else {
					segment.m_AttributeData = nullptr;
				}

				m_Segments.push_back(segment);

				storeProfileData(pToolpath, sProfileUUID);
			}

		}

		// Read point information
		m_Points.resize(nTotalPointCount);
		m_OverrideFactors.resize(nTotalPointCount);
		for (auto & segment : m_Segments) {

			Lib3MF::eToolpathSegmentType eType = Lib3MF::eToolpathSegmentType::Unknown;
			uint32_t nPointCount = 0;
			p3MFLayer->GetSegmentInfo(segment.m_3MFSegmentIndex, eType, nPointCount);

			switch (eType) {
				case Lib3MF::eToolpathSegmentType::Polyline: 
				{
					std::vector<Lib3MF::sDiscretePosition2D> PointData;
					p3MFLayer->GetSegmentPointDataDiscrete(segment.m_3MFSegmentIndex, PointData);

					if ((uint32_t)PointData.size() != segment.m_PointCount)
						throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

					if (segment.m_PointCount > 0) {

						if ((size_t)segment.m_PointStartIndex + segment.m_PointCount > m_Points.size())
							throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

						auto pSrcPoint = &PointData[0];
						auto pDstPoint = &m_Points[segment.m_PointStartIndex];

						for (uint32_t nPointIndex = 0; nPointIndex < segment.m_PointCount; nPointIndex++) {

							pDstPoint->m_Coordinates[0] = pSrcPoint->m_Coordinates[0];
							pDstPoint->m_Coordinates[1] = pSrcPoint->m_Coordinates[1];
							pSrcPoint++;
							pDstPoint++;

						}
					}

					break;
				}

				case Lib3MF::eToolpathSegmentType::Loop: {

					std::vector<Lib3MF::sDiscretePosition2D> PointData;
					p3MFLayer->GetSegmentPointDataDiscrete(segment.m_3MFSegmentIndex, PointData);

					if ((uint32_t)(PointData.size() + 1) != segment.m_PointCount)
						throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

					if (segment.m_PointCount > 0) {

						if ((size_t)segment.m_PointStartIndex + segment.m_PointCount > m_Points.size())
							throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

						auto pSrcPoint = &PointData[0];
						auto pDstPoint = &m_Points[segment.m_PointStartIndex];

						uint32_t nLoopPointCount = (uint32_t)PointData.size();

						for (uint32_t nPointIndex = 0; nPointIndex < nLoopPointCount; nPointIndex++) {

							pDstPoint->m_Coordinates[0] = pSrcPoint->m_Coordinates[0];
							pDstPoint->m_Coordinates[1] = pSrcPoint->m_Coordinates[1];
							pSrcPoint++;
							pDstPoint++;

						}

						// Close loop
						pSrcPoint = &PointData[0];
						pDstPoint->m_Coordinates[0] = pSrcPoint->m_Coordinates[0];
						pDstPoint->m_Coordinates[1] = pSrcPoint->m_Coordinates[1];
					}

					break;
				}

				case Lib3MF::eToolpathSegmentType::Hatch: {

					std::vector<Lib3MF::sDiscreteHatch2D> HatchData;
					p3MFLayer->GetSegmentHatchDataDiscrete(segment.m_3MFSegmentIndex, HatchData);

					if ((uint32_t)(HatchData.size() * 2) != segment.m_PointCount)
						throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

					if (segment.m_PointCount > 0) {

						if ((size_t)segment.m_PointStartIndex + segment.m_PointCount > m_Points.size())
							throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTCOUNT, m_sDebugName);

						auto pSrcHatch = &HatchData[0];
						auto pDstPoint = &m_Points[segment.m_PointStartIndex];

						uint32_t nHatchCount = (uint32_t)HatchData.size();

						for (uint32_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {

							pDstPoint->m_Coordinates[0] = pSrcHatch->m_Point1Coordinates[0];
							pDstPoint->m_Coordinates[1] = pSrcHatch->m_Point1Coordinates[1];
							pDstPoint++;

							pDstPoint->m_Coordinates[0] = pSrcHatch->m_Point2Coordinates[0];
							pDstPoint->m_Coordinates[1] = pSrcHatch->m_Point2Coordinates[1];
							pDstPoint++;

							pSrcHatch++;

						}

#if USEALLMODIFICATIONFACTORS 				
						for (uint32_t nFactorIndex = 0; nFactorIndex < 3; nFactorIndex++) {
#else // USE ONLY FACTOR_F MODIFICATOR
						for (uint32_t nFactorIndex = 0; nFactorIndex < 1; nFactorIndex++) {
#endif
							Lib3MF::eToolpathProfileModificationFactor factorType = Lib3MF::eToolpathProfileModificationFactor::Unknown;
							uint32_t factorFlag = 0;
							switch (nFactorIndex) {
							case 0: factorType = Lib3MF::eToolpathProfileModificationFactor::FactorF;
								factorFlag = TOOLPATHSEGMENTOVERRIDEFACTOR_F;
								break;
							case 1: factorType = Lib3MF::eToolpathProfileModificationFactor::FactorG;
								factorFlag = TOOLPATHSEGMENTOVERRIDEFACTOR_G;
								break;
							case 2: factorType = Lib3MF::eToolpathProfileModificationFactor::FactorH;
								factorFlag = TOOLPATHSEGMENTOVERRIDEFACTOR_H;
								break;
							}

							if (p3MFLayer->SegmentHasModificationFactors(segment.m_3MFSegmentIndex, factorType)) {

								segment.m_HasOverrideFactors |= factorFlag;

								std::vector<uint32_t> nonLinearCounts;
								std::vector<Lib3MF::sHatchModificationInterpolationData> nonLinearValues;

								std::vector<Lib3MF::sHatch2DFactors> hatchFactors;
								p3MFLayer->GetLinearSegmentHatchModificationFactors(segment.m_3MFSegmentIndex, factorType, hatchFactors);
								p3MFLayer->GetSegmentAllNonlinearHatchesModificationInterpolation(segment.m_3MFSegmentIndex, factorType, nonLinearCounts, nonLinearValues);

								if ((uint32_t)(hatchFactors.size() * 2) != segment.m_PointCount)
									throw ELibMCCustomException(LIBMC_ERROR_INVALIDHATCHOVERRIDECOUNT, m_sDebugName);

								size_t nInterpolationDataStartIndex = m_InterpolationData.size();
								if (nonLinearValues.size() > 0) {
									double dPreviousParameter = 0.0;
									double dMinParameterIncrease = 1e-5;
									double dMaxParameter = 1.0 - dMinParameterIncrease;

									for (auto& interpolationData : nonLinearValues) {
										if (interpolationData.m_Parameter < dPreviousParameter + dMinParameterIncrease) 
											throw ELibMCCustomException(LIBMC_ERROR_NONLINEAROVERRIDEPARAMETERNOTINCREASING, std::to_string (interpolationData.m_Parameter) + " in " + m_sDebugName);

										if (interpolationData.m_Parameter > dMaxParameter)
											throw ELibMCCustomException(LIBMC_ERROR_NONLINEAROVERRIDEPARAMETEROUTOFRANGE, std::to_string(interpolationData.m_Parameter) + " in " + m_sDebugName);

										if (interpolationData.m_Factor < 0.0 || interpolationData.m_Factor > 1.0)
											throw ELibMCCustomException(LIBMC_ERROR_NONLINEAROVERRIDEFACTOROUTOFRANGE, std::to_string(interpolationData.m_Factor) + " in " + m_sDebugName);

										m_InterpolationData.push_back(interpolationData);
									}
								}

								if (hatchFactors.size() > 0) {

									auto pSrcOverride = &hatchFactors[0];
									auto pDstOverride = &m_OverrideFactors.at(segment.m_PointStartIndex);

									uint32_t nTotalSubInterpolationCount = 0;

									for (uint32_t nHatchIndex = 0; nHatchIndex < hatchFactors.size(); nHatchIndex++) {
										uint32_t nSubInterpolationCount = 0;
										uint32_t nSubInterpolationOffset = 0;
										//Lib3MF::sHatchModificationInterpolationData* pSubInterpolationData = nullptr;
										if (nonLinearCounts.size() > 0) {
											nSubInterpolationCount = nonLinearCounts.at(nHatchIndex);
											if (nSubInterpolationCount > 0)
												nSubInterpolationOffset = (uint32_t)(nInterpolationDataStartIndex + nTotalSubInterpolationCount);
										}

										pDstOverride->m_dFactors[nFactorIndex] = pSrcOverride->m_Point1Factor;
										pDstOverride->m_nSubInterpolationCount = nSubInterpolationCount;
										pDstOverride->m_nSubInterpolationOffset = nSubInterpolationOffset;
										pDstOverride++;
										pDstOverride->m_dFactors[nFactorIndex] = pSrcOverride->m_Point2Factor;
										pDstOverride->m_nSubInterpolationCount = nSubInterpolationCount;
										pDstOverride->m_nSubInterpolationOffset = nSubInterpolationOffset;
										pDstOverride++;
										pSrcOverride++;

										nTotalSubInterpolationCount += nSubInterpolationCount;

									}

									if (nTotalSubInterpolationCount != nonLinearValues.size())
										throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODIFIERINTERPOLATIONCOUNT, m_sDebugName);

									segment.m_TotalSubinterpolationCount = nTotalSubInterpolationCount;
								}
								
							}
						}


					}

					break;
				}

				}

			}



				/*for (uint32_t nFactorIndex = 0; nFactorIndex < 3; nFactorIndex++) {

					Lib3MF::eToolpathProfileModificationFactor factorType = Lib3MF::eToolpathProfileModificationFactor::Unknown;
					uint32_t factorFlag = 0;
					switch (nFactorIndex) {
						case 0: factorType = Lib3MF::eToolpathProfileModificationFactor::FactorF;
							factorFlag = TOOLPATHSEGMENTOVERRIDEFACTOR_F;
							break;
						case 1: factorType = Lib3MF::eToolpathProfileModificationFactor::FactorG;
							factorFlag = TOOLPATHSEGMENTOVERRIDEFACTOR_G;
							break;
						case 2: factorType = Lib3MF::eToolpathProfileModificationFactor::FactorH;
							factorFlag = TOOLPATHSEGMENTOVERRIDEFACTOR_H;
							break;
					}

					if (p3MFLayer->SegmentHasModificationFactors(nSegmentIndex, factorType)) {

						if (pSegment->m_Type == LibMCEnv::eToolpathSegmentType::Hatch) {
							pSegment->m_HasOverrideFactors |= factorFlag;

							std::vector<uint32_t> nonLinearCounts;
							std::vector<Lib3MF::sHatchModificationInterpolationData> nonLinearValues;

							std::vector<Lib3MF::sHatch2DFactors> hatchFactors;
							p3MFLayer->GetLinearSegmentHatchModificationFactors(nSegmentIndex, factorType, hatchFactors);
							p3MFLayer->GetSegmentAllNonlinearHatchesModificationInterpolation(nSegmentIndex, factorType, nonLinearCounts, nonLinearValues);

							if ((uint32_t)(hatchFactors.size() * 2) != pSegment->m_PointCount)
								throw ELibMCCustomException(LIBMC_ERROR_INVALIDHATCHOVERRIDECOUNT, m_sDebugName);

							if (hatchFactors.size() > 0) {

								auto pSrcOverride = &hatchFactors[0];
								auto pDstOverride = &m_OverrideFactors.at(pSegment->m_PointStartIndex);

								uint32_t nTotalSubInterpolationCount = 0;

								for (uint32_t nHatchIndex = 0; nHatchIndex < hatchFactors.size(); nHatchIndex++) {
									uint32_t nSubInterpolationCount = 0;
									if (nonLinearCounts.size () > 0)
										nSubInterpolationCount = nonLinearCounts.at(nHatchIndex);

									pDstOverride->m_dFactors[nFactorIndex] = pSrcOverride->m_Point1Factor;
									pDstOverride->m_nSubInterpolationCount = nSubInterpolationCount;
									pDstOverride++;
									pDstOverride->m_dFactors[nFactorIndex] = pSrcOverride->m_Point2Factor;
									pDstOverride->m_nSubInterpolationCount = nSubInterpolationCount;
									pDstOverride++;
									pSrcOverride++;

									nTotalSubInterpolationCount += nSubInterpolationCount;

								}

								if (nTotalSubInterpolationCount != nonLinearValues.size ())
									throw ELibMCCustomException(LIBMC_ERROR_INVALIDMODIFIERINTERPOLATIONCOUNT, m_sDebugName);

							}
						}

						if (pSegment->m_Type == LibMCEnv::eToolpathSegmentType::Polyline) {
							pSegment->m_HasOverrideFactors |= factorFlag;

							std::vector<double> pointFactors;
							p3MFLayer->GetSegmentPointModificationFactors(nSegmentIndex, factorType, pointFactors);

							if ((uint32_t)pointFactors.size() != pSegment->m_PointCount)
								throw ELibMCCustomException(LIBMC_ERROR_INVALIDPOINTOVERRIDECOUNT, m_sDebugName);

							if (pointFactors.size() > 0) {

								auto pSrcOverride = &pointFactors[0];
								auto pDstOverride = &m_OverrideFactors.at(pSegment->m_PointStartIndex);

								for (uint32_t nPointIndex = 0; nPointIndex < pSegment->m_PointCount; nPointIndex++) {
									uint32_t nSubInterpolationCount = 0;

									pDstOverride->m_dFactors[nFactorIndex] = *pSrcOverride;
									pDstOverride->m_nSubInterpolationCount = nSubInterpolationCount;
									pSrcOverride++;
									pDstOverride++;

								}
							}

						}

					} 

				}  

			}

		} */

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

	uint32_t CToolpathLayerData::getSegmentTotalSubinterpolationCount(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return m_Segments[nSegmentIndex].m_TotalSubinterpolationCount;

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

		return getRegisteredUUID(m_Segments[nSegmentIndex].m_ProfileUUID);
	}

	std::string CToolpathLayerData::getSegmentPartUUID(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return getRegisteredUUID(m_Segments[nSegmentIndex].m_PartUUID);
	}

	uint32_t CToolpathLayerData::getSegmentLocalPartID(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return m_Segments[nSegmentIndex].m_LocalPartID;

	}

	uint32_t CToolpathLayerData::getSegmentLaserIndex(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		return m_Segments[nSegmentIndex].m_LaserIndex;

	}



	PToolpathLayerProfile CToolpathLayerData::getSegmentProfile(const uint32_t nSegmentIndex)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto sProfileUUID = getRegisteredUUID(m_Segments[nSegmentIndex].m_ProfileUUID);
		return retrieveProfileData(sProfileUUID );
	}

	bool CToolpathLayerData::findCustomSegmentAttribute(const std::string& sNameSpace, const std::string& sAttributeName, uint32_t& nAttributeID, LibMCEnv::eToolpathAttributeType& attributeType)
	{
		auto iIter = m_CustomSegmentAttributeMap.find(std::make_pair(sNameSpace, sAttributeName));
		if (iIter != m_CustomSegmentAttributeMap.end()) {
			nAttributeID = iIter->second->getInternalAttributeID ();
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

		auto eAttributeType = m_CustomSegmentAttributes.at(nAttributeID - 1)->getAttributeType();
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

		auto eAttributeType = m_CustomSegmentAttributes.at(nAttributeID - 1)->getAttributeType();
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

			auto p3MFProfile = pToolpath->GetProfileByUUID(sProfileUUID);
			std::string sProfileName = p3MFProfile->GetName();			
			PToolpathLayerProfile pLayerProfile = std::make_shared<CToolpathLayerProfile> ((uint32_t)m_ProfileMap.size (), sProfileUUID, sProfileName);

			uint32_t nParameterCount = p3MFProfile->GetParameterCount();
			for (uint32_t nParameterIndex = 0; nParameterIndex < nParameterCount; nParameterIndex++) {
				std::string sParameterName = p3MFProfile->GetParameterName(nParameterIndex);
				std::string sParameterNamespace = p3MFProfile->GetParameterNameSpace(nParameterIndex);
				std::string sParameterValue = p3MFProfile->GetParameterValue(sParameterNamespace, sParameterName);

				pLayerProfile->addValue(sParameterNamespace, sParameterName, sParameterValue);
			}

			uint32_t nModifierCount = p3MFProfile->GetModifierCount();
			for (uint32_t nModifierIndex = 0; nModifierIndex < nModifierCount; nModifierIndex++) {
				double dMinValue = 0.0;
				double dMaxValue = 0.0;
				std::string sNameSpace;
				std::string sValue;
				Lib3MF::eToolpathProfileModificationType modificationType3MF = Lib3MF::eToolpathProfileModificationType::NoModification;
				Lib3MF::eToolpathProfileModificationFactor modificationFactor3MF = Lib3MF::eToolpathProfileModificationFactor::Unknown;

				p3MFProfile->GetModifierInformationByIndex(nModifierIndex, sNameSpace, sValue, modificationType3MF, modificationFactor3MF, dMinValue, dMaxValue);

				LibMCEnv::eToolpathProfileModificationType modificationType = LibMCEnv::eToolpathProfileModificationType::NoModification;
				LibMCEnv::eToolpathProfileModificationFactor modificationFactor = LibMCEnv::eToolpathProfileModificationFactor::Unknown;
				switch (modificationType3MF) {
					case Lib3MF::eToolpathProfileModificationType::NoModification:
						modificationType = LibMCEnv::eToolpathProfileModificationType::NoModification;
						break;
					case Lib3MF::eToolpathProfileModificationType::ConstantModification:
						modificationType = LibMCEnv::eToolpathProfileModificationType::ConstantModification;
						break;
					case Lib3MF::eToolpathProfileModificationType::LinearModification:
						modificationType = LibMCEnv::eToolpathProfileModificationType::LinearModification;
						break;
					case Lib3MF::eToolpathProfileModificationType::NonlinearModification:
						modificationType = LibMCEnv::eToolpathProfileModificationType::NonlinearModification;
						break;
					default:
						throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHMODIFIERTYPE, m_sDebugName);

				}

				switch (modificationFactor3MF) {
				case Lib3MF::eToolpathProfileModificationFactor::FactorF:
					modificationFactor = LibMCEnv::eToolpathProfileModificationFactor::FactorF;
					break;
				case Lib3MF::eToolpathProfileModificationFactor::FactorG:
					modificationFactor = LibMCEnv::eToolpathProfileModificationFactor::FactorG;
					break;
				case Lib3MF::eToolpathProfileModificationFactor::FactorH:
					modificationFactor = LibMCEnv::eToolpathProfileModificationFactor::FactorH;
					break;
				default:
					throw ELibMCCustomException(LIBMC_ERROR_INVALIDTOOLPATHMODIFIERFACTOR, m_sDebugName);

				}


				pLayerProfile->addModifier(sNameSpace, sValue, modificationType, modificationFactor, dMinValue, dMaxValue);
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

	bool CToolpathLayerData::segmentHasOverrideFactors(uint32_t nSegmentIndex, LibMCEnv::eToolpathProfileModificationFactor eOverrideFactor)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		switch (eOverrideFactor) {
		case LibMCEnv::eToolpathProfileModificationFactor::FactorF:
			return (pSegment->m_HasOverrideFactors | TOOLPATHSEGMENTOVERRIDEFACTOR_F) != 0;
		case LibMCEnv::eToolpathProfileModificationFactor::FactorG:
			return (pSegment->m_HasOverrideFactors | TOOLPATHSEGMENTOVERRIDEFACTOR_G) != 0;
		case LibMCEnv::eToolpathProfileModificationFactor::FactorH:
			return (pSegment->m_HasOverrideFactors | TOOLPATHSEGMENTOVERRIDEFACTOR_H) != 0;

		default:
			return false;
		}
	}

	void CToolpathLayerData::getHatchSubinterpolationData(const uint32_t nSegmentIndex, const uint32_t nHatchIndex, uint32_t& nSubInterpolationCount, Lib3MF::sHatchModificationInterpolationData*& pSubInterpolationData)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		uint32_t nPointCount = pSegment->m_PointCount;
		uint32_t nStartIndex = pSegment->m_PointStartIndex;
		if (pSegment->m_PointCount > 0) {
			uint32_t nPointIndex = nStartIndex + nHatchIndex * 2;
			uint32_t nSubInterpolationOffset = m_OverrideFactors.at(nPointIndex).m_nSubInterpolationOffset;
			pSubInterpolationData = &m_InterpolationData.at (nSubInterpolationOffset);
			nSubInterpolationCount = m_OverrideFactors.at(nPointIndex).m_nSubInterpolationCount;
		}
		else {
			pSubInterpolationData = nullptr;
			nSubInterpolationCount = 0;
		}

	}


	void CToolpathLayerData::getHatchModificationFactors(uint32_t nSegmentIndex, uint32_t nHatchIndex, LibMCEnv::eToolpathProfileModificationFactor eModificationFactor, double& dFactor1, double& dFactor2)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		uint32_t nPointCount = pSegment->m_PointCount;
		uint32_t nStartIndex = pSegment->m_PointStartIndex;
		if (pSegment->m_PointCount > 0) {
			uint32_t nPointIndex = nStartIndex + nHatchIndex * 2;

			switch (eModificationFactor) {
			case LibMCEnv::eToolpathProfileModificationFactor::FactorF: {
				dFactor1 = m_OverrideFactors.at(nPointIndex).m_dFactors[0];
				dFactor2 = m_OverrideFactors.at(nPointIndex + 1).m_dFactors[0];
				break;
			}
			case LibMCEnv::eToolpathProfileModificationFactor::FactorG: {
				dFactor1 = m_OverrideFactors.at(nPointIndex).m_dFactors[1];
				dFactor2 = m_OverrideFactors.at(nPointIndex + 1).m_dFactors[1];
				break;
			}
			case LibMCEnv::eToolpathProfileModificationFactor::FactorH: {
				dFactor1 = m_OverrideFactors.at(nPointIndex).m_dFactors[2];
				dFactor2 = m_OverrideFactors.at(nPointIndex + 1).m_dFactors[2];
				break;
			}
			default:
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDOVERRIDEFACTORINDEX, std::to_string((int)eModificationFactor));

			}

		}

	}


/*	void CToolpathLayerData::storePointOverrides(uint32_t nSegmentIndex, LibMCEnv::eToolpathProfileModificationFactor eOverrideFactor, double* pOverrideData)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		uint32_t nPointCount = pSegment->m_PointCount;
		uint32_t nStartIndex = pSegment->m_PointStartIndex;
		if (pSegment->m_PointCount > 0) {

			uint32_t nFactorIndex = 0;
			switch (eOverrideFactor) {
				case LibMCEnv::eToolpathProfileModificationFactor::FactorF: {
					nFactorIndex = 0;
					break;
				}
				case LibMCEnv::eToolpathProfileModificationFactor::FactorG: {
					nFactorIndex = 1;
					break;
				}
				case LibMCEnv::eToolpathProfileModificationFactor::FactorH: {
					nFactorIndex = 2;
					break;
				}
				default:
					throw ELibMCCustomException(LIBMC_ERROR_INVALIDOVERRIDEFACTORINDEX, std::to_string ((int)eOverrideFactor));

			}


			for (uint32_t nPointIndex = 0; nPointIndex < nPointCount; nPointIndex++) {
				auto& overrideFactor = m_OverrideFactors.at(nStartIndex + nPointIndex);
				pOverrideData[nPointIndex] = overrideFactor.m_dFactors[nFactorIndex];
			}

		}
	}


	void CToolpathLayerData::storeHatchOverrides(uint32_t nSegmentIndex, LibMCEnv::eToolpathProfileModificationFactor eOverrideFactor, LibMCEnv::sHatch2DModificationFactors* pOverrideData)
	{
		if (nSegmentIndex >= m_Segments.size())
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDSEGMENTINDEX, m_sDebugName);

		auto pSegment = &m_Segments[nSegmentIndex];
		uint32_t nPointCount = pSegment->m_PointCount;
		uint32_t nHatchCount = nPointCount / 2;
		uint32_t nStartIndex = pSegment->m_PointStartIndex;
		if (pSegment->m_PointCount > 0) {

			uint32_t nFactorIndex = 0;
			switch (eOverrideFactor) {
			case LibMCEnv::eToolpathProfileModificationFactor::FactorF: {
				nFactorIndex = 0;
				break;
			}
			case LibMCEnv::eToolpathProfileModificationFactor::FactorG: {
				nFactorIndex = 1;
				break;
			}
			case LibMCEnv::eToolpathProfileModificationFactor::FactorH: {
				nFactorIndex = 2;
				break;
			}
			default:
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDOVERRIDEFACTORINDEX, std::to_string((int)eOverrideFactor));

			}

			auto pTarget = &pOverrideData[0];
			for (uint32_t nHatchIndex = 0; nHatchIndex < nHatchCount; nHatchIndex++) {
				auto& overrideFactor1 = m_OverrideFactors.at(nStartIndex + nHatchIndex * 2);
				auto& overrideFactor2 = m_OverrideFactors.at(nStartIndex + nHatchIndex * 2 + 1);
				pTarget->m_Point1Factor = overrideFactor1.m_dFactors[nFactorIndex];
				pTarget->m_Point2Factor = overrideFactor2.m_dFactors[nFactorIndex];
				pTarget->m_NumberOfNonLinearInterpolationBases = overrideFactor1.m_nSubInterpolationCount;
				pTarget++;
			}

		}

	} */


	void CToolpathLayerData::calculateExtents(int32_t & nMinX, int32_t & nMinY, int32_t & nMaxX, int32_t & nMaxY)
	{
		nMinX = 0;
		nMinY = 0;
		nMaxX = 0;
		nMaxY = 0;
		bool bIsFirst = true;

		for (auto& segment : m_Segments) {
			switch (segment.m_Type) {
			case LibMCEnv::eToolpathSegmentType::Polyline:
			case LibMCEnv::eToolpathSegmentType::Hatch:

				if (segment.m_PointCount > 0) {
					auto pSrcPoint = &m_Points.at(segment.m_PointStartIndex);
					for (uint32_t nPointIndex = 0; nPointIndex < segment.m_PointCount; nPointIndex++) {
						int32_t nX = pSrcPoint->m_Coordinates[0];
						int32_t nY = pSrcPoint->m_Coordinates[1];

						if (bIsFirst) {
							nMinX = nX;
							nMinY = nY;
							nMaxX = nX;
							nMaxY = nY;

							bIsFirst = false;
						}
						else {
							if (nMinX > nX)
								nMinX = nX;
							if (nMaxX < nX)
								nMaxX = nX;
							if (nMinY > nY)
								nMinY = nY;
							if (nMaxY < nY)
								nMaxY = nY;

						}

						pSrcPoint++;
					}
				}

				break;
			}
		}
	}


	std::string CToolpathLayerData::getValueNameByType(const LibMCEnv::eToolpathProfileValueType eValueType)
	{
		switch (eValueType) {
		case LibMCEnv::eToolpathProfileValueType::Speed:
			return "laserspeed";
		case LibMCEnv::eToolpathProfileValueType::LaserPower:
			return "laserpower";
		case LibMCEnv::eToolpathProfileValueType::LaserFocus:
			return "laserfocus";
		case LibMCEnv::eToolpathProfileValueType::JumpSpeed:
			return "jumpspeed";
		case LibMCEnv::eToolpathProfileValueType::ExtrusionFactor:
			return "extrusionfactor";
		case LibMCEnv::eToolpathProfileValueType::StartDelay:
			return "startdelay";
		case LibMCEnv::eToolpathProfileValueType::EndDelay:
			return "enddelay";
		case LibMCEnv::eToolpathProfileValueType::PolyDelay:
			return "polydelay";
		case LibMCEnv::eToolpathProfileValueType::JumpDelay:
			return "jumpdelay";
		case LibMCEnv::eToolpathProfileValueType::LaserOnDelay:
			return "laserondelay";
		case LibMCEnv::eToolpathProfileValueType::LaserOffDelay:
			return "laseroffdelay";
		case LibMCEnv::eToolpathProfileValueType::PreSegmentDelay:
			return "predelay";
		case LibMCEnv::eToolpathProfileValueType::PostSegmentDelay:
			return "postdelay";
		default:
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDPROFILEVALUETYPE, std::to_string ((int)eValueType));
		}

	}


}


