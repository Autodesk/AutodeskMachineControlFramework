/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is a stub class definition of CLayerObject

*/

#include "librasterizer_layerobject.hpp"
#include "librasterizer_interfaceexception.hpp"

#include "clipper.hpp"

// Include custom headers here.


using namespace LibRasterizer;
using namespace LibRasterizer::Impl;


CLayerDataEntity::CLayerDataEntity(const LibRasterizer::eGeometryType GeometryType)
	: m_GeometryType (GeometryType)
{

}

LibRasterizer::eGeometryType CLayerDataEntity::getGeometryType()
{
	return m_GeometryType;
}

std::vector<LibRasterizer::sPosition2D>& CLayerDataEntity::getPoints()
{
	return m_Points;
}

CLayerDataObject::CLayerDataObject()
{

}

CLayerDataObject::CLayerDataObject(const ClipperLib::Paths& clipperPaths, const double dUnits)
{
	for (auto& path : clipperPaths) {
		auto & entity = addEntity(eGeometryType::SolidGeometry);
		auto & points = entity.getPoints();
		for (auto& clipperPoint : path) {
			points.push_back(sPosition2D { clipperPoint.X * dUnits, clipperPoint.Y * dUnits });
		}
	}

}

CLayerDataObject::~CLayerDataObject()
{

}

CLayerDataEntity& CLayerDataObject::addEntity(eGeometryType GeometryType)
{
	m_Entities.push_back(CLayerDataEntity(GeometryType));
	return *m_Entities.rbegin();
}

size_t CLayerDataObject::getEntityCount()
{
	return m_Entities.size();
}

CLayerDataEntity& CLayerDataObject::getEntity(const size_t& nIndex)
{
	return m_Entities[nIndex];
}

PClipperPaths CLayerDataObject::createPathsFromGeometry(double& dUnits)
{
	if (dUnits <= 0.0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	auto pResult = std::make_shared<ClipperLib::Paths>();

	for (auto entity : m_Entities) {
		if (entity.getGeometryType() == eGeometryType::SolidGeometry) {
			auto& points = entity.getPoints();
			ClipperLib::Path path;
			for (auto point : points) {
				int32_t intX = (int32_t)(point.m_Coordinates[0] / dUnits);
				int32_t intY = (int32_t)(point.m_Coordinates[1] / dUnits);

				path.push_back(ClipperLib::IntPoint(intX, intY));
			}

			pResult->push_back(path);

		}
	}

	return pResult;
}

void CLayerDataObject::mergeInto(CLayerDataObject* pOtherDataObject)
{
	if (pOtherDataObject == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	for (auto & entity : m_Entities) {
		auto & newEntity = pOtherDataObject->addEntity(entity.getGeometryType());
		auto& points = entity.getPoints();
		auto& newPoints = newEntity.getPoints();

		newPoints.reserve(points.size());
		for (auto point : points) {
			newPoints.push_back (point);
		}


	}
}

/*************************************************************************************************************************
 Class definition of CLayerObject 
**************************************************************************************************************************/
CLayerObject::CLayerObject(PLayerDataObject pLayerDataObject)
	: m_pLayerDataObject(pLayerDataObject), m_dCalculationUnits(0.001)
{
	if (pLayerDataObject.get() == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
}

CLayerObject::~CLayerObject()
{

}


LibRasterizer_uint32 CLayerObject::GetEntityCount()
{
	return (uint32_t) m_pLayerDataObject->getEntityCount();
}

void CLayerObject::GetEntity(const LibRasterizer_uint32 nEntityIndex, LibRasterizer::eGeometryType & eGeometryType, LibRasterizer_uint64 nPointsBufferSize, LibRasterizer_uint64* pPointsNeededCount, LibRasterizer::sPosition2D * pPointsBuffer)
{
	auto & entity = m_pLayerDataObject->getEntity(nEntityIndex);
	eGeometryType = entity.getGeometryType();
	auto & points = entity.getPoints();

	if (pPointsNeededCount != nullptr)
		*pPointsNeededCount = points.size();

	if (pPointsBuffer != nullptr) {
		if (nPointsBufferSize < points.size())
			throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_BUFFERTOOSMALL);

		for (size_t nIndex = 0; nIndex < nPointsBufferSize; nIndex++) {
			pPointsBuffer[nIndex] = points[nIndex];
		}
	}
}

LibRasterizer_uint32 CLayerObject::AddEntity(const LibRasterizer_uint64 nPointsBufferSize, const LibRasterizer::sPosition2D * pPointsBuffer, const LibRasterizer::eGeometryType eGeometryType)
{
	if (nPointsBufferSize <= 0)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);
	if (pPointsBuffer == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	auto & entity = m_pLayerDataObject->addEntity(eGeometryType);
	auto & points = entity.getPoints();

	points.resize(nPointsBufferSize);

	for (size_t nIndex = 0; nIndex < nPointsBufferSize; nIndex++) {
		points[nIndex] = pPointsBuffer[nIndex];
	}
}

ILayerObject * CLayerObject::RemoveSelfIntersections()
{
	auto pSourcePaths = m_pLayerDataObject->createPathsFromGeometry(m_dCalculationUnits);

	ClipperLib::Clipper clipper;
	clipper.AddPaths(*pSourcePaths, ClipperLib::PolyType::ptClip, true);
	
	ClipperLib::Paths solution;
	clipper.Execute(ClipperLib::ClipType::ctUnion, solution, ClipperLib::pftPositive);

	auto pTargetData = std::make_shared<CLayerDataObject> (solution, m_dCalculationUnits);
	return new CLayerObject(pTargetData);
}

void CLayerObject::MergeInto(ILayerObject* pOtherLayerObject)
{
	if (pOtherLayerObject == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDPARAM);

	CLayerObject* pOtherLayerObjectInstance = dynamic_cast <CLayerObject*> (pOtherLayerObject);
	if (pOtherLayerObjectInstance == nullptr)
		throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_INVALIDCAST);

	auto pOtherDataObject = pOtherLayerObjectInstance->getDataObject();
	m_pLayerDataObject->mergeInto(pOtherDataObject.get ());
}

ILayerObject * CLayerObject::CalculateOffset(const LibRasterizer_double dOffsetValue)
{
	auto pSourcePaths = m_pLayerDataObject->createPathsFromGeometry(m_dCalculationUnits);

	ClipperLib::ClipperOffset clipper;
	clipper.AddPaths(*pSourcePaths, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etClosedPolygon);

	ClipperLib::Paths solution;
	clipper.Execute(solution, - dOffsetValue / m_dCalculationUnits);

	auto pTargetData = std::make_shared<CLayerDataObject>(solution, m_dCalculationUnits);
	return new CLayerObject(pTargetData);
}

ILayerObject * CLayerObject::ThickenPolylines(const LibRasterizer_double dThickness)
{
	throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_NOTIMPLEMENTED);
}

ILayerObject * CLayerObject::ThickenHatches(const LibRasterizer_double dThickness)
{
	throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_NOTIMPLEMENTED);
}

ILayerObject* CLayerObject::DistortLayer(const LibRasterizer_double dRefinementValue, const LibRasterizer::DistortionCallback pDistortionCallback, const LibRasterizer_pvoid pUserData)
{
	throw ELibRasterizerInterfaceException(LIBRASTERIZER_ERROR_NOTIMPLEMENTED);
}

PLayerDataObject CLayerObject::getDataObject()
{
	return m_pLayerDataObject;
}