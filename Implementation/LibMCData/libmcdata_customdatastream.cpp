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


Abstract: This is a stub class definition of CCustomDataStream

*/

#include "libmcdata_customdatastream.hpp"
#include "libmcdata_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

using namespace LibMCData::Impl;

/*************************************************************************************************************************
 Class definition of CCustomDataStream 
**************************************************************************************************************************/

CCustomDataStream::CCustomDataStream(const std::string& sDataUUID, const std::string& sIdentifier, const std::string& sName, LibMCData::eCustomDataType eDataType, std::string& sTimeStamp, std::string& sStorageStreamUUID, std::string& sUserID, std::string& sSHA2, uint64_t nStreamSize)
	: m_sIdentifier(sIdentifier),
    m_sName(sName),
    m_eDataType(eDataType),
    m_sDataUUID(AMCCommon::CUtils::normalizeUUIDString(sDataUUID)),
    m_sTimeStamp(sTimeStamp),
    m_sStorageStreamUUID(AMCCommon::CUtils::normalizeUUIDString(sStorageStreamUUID)),
    m_sUserID(sUserID),
    m_sSHA256(sSHA2),
    m_nStreamSize(nStreamSize)

{

}

CCustomDataStream::~CCustomDataStream()
{

}


std::string CCustomDataStream::GetDataUUID()
{
    return m_sDataUUID;
}

std::string CCustomDataStream::GetIdentifier()
{
    return m_sIdentifier;
}

std::string CCustomDataStream::GetName()
{
    return m_sName;
}

std::string CCustomDataStream::GetTimeStamp()
{
    return m_sTimeStamp;
}

std::string CCustomDataStream::GetStorageStreamUUID()
{
    return m_sStorageStreamUUID;
}

std::string CCustomDataStream::GetStorageStreamSHA2()
{
    return m_sSHA256;
}

LibMCData_uint64 CCustomDataStream::GetStorageStreamSize()
{
    return m_nStreamSize;
}

LibMCData::eCustomDataType CCustomDataStream::GetDataType()
{
    return m_eDataType;
}

std::string CCustomDataStream::GetDataTypeAsString()
{
    return convertCustomDataTypeToString(m_eDataType);
}

std::string CCustomDataStream::GetUserUUID()
{
    return m_sUserID;
}

std::string CCustomDataStream::convertCustomDataTypeToString(const LibMCData::eCustomDataType dataType)
{
    switch (dataType) {
    case LibMCData::eCustomDataType::Unknown: return "unknown";
    case LibMCData::eCustomDataType::Toolpath: return "toolpath";
    case LibMCData::eCustomDataType::Timeline: return "timeline";
    case LibMCData::eCustomDataType::CustomBinaryData: return "custombinarydata";
    case LibMCData::eCustomDataType::PNGImage: return "pngimage";
    case LibMCData::eCustomDataType::JPEGImage: return "jpegimage";
    case LibMCData::eCustomDataType::Thumbnail: return "thumbnail";
    default:
        throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE);
    }
}

LibMCData::eCustomDataType CCustomDataStream::convertStringToCustomDataType(const std::string& sValue)
{
    if (sValue == "unknown")
        return LibMCData::eCustomDataType::Unknown;
    if (sValue == "toolpath")
        return LibMCData::eCustomDataType::Toolpath;
    if (sValue == "timeline")
        return LibMCData::eCustomDataType::Timeline;
    if (sValue == "custombinarydata")
        return LibMCData::eCustomDataType::CustomBinaryData;
    if (sValue == "pngimage")
        return LibMCData::eCustomDataType::PNGImage;
    if (sValue == "jpegimage")
        return LibMCData::eCustomDataType::JPEGImage;
    if (sValue == "thumbnail")
        return LibMCData::eCustomDataType::Thumbnail;

    throw ELibMCDataInterfaceException(LIBMCDATA_ERROR_INVALIDDATATYPE, "invalid data type: " + sValue);

}
