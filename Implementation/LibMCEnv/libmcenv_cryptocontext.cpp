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


Abstract: This is a stub class definition of CCryptoContext

*/

#include "libmcenv_cryptocontext.hpp"
#include "libmcenv_interfaceexception.hpp"

// Include custom headers here.
#include "common_utils.hpp"

#define SHA256_RANDOM_ITERATIONS 32

using namespace LibMCEnv::Impl;

/*************************************************************************************************************************
 Class definition of CCryptoContext 
**************************************************************************************************************************/

CCryptoContext::CCryptoContext()
{

}

CCryptoContext::~CCryptoContext()
{

}

std::string CCryptoContext::CalculateSHA256FromString(const std::string & sValue)
{
    if (sValue.empty ())
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYSHA256SOURCE);

    return AMCCommon::CUtils::calculateSHA256FromString(sValue);
}

std::string CCryptoContext::CalculateSHA256FromBytes(const LibMCEnv_uint64 nValueBufferSize, const LibMCEnv_uint8 * pValueBuffer)
{
    if ((nValueBufferSize == 0) || (pValueBuffer == nullptr))
        throw ELibMCEnvInterfaceException(LIBMCENV_ERROR_EMPTYSHA256SOURCE);

    return AMCCommon::CUtils::calculateSHA256FromData (pValueBuffer, nValueBufferSize);
}

std::string CCryptoContext::NormalizeSHA256String(const std::string & sValue)
{
    return AMCCommon::CUtils::normalizeSHA256String(sValue);
}

std::string CCryptoContext::CreateRandomSHA256Hash()
{
    return AMCCommon::CUtils::calculateRandomSHA256String (SHA256_RANDOM_ITERATIONS);
}

std::string CCryptoContext::CreateUUID()
{
    return AMCCommon::CUtils::createUUID();
}

std::string CCryptoContext::NormalizeUUIDString(const std::string& sValue)
{
    return AMCCommon::CUtils::normalizeUUIDString(sValue);
}


