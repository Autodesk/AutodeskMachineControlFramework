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


Abstract: This is the class declaration of CDriver_S7Net

*/


#ifndef __LIBMCDRIVER_S7NET_DRIVER_S7NET
#define __LIBMCDRIVER_S7NET_DRIVER_S7NET

#include "libmcdriver_s7net_interfaces.hpp"

// Parent classes
#include "libmcdriver_s7net_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libs7com_dynamic.hpp"
#include "libs7net_dynamic.hpp"

namespace LibMCDriver_S7Net {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_S7Net 
**************************************************************************************************************************/

class CDriver_S7Net : public virtual IDriver_S7Net, public virtual CDriver {
private:

protected:

    LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
    LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

    std::string m_sDriverType;
    std::string m_sProtocolXML;

    LibS7Net::PWrapper m_pPLCWrapper;
    LibS7Net::PPLC m_pPLC;
    LibS7Com::PWrapper m_pCommunicationWrapper;
    LibS7Com::PPLCCommunication m_pCommunication;

public:

    CDriver_S7Net(const std::string& sName, const std::string& sType, LibMCEnv::PDriverEnvironment pDriverEnvironment);
    virtual ~CDriver_S7Net();

    void Initialise() override;

    void Connect(const std::string& sIPAddress, const LibMCDriver_S7Net_uint32 nRack, const LibMCDriver_S7Net_uint32 nSlot) override;

	void Disconnect() override;

	void QueryParameters() override;

};

} // namespace Impl
} // namespace LibMCDriver_S7Net

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_S7NET_DRIVER_S7NET
