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


#ifndef __LIBMCDRIVER_BK9XXX_DIGITALIO
#define __LIBMCDRIVER_BK9XXX_DIGITALIO

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


		class CDriver_BK9xxx_DigitalIODefinition;
		class CDriver_BK9xxx_DigitalIODefinitionBlock;
		class CDriver_BK9xxx_DigitalInputsDefinition;
		class CDriver_BK9xxx_DigitalOutputsDefinition;

		typedef std::shared_ptr<CDriver_BK9xxx_DigitalIODefinition> PDriver_BK9xxx_DigitalIODefinition;
		typedef std::shared_ptr<CDriver_BK9xxx_DigitalIODefinitionBlock> PDriver_BK9xxx_DigitalIODefinitionBlock;
		typedef std::shared_ptr<CDriver_BK9xxx_DigitalInputsDefinition> PDriver_BK9xxx_DigitalInputsDefinition;
		typedef std::shared_ptr<CDriver_BK9xxx_DigitalOutputsDefinition> PDriver_BK9xxx_DigitalOutputsDefinition;


		class CDriver_BK9xxx_DigitalIODefinition
		{
		private:

			uint32_t m_nOffset;

			std::string m_sName;

			std::string m_sDescription;

			bool m_bValue;

		public:

			CDriver_BK9xxx_DigitalIODefinition(pugi::xml_node& xmlNode);

			virtual ~CDriver_BK9xxx_DigitalIODefinition();

			uint32_t getOffset();

			std::string getName();

			std::string getDescription();

			bool getValue();

			void setValue(bool bValue);

		};


		class CDriver_BK9xxx_DigitalIODefinitionBlock
		{
		private:
			uint32_t m_nStartAddress;
			uint32_t m_nBitCount;

			std::vector<PDriver_BK9xxx_DigitalIODefinition> m_IODefinitions;
			std::map<std::string, PDriver_BK9xxx_DigitalIODefinition> m_IODefinitionMap;

		public:

			CDriver_BK9xxx_DigitalIODefinitionBlock(pugi::xml_node& xmlNode, const std::string& sNodeName);
			virtual ~CDriver_BK9xxx_DigitalIODefinitionBlock();

			uint32_t getStartAddress();

			uint32_t getBitCount();

			uint32_t getCount();

			PDriver_BK9xxx_DigitalIODefinition getIODefinition(uint32_t nIndex);

			PDriver_BK9xxx_DigitalIODefinition findIODefinition(const std::string& sName, bool bFailIfNotExisting);

		};



		class CDriver_BK9xxx_DigitalInputsDefinition : public CDriver_BK9xxx_DigitalIODefinitionBlock
		{
		private:
		public:

			CDriver_BK9xxx_DigitalInputsDefinition(pugi::xml_node& xmlNode);

			virtual ~CDriver_BK9xxx_DigitalInputsDefinition();

		};


		class CDriver_BK9xxx_DigitalOutputsDefinition : public CDriver_BK9xxx_DigitalIODefinitionBlock
		{
		private:
		public:

			CDriver_BK9xxx_DigitalOutputsDefinition(pugi::xml_node& xmlNode);

			virtual ~CDriver_BK9xxx_DigitalOutputsDefinition();

			void readOutputs(LibMCEnv::PModbusTCPConnection pModBusTCPConnection);

			void writeOutputs(LibMCEnv::PModbusTCPConnection pModBusTCPConnection);

		};



} // namespace Impl
} // namespace LibMCDriver_BK9xxx

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_BK9XXX_DIGITALIO
