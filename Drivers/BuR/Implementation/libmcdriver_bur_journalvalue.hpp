/*++

Copyright (C) 2024 Autodesk Inc.

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


Abstract: This is the class declaration of CPLCCommand

*/


#ifndef __LIBMCDRIVER_BUR_JOURNALVALUE
#define __LIBMCDRIVER_BUR_JOURNALVALUE


#include <memory>
#include <string>
#include <vector>
#include <map>


namespace LibMCDriver_BuR {
namespace Impl {

class CDriver_BuRJournalValue {
private:
    uint32_t m_nValueID;
    std::string m_sValueName;
    std::string m_sValueType;
    uint32_t m_nValueSize;
    std::string m_sDriverParameterName;

public:

    CDriver_BuRJournalValue(uint32_t nValueID, const std::string& sValueName, const std::string& sValueType, uint32_t nValueSize);

    virtual ~CDriver_BuRJournalValue();

    uint32_t getValueID ();

    std::string getValueName ();

    std::string getValueType ();

    uint32_t getValueSize ();

    std::string getDriverParameterName();

    void setDriverParameterName(const std::string & sDriverParameterName);

    static bool nameIsValidJournalName(const std::string & sName);

};

typedef std::shared_ptr<CDriver_BuRJournalValue> PDriver_BuRJournalValue;


} // namespace Impl
} // namespace LibMCDriver_BuR

#endif // __LIBMCDRIVER_BUR_JOURNALVALUE
