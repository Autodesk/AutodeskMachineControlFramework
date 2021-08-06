/*++

Copyright (C) 2021 Autodesk Inc.

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


Abstract: This is a stub class definition of CDataStream

*/

#include "libamcf_datastream.hpp"
#include "libamcf_interfaceexception.hpp"

using namespace LibAMCF::Impl;

/*************************************************************************************************************************
 Class definition of CDataStream 
**************************************************************************************************************************/

CDataStream::CDataStream(const std::string& sUUID, const std::string& sContextUUID, const std::string& sName, const std::string& sMimeType, const std::string& sSHA256, uint64_t nSize, const std::string& sTimestamp)
    : m_sUUID (sUUID), m_sContextUUID (sContextUUID), m_sName (sName), m_sMimeType (sMimeType), m_sSHA256 (sSHA256), m_nSize (nSize), m_sTimestamp (sTimestamp)
{

}


std::string CDataStream::GetUUID()
{
    return m_sUUID;
}

std::string CDataStream::GetContextUUID()
{
    return m_sContextUUID;
}

std::string CDataStream::GetName()
{
    return m_sName;
}

std::string CDataStream::GetMimeType()
{
    return m_sMimeType;
}

std::string CDataStream::GetSHA256()
{
    return m_sSHA256;
}

LibAMCF_uint64 CDataStream::GetSize()
{
    return m_nSize;
}

std::string CDataStream::GetTimestamp()
{
    return m_sTimestamp;
}

