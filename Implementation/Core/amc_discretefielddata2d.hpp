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
		
		std::vector<double> m_Data;

	public:

		CDiscreteFieldData2DInstance(size_t nPixelCountX, size_t nPixelCountY, double dDPIX, double dDPIY, double dOriginX, double dOriginY, double dValue);
		
		virtual ~CDiscreteFieldData2DInstance();


	};

	


	
}


#endif //__AMC_DISCRETEFIELDDATA2D

