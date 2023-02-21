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

#include "libmcdriver_bk9xxx_analogio.hpp"
#include "libmcdriver_bk9xxx_interfaceexception.hpp"

#include <cmath>

using namespace LibMCDriver_BK9xxx::Impl;

#define BK9XXX_ANALOGIO_MINOFFSET 0
#define BK9XXX_ANALOGIO_MAXOFFSET 1023
#define BK9XXX_ANALOGIO_MINSTARTADDRESS 0
#define BK9XXX_ANALOGIO_MAXSTARTADDRESS 1023
#define BK9XXX_ANALOGIO_MINREGISTERCOUNT 0
#define BK9XXX_ANALOGIO_MAXREGISTERCOUNT 1023

#define BK9XXX_ANALOGIO_MINRAWVALUE 0
#define BK9XXX_ANALOGIO_MAXRAWVALUE 0xffffffffUL

#define BK9XXX_ANALOGIO_MINSCALEDVALUE -1.0E30
#define BK9XXX_ANALOGIO_MAXSCALEDVALUE +1.0E30
#define BK9XXX_ANALOGIO_INVALIDSCALEDVALUE +1.0E40
#define BK9XXX_ANALOGIO_MINSCALEDINTERVAL 1.0E-5


CDriver_BK9xxx_AnalogIODefinition::CDriver_BK9xxx_AnalogIODefinition(pugi::xml_node& xmlNode)
	: m_nOffset(0), m_nActualRawValue(0), m_nTargetRawValue (0), m_nRawMin (0), m_nRawMax (65535), m_dScaledMin (0.0), m_dScaledMax (100.0)
{

	auto offsetAttrib = xmlNode.attribute("offset");
	if (offsetAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIOOFFSET);
	int32_t nOffset = offsetAttrib.as_int(-1);

	if ((nOffset < BK9XXX_ANALOGIO_MINOFFSET) || (nOffset > BK9XXX_ANALOGIO_MAXOFFSET))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOOFFSET);
	m_nOffset = (uint32_t)nOffset;

	auto nameAttrib = xmlNode.attribute("name");
	if (nameAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIONAME);
	m_sName = nameAttrib.as_string();
	if (m_sName.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_EMPTYANALOGIONAME);

	auto descriptionAttrib = xmlNode.attribute("description");
	m_sDescription = descriptionAttrib.as_string();

	auto rawMinAttrib = xmlNode.attribute("raw_min");
	if (rawMinAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIORAWMINVALUE);
	int32_t nRawMin = rawMinAttrib.as_int(-1);

	auto rawMaxAttrib = xmlNode.attribute("raw_max");
	if (rawMaxAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIORAWMAXVALUE);
	int32_t nRawMax = rawMaxAttrib.as_int(-1);

	if ((nRawMin < BK9XXX_ANALOGIO_MINRAWVALUE) || (nRawMin > BK9XXX_ANALOGIO_MAXRAWVALUE))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIORAWMINVALUE);
	if ((nRawMax < BK9XXX_ANALOGIO_MINRAWVALUE) || (nRawMax > BK9XXX_ANALOGIO_MAXRAWVALUE))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIORAWMAXVALUE);
	if (nRawMin >= nRawMax)
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIORAWINTERVAL);

	m_nRawMin = (uint32_t)nRawMin;
	m_nRawMax = (uint32_t)nRawMax;
	m_nActualRawValue = m_nRawMin;
	m_nTargetRawValue = m_nRawMin;

	auto scaledMinAttrib = xmlNode.attribute("scaled_min");
	if (scaledMinAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIOSCALEDMINVALUE);
	double dScaledMin = scaledMinAttrib.as_double(BK9XXX_ANALOGIO_INVALIDSCALEDVALUE);

	auto scaledMaxAttrib = xmlNode.attribute("scaled_max");
	if (scaledMaxAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIOSCALEDMAXVALUE);
	double dScaledMax = scaledMaxAttrib.as_double(BK9XXX_ANALOGIO_INVALIDSCALEDVALUE);

	if ((dScaledMin < BK9XXX_ANALOGIO_MINSCALEDVALUE) || (dScaledMin > BK9XXX_ANALOGIO_MAXSCALEDVALUE))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOSCALEDMINVALUE);
	if ((dScaledMax < BK9XXX_ANALOGIO_MINSCALEDVALUE) || (dScaledMax > BK9XXX_ANALOGIO_MAXSCALEDVALUE))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOSCALEDMAXVALUE);
	if (dScaledMin + BK9XXX_ANALOGIO_MINSCALEDINTERVAL > dScaledMax)
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOSCALEDINTERVAL);

	m_dScaledMin = dScaledMin;
	m_dScaledMax = dScaledMax;

}

