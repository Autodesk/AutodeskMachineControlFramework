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


#ifndef __LIBMCDRIVER_BK9XXX_ANALOGIO
#define __LIBMCDRIVER_BK9XXX_ANALOGIO

#include "libmcdriver_bk9xxx_interfaces.hpp"

// Parent classes
#include "libmcdriver_bk9xxx_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "pugixml.hpp"
#include <map>

namespace LibMCDriver_BK9xxx {
namespace Impl {


		class CDriver_BK9xxx_AnalogIODefinition;
		class CDriver_BK9xxx_AnalogIODefinitionBlock;
		class CDriver_BK9xxx_AnalogInputsDefinition;
		class CDriver_BK9xxx_AnalogOutputsDefinition;

		typedef std::shared_ptr<CDriver_BK9xxx_AnalogIODefinition> PDriver_BK9xxx_AnalogIODefinition;
		typedef std::shared_ptr<CDriver_BK9xxx_AnalogIODefinitionBlock> PDriver_BK9xxx_AnalogIODefinitionBlock;
		typedef std::shared_ptr<CDriver_BK9xxx_AnalogInputsDefinition> PDriver_BK9xxx_AnalogInputsDefinition;
		typedef std::shared_ptr<CDriver_BK9xxx_AnalogOutputsDefinition> PDriver_BK9xxx_AnalogOutputsDefinition;


		class CDriver_BK9xxx_AnalogIODefinition
		{
		private:

			uint32_t m_nOffset;

			std::string m_sName;

			std::string m_sDescription;

			uint32_t m_nRawValue;

			uint32_t m_nRawMin;

			uint32_t m_nRawMax;

			double m_dScaledMin;

			double m_dScaledMax;

			std::string m_sUnits;


		public:

			CDriver_BK9xxx_AnalogIODefinition(pugi::xml_node& xmlNode);

			virtual ~CDriver_BK9xxx_AnalogIODefinition();

			uint32_t getOffset();

			std::string getName();

			std::string getDescription();

			uint32_t getRawValue();

			void setRawValue(int64_t nRawValue);

			double getScaledValue();

			void setScaledValue(double dScaledValue);

		};


		class CDriver_BK9xxx_AnalogIODefinitionBlock
		{
		private:
			uint32_t m_nStartAddress;
			uint32_t m_nRegisterCount;

			std::vector<PDriver_BK9xxx_AnalogIODefinition> m_IODefinitions;
			std::map<std::string, PDriver_BK9xxx_AnalogIODefinition> m_IODefinitionMap;

		public:

			CDriver_BK9xxx_AnalogIODefinitionBlock(pugi::xml_node& xmlNode, const std::string& sNodeName);
			virtual ~CDriver_BK9xxx_AnalogIODefinitionBlock();

			uint32_t getStartAddress();

			uint32_t getRegisterCount();

			uint32_t getCount();

			PDriver_BK9xxx_AnalogIODefinition getIODefinition(uint32_t nIndex);

			PDriver_BK9xxx_AnalogIODefinition findIODefinition(const std::string& sName, bool bFailIfNotExisting);

		};



		class CDriver_BK9xxx_AnalogInputsDefinition : public CDriver_BK9xxx_AnalogIODefinitionBlock
		{
		private:
		public:

			CDriver_BK9xxx_AnalogInputsDefinition(pugi::xml_node& xmlNode);

			virtual ~CDriver_BK9xxx_AnalogInputsDefinition();

		};


		class CDriver_BK9xxx_AnalogOutputsDefinition : public CDriver_BK9xxx_AnalogIODefinitionBlock
		{
		private:
		public:

			CDriver_BK9xxx_AnalogOutputsDefinition(pugi::xml_node& xmlNode);

			virtual ~CDriver_BK9xxx_AnalogOutputsDefinition();

			void readOutputs(LibMCEnv::PModbusTCPConnection pModBusTCPConnection);

			void writeOutputs(LibMCEnv::PModbusTCPConnection pModBusTCPConnection);

		};



} // namespace Impl
} // namespace LibMCDriver_BK9xxx

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_BK9XXX_ANALOGIO
