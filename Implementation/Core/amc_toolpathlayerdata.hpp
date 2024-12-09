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
#include <vector>

#include "lib3mf/lib3mf_dynamic.hpp"
#include "libmcenv_types.hpp"
#include "amc_xmldocument.hpp"

#define TOOLPATHSEGMENTOVERRIDEFACTOR_F 1
#define TOOLPATHSEGMENTOVERRIDEFACTOR_G 2
#define TOOLPATHSEGMENTOVERRIDEFACTOR_H 4

namespace AMC {


	typedef struct _sToolpathLayerSegment {
		LibMCEnv::eToolpathSegmentType m_Type;
		uint32_t m_PointStartIndex;
		uint32_t m_PointCount;
		uint32_t m_ProfileUUID;
		uint32_t m_PartUUID;
		uint32_t m_LocalPartID;
		uint32_t m_LaserIndex;
		uint32_t m_HasOverrideFactors;
		int64_t* m_AttributeData;
	} sToolpathLayerSegment;

	typedef struct _sToolpathLayerOverride {
		double m_dFactors[3]; // F, G and H
	} sToolpathLayerOverride;

	class CToolpathCustomSegmentAttribute {
	private:
		uint32_t m_nAttributeID;
		LibMCEnv::eToolpathAttributeType m_AttributeType;
		std::string m_sNameSpace;
		std::string m_sAttributeName;

	public:

		CToolpathCustomSegmentAttribute (const std::string & sNameSpace, const std::string & sAttributeName, LibMCEnv::eToolpathAttributeType attributeType);

		virtual ~CToolpathCustomSegmentAttribute();

		uint32_t getAttributeID();
		void setAttributeID(uint32_t nAttributeID);
		LibMCEnv::eToolpathAttributeType getAttributeType ();
		std::string getNameSpace();
		std::string getAttributeName ();

	};

	typedef std::shared_ptr<CToolpathCustomSegmentAttribute> PToolpathCustomSegmentAttribute;



	class CToolpathLayerProfile {
		private:
			std::string m_sUUID;
			std::string m_sName;
			std::map<std::pair<std::string, std::string>, std::string> m_ProfileValues;

		public:

			CToolpathLayerProfile(const std::string & sUUID, const std::string & sName);
			virtual ~CToolpathLayerProfile();

			std::string getUUID();
			std::string getName();
			void addValue(const std::string & sNameSpace, const std::string & sValueName, const std::string & sValue);

			bool hasValue(const std::string& sNameSpace, const std::string& sValueName);
			std::string getValue(const std::string& sNameSpace, const std::string& sValueName);
			std::string getValueDef(const std::string& sNameSpace, const std::string& sValueName, const std::string & sDefaultValue);
			double getDoubleValue(const std::string& sNameSpace, const std::string& sValueName);
			double getDoubleValueDef(const std::string& sNameSpace, const std::string& sValueName, double dDefaultValue);
			int64_t getIntegerValue(const std::string& sNameSpace, const std::string& sValueName);
			int64_t getIntegerValueDef(const std::string& sNameSpace, const std::string& sValueName, int64_t nDefaultValue);
	};

	typedef std::shared_ptr<CToolpathLayerProfile> PToolpathLayerProfile;


	class CToolpathLayerData {
	private:
		double m_dUnits;
		int32_t m_nZValue;
		std::string m_sUUID;
		
		std::vector<sToolpathLayerSegment> m_Segments;
		std::vector<int64_t> m_SegmentAttributeData;
		std::vector<LibMCEnv::sPosition2D> m_Points;
		std::vector<sToolpathLayerOverride> m_OverrideFactors;

		std::vector<std::string> m_UUIDs;
		std::map<std::string, uint32_t> m_UUIDMap;
		std::map<std::string, PToolpathLayerProfile> m_ProfileMap;

		std::vector<std::pair<std::pair<std::string, std::string>, std::string>> m_CustomData;

		std::string m_sDebugName;

		std::map<std::pair<std::string, std::string>, PToolpathCustomSegmentAttribute> m_CustomSegmentAttributeMap;
		std::vector<PToolpathCustomSegmentAttribute> m_CustomSegmentAttributes;

		uint32_t registerUUID(const std::string& sUUID);
		std::string getRegisteredUUID(const uint32_t nID);

		void storeProfileData(Lib3MF::PToolpath pToolpath, const std::string& sProfileUUID);
		PToolpathLayerProfile retrieveProfileData(const std::string& sProfileUUID);

	public:

		CToolpathLayerData(Lib3MF::PToolpath pToolpath, Lib3MF::PToolpathLayerReader p3MFLayer, double dUnits, int32_t nZValue, const std::string & sDebugName, std::vector<PToolpathCustomSegmentAttribute> customSegmentAttributes);
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
		uint32_t getSegmentLocalPartID(const uint32_t nSegmentIndex);
		uint32_t getSegmentLaserIndex(const uint32_t nSegmentIndex);
		PToolpathLayerProfile getSegmentProfile(const uint32_t nSegmentIndex);

		bool findCustomSegmentAttribute(const std::string& sNameSpace, const std::string& sName, uint32_t& nAttributeID, LibMCEnv::eToolpathAttributeType & attributeType);
		int64_t getSegmentIntegerAttribute(const uint32_t nSegmentIndex, uint32_t nAttributeID);
		double getSegmentDoubleAttribute(const uint32_t nSegmentIndex, uint32_t nAttributeID);

		double getUnits();

		int32_t getZValue ();

		uint32_t getMetaDataCount();
		void getMetaDataInfo(uint32_t nMetaDataIndex, std::string& sNameSpace, std::string& sName);
		PXMLDocumentInstance getMetaData(uint32_t nMetaDataIndex);
		bool hasUniqueMetaData(const std::string& sNameSpace, const std::string& sName);
		PXMLDocumentInstance findUniqueMetaData(const std::string& sNameSpace, const std::string& sName);

		void calculateExtents(int32_t & nMinX, int32_t & nMinY, int32_t & nMaxX, int32_t & nMaxY);

		bool segmentHasOverrideFactors(uint32_t nSegmentIndex, LibMCEnv::eToolpathProfileOverrideFactor eOverrideFactor);

		void storePointOverrides(uint32_t nSegmentIndex, LibMCEnv::eToolpathProfileOverrideFactor eOverrideFactor, double * pOverrideData);

		void storeHatchOverrides(uint32_t nSegmentIndex, LibMCEnv::eToolpathProfileOverrideFactor eOverrideFactor, LibMCEnv::sHatch2DOverrides* pOverrideData);

		static std::string getValueNameByType(const LibMCEnv::eToolpathProfileValueType eValueType);

	};


	typedef std::shared_ptr<CToolpathLayerData> PToolpathLayerData;
	
}


#endif //__AMC_TOOLPATHLAYERDATA

