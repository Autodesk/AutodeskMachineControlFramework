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


Abstract: This is a stub class definition of CRaylaseCard

*/

#include "libmcdriver_raylase_apifield.hpp"
#include "libmcdriver_raylase_interfaceexception.hpp"
#include <cstdint>

using namespace LibMCDriver_Raylase::Impl;


/*************************************************************************************************************************
 Class definition of CRaylaseCard
**************************************************************************************************************************/


CRaylaseAPIVariable::CRaylaseAPIVariable(const std::string& sName, size_t nOffset, eRaylaseAPIVariableType variableType)
    : m_sName(sName), m_nOffset(nOffset), m_Type(variableType)
{

}

CRaylaseAPIVariable::~CRaylaseAPIVariable()
{

}

std::string CRaylaseAPIVariable::getName()
{
    return m_sName;
}

eRaylaseAPIVariableType CRaylaseAPIVariable::getType()
{
    return m_Type;
}

size_t CRaylaseAPIVariable::getOffset()
{
    return m_nOffset;
}

bool CRaylaseAPIVariable::isBool()
{
    return (m_Type == eRaylaseAPIVariableType::evtBool);

}

bool CRaylaseAPIVariable::isInteger()
{
    return (m_Type == eRaylaseAPIVariableType::evtEnum16) ||
        (m_Type == eRaylaseAPIVariableType::evtEnum32) ||
        (m_Type == eRaylaseAPIVariableType::evtEnum8) ||
        (m_Type == eRaylaseAPIVariableType::evtUint8) ||
        (m_Type == eRaylaseAPIVariableType::evtUint16) ||
        (m_Type == eRaylaseAPIVariableType::evtUint32) ||
        (m_Type == eRaylaseAPIVariableType::evtUint64) ||
        (m_Type == eRaylaseAPIVariableType::evtInt8) ||
        (m_Type == eRaylaseAPIVariableType::evtInt16) ||
        (m_Type == eRaylaseAPIVariableType::evtInt32) ||
        (m_Type == eRaylaseAPIVariableType::evtInt64);

}

bool CRaylaseAPIVariable::isDouble()
{
    return (m_Type == eRaylaseAPIVariableType::evtDouble) || (m_Type == eRaylaseAPIVariableType::evtFloat);
}



CRaylaseAPIField::CRaylaseAPIField(const std::string& sFieldName, uint32_t nSchemaVersion, size_t nDataSizeInBytes)
    : m_sFieldName (sFieldName), m_nSchemaVersion (nSchemaVersion)
{
    if (!checkName(sFieldName))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDAPIFIELDNAME, "Invalid API Field Name: " + sFieldName);

    if ((nDataSizeInBytes < RAYLASEAPI_MINDATASIZE) || (nDataSizeInBytes > RAYLASEAPI_MAXDATASIZE))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDFIELDDATASIZE, "Invalid API Field Data Size: " + std::to_string (nDataSizeInBytes));


    m_Data.resize(nDataSizeInBytes);   
}

CRaylaseAPIField::~CRaylaseAPIField()
{

}

void CRaylaseAPIField::registerVariable(const std::string& sVariableName, size_t nOffset, eRaylaseAPIVariableType variableType)
{

    if (!checkName(sVariableName))
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDAPIVARIABLENAME, "Invalid API Variable Name: " + sVariableName);

    auto iIter = m_Variables.find(sVariableName);
    if (iIter != m_Variables.end ())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_DUPLICATEAPIVARIABLENAME, "Duplicate API Variable Name: " + sVariableName);

    if (nOffset >= m_Data.size())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDVARIABLEOFFSET, "Invalid variable offset: " + std::to_string (nOffset));

    if (nOffset + getVariableSize (variableType) > m_Data.size())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + std::to_string(nOffset));

    m_Variables.insert(std::make_pair(sVariableName, CRaylaseAPIVariable(sVariableName, nOffset, variableType)));
    m_VariableNames.push_back(sVariableName);
}

std::string CRaylaseAPIField::getFieldName()
{
    return m_sFieldName;
}

