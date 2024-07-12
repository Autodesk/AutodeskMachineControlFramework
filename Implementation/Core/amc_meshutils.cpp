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


#include "amc_meshutils.hpp"

#include <string>

#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

#include "amc_geometryutils.hpp"
#include "amc_constants.hpp"

namespace AMC {

	LibMCEnv::sModelDataTransform CMeshUtils::map3MFTransform(const Lib3MF::sTransform transform3MF)
	{
		LibMCEnv::sModelDataTransform transform;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				transform.m_Matrix[j][i] = transform3MF.m_Fields[j][i];
			}

			transform.m_Translation[i] = transform3MF.m_Fields[3][i];
		}

		return transform;

	}

	LibMCEnv::sModelDataTransform CMeshUtils::createIdentityTransform()
	{
		// Create identity transform
		LibMCEnv::sModelDataTransform transform;
		memset((void*)&transform, 0, sizeof(transform));
		for (uint32_t i = 0; i < 3; i++)
			transform.m_Matrix[i][i] = 1.0;

		return transform;
	}

	LibMCEnv::sModelDataTransform CMeshUtils::multiplyTransforms(const LibMCEnv::sModelDataTransform& transform1, const LibMCEnv::sModelDataTransform& transform2)
	{
		LibMCEnv::sModelDataTransform resultTransform;

		for (uint32_t i = 0; i < 3; i++) {
			for (uint32_t j = 0; j < 3; j++) {
				resultTransform.m_Matrix[i][j] =	transform1.m_Matrix[i][0] * transform2.m_Matrix[0][j] +
													transform1.m_Matrix[i][1] * transform2.m_Matrix[1][j] +
													transform1.m_Matrix[i][2] * transform2.m_Matrix[2][j];
			}

			resultTransform.m_Translation[i] = transform1.m_Matrix[i][0] * transform2.m_Translation[0] +
											   transform1.m_Matrix[i][1] * transform2.m_Translation[1] + 
											   transform1.m_Matrix[i][2] * transform2.m_Translation[2] + 
											   transform1.m_Translation[i];

		}

		return resultTransform;
	}

}


