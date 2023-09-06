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


#ifndef __AMC_DISCRETEFIELDDATA2D
#define __AMC_DISCRETEFIELDDATA2D

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "libmcenv_types.hpp"

namespace AMC {

	class CDiscreteFieldData2DInstance;

	typedef std::shared_ptr<CDiscreteFieldData2DInstance> PDiscreteFieldData2DInstance;

	class CDiscreteFieldData2DInstance {
	private:
	
		size_t m_nPixelCountX;
		size_t m_nPixelCountY;
		
		double m_dDPIX;
		double m_dDPIY;
		
		double m_dOriginX;
		double m_dOriginY;
		
		std::unique_ptr<std::vector<double>> m_Data;

	public:

		static PDiscreteFieldData2DInstance createFromBuffer(const std::vector<uint8_t> & Buffer);

		CDiscreteFieldData2DInstance(size_t nPixelCountX, size_t nPixelCountY, double dDPIX, double dDPIY, double dOriginX, double dOriginY, double dDefaultValue, bool bDoClear);
		
		virtual ~CDiscreteFieldData2DInstance();

		void GetDPI(double& dDPIValueX, double& dDPIValueY);

		void SetDPI(const double dDPIValueX, const double dDPIValueY);

		void GetOriginInMM(double& dOriginX, double& dOriginY);

		void SetOriginInMM(const double dOriginX, const double dOriginY);

		void GetSizeInMM(double& dSizeX, double& dSizeY);

		void GetSizeInPixels(uint32_t& nPixelSizeX, uint32_t& nPixelSizeY) ;

		void ResizeField(uint32_t& nPixelSizeX, uint32_t& nPixelSizeY, double dDefaultValue);

		void Clear(const double dValue);

		void Clamp(const double dMinValue, const double dMaxValue);

		double GetPixel(const uint32_t nX, const uint32_t nY);

		void SetPixel(const uint32_t nX, const uint32_t nY, const double dValue);

		void GetPixelRange(const uint32_t nXMin, const uint32_t nYMin, const uint32_t nXMax, const uint32_t nYMax, uint64_t nValueBufferSize, uint64_t* pValueNeededCount, double* pValueBuffer);

		void SetPixelRange(const uint32_t nXMin, const uint32_t nYMin, const uint32_t nXMax, const uint32_t nYMax, const uint64_t nValueBufferSize, const double* pValueBuffer);

		//void RenderAveragePointValues(const double dDefaultValue, const LibMCEnv::eFieldSamplingMode eSamplingMode, const double dSampleSizeX, const double dSampleSizeY, const uint64_t nPointValuesBufferSize, const LibMCEnv::sFieldData2DPoint* pPointValuesBuffer);

		PDiscreteFieldData2DInstance ScaleFieldDown(const uint32_t nFactorX, const uint32_t nFactorY);

		PDiscreteFieldData2DInstance ScaleFieldUp(const uint32_t nFactorX, const uint32_t nFactorY);

		void DiscretizeWithMapping(const uint64_t nDiscreteValuesBufferSize, const double* pDiscreteValuesBuffer, const uint64_t nNewValuesBufferSize, const double* pNewValuesBuffer);

		void TransformField(const double dScale, const double dOffset);

		void AddField(CDiscreteFieldData2DInstance* pOtherField, const double dScale, const double dOffset);

		PDiscreteFieldData2DInstance Duplicate();

		void renderRGBImage(std::vector<uint8_t>* pPixelData, double minValue, double minRed, double minGreen, double minBlue, double midValue, double midRed, double midGreen, double midBlue, double maxValue, double maxRed, double maxGreen, double maxBlue);

		void renderAveragePointValues_FloorSampling(const LibMCEnv_double dDefaultValue, const uint64_t nPointValuesBufferSize, const LibMCEnv::sFieldData2DPoint* pPointValuesBuffer);

		void saveToBuffer (std::vector<uint8_t> & Buffer);
		
	};

	


	
}


#endif //__AMC_DISCRETEFIELDDATA2D

