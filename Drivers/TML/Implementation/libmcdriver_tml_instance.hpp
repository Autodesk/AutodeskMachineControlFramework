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


Abstract: This is the class declaration of CChannel

*/


#ifndef __LIBMCDRIVER_TML_INSTANCE
#define __LIBMCDRIVER_TML_INSTANCE

#include "libmcdriver_tml_base.hpp"
#include "libmcdriver_tml_sdk.hpp"

#include <map>

namespace LibMCDriver_TML {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CChannel 
**************************************************************************************************************************/

class CTMLAxisInstance {
private:

    std::string m_sAxisIdentifier;
    std::string m_sChannelIdentifier;
    uint8_t m_nHardwareID;

public:

    CTMLAxisInstance(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier, uint8_t nHardwareID);

    virtual ~CTMLAxisInstance();

    std::string getAxisIdentifier ();

    std::string getChannelIdentifier ();

    uint8_t getHardwareID ();

};

class CTMLInstance {
private:
    PTMLSDK m_pTMLSDK;

    LibMCEnv::PWorkingDirectory m_pWorkingDirectory;

    std::map<std::string, int32_t> m_ChannelFileDescriptorMap;

    std::map<std::string, CTMLAxisInstance> m_AxisMap;

    void ensureAxisExistsInChannel(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier);

public:

    CTMLInstance(PTMLSDK pTMLSDK, LibMCEnv::PWorkingDirectory pWorkingDirectory);

    virtual ~CTMLInstance();

    void openChannel(const std::string& sIdentifier, const std::string& sDeviceName, const LibMCDriver_TML::eChannelType eChannelTypeToUse, const LibMCDriver_TML::eProtocolType eProtocolTypeToUse, const LibMCDriver_TML_uint32 nHostID, const LibMCDriver_TML_uint32 nBaudrate);

    void closeChannel(const std::string& sIdentifier);

    bool channelExists(const std::string& sIdentifier);

    static bool checkIdentifierString(const std::string& sIdentifier);

    bool axisExistsInChannel(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier);

    bool axisExists(const std::string& sAxisIdentifier);

    void setupAxis(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier, uint32_t nAxisID, size_t nConfigurationBufferSize, const uint8_t* pConfigurationBuffer);

    void selectAxisInternal(const std::string& sAxisIdentifier);

    void setAxisPower(const std::string& sChannelIdentifier, const std::string& sAxisIdentifier, bool bEnable);

};

typedef std::shared_ptr<CTMLInstance> PTMLInstance;

} // namespace Impl
} // namespace __LIBMCDRIVER_TML_INSTANCE


#endif // __LIBMCDRIVER_TML_INSTANCE
