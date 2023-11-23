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


#ifndef __AMC_GEOMETRYUTILS
#define __AMC_GEOMETRYUTILS

#include <cstdint>
#include <cmath>

#define CONST_PI 3.14159265358979323846

namespace AMC {
	
	typedef struct _sMeshVector3D {
		double m_dCoordinates[3];
	} sMeshVector3D;
	
	class CVectorUtils {

	public:
		
		static sMeshVector3D vectorCrossProduct(const sMeshVector3D& vec1, const sMeshVector3D& vec2)
		{
			return sMeshVector3D{
				vec1.m_dCoordinates[1] * vec2.m_dCoordinates[2] - vec1.m_dCoordinates[2] * vec2.m_dCoordinates[1],
				vec1.m_dCoordinates[2] * vec2.m_dCoordinates[0] - vec1.m_dCoordinates[0] * vec2.m_dCoordinates[2],
				vec1.m_dCoordinates[0] * vec2.m_dCoordinates[1] - vec1.m_dCoordinates[1] * vec2.m_dCoordinates[0]
			};
		}

		static sMeshVector3D vectorZero()
		{
			return sMeshVector3D{
				0.0, 0.0, 0.0
			};

		}

		static sMeshVector3D vectorMake(double dX, double dY, double dZ)
		{
			return sMeshVector3D{
				dX, dY, dZ
			};

		}

		static sMeshVector3D vectorAdd (const sMeshVector3D& vec1, const sMeshVector3D& vec2)
		{
			return sMeshVector3D{
				vec1.m_dCoordinates[0] + vec2.m_dCoordinates[0],
				vec1.m_dCoordinates[1] + vec2.m_dCoordinates[1],
				vec1.m_dCoordinates[2] + vec2.m_dCoordinates[2]
			};

		}
		
		static sMeshVector3D vectorSubtract (const sMeshVector3D& vec1, const sMeshVector3D& vec2)
		{
			return sMeshVector3D{
				vec1.m_dCoordinates[0] - vec2.m_dCoordinates[0],
				vec1.m_dCoordinates[1] - vec2.m_dCoordinates[1],
				vec1.m_dCoordinates[2] - vec2.m_dCoordinates[2]
			};
		}
		
		static sMeshVector3D vectorScale (const sMeshVector3D& vec, double dFactor)
		{
			return sMeshVector3D{
				vec.m_dCoordinates[0] * dFactor,
				vec.m_dCoordinates[1] * dFactor,
				vec.m_dCoordinates[2] * dFactor
			};
		}
		
		static double vectorLength (const sMeshVector3D& vec)
		{
			double dX = vec.m_dCoordinates[0];
			double dY = vec.m_dCoordinates[1];
			double dZ = vec.m_dCoordinates[2];

			return sqrt(dX * dX + dY * dY + dZ * dZ);
		}
		
		static sMeshVector3D vectorNormalize (const sMeshVector3D& vec, double dLengthZeroEpsilon)
		{
			double dLength = vectorLength(vec);
			if (dLength < dLengthZeroEpsilon)
				return vectorZero();

			return vectorScale(vec, 1.0 / dLength);
		}
		
		static double vectorAngleInRadian (const sMeshVector3D& vec1, const sMeshVector3D& vec2, double dLengthZeroEpsilon)
		{
			double dLength1 = vectorLength(vec1);
			if (dLength1 < dLengthZeroEpsilon)
				return 0.0;

			double dLength2 = vectorLength(vec2);
			if (dLength2 < dLengthZeroEpsilon)
				return 0.0;

			double dotProduct = vectorDotProduct(vec1, vec2) / (dLength1 * dLength2);
			if (dotProduct < -1.0)
				dotProduct = -1.0;
			if (dotProduct > 1.0)
				dotProduct = 1.0;

			return acos(dotProduct);

		}


		static double vectorAngleInDegree(const sMeshVector3D& vec1, const sMeshVector3D& vec2, double dLengthZeroEpsilon)
		{
			return vectorAngleInRadian(vec1, vec2, dLengthZeroEpsilon) * 180.0 / CONST_PI;
		}

		static double vectorDotProduct(const sMeshVector3D& vec1, const sMeshVector3D& vec2)
		{
			return vec1.m_dCoordinates[0] * vec2.m_dCoordinates[0]
				+ vec1.m_dCoordinates[1] * vec2.m_dCoordinates[1]
				+ vec1.m_dCoordinates[2] * vec2.m_dCoordinates[2];
		}
		

	};
	
	
}


#endif //__AMC_GEOMETRYUTILS

