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


Abstract: This is the class declaration of CLayerObject

*/


#ifndef __LIBRASTERIZER_LAYEROBJECT
#define __LIBRASTERIZER_LAYEROBJECT

#include "librasterizer_interfaces.hpp"

// Parent classes
#include "librasterizer_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <memory>
#include "clipper.hpp"

namespace LibRasterizer {
namespace Impl {

	typedef std::shared_ptr<ClipperLib::Paths> PClipperPaths;

/*************************************************************************************************************************
 Class declaration of CLayerObject 
**************************************************************************************************************************/

	class CLayerDataEntity {
	private:
		eGeometryType m_GeometryType;
		std::vector<sPosition2D> m_Points;
	public:
		CLayerDataEntity(const eGeometryType GeometryType);

		eGeometryType getGeometryType();
		std::vector<sPosition2D>& getPoints();
	};


	class CLayerDataObject {
	private:
		std::vector<CLayerDataEntity> m_Entities;

	public:
		CLayerDataObject();
		CLayerDataObject(const ClipperLib::Paths& clipperPaths, const double dUnits);

		virtual ~CLayerDataObject();

		CLayerDataEntity & addEntity(eGeometryType GeometryType);

		size_t getEntityCount();
		CLayerDataEntity& getEntity(const size_t& nIndex);

		PClipperPaths createPathsFromGeometry(double & dUnits);

		void mergeInto (CLayerDataObject * pOtherDataObject);
	};


	typedef std::shared_ptr<CLayerDataObject> PLayerDataObject;

class CLayerObject : public virtual ILayerObject, public virtual CBase {
private:
	PLayerDataObject m_pLayerDataObject;
	double m_dCalculationUnits;

protected:


public:

	CLayerObject(PLayerDataObject pLayerDataObject);

	virtual ~CLayerObject();

	LibRasterizer_uint32 GetEntityCount() override;

	void GetEntity(const LibRasterizer_uint32 nEntityIndex, LibRasterizer::eGeometryType & eGeometryType, LibRasterizer_uint64 nPointsBufferSize, LibRasterizer_uint64* pPointsNeededCount, LibRasterizer::sPosition2D * pPointsBuffer) override;

	LibRasterizer_uint32 AddEntity(const LibRasterizer_uint64 nPointsBufferSize, const LibRasterizer::sPosition2D * pPointsBuffer, const LibRasterizer::eGeometryType eGeometryType) override;

	ILayerObject * RemoveSelfIntersections() override;

	void MergeInto(ILayerObject* pOtherLayerObject) override;

	ILayerObject * CalculateOffset(const LibRasterizer_double dOffsetValue) override;

	ILayerObject * ThickenPolylines(const LibRasterizer_double dThickness) override;

	ILayerObject * ThickenHatches(const LibRasterizer_double dThickness) override;

	ILayerObject* DistortLayer(const LibRasterizer_double dRefinementValue, const LibRasterizer::DistortionCallback pDistortionCallback, const LibRasterizer_pvoid pUserData) override;

	PLayerDataObject getDataObject();

};

} // namespace Impl
} // namespace LibRasterizer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBRASTERIZER_LAYEROBJECT
