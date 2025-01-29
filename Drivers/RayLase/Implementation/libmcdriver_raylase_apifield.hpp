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


*/


#ifndef __LIBMCDRIVER_RAYLASE_APIFIELD
#define __LIBMCDRIVER_RAYLASE_APIFIELD

#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <string>

#define RAYLASEAPI_MAXNAMELENGTH 256
#define RAYLASEAPI_MINDATASIZE 4
#define RAYLASEAPI_MAXDATASIZE (1024 * 1024)


namespace LibMCDriver_Raylase {
	namespace Impl {


		/*************************************************************************************************************************
		 Class declaration of CRaylaseCard
		**************************************************************************************************************************/

		enum class eRaylaseAPIVariableType : uint32_t {
			evtUnknown = 0,
			evtUint8 = 1,
			evtUint16 = 2,
			evtUint32 = 3,
			evtUint64 = 4,
			evtInt8 = 5,
			evtInt16 = 6,
			evtInt32 = 7,
			evtInt64 = 8,
			evtFloat = 9,
			evtDouble = 10,
			evtCharArray = 11,
			evtBool = 12,
			evtEnum8 = 13,
			evtEnum16 = 14,
			evtEnum32 = 15
		};


		class CRaylaseAPIVariable {
		private:
			std::string m_sName;
			eRaylaseAPIVariableType m_Type;
			size_t m_nOffset;

		public:
			CRaylaseAPIVariable(const std::string& sName, size_t nOffset, eRaylaseAPIVariableType variableType);

			virtual ~CRaylaseAPIVariable();

			std::string getName();

			eRaylaseAPIVariableType getType();

			size_t getOffset();

			bool isBool();

			bool isInteger();

			bool isDouble();

		};


		class CRaylaseAPIField  {
		private:

			std::string m_sFieldName;
			uint32_t m_nSchemaVersion;

			std::vector<uint8_t> m_Data;
			std::unordered_map<std::string, CRaylaseAPIVariable> m_Variables;
			std::vector<std::string> m_VariableNames;

			CRaylaseAPIVariable& findVariable(const std::string& sVariableName);

		protected:

			void registerVariable(const std::string& sVariableName, size_t nOffset, eRaylaseAPIVariableType variableType);

		public:

			CRaylaseAPIField(const std::string & sFieldName, uint32_t nSchemaVersion, size_t nDataSizeInBytes);

			virtual ~CRaylaseAPIField();

			std::string getFieldName ();

			uint32_t getSchemaVersion ();

			size_t getDataSize();

			void setInteger (const std::string & sVariableName, int64_t nValue);
			void setDouble (const std::string& sVariableName, double dValue);
			void setBool (const std::string& sVariableName, bool bValue);
			void setEnum (const std::string& sVariableName, uint32_t nValue);

			int64_t getInteger(const std::string& sVariableName);
			double getDouble(const std::string& sVariableName);
			bool getBool(const std::string& sVariableName);
			uint32_t getEnum(const std::string& sVariableName, uint32_t);

			void resetToZero();

			uint8_t* getData();

			static uint32_t getVariableSize(eRaylaseAPIVariableType variableType);

			static bool checkName(const std::string& sName);

			uint32_t getVariableCount();

			std::string getVariableName(uint32_t nIndex);

			CRaylaseAPIVariable getVariable(uint32_t nIndex);

			std::string getVariableValueAsString(const std::string& sName);

		};

	} // namespace Impl
} // namespace LibMCDriver_Raylase

#endif // __LIBMCDRIVER_RAYLASE_APIFIELD