CDriver_BK9xxx_AnalogIODefinition::~CDriver_BK9xxx_AnalogIODefinition()
{

}

uint32_t CDriver_BK9xxx_AnalogIODefinition::getOffset()  const
{
	return m_nOffset;
}

std::string CDriver_BK9xxx_AnalogIODefinition::getName() const
{
	return m_sName;
}

std::string CDriver_BK9xxx_AnalogIODefinition::getDescription() const
{
	return m_sDescription;
}

uint32_t CDriver_BK9xxx_AnalogIODefinition::getActualRawValue() const
{ 
	return m_nActualRawValue;
}

uint32_t CDriver_BK9xxx_AnalogIODefinition::getTargetRawValue() const
{
	return m_nTargetRawValue;
}

uint32_t CDriver_BK9xxx_AnalogIODefinition::setActualRawValue(int64_t nRawValue)
{
	if (nRawValue < (int64_t)m_nRawMin) {
		m_nActualRawValue = m_nRawMin;
	}
	else if (nRawValue > (int64_t)m_nRawMax) {
		m_nActualRawValue = m_nRawMax;
	}
	else {
		m_nActualRawValue = (uint32_t) nRawValue;
	}

	return m_nActualRawValue;
}

uint32_t CDriver_BK9xxx_AnalogIODefinition::setTargetRawValue(int64_t nRawValue)
{
	if (nRawValue < (int64_t)m_nRawMin) {
		m_nTargetRawValue = m_nRawMin;
	}
	else if (nRawValue > (int64_t)m_nRawMax) {
		m_nTargetRawValue = m_nRawMax;
	}
	else {
		m_nTargetRawValue = (uint32_t)nRawValue;
	}

	return m_nTargetRawValue;
}


double CDriver_BK9xxx_AnalogIODefinition::rawValueToScaledValue(int64_t nRawValue) const
{
	if (nRawValue <= (int64_t)m_nRawMin) {
		return m_dScaledMin;
	}
	else if (nRawValue >= (int64_t)m_nRawMax) {
		return m_dScaledMax;
	}
	else {
		double dFactor = ((double)(nRawValue - (int64_t) m_nRawMin)) / ((double)(m_nRawMax - m_nRawMin));
		return (1.0 - dFactor) * m_dScaledMin + dFactor * m_dScaledMax;
	}

}

uint32_t CDriver_BK9xxx_AnalogIODefinition::scaledValueToRawValue(double dScaledValue) const
{
	double dFactor = (dScaledValue - m_dScaledMin) / (m_dScaledMax - m_dScaledMin);
	if (dFactor < 0.0)
		dFactor = 0.0;
	if (dFactor > 1.0)
		dFactor = 1.0;

	double dRawValue = ((1.0 - dFactor) * (double)m_nRawMin) + (dFactor * (double)m_nRawMax);
	int64_t nRawValue = (int64_t)dRawValue;
	if (nRawValue < (int64_t)m_nRawMin) {
		return m_nRawMin;
	}
	else if (nRawValue > (int64_t)m_nRawMax) {
		return m_nRawMax;
	}
	else {
		return (uint32_t)nRawValue;
	}
}

CDriver_BK9xxx_AnalogIODefinitionBlock::CDriver_BK9xxx_AnalogIODefinitionBlock(pugi::xml_node& xmlNode, const std::string& sNodeName)
	: m_nStartAddress(0), m_nRegisterCount(0)
{
	auto startAddressAttrib = xmlNode.attribute("startaddress");
	if (startAddressAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIOSTARTADDRESS);
	int32_t nStartAddress = startAddressAttrib.as_int(-1);
	if ((nStartAddress < BK9XXX_ANALOGIO_MINSTARTADDRESS) || (nStartAddress > BK9XXX_ANALOGIO_MAXSTARTADDRESS))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOSTARTADDRESS);
	m_nStartAddress = (uint32_t)nStartAddress;

	auto registerCountAttrib = xmlNode.attribute("registercount");
	if (registerCountAttrib.empty())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_MISSINGANALOGIOREGISTERCOUNT);
	int32_t nRegisterCount = registerCountAttrib.as_int(-1);
	if ((nRegisterCount < BK9XXX_ANALOGIO_MINREGISTERCOUNT) || (nRegisterCount > BK9XXX_ANALOGIO_MAXREGISTERCOUNT))
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOREGISTERCOUNT);
	m_nRegisterCount = (uint32_t)nRegisterCount;

	auto analogIONodes = xmlNode.children(sNodeName.c_str());
	for (auto analogIONode : analogIONodes) {
		auto pAnalogIO = std::make_shared<CDriver_BK9xxx_AnalogIODefinition>(analogIONode);
		if (pAnalogIO->getOffset() >= m_nRegisterCount)
			throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_ANALOGIOOFFSETOUTOFRANGE);

		std::string sName = pAnalogIO->getName();
		m_IODefinitions.push_back(pAnalogIO);
		m_IODefinitionMap.insert(std::make_pair(sName, pAnalogIO));
	}

}

CDriver_BK9xxx_AnalogIODefinitionBlock::~CDriver_BK9xxx_AnalogIODefinitionBlock()
{

}

uint32_t CDriver_BK9xxx_AnalogIODefinitionBlock::getStartAddress()
{
	return m_nStartAddress;
}

uint32_t CDriver_BK9xxx_AnalogIODefinitionBlock::getRegisterCount()
{
	return m_nRegisterCount;
}

uint32_t CDriver_BK9xxx_AnalogIODefinitionBlock::getCount()
{
	return (uint32_t)m_IODefinitions.size();
}

PDriver_BK9xxx_AnalogIODefinition CDriver_BK9xxx_AnalogIODefinitionBlock::getIODefinition(uint32_t nIndex)
{
	if (nIndex >= m_IODefinitions.size())
		throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_INVALIDANALOGIOINDEX);

	return m_IODefinitions.at(nIndex);
}

PDriver_BK9xxx_AnalogIODefinition CDriver_BK9xxx_AnalogIODefinitionBlock::findIODefinition(const std::string& sName, bool bFailIfNotExisting)
{
	auto iIterator = m_IODefinitionMap.find(sName);
	if (iIterator == m_IODefinitionMap.end()) {
		if (bFailIfNotExisting)
			throw ELibMCDriver_BK9xxxInterfaceException(LIBMCDRIVER_BK9XXX_ERROR_ANALOGIONOTFOUND, "analog io not found: " + sName);

		return nullptr;
	}

	return iIterator->second;
}


CDriver_BK9xxx_AnalogInputsDefinition::CDriver_BK9xxx_AnalogInputsDefinition(pugi::xml_node& xmlNode)
	: CDriver_BK9xxx_AnalogIODefinitionBlock(xmlNode, "input")
{

}

CDriver_BK9xxx_AnalogInputsDefinition::~CDriver_BK9xxx_AnalogInputsDefinition()
{

}


CDriver_BK9xxx_AnalogOutputsDefinition::CDriver_BK9xxx_AnalogOutputsDefinition(pugi::xml_node& xmlNode)
	: CDriver_BK9xxx_AnalogIODefinitionBlock(xmlNode, "output")
{

}

CDriver_BK9xxx_AnalogOutputsDefinition::~CDriver_BK9xxx_AnalogOutputsDefinition()
{

}