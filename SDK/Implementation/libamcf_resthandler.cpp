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


*/


#include "libamcf_resthandler.hpp"
#include "libamcf_interfaceexception.hpp"

#include <RapidJSON/rapidjson.h>
#include <RapidJSON/document.h>
#include <RapidJSON/writer.h>

#ifndef _WASM
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>
#endif //_WASM

CRestHandler::CRestHandler(const std::string& sIdentifier, const std::string& sURL, const std::string& sAuthToken, uint32_t nTimeOut, uint32_t nRetryCount)
    : m_sIdentifier(sIdentifier), m_sURL (sURL), m_sAuthToken (sAuthToken), m_nTimeout (nTimeOut), m_nRetryCount (nRetryCount)
{

}

std::string CRestHandler::getResultValue(const std::string& sKey)
{
    auto iIter = m_ResultValues.find(sKey);
    if (iIter != m_ResultValues.end())
        return iIter->second;

    return "";
}

bool CRestHandler::hasResultValue(const std::string& sKey)
{
    auto iIter = m_ResultValues.find(sKey);
    return (iIter != m_ResultValues.end());

}


CRestHandler_Post::CRestHandler_Post(const std::string& sIdentifier, const std::string& sURL, const std::string& sAuthToken, uint32_t nTimeOut, uint32_t nRetryCount)
    : CRestHandler (sIdentifier, sURL, sAuthToken, nTimeOut, nRetryCount)
{

}

void CRestHandler_Post::addValue(const std::string& sKey, const std::string& sValue)
{
    if (sKey.empty())
        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDKEYNAME);

    m_PostValues.insert(std::make_pair(sKey, sValue));
}

void CRestHandler_Post::sendRequest()
{

    rapidjson::Document Request;
    Request.SetObject();

    for (auto iPair : m_PostValues) {
        rapidjson::Value KeyValue(iPair.first.c_str(), Request.GetAllocator());
        rapidjson::Value StringValue(iPair.second.c_str(), Request.GetAllocator());
        Request.AddMember(KeyValue, StringValue, Request.GetAllocator());
    }


    rapidjson::StringBuffer jsonBuffer;
    jsonBuffer.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(jsonBuffer);
    Request.Accept(writer);
    std::string sRequestBody = jsonBuffer.GetString();

#ifndef _WASM
    RestClient::HeaderFields headers;
    if (!m_sAuthToken.empty())
        headers.insert(std::make_pair("Authorization", "Bearer " + m_sAuthToken));
#endif // _WASM

    std::string sResponseBody;
    int32_t nResponseCode = -1;
    uint32_t nRetries = m_nRetryCount;

    while ((nResponseCode < 0) && (nRetries > 0)) {
#ifndef _WASM
        RestClient::Response RESTresponse = RestClient::post(m_sURL, "application/json", sRequestBody, headers, m_nTimeout, true);
        sResponseBody = RESTresponse.body;
        nResponseCode = RESTresponse.code;
#endif //_WASM
        nRetries--;
    }


    if ((nResponseCode == 200) || (nResponseCode == 400)) {

        rapidjson::Document JSONDocument;
        JSONDocument.Parse(sResponseBody.c_str());
        if (!JSONDocument.HasParseError()) {

            if (JSONDocument.IsObject()) {

                for (auto member = JSONDocument.MemberBegin(); member != JSONDocument.MemberEnd(); member++) {
                    std::string sName = member->name.GetString();
                    if (member->value.IsString()) {
                        std::string sValue = member->value.GetString();
                        m_ResultValues.insert(std::make_pair (sName, sValue));                        
                    }                    
                    if (member->value.IsInt64()) {
                        int64_t nValue = member->value.GetInt64();
                        m_ResultValues.insert(std::make_pair(sName, std::to_string (nValue)));
                    }
                }              

                if (nResponseCode == 400) {
                    std::string sErrorMessage = getResultValue("message");
                    std::string sErrorCode = getResultValue("errorcode");
                    throw ELibAMCFInterfaceException(LIBAMCF_ERROR_RESTERROR, "REST error: " + m_sIdentifier + " (#" + sErrorCode + ": " + sErrorMessage + ")");

                }

            } else
                throw ELibAMCFInterfaceException(LIBAMCF_ERROR_RESTRESPONSEISNOOBJECT, "REST response is no object: " + m_sIdentifier);

        } else
            throw ELibAMCFInterfaceException(LIBAMCF_ERROR_COULDNOTPARSERESTRESPONSE, "Could not parse REST response: " + m_sIdentifier);

    }
    else {

        if (nResponseCode < 0)
            throw ELibAMCFInterfaceException(LIBAMCF_ERROR_RESTRESPONSETIMEOUT, "REST response timeout: " + m_sIdentifier + " (" + std::to_string(nResponseCode) + ")");

        throw ELibAMCFInterfaceException(LIBAMCF_ERROR_INVALIDRESTRESPONSE, "Invalid REST response: " + m_sIdentifier + " (" + std::to_string (nResponseCode) + ")");
    }

}
