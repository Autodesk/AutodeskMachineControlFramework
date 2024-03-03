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

#include "libmcenv_toolpathlayer.hpp"
#include "libmcenv_interfaceexception.hpp"
#include "libmcenv_xmldocument.hpp"
#include "libmcenv_xmldocumentnode.hpp"

// Include custom headers here.
#include "Common/common_utils.hpp"

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CToolpathLayer 
**************************************************************************************************************************/

CToolpathLayer::CToolpathLayer(AMC::PToolpathLayerData pToolpathLayerData)
	: m_pToolpathLayerData (pToolpathLayerData)
{
	if (pToolpathLayerData.get() == nullptr)
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPARAM);

}

std::string CToolpathLayer::GetLayerDataUUID()
{
	return m_pToolpathLayerData->getUUID ();
}

LibMCEnv_uint32 CToolpathLayer::GetSegmentCount()
{
	return m_pToolpathLayerData->getSegmentCount ();
}

void CToolpathLayer::GetSegmentInfo(const LibMCEnv_uint32 nIndex, LibMCEnv::eToolpathSegmentType & eType, LibMCEnv_uint32 & nPointCount)
{
	eType = m_pToolpathLayerData->getSegmentType (nIndex);
	nPointCount = m_pToolpathLayerData->getSegmentPointCount (nIndex);
}

LibMCEnv::eToolpathSegmentType CToolpathLayer::GetSegmentType(const LibMCEnv_uint32 nIndex)
{
	return m_pToolpathLayerData->getSegmentType(nIndex);
}

bool CToolpathLayer::SegmentIsLoop(const LibMCEnv_uint32 nIndex)
{
	return (m_pToolpathLayerData->getSegmentType(nIndex) == LibMCEnv::eToolpathSegmentType::Loop);
}

bool CToolpathLayer::SegmentIsPolyline(const LibMCEnv_uint32 nIndex)
{
	return (m_pToolpathLayerData->getSegmentType(nIndex) == LibMCEnv::eToolpathSegmentType::Polyline);
}

bool CToolpathLayer::SegmentIsHatchSegment(const LibMCEnv_uint32 nIndex)
{
	return (m_pToolpathLayerData->getSegmentType(nIndex) == LibMCEnv::eToolpathSegmentType::Hatch);
}


LibMCEnv_uint32 CToolpathLayer::GetSegmentPointCount(const LibMCEnv_uint32 nIndex)
{
	return m_pToolpathLayerData->getSegmentPointCount(nIndex);
}

LibMCEnv_int64 CToolpathLayer::GetSegmentIntegerAttribute(const LibMCEnv_uint32 nIndex, const LibMCEnv_uint32 nAttributeID)
{
	return m_pToolpathLayerData->getSegmentIntegerAttribute(nIndex, nAttributeID);
}

LibMCEnv_double CToolpathLayer::GetSegmentDoubleAttribute(const LibMCEnv_uint32 nIndex, const LibMCEnv_uint32 nAttributeID)
{
	return m_pToolpathLayerData->getSegmentDoubleAttribute(nIndex, nAttributeID);
}

bool CToolpathLayer::HasCustomSegmentAttribute(const std::string& sNamespace, const std::string& sAttributeName)
{
	uint32_t nAttributeID = 0;
	LibMCEnv::eToolpathAttributeType attributeType = LibMCEnv::eToolpathAttributeType::Unknown;

	return m_pToolpathLayerData->findCustomSegmentAttribute(sNamespace, sAttributeName, nAttributeID, attributeType);
}

LibMCEnv_uint32 CToolpathLayer::FindCustomSegmentAttributeID(const std::string& sNamespace, const std::string& sAttributeName)
{
	uint32_t nAttributeID = 0;
	LibMCEnv::eToolpathAttributeType attributeType = LibMCEnv::eToolpathAttributeType::Unknown;

	if (!m_pToolpathLayerData->findCustomSegmentAttribute(sNamespace, sAttributeName, nAttributeID, attributeType))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SEGMENTATTRIBUTENOTFOUND, "segment attribute not found: " + sNamespace + "/" + sAttributeName);

	return nAttributeID;
}

