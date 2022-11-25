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


#ifndef __AMC_TOOLPATHLAYERDATA
#define __AMC_TOOLPATHLAYERDATA

#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <map>

#include "lib3mf/lib3mf_dynamic.hpp"
#include "libmcenv_types.hpp"

namespace AMC {


	typedef struct _sToolpathLayerSegment {
		LibMCEnv::eToolpathSegmentType m_Type;
		uint32_t m_PointStartIndex;
		uint32_t m_PointCount;
		uint32_t m_ProfileID;
		uint32_t m_PartID;
	} sToolpathLayerSegment;

	typedef struct _sToolpathLayerProfile {
		double m_dLaserSpeed;
		double m_dLaserPower;
		double m_dLaserFocus;
		double m_dJumpSpeed;
		double m_dExtrusionFactor;
		double m_dStartDelay;
		double m_dEndDelay;
		double m_dPolyDelay;
		double m_dJumpDelay;
		double m_dLaserOnDelay;
		double m_dLaserOffDelay;
	} sToolpathLayerProfile;


	class CToolpathLayerData {
	private:
		double m_dUnits;
		int32_t m_nZValue;
		std::string m_sUUID;
		
		std::vector<sToolpathLayerSegment> m_Segments;
		std::vector<LibMCEnv::sPosition2D> m_Points;

		std::vector<std::string> m_UUIDs;
		std::map<std::string, uint32_t> m_UUIDMap;
		std::map<std::string, sToolpathLayerProfile> m_ProfileMap;

		std::string m_sDebugName;

		uint32_t registerUUID(const std::string& sUUID);
		std::string getRegisteredUUID(const uint32_t nID);

		void storeProfileData(Lib3MF::PToolpath pToolpath, const std::string& sProfileUUID);
		sToolpathLayerProfile retrieveProfileData(const std::string& sProfileUUID);

	public:

		CToolpathLayerData(Lib3MF::PToolpath pToolpath, Lib3MF::PToolpathLayerReader p3MFLayer, double dUnits, int32_t nZValue, const std::string & sDebugName);
		virtual ~CToolpathLayerData();		

		std::string getUUID ();

		uint32_t getSegmentCount();	
		uint32_t getSegmentPointCount (const uint32_t nSegmentIndex);
		LibMCEnv::eToolpathSegmentType getSegmentType (const uint32_t nSegmentIndex);

		void storePointsToBufferInUnits (const uint32_t nSegmentIndex, LibMCEnv::sPosition2D * pPositionData);
		void storeHatchesToBufferInUnits (const uint32_t nSegmentIndex, LibMCEnv::sHatch2D* pHatchData);
		void storePointsToBufferInMM(const uint32_t nSegmentIndex, LibMCEnv::sFloatPosition2D* pPositionData);
		void storeHatchesToBufferInMM(const uint32_t nSegmentIndex, LibMCEnv::sFloatHatch2D* pHatchData);

		std::string getSegmentProfileUUID(const uint32_t nSegmentIndex);
		std::string getSegmentPartUUID(const uint32_t nSegmentIndex);
		sToolpathLayerProfile getSegmentProfile(const uint32_t nSegmentIndex);

		double getUnits();

		int32_t getZValue ();

	};


	typedef std::shared_ptr<CToolpathLayerData> PToolpathLayerData;
	
}


#endif //__AMC_TOOLPATHLAYERDATA

