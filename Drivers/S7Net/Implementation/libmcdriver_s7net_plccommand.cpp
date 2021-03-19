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

#include "libmcdriver_s7net_plccommand.hpp"
#include "libmcdriver_s7net_interfaceexception.hpp"

// Include custom headers here.


using namespace LibMCDriver_S7Net::Impl;

/*************************************************************************************************************************
 Class definition of CPLCCommand 
**************************************************************************************************************************/

CPLCCommand::CPLCCommand(const std::string& sName)
    : m_sName (sName), m_nSequenceID (0)
{

}

std::string CPLCCommand::getName()
{
    return m_sName;
}

uint32_t CPLCCommand::getSequenceID()
{
    return m_nSequenceID;
}

void CPLCCommand::setSequenceID(const uint32_t nSequenceID)
{
    m_nSequenceID = nSequenceID;
}

void CPLCCommand::addParameterDefinition(const std::string& sName, const ePLCFieldType fieldType)
{
    if (sName.empty())
        throw ELibMCDriver_S7NetInterfaceException (LIBMCDRIVER_S7NET_ERROR_INVALIDPARAM);

    m_ParameterTypeMap.insert(std::make_pair (sName, fieldType));
}



void CPLCCommand::SetIntegerParameter(const std::string& sParameterName, const LibMCDriver_S7Net_int32 nValue)
{
    auto iParameterType = m_ParameterTypeMap.find(sParameterName);
    if (iParameterType == m_ParameterTypeMap.end ())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDPARAMETERNOTFOUND, "command parameter not found: " + sParameterName);

    if ((iParameterType->second != ePLCFieldType::ftDInt) && (iParameterType->second != ePLCFieldType::ftInt))
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAMETERTYPE, "command parameter is not of type integer: " + sParameterName);

    if (iParameterType->second == ePLCFieldType::ftInt) {
        if ((nValue < INT16_MIN) || (nValue > INT16_MAX))
            throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDPARAMETEROUTOFBOUNDS, "command field parameter is out of bounds: " + sParameterName);        
    }
        

    m_ParameterValues.insert(std::make_pair(sParameterName, std::to_string (nValue)));
}

void CPLCCommand::SetStringParameter(const std::string& sParameterName, const std::string& sValue)
{
    auto iParameterType = m_ParameterTypeMap.find(sParameterName);
    if (iParameterType == m_ParameterTypeMap.end())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDPARAMETERNOTFOUND, "command parameter not found: " + sParameterName);

    if (iParameterType->second != ePLCFieldType::ftString)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAMETERTYPE, "command parameter is not of type string: " + sParameterName);

    m_ParameterValues.insert(std::make_pair(sParameterName, sValue));
}

void CPLCCommand::SetBoolParameter(const std::string& sParameterName, const bool bValue)
{
    auto iParameterType = m_ParameterTypeMap.find(sParameterName);
    if (iParameterType == m_ParameterTypeMap.end())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDPARAMETERNOTFOUND, "command parameter not found: " + sParameterName);

    if (iParameterType->second != ePLCFieldType::ftBool)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAMETERTYPE, "command parameter is not of type bool: " + sParameterName);

    m_ParameterValues.insert(std::make_pair(sParameterName, bValue ? "1" : "0" ));
}

void CPLCCommand::SetDoubleParameter(const std::string& sParameterName, const LibMCDriver_S7Net_double dValue)
{
    auto iParameterType = m_ParameterTypeMap.find(sParameterName);
    if (iParameterType == m_ParameterTypeMap.end())
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_COMMANDPARAMETERNOTFOUND, "command parameter not found: " + sParameterName);

    if (iParameterType->second != ePLCFieldType::ftReal)
        throw ELibMCDriver_S7NetInterfaceException(LIBMCDRIVER_S7NET_ERROR_INVALIDPARAMETERTYPE, "command parameter is not of type double: " + sParameterName);

    m_ParameterValues.insert(std::make_pair(sParameterName, std::to_string(dValue)));
}


std::string CPLCCommand::getParameterValue(const std::string& sParameterName)
{
    auto iParameterValue = m_ParameterValues.find(sParameterName);
    if (iParameterValue != m_ParameterValues.end())
        return iParameterValue->second;

    return "";
}