LibMCEnv::eToolpathAttributeType CToolpathLayer::FindCustomSegmentAttributeType(const std::string& sNamespace, const std::string& sAttributeName)
{
	uint32_t nAttributeID = 0;
	LibMCEnv::eToolpathAttributeType attributeType = LibMCEnv::eToolpathAttributeType::Unknown;

	if (!m_pToolpathLayerData->findCustomSegmentAttribute(sNamespace, sAttributeName, nAttributeID, attributeType))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SEGMENTATTRIBUTENOTFOUND, "segment attribute not found: " + sNamespace + "/" + sAttributeName);

	return attributeType;

}

void CToolpathLayer::FindCustomSegmentAttributeInfo(const std::string& sNamespace, const std::string& sAttributeName, LibMCEnv_uint32& nAttributeID, LibMCEnv::eToolpathAttributeType& eAttributeType)
{
	nAttributeID = 0;
	eAttributeType = LibMCEnv::eToolpathAttributeType::Unknown;

	if (!m_pToolpathLayerData->findCustomSegmentAttribute(sNamespace, sAttributeName, nAttributeID, eAttributeType))
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SEGMENTATTRIBUTENOTFOUND, "segment attribute not found: " + sNamespace + "/" + sAttributeName);

}

LibMCEnv_uint32 CToolpathLayer::GetSegmentHatchCount(const LibMCEnv_uint32 nIndex)
{
	auto segmentType = m_pToolpathLayerData->getSegmentType(nIndex);
	if (segmentType == LibMCEnv::eToolpathSegmentType::Hatch) {
		uint32_t nPointCount = m_pToolpathLayerData->getSegmentPointCount(nIndex);
		if (nPointCount % 2 != 0)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDHATCHCOUNT);

		return nPointCount / 2;
	}
	else {
		return 0;
	}
}


std::string CToolpathLayer::GetSegmentProfileUUID(const LibMCEnv_uint32 nIndex)
{
	return m_pToolpathLayerData->getSegmentProfileUUID(nIndex);
}

std::string CToolpathLayer::GetSegmentPartUUID(const LibMCEnv_uint32 nIndex)
{
	return m_pToolpathLayerData->getSegmentPartUUID(nIndex);
}

bool CToolpathLayer::SegmentProfileHasValue(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	return pProfile->hasValue (sNamespace, sValueName);

}

std::string CToolpathLayer::GetSegmentProfileValue(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	return pProfile->getValue(sNamespace, sValueName);

}

std::string CToolpathLayer::GetSegmentProfileValueDef(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName, const std::string& sDefaultValue) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	return pProfile->getValueDef(sNamespace, sValueName, sDefaultValue);

}

LibMCEnv_double CToolpathLayer::GetSegmentProfileDoubleValue(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	std::string sValue = pProfile->getValue(sNamespace, sValueName);
	return std::stod(sValue);

}

LibMCEnv_double CToolpathLayer::GetSegmentProfileDoubleValueDef(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName, const LibMCEnv_double dDefaultValue) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	std::string sValue = pProfile->getValueDef(sNamespace, sValueName, std::to_string (dDefaultValue));
	return std::stod(sValue);

}

LibMCEnv_int64 CToolpathLayer::GetSegmentProfileIntegerValue(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	std::string sValue = pProfile->getValue(sNamespace, sValueName);
	return std::stoll(sValue);

}

LibMCEnv_int64 CToolpathLayer::GetSegmentProfileIntegerValueDef(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName, const LibMCEnv_int64 nDefaultValue)
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	std::string sValue = pProfile->getValueDef(sNamespace, sValueName, std::to_string(nDefaultValue));
	return std::stoll(sValue);

}

bool CToolpathLayer::GetSegmentProfileBoolValue(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName) 
{
	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	std::string sValue = pProfile->getValue(sNamespace, sValueName);
	std::string sTrimmedValue = AMCCommon::CUtils::trimString(AMCCommon::CUtils::toLowerString(sValue));
	if (sTrimmedValue == "true")
		return true;
	if (sTrimmedValue == "false")
		return false;
	return (std::stoll(sValue) != 0);
}

