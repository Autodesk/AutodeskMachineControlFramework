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

// Include custom headers here.


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

LibMCEnv_uint32 CToolpathLayer::GetSegmentPointCount(const LibMCEnv_uint32 nIndex)
{
	return m_pToolpathLayerData->getSegmentPointCount(nIndex);
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

std::string CToolpathLayer::GetSegmentProfileValue(const LibMCEnv_uint32 nIndex, const std::string& sValueName)
{
	return "";
}

LibMCEnv_double CToolpathLayer::GetSegmentProfileTypedValue(const LibMCEnv_uint32 nIndex, const LibMCEnv::eToolpathProfileValueType eValueType)
{	
	auto sProfile = m_pToolpathLayerData->getSegmentProfile(nIndex);

	switch (eValueType) {
		case LibMCEnv::eToolpathProfileValueType::Speed:
			return sProfile.m_dLaserSpeed;
		case LibMCEnv::eToolpathProfileValueType::LaserPower:
			return sProfile.m_dLaserPower;
		case LibMCEnv::eToolpathProfileValueType::LaserFocus:
			return sProfile.m_dLaserFocus;
		case LibMCEnv::eToolpathProfileValueType::JumpSpeed:
			return sProfile.m_dJumpSpeed;
		case LibMCEnv::eToolpathProfileValueType::ExtrusionFactor:
			return sProfile.m_dExtrusionFactor;
		case LibMCEnv::eToolpathProfileValueType::StartDelay:
			return sProfile.m_dStartDelay;
		case LibMCEnv::eToolpathProfileValueType::EndDelay:
			return sProfile.m_dEndDelay;
		case LibMCEnv::eToolpathProfileValueType::PolyDelay:
			return sProfile.m_dPolyDelay;
		case LibMCEnv::eToolpathProfileValueType::JumpDelay:
			return sProfile.m_dJumpDelay;
		case LibMCEnv::eToolpathProfileValueType::LaserOnDelay:
			return sProfile.m_dLaserOnDelay;
		case LibMCEnv::eToolpathProfileValueType::LaserOffDelay:
			return sProfile.m_dLaserOffDelay;
		default:
			return 0.0;
	}

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
