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


#ifndef __LIBAMCF_RESTHANDLER
#define __LIBAMCF_RESTHANDLER

#include <cstdint>
#include <string>
#include <map>

class CRestHandler {
protected:
    std::string m_sIdentifier;
    std::string m_sURL;
    std::string m_sAuthToken;
    uint32_t m_nTimeout;
    uint32_t m_nRetryCount;

    std::map<std::string, std::string> m_ResultValues;

public:

    CRestHandler(const std::string& sIdentifier, const std::string & sURL, const std::string & sAuthToken, uint32_t nTimeOut, uint32_t nRetryCount);

    std::string getResultValue(const std::string& sKey);
    bool hasResultValue(const std::string& sKey);


};


class CRestHandler_Post : public CRestHandler {
protected:
    std::map<std::string, std::string> m_PostValues;

public:
    CRestHandler_Post(const std::string& sIdentifier, const std::string& sURL, const std::string& sAuthToken, uint32_t nTimeOut, uint32_t nRetryCount);

    void addValue(const std::string & sKey, const std::string & sValue);

    void sendRequest ();

};


#endif // __LIBAMCF_RESTHANDLER