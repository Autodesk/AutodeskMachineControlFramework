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

#include "libmcdriver_bk9xxx_digitalio.hpp"
#include "libmcdriver_bk9xxx_interfaceexception.hpp"


using namespace LibMCDriver_BK9xxx::Impl;

#define BK9XXX_DIGITALIO_MINOFFSET 0
#define BK9XXX_DIGITALIO_MAXOFFSET 1023
#define BK9XXX_DIGITALIO_MINSTARTADDRESS 0
#define BK9XXX_DIGITALIO_MAXSTARTADDRESS 1023
#define BK9XXX_DIGITALIO_MINBITCOUNT 0
#define BK9XXX_DIGITALIO_MAXBITCOUNT 1023


CDriver_BK9xxx_DigitalIODefinition::CDriver_BK9xxx_DigitalIODefinition(pugi::xml_node& xmlNode)
	: m_nOffset(0), m_bValue(false)
{

	auto offsetAttrib = xmlNode.attribute("offset");
	if (offsetAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGDIGITALIOOFFSET);
	int32_t nOffset = offsetAttrib.as_int(-1);

	if ((nOffset < BK9XXX_DIGITALIO_MINOFFSET) || (nOffset > BK9XXX_DIGITALIO_MAXOFFSET))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDDIGITALIOOFFSET);
	m_nOffset = (uint32_t)nOffset;

	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGDIGITALIONAME);
	m_sName = nameAttrib.as_string();
	if (m_sName.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_EMPTYDIGITALIONAME);

	auto descriptionAttrib = xmlNode.attribute("description");
	m_sDescription = descriptionAttrib.as_string();

}

CDriver_BK9xxx_DigitalIODefinition::~CDriver_BK9xxx_DigitalIODefinition()
{

}

uint32_t CDriver_BK9xxx_DigitalIODefinition::getOffset()
{
	return m_nOffset;
}

std::string CDriver_BK9xxx_DigitalIODefinition::getName()
{
	return m_sName;
}

std::string CDriver_BK9xxx_DigitalIODefinition::getDescription()
{
	return m_sDescription;
}

bool CDriver_BK9xxx_DigitalIODefinition::getValue()
{
	return m_bValue;
}

void CDriver_BK9xxx_DigitalIODefinition::setValue(bool bValue)
{
	m_bValue = bValue;
}


CDriver_BK9xxx_DigitalIODefinitionBlock::CDriver_BK9xxx_DigitalIODefinitionBlock(pugi::xml_node& xmlNode, const std::string& sNodeName)
	: m_nStartAddress(0), m_nBitCount(0)
{
	auto startAddressAttrib = xmlNode.attribute("startaddress");
	if (startAddressAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGDIGITALIOSTARTADDRESS);
	int32_t nStartAddress = startAddressAttrib.as_int(-1);
	if ((nStartAddress < BK9XXX_DIGITALIO_MINSTARTADDRESS) || (nStartAddress > BK9XXX_DIGITALIO_MAXSTARTADDRESS))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDDIGITALIOSTARTADDRESS);
	m_nStartAddress = (uint32_t)nStartAddress;

	auto bitCountAttrib = xmlNode.attribute("bitcount");
	if (bitCountAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGDIGITALIOBITCOUNT);
	int32_t nBitCount = bitCountAttrib.as_int(-1);
	if ((nBitCount < BK9XXX_DIGITALIO_MINBITCOUNT) || (nBitCount > BK9XXX_DIGITALIO_MAXBITCOUNT))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDDIGITALIOBITCOUNT);
	m_nBitCount = (uint32_t)nBitCount;

	auto digitalIONodes = xmlNode.children(sNodeName.c_str());
	for (auto digitalIONode : digitalIONodes) {
		auto pDigitalIO = std::make_shared<CDriver_BK9xxx_DigitalIODefinition>(digitalIONode);
		std::string sName = pDigitalIO->getName();
		m_IODefinitions.push_back(pDigitalIO);
		m_IODefinitionMap.insert(std::make_pair(sName, pDigitalIO));
	}

}

CDriver_BK9xxx_DigitalIODefinitionBlock::~CDriver_BK9xxx_DigitalIODefinitionBlock()
{

}

uint32_t CDriver_BK9xxx_DigitalIODefinitionBlock::getStartAddress()
{
	return m_nStartAddress;
}

uint32_t CDriver_BK9xxx_DigitalIODefinitionBlock::getBitCount()
{
	return m_nBitCount;
}

uint32_t CDriver_BK9xxx_DigitalIODefinitionBlock::getCount()
{
	return (uint32_t)m_IODefinitions.size();
}

PDriver_BK9xxx_DigitalIODefinition CDriver_BK9xxx_DigitalIODefinitionBlock::getIODefinition(uint32_t nIndex)
{
	if (nIndex >= m_IODefinitions.size())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDDIGITALIOINDEX);

	return m_IODefinitions.at(nIndex);
}

PDriver_BK9xxx_DigitalIODefinition CDriver_BK9xxx_DigitalIODefinitionBlock::findIODefinition(const std::string& sName, bool bFailIfNotExisting)
{
	auto iIterator = m_IODefinitionMap.find(sName);
	if (iIterator == m_IODefinitionMap.end()) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_DIGITALIONOTFOUND, "digital io not found: " + sName);

		return nullptr;
	}

	return iIterator->second;
}


CDriver_BK9xxx_DigitalInputsDefinition::CDriver_BK9xxx_DigitalInputsDefinition(pugi::xml_node& xmlNode)
	: CDriver_BK9xxx_DigitalIODefinitionBlock(xmlNode, "input")
{

}

CDriver_BK9xxx_DigitalInputsDefinition::~CDriver_BK9xxx_DigitalInputsDefinition()
{

}


CDriver_BK9xxx_DigitalOutputsDefinition::CDriver_BK9xxx_DigitalOutputsDefinition(pugi::xml_node& xmlNode)
	: CDriver_BK9xxx_DigitalIODefinitionBlock(xmlNode, "output")
{

}

CDriver_BK9xxx_DigitalOutputsDefinition::~CDriver_BK9xxx_DigitalOutputsDefinition()
{

}