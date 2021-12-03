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


Abstract: This is a stub class definition of CPLCCommand

*/

#include "libmcdriver_bur_definitions.hpp"
#include "libmcdriver_bur_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_BuR::Impl;




CDriver_BuRValue::CDriver_BuRValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress)
    : m_sName(sName), m_sDescription (sDescription), m_nAddress(nAddress)
{

}

CDriver_BuRValue::~CDriver_BuRValue()
{

}

std::string CDriver_BuRValue::getName()
{
    return m_sName;
}

std::string CDriver_BuRValue::getDescription()
{
    return m_sDescription;
}

uint32_t CDriver_BuRValue::getAddress()
{
    return m_nAddress;
}


void CDriver_BuRValue::writeDataToPayload(sAMCFToPLCPacketPayload& payLoad, uint8_t* pData, size_t nSize)
{
    if (pData == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    if (m_nAddress >= sizeof (sAMCFToPLCPacketPayload))
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPAYLOADADDRESS, "invalid payroll address: " + m_sName + " / " + std::to_string (m_nAddress));
    if (m_nAddress + nSize > sizeof(sAMCFToPLCPacketPayload))
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPAYLOADADDRESS, "invalid payroll size: " + m_sName + " / " + std::to_string(nSize));

    uint8_t* pTarget = (uint8_t*)&payLoad;
    for (uint32_t nIndex = 0; nIndex < nSize; nIndex++) {
        pTarget[m_nAddress + nIndex] = pData[nIndex];
    }
}

void CDriver_BuRValue::writeToPayload(sAMCFToPLCPacketPayload& payLoad)
{
    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_COULDNOTWRITETOPAYLOAD, "could not write to payload: " + m_sName);
}

void CDriver_BuRValue::setIntegerValue(uint64_t nValue)
{
    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_VALUEISNOTINTEGERVALUE, "value is not integer value: " + m_sName);
}

void CDriver_BuRValue::setBoolValue(bool bValue)
{
    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_VALUEISNOTBOOLVALUE, "value is not bool value: " + m_sName);
}

void CDriver_BuRValue::setDoubleValue(double dValue)
{
    throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_VALUEISNOTDOUBLEVALUE, "value is not double value: " + m_sName);
}

CDriver_BuRStringValue::CDriver_BuRStringValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress, const uint32_t nLength)
    : CDriver_BuRValue(sName, sDescription, nAddress), m_nLength(nLength)
{

}

ePLCFieldType CDriver_BuRStringValue::getFieldType()
{
    return ePLCFieldType::ftString;
}

uint32_t CDriver_BuRStringValue::getLength()
{
    return m_nLength;
}

PDriver_BuRValue CDriver_BuRStringValue::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRStringValue>(m_sName, m_sDescription, m_nAddress, m_nLength);
    pDuplicate->m_sValue = m_sValue;
    return pDuplicate;
}



CDriver_BuRRealValue::CDriver_BuRRealValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress)
    : CDriver_BuRValue(sName, sDescription, nAddress), m_dValue(0.0)
{

}

ePLCFieldType CDriver_BuRRealValue::getFieldType()
{
    return ePLCFieldType::ftReal;
}

void CDriver_BuRRealValue::setDoubleValue(double dValue)
{
    m_dValue = dValue;
}

PDriver_BuRValue CDriver_BuRRealValue::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRRealValue>(m_sName, m_sDescription, m_nAddress);
    pDuplicate->m_dValue = m_dValue;
    return pDuplicate;
}


void CDriver_BuRRealValue::writeToPayload(sAMCFToPLCPacketPayload& payLoad)
{
    float fValue = (float) m_dValue;
    writeDataToPayload(payLoad, (uint8_t*) &fValue, sizeof (fValue));    
}

CDriver_BuRLRealValue::CDriver_BuRLRealValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress)
    : CDriver_BuRValue(sName, sDescription, nAddress), m_dValue(0.0)
{

}

ePLCFieldType CDriver_BuRLRealValue::getFieldType()
{
    return ePLCFieldType::ftReal;
}

PDriver_BuRValue CDriver_BuRLRealValue::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRLRealValue>(m_sName, m_sDescription, m_nAddress);
    pDuplicate->m_dValue = m_dValue;
    return pDuplicate;
}


void CDriver_BuRLRealValue::setDoubleValue(double dValue)
{
    m_dValue = dValue;
}

void CDriver_BuRLRealValue::writeToPayload(sAMCFToPLCPacketPayload& payLoad)
{
    double dValue = m_dValue;
    writeDataToPayload(payLoad, (uint8_t*)&dValue, sizeof(dValue));
}


