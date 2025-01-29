/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CVideoDevice

*/


#ifndef __LIBMCDRIVER_CAMERA_VIDEORESOLUTION
#define __LIBMCDRIVER_CAMERA_VIDEORESOLUTION

#include <cstdint>
#include <memory>

#define CAMERARESOLUTION_MIN 8
#define CAMERARESOLUTION_MAX (256 * 1024)
#define CAMERAFRAMERATE_MIN 1
#define CAMERAFRAMERATE_MAX 1000000

namespace LibMCDriver_Camera {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CVideoResolution
**************************************************************************************************************************/

class CVideoResolution {
private:
	uint32_t m_nWidth;
	uint32_t m_nHeight;
	uint32_t m_nFramerate;

public:
	
	CVideoResolution(uint32_t nWidth, uint32_t nHeight, uint32_t nFramerate);

	virtual ~CVideoResolution();
	
	uint32_t getWidth ();
	uint32_t getHeight ();
	uint32_t getFramerate ();

};

typedef std::shared_ptr<CVideoResolution> PVideoResolution;

} // namespace Impl
} // namespace LibMCDriver_Camera

#endif // __LIBMCDRIVER_CAMERA_VIDEORESOLUTION