bool CToolpathLayer::GetSegmentProfileBoolValueDef(const LibMCEnv_uint32 nIndex, const std::string& sNamespace, const std::string& sValueName, const bool bDefaultValue) 
{
	std::string sDefaultValue;
	if (bDefaultValue)
		sDefaultValue = "true";
	else
		sDefaultValue = "false";

	auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
	std::string sValue = pProfile->getValueDef(sNamespace, sValueName, sDefaultValue);
	std::string sTrimmedValue = AMCCommon::CUtils::trimString(AMCCommon::CUtils::toLowerString(sValue));
	if (sTrimmedValue == "true")
		return true;
	if (sTrimmedValue == "false")
		return false;
	return (std::stoll(sValue) != 0);

}

std::string CToolpathLayer::getValueNameByType(const LibMCEnv::eToolpathProfileValueType eValueType)
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
	default:
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDPROFILEVALUETYPE);
	}

}

LibMCEnv_double CToolpathLayer::GetSegmentProfileTypedValue(const LibMCEnv_uint32 nIndex, const LibMCEnv::eToolpathProfileValueType eValueType)
{	
	// Legacy behaviour: Fall back to Laser Speed if no jump speed is available.
	if (eValueType == LibMCEnv::eToolpathProfileValueType::JumpSpeed) {
		auto pProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);
		if (!pProfile->hasValue("", "jumpspeed"))
			return GetSegmentProfileTypedValue (nIndex, LibMCEnv::eToolpathProfileValueType::Speed);
	}

	std::string sValueName = getValueNameByType(eValueType);
	return GetSegmentProfileDoubleValue(nIndex, "", sValueName);

}

LibMCEnv_double CToolpathLayer::GetSegmentProfileTypedValueDef(const LibMCEnv_uint32 nIndex, const LibMCEnv::eToolpathProfileValueType eValueType, const LibMCEnv_double dDefaultValue)
{
	std::string sValueName = getValueNameByType(eValueType);
	return GetSegmentProfileDoubleValueDef(nIndex, "", sValueName, dDefaultValue);
}


void CToolpathLayer::GetSegmentPointData(const LibMCEnv_uint32 nIndex, LibMCEnv_uint64 nPointDataBufferSize, LibMCEnv_uint64* pPointDataNeededCount, LibMCEnv::sPosition2D * pPointDataBuffer)
{

	uint64_t nNeededPointCount = m_pToolpathLayerData->getSegmentPointCount(nIndex);
	if (pPointDataNeededCount != nullptr)
		*pPointDataNeededCount = nNeededPointCount;

	if (pPointDataBuffer != nullptr) {
		if (nPointDataBufferSize < nNeededPointCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		m_pToolpathLayerData->storePointsToBufferInUnits (nIndex, pPointDataBuffer);
	}
}

void CToolpathLayer::GetSegmentHatchData(const LibMCEnv_uint32 nIndex, LibMCEnv_uint64 nHatchDataBufferSize, LibMCEnv_uint64* pHatchDataNeededCount, LibMCEnv::sHatch2D* pHatchDataBuffer)
{
	auto segmentType = m_pToolpathLayerData->getSegmentType(nIndex);
	if (segmentType == LibMCEnv::eToolpathSegmentType::Hatch) {
		uint64_t nNeededPointCount = m_pToolpathLayerData->getSegmentPointCount(nIndex);
		if (nNeededPointCount % 2 != 0)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDHATCHCOUNT);
		uint64_t nNeededHatchCount = nNeededPointCount / 2;

		if (pHatchDataNeededCount != nullptr)
			*pHatchDataNeededCount = nNeededHatchCount;

		if (pHatchDataBuffer != nullptr) {
			if (nHatchDataBufferSize < nNeededHatchCount)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

			m_pToolpathLayerData->storeHatchesToBufferInUnits(nIndex, pHatchDataBuffer);
		}

	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SEGMENTISNOTOFTYPEHATCH);
	}

}

void CToolpathLayer::GetSegmentPointDataInMM(const LibMCEnv_uint32 nIndex, LibMCEnv_uint64 nPointDataBufferSize, LibMCEnv_uint64* pPointDataNeededCount, LibMCEnv::sFloatPosition2D* pPointDataBuffer)
{

	uint64_t nNeededPointCount = m_pToolpathLayerData->getSegmentPointCount(nIndex);
	if (pPointDataNeededCount != nullptr)
		*pPointDataNeededCount = nNeededPointCount;

	if (pPointDataBuffer != nullptr) {
		if (nPointDataBufferSize < nNeededPointCount)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

		m_pToolpathLayerData->storePointsToBufferInMM(nIndex, pPointDataBuffer);
	}

}

void CToolpathLayer::GetSegmentHatchDataInMM(const LibMCEnv_uint32 nIndex, LibMCEnv_uint64 nHatchDataBufferSize, LibMCEnv_uint64* pHatchDataNeededCount, LibMCEnv::sFloatHatch2D* pHatchDataBuffer)
{
	auto segmentType = m_pToolpathLayerData->getSegmentType(nIndex);
	if (segmentType == LibMCEnv::eToolpathSegmentType::Hatch) {
		uint64_t nNeededPointCount = m_pToolpathLayerData->getSegmentPointCount(nIndex);
		if (nNeededPointCount % 2 != 0)
			throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_INVALIDHATCHCOUNT);
		uint64_t nNeededHatchCount = nNeededPointCount / 2;

		if (pHatchDataNeededCount != nullptr)
			*pHatchDataNeededCount = nNeededHatchCount;

		if (pHatchDataBuffer != nullptr) {
			if (nHatchDataBufferSize < nNeededHatchCount)
				throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_BUFFERTOOSMALL);

			m_pToolpathLayerData->storeHatchesToBufferInMM(nIndex, pHatchDataBuffer);
		}

	}
	else {
		throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_SEGMENTISNOTOFTYPEHATCH);
	}

}


LibMCEnv_double CToolpathLayer::GetUnits()
{
	return m_pToolpathLayerData->getUnits();
}

LibMCEnv_int32 CToolpathLayer::GetZValue()
{
	return m_pToolpathLayerData->getZValue();
}

LibMCEnv_double CToolpathLayer::GetZValueInMM()
{
	return m_pToolpathLayerData->getUnits() * m_pToolpathLayerData->getZValue();
}

LibMCEnv_uint32 CToolpathLayer::GetMetaDataCount()
{
	return m_pToolpathLayerData->getMetaDataCount();
}

void CToolpathLayer::GetMetaDataInfo(const LibMCEnv_uint32 nMetaDataIndex, std::string& sNamespace, std::string& sName)
{
	m_pToolpathLayerData->getMetaDataInfo(nMetaDataIndex, sNamespace, sName);

}

IXMLDocumentNode* CToolpathLayer::GetMetaDataContent(const LibMCEnv_uint32 nMetaDataIndex)
{
	auto pXMLDocumentInstance = m_pToolpathLayerData->getMetaData(nMetaDataIndex);
	return new CXMLDocumentNode(pXMLDocumentInstance, pXMLDocumentInstance->GetRootNode());

}

bool CToolpathLayer::HasUniqueMetaData(const std::string& sNamespace, const std::string& sName)
{
	return m_pToolpathLayerData->hasUniqueMetaData(sNamespace, sName);

}

IXMLDocumentNode* CToolpathLayer::FindUniqueMetaData(const std::string& sNamespace, const std::string& sName)
{
	auto pXMLDocumentInstance = m_pToolpathLayerData->findUniqueMetaData(sNamespace, sName);
	return new CXMLDocumentNode(pXMLDocumentInstance, pXMLDocumentInstance->GetRootNode());

}

void CToolpathLayer::CalculateExtents(LibMCEnv_int32& nMinX, LibMCEnv_int32& nMinY, LibMCEnv_int32& nMaxX, LibMCEnv_int32& nMaxY)
{
	m_pToolpathLayerData->calculateExtents(nMinX, nMinY, nMaxX, nMaxY);
}

void CToolpathLayer::CalculateExtentsInMM(LibMCEnv_double& dMinX, LibMCEnv_double& dMinY, LibMCEnv_double& dMaxX, LibMCEnv_double& dMaxY)
{
	int32_t nMinX = 0;
	int32_t nMinY = 0;
	int32_t nMaxX = 0;
	int32_t nMaxY = 0;

	m_pToolpathLayerData->calculateExtents(nMinX, nMinY, nMaxX, nMaxY);
	double dUnits = m_pToolpathLayerData->getUnits();
	dMinX = nMinX * dUnits;
	dMinY = nMinY * dUnits;
	dMaxX = nMaxX * dUnits;
	dMaxY = nMaxY * dUnits;

}