CDriver_BuRDIntValue::CDriver_BuRDIntValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress)
    : CDriver_BuRValue(sName, sDescription, nAddress), m_nValue (0)
{

}

ePLCFieldType CDriver_BuRDIntValue::getFieldType()
{
    return ePLCFieldType::ftDInt;
}

void CDriver_BuRDIntValue::setIntegerValue(uint64_t nValue)
{
    m_nValue = nValue;
}

PDriver_BuRValue CDriver_BuRDIntValue::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRDIntValue>(m_sName, m_sDescription, m_nAddress);
    pDuplicate->m_nValue = m_nValue;
    return pDuplicate;
}

void CDriver_BuRDIntValue::writeToPayload(sAMCFToPLCPacketPayload& payLoad)
{
    uint32_t nValue = (uint32_t) m_nValue;
    writeDataToPayload(payLoad, (uint8_t*)&nValue, sizeof(nValue));
}


CDriver_BuRIntValue::CDriver_BuRIntValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress)
    : CDriver_BuRValue(sName, sDescription, nAddress), m_nValue(0)
{

}

PDriver_BuRValue CDriver_BuRIntValue::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRIntValue>(m_sName, m_sDescription, m_nAddress);
    pDuplicate->m_nValue = m_nValue;
    return pDuplicate;
}


ePLCFieldType CDriver_BuRIntValue::getFieldType()
{
    return ePLCFieldType::ftInt;
}

void CDriver_BuRIntValue::setIntegerValue(uint64_t nValue)
{
    m_nValue = nValue;
}

void CDriver_BuRIntValue::writeToPayload(sAMCFToPLCPacketPayload& payLoad)
{
    uint16_t nValue = (uint16_t) m_nValue;
    writeDataToPayload(payLoad, (uint8_t*)&nValue, sizeof(nValue));
}


CDriver_BuRBoolValue::CDriver_BuRBoolValue(const std::string& sName, const std::string& sDescription, const uint32_t nAddress)
    : CDriver_BuRValue(sName, sDescription, nAddress), m_bValue(false)
{
}

ePLCFieldType CDriver_BuRBoolValue::getFieldType()
{
    return ePLCFieldType::ftBool;
}

void CDriver_BuRBoolValue::setBoolValue(bool bValue)
{
    m_bValue = bValue;
}

PDriver_BuRValue CDriver_BuRBoolValue::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRBoolValue>(m_sName, m_sDescription, m_nAddress);
    pDuplicate->m_bValue = m_bValue;
    return pDuplicate;
}

void CDriver_BuRBoolValue::writeToPayload(sAMCFToPLCPacketPayload& payLoad)
{
    uint8_t nValue = m_bValue ? 1 : 0;
    writeDataToPayload(payLoad, (uint8_t*)&nValue, sizeof(nValue));
}

CDriver_BuRCommandDefinition::CDriver_BuRCommandDefinition(const std::string& sName, const uint32_t nCommandID)
    : m_sName (sName), m_nCommandID (nCommandID)
{

}

CDriver_BuRCommandDefinition::~CDriver_BuRCommandDefinition()
{

}

std::string CDriver_BuRCommandDefinition::getName()
{
    return m_sName;
}

uint32_t CDriver_BuRCommandDefinition::getCommandID()
{
    return m_nCommandID;
}


void CDriver_BuRCommandDefinition::addParameter(PDriver_BuRValue pValue)
{
    if (pValue.get () == nullptr)
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_INVALIDPARAM);

    std::string sName = pValue->getName();

    auto iIter = m_Parameters.find(sName);
    if (iIter != m_Parameters.end())
        throw ELibMCDriver_BuRInterfaceException(LIBMCDRIVER_BUR_ERROR_DUPLICATECOMMANDPARAMETER);

    m_ParameterNames.push_back(sName);
    m_Parameters.insert(std::make_pair(sName, pValue));

}


std::list <std::string> CDriver_BuRCommandDefinition::getParameterNames()
{
    return m_ParameterNames;
}

CDriver_BuRValue* CDriver_BuRCommandDefinition::findParameter(const std::string& sName)
{
    auto iIter = m_Parameters.find(sName);
    if (iIter != m_Parameters.end())
        return iIter->second.get();

    return nullptr;
}


PDriver_BuRCommandDefinition CDriver_BuRCommandDefinition::duplicate()
{
    auto pDuplicate = std::make_shared<CDriver_BuRCommandDefinition>(m_sName, m_nCommandID);
    for (auto pParameter : m_Parameters)
        pDuplicate->addParameter(pParameter.second->duplicate ());

    return pDuplicate;
}