uint32_t CRaylaseAPIField::getSchemaVersion()
{
    return m_nSchemaVersion;
}

size_t CRaylaseAPIField::getDataSize()
{
    return m_Data.size();
}

void CRaylaseAPIField::setInteger(const std::string& sVariableName, int64_t nValue)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {
        case eRaylaseAPIVariableType::evtUint8: {
            if ((nValue < 0) || (nValue > UINT8_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Uint8 range: " + std::to_string (nValue));
            if (nOffset + sizeof (uint8_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            uint8_t *pTarget = (uint8_t*) &m_Data.at(nOffset);
            *pTarget = (uint8_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtUint16: {
            if ((nValue < 0) || (nValue > UINT16_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Uint16 range: " + std::to_string(nValue));
            if (nOffset + sizeof(uint16_t) > m_Data.size ())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            uint16_t* pTarget = (uint16_t*)&m_Data.at(nOffset);
            *pTarget = (uint16_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtUint32: {
            if ((nValue < 0) || (nValue > UINT32_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Uint32 range: " + std::to_string(nValue));
            if (nOffset + sizeof(uint32_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            uint32_t* pTarget = (uint32_t*)&m_Data.at(nOffset);
            *pTarget = (uint32_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtUint64: {
            if ((nValue < 0) || (nValue > UINT64_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Uint64 range: " + std::to_string(nValue));
            if (nOffset + sizeof(uint64_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            uint64_t* pTarget = (uint64_t*)&m_Data.at(nOffset);
            *pTarget = (uint64_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtInt8: {
            if ((nValue < INT8_MIN) || (nValue > INT8_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Int8 range: " + std::to_string(nValue));
            if (nOffset + sizeof(int8_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            int8_t* pTarget = (int8_t*)&m_Data.at(nOffset);
            *pTarget = (int8_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtInt16: {
            if ((nValue < INT16_MIN) || (nValue > INT16_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Int16 range: " + std::to_string(nValue));
            if (nOffset + sizeof(int16_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            int16_t* pTarget = (int16_t*)&m_Data.at(nOffset);
            *pTarget = (int16_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtInt32: {
            if ((nValue < INT32_MIN) || (nValue > INT32_MAX))
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INTEGERVALUEOUTOFRANGE, "Integer value out of Int32 range: " + std::to_string(nValue));
            if (nOffset + sizeof(int32_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            int32_t* pTarget = (int32_t*)&m_Data.at(nOffset);
            *pTarget = (int32_t)nValue;

            break;
        }

        case eRaylaseAPIVariableType::evtInt64: {
            if (nOffset + sizeof(int64_t) > m_Data.size())
                throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

            int64_t* pTarget = (int64_t*)&m_Data.at(nOffset);
            *pTarget = (int64_t)nValue;

            break;
        }

        default:
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEINTEGER, "Variable is not of type integer: " + sVariableName);

    }


}

void CRaylaseAPIField::setDouble(const std::string& sVariableName, double dValue)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {

    case eRaylaseAPIVariableType::evtFloat: {
        if (nOffset + sizeof (float) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        float* pTarget = (float*)&m_Data.at(nOffset);
        *pTarget = (float)dValue;

        break;
    }

    case eRaylaseAPIVariableType::evtDouble: {
        if (nOffset + sizeof (double) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        double* pTarget = (double*)&m_Data.at(nOffset);
        *pTarget = (double)dValue;

        break;
    }

    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEDOUBLE, "Variable is not of type double: " + sVariableName);

    }


}

void CRaylaseAPIField::setBool(const std::string& sVariableName, bool bValue)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {

    case eRaylaseAPIVariableType::evtBool: {
        if (nOffset + sizeof (uint8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint8_t* pTarget = (uint8_t*)&m_Data.at(nOffset);
        if (bValue)
            *pTarget = 1;
        else
            *pTarget = 0;

        break;
    }

    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEBOOL, "Variable is not of type bool: " + sVariableName);

    }

}

void CRaylaseAPIField::setEnum(const std::string& sVariableName, uint32_t nValue)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {
    case eRaylaseAPIVariableType::evtEnum8: {
        if ((nValue < 0) || (nValue > UINT8_MAX))
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_ENUMVALUEOUTOFRANGE, "Enum value out of Enum8 range: " + std::to_string(nValue));
        if (nOffset + sizeof (uint8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint8_t* pTarget = (uint8_t*)&m_Data.at(nOffset);
        *pTarget = (uint8_t)nValue;

        break;
    }

    case eRaylaseAPIVariableType::evtEnum16: {
        if ((nValue < 0) || (nValue > UINT16_MAX))
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_ENUMVALUEOUTOFRANGE, "Enum value out of Enum16 range: " + std::to_string(nValue));
        if (nOffset + sizeof (uint16_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint16_t* pTarget = (uint16_t*)&m_Data.at(nOffset);
        *pTarget = (uint16_t)nValue;

        break;
    }

    case eRaylaseAPIVariableType::evtEnum32: {
        if ((nValue < 0) || (nValue > UINT32_MAX))
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_ENUMVALUEOUTOFRANGE, "Enum value out of Enum32 range: " + std::to_string(nValue));
        if (nOffset + sizeof (uint32_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint32_t* pTarget = (uint32_t*)&m_Data.at(nOffset);
        *pTarget = (uint32_t)nValue;

        break;
    }

    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEENUM, "Variable is not of type enum: " + sVariableName);

    }


}

int64_t CRaylaseAPIField::getInteger(const std::string& sVariableName)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {
    case eRaylaseAPIVariableType::evtUint8: {
        if (nOffset + sizeof (uint8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint8_t* pSource = (uint8_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtUint16: {
        if (nOffset + sizeof(uint16_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint16_t* pSource = (uint16_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtUint32: {
        if (nOffset + sizeof(uint32_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint32_t* pSource = (uint32_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtUint64: {
        if (nOffset + sizeof(uint64_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint64_t* pSource = (uint64_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtInt8: {
        if (nOffset + sizeof(int8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        int8_t* pSource = (int8_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtInt16: {
        if (nOffset + sizeof(int16_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        int16_t* pSource = (int16_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtInt32: {
        if (nOffset + sizeof(int32_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        int32_t* pSource = (int32_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtInt64: {
        if (nOffset + sizeof(int64_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        int64_t* pSource = (int64_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtEnum8: {
        if (nOffset + sizeof(uint8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint8_t* pSource = (uint8_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtEnum16: {
        if (nOffset + sizeof(uint16_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint16_t* pSource = (uint16_t*)&m_Data.at(nOffset);
        return (int64_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtEnum32: {
        if (nOffset + sizeof(uint32_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint32_t* pSource = (uint32_t*)&m_Data.at(nOffset);
        return (int32_t)*pSource;
    }


    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEINTEGER, "Variable is not of type integer: " + sVariableName);

    }

}

double CRaylaseAPIField::getDouble(const std::string& sVariableName)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {

    case eRaylaseAPIVariableType::evtFloat: {
        if (nOffset + sizeof(float) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        float* pSource = (float*)&m_Data.at(nOffset);
        return (double)*pSource;
    }

    case eRaylaseAPIVariableType::evtDouble: {
        if (nOffset + sizeof(double) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        double* pSource = (double*)&m_Data.at(nOffset);
        return (double)*pSource;
    }

    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEDOUBLE, "Variable is not of type double: " + sVariableName);

    }

}

bool CRaylaseAPIField::getBool(const std::string& sVariableName)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {

    case eRaylaseAPIVariableType::evtBool: {
        if (nOffset + sizeof(uint8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint8_t* pSource = (uint8_t*)&m_Data.at(nOffset);
        return ((*pSource) != 0);
    }

    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEBOOL, "Variable is not of type bool: " + sVariableName);

    }

}

uint32_t CRaylaseAPIField::getEnum(const std::string& sVariableName, uint32_t)
{
    auto& variable = findVariable(sVariableName);
    size_t nOffset = variable.getOffset();

    switch (variable.getType()) {
    case eRaylaseAPIVariableType::evtEnum8: {
        if (nOffset + sizeof(uint8_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint8_t* pSource = (uint8_t*)&m_Data.at(nOffset);
        return (uint32_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtEnum16: {
        if (nOffset + sizeof(uint16_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint16_t* pSource = (uint16_t*)&m_Data.at(nOffset);
        return (uint32_t)*pSource;
    }

    case eRaylaseAPIVariableType::evtEnum32: {
        if (nOffset + sizeof(uint32_t) > m_Data.size())
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEEXCEEDSBUFFER, "Variable exceeds buffer: " + sVariableName);

        uint32_t* pSource = (uint32_t*)&m_Data.at(nOffset);
        return (uint32_t)*pSource;
    }

    default:
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLEISNOTOFTYPEENUM, "Variable is not of type enum: " + sVariableName);

    }

}

void CRaylaseAPIField::resetToZero()
{
    for (auto& dataByte : m_Data)
        dataByte = 0;
}

uint8_t* CRaylaseAPIField::getData()
{
    return m_Data.data();
}

uint32_t CRaylaseAPIField::getVariableSize(eRaylaseAPIVariableType variableType)
{
    switch (variableType) {
        case eRaylaseAPIVariableType::evtUint8: return sizeof(uint8_t);
        case eRaylaseAPIVariableType::evtUint16: return sizeof(uint16_t);
        case eRaylaseAPIVariableType::evtUint32: return sizeof(uint32_t);
        case eRaylaseAPIVariableType::evtUint64: return sizeof(uint64_t);
        case eRaylaseAPIVariableType::evtInt8: return sizeof(int8_t);
        case eRaylaseAPIVariableType::evtInt16: return sizeof(int16_t);
        case eRaylaseAPIVariableType::evtInt32: return sizeof(int32_t);
        case eRaylaseAPIVariableType::evtInt64: return sizeof(int64_t);
        case eRaylaseAPIVariableType::evtFloat: return sizeof(float);
        case eRaylaseAPIVariableType::evtDouble: return sizeof(double);
        case eRaylaseAPIVariableType::evtBool: return sizeof(uint8_t);
        case eRaylaseAPIVariableType::evtEnum8: return sizeof(uint8_t);
        case eRaylaseAPIVariableType::evtEnum16: return sizeof(uint16_t);
        case eRaylaseAPIVariableType::evtEnum32: return sizeof(uint32_t);
        default: 
            throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLETYPENOTSUPPORTED, "Variable type not supported: " + std::to_string ((uint32_t) variableType));

    }
}

bool CRaylaseAPIField::checkName(const std::string& sName)
{
    if (sName.empty())
        return false;

    if (sName.length() > RAYLASEAPI_MAXNAMELENGTH)
        return false;

    for (auto ch : sName)
        if (!(isalnum(ch) || (ch == '.') || (ch == '_')))
            return false;

    return true;
}

CRaylaseAPIVariable& CRaylaseAPIField::findVariable(const std::string& sVariableName)
{
    auto iIter = m_Variables.find(sVariableName);
    if (iIter == m_Variables.end())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLENOTFOUND, "Variable not found: " + sVariableName);

    return iIter->second;

}

uint32_t CRaylaseAPIField::getVariableCount()
{
    return (uint32_t)m_VariableNames.size();
}

std::string CRaylaseAPIField::getVariableName(uint32_t nIndex)
{
    if (nIndex >= m_VariableNames.size())
        throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_INVALIDVARIABLEINDEX);

    return m_VariableNames.at(nIndex);
}


CRaylaseAPIVariable CRaylaseAPIField::getVariable(uint32_t nIndex)
{
    return findVariable(getVariableName(nIndex));

}

std::string CRaylaseAPIField::getVariableValueAsString(const std::string& sName)
{
    auto variable = findVariable(sName);
    if (variable.isInteger())
        return std::to_string(getInteger (sName));
    if (variable.isBool())
        return std::to_string(getBool(sName));
    if (variable.isDouble())
        return std::to_string(getDouble(sName));

    throw ELibMCDriver_RaylaseInterfaceException(LIBMCDRIVER_RAYLASE_ERROR_VARIABLENOTFOUND, "Variable not found: " + sName);
}


 