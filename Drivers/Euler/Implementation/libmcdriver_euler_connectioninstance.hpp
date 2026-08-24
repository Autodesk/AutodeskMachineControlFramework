/*++

Copyright (C) 2025 Autodesk Inc.

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


Abstract: This is the class declaration of CEulerConnection

*/


#ifndef __LIBMCDRIVER_EULER_CONNECTIONINSTANCE
#define __LIBMCDRIVER_EULER_CONNECTIONINSTANCE

#include "libmcdriver_euler_interfaces.hpp"
#include "libmcdriver_euler_sdk.hpp"

namespace LibMCDriver_Euler {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CEulerConnectionInstance 
**************************************************************************************************************************/

class CEulerConnectionInstance  {
private:

	PLibEulerSDK m_pLibEulerSDK;

	std::string m_sIdentifier;

	std::string m_sApplicationName;

	std::string m_sApplicationVersion;

	std::string m_sBaseURL;

	std::string m_sAPIKey;

	std::string m_sDeviceID;

	libEulerHandle m_hConnectionHandle;

	sLibEulerConnectConfig m_ConnectConfig;

public:

	CEulerConnectionInstance(PLibEulerSDK pLibEulerSDK, const std::string & sIdentifier, const std::string & sApplicationName, const std::string & sApplicationVersion, const std::string & sBaseURL, const std::string &sAPIKey, const std::string & sDeviceID);

	virtual ~CEulerConnectionInstance();

	static bool checkIdentifier(const std::string & sIdentifier);

	std::string getIdentifier();

	std::string getApplicationName();

	std::string getApplicationVersion();

	std::string getBaseURL();

	std::string getAPIKey();

	std::string getDeviceID();

	void startHeartBeat();

	void stopHeartBeat();

	std::string createBuild(const std::string & sJobName, const LibMCDriver_Euler_uint32 nLayerCount);

	void uploadImage(const std::string & sBuildJobID, const LibMCDriver_Euler_uint32 nLayerIndex, const LibMCDriver_Euler::eEulerImageType eImageType, LibMCEnv::PImageData pImage);

	void setJobStatus(const std::string & sBuildJobID, const LibMCDriver_Euler::eEulerJobStatus eJobStatus);

	void closeConnection();

};

typedef std::shared_ptr<CEulerConnectionInstance> PEulerConnectionInstance;

} // namespace Impl
} // namespace LibMCDriver_Euler


#endif // __LIBMCDRIVER_EULER_CONNECTIONINSTANCE
