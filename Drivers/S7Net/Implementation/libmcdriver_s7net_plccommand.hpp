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


Abstract: This is the class declaration of CPLCCommand

*/


#ifndef __LIBMCDRIVER_S7NET_PLCCOMMAND
#define __LIBMCDRIVER_S7NET_PLCCOMMAND

#include "libmcdriver_s7net_interfaces.hpp"

// Parent classes
#include "libmcdriver_s7net_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include <map>

namespace LibMCDriver_S7Net {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CPLCCommand 
**************************************************************************************************************************/

enum class ePLCFieldType : int32_t {
    ftUnknown = 0,
    ftString = 1,
    ftInt = 2,
    ftDInt = 3,
    ftBool = 4,
    ftReal = 5,
};


class CPLCCommand : public virtual IPLCCommand, public virtual CBase {
private:


protected:
    std::string m_sName;
    uint32_t m_nSequenceID;

    std::map<std::string, ePLCFieldType> m_ParameterTypeMap;
    std::map<std::string, std::string> m_ParameterValues; // Store values as strings

public:

    CPLCCommand(const std::string& sName);

    std::string getName();
    uint32_t getSequenceID ();
    void setSequenceID(const uint32_t nSequenceID);

    void addParameterDefinition (const std::string & sName, const ePLCFieldType fieldType);
    
    void SetIntegerParameter(const std::string& sParameterName, const LibMCDriver_S7Net_int32 nValue) override;    

    void SetStringParameter(const std::string& sParameterName, const std::string& sValue) override;

    void SetBoolParameter(const std::string& sParameterName, const bool bValue) override;

    void SetDoubleParameter(const std::string& sParameterName, const LibMCDriver_S7Net_double dValue) override;

    std::string getParameterValue (const std::string& sParameterName);

};

} // namespace Impl
} // namespace LibMCDriver_S7Net

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_S7NET_PLCCOMMAND
