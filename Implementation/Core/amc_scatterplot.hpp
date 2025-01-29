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


#ifndef __AMC_SCATTERPLOT
#define __AMC_SCATTERPLOT

#include <memory>
#include <map>
#include <string>
#include <cstdint>
#include <vector>

namespace AMC {

	typedef struct _sScatterplotEntry {		
		double m_dX;
		double m_dY;
	} sScatterplotEntry;


	class CScatterplot;
	typedef std::shared_ptr<CScatterplot> PScatterplot;

	class CScatterplot {
	private:

		std::string m_sUUID;
		
		std::vector<sScatterplotEntry> m_PointEntries;

		double m_dMinX;
		double m_dMinY;
		double m_dMaxX;
		double m_dMaxY;

	public:

		CScatterplot(const std::string & sUUID);

		virtual ~CScatterplot();

		std::string getUUID();

		void clearData();

		bool isEmpty();
		
		uint64_t getEntryCount();

		std::vector<sScatterplotEntry> & getEntries ();

		void getBoundaries(double& dMinX, double& dMinY, double& dMaxX, double& dMaxY);

		void computeBoundaries();


	};

	
}


#endif //__AMC_SCATTERPLOT

