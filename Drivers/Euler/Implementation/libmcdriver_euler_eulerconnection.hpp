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


Abstract: This is the class declaration of CEulerConnection

*/


#ifndef __LIBMCDRIVER_EULER_EULERCONNECTION
#define __LIBMCDRIVER_EULER_EULERCONNECTION

#include "libmcdriver_euler_interfaces.hpp"
#include "libmcdriver_euler_connectioninstance.hpp"

// Parent classes
#include "libmcdriver_euler_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCDriver_Euler {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CEulerConnection 
**************************************************************************************************************************/

class CEulerConnection : public virtual IEulerConnection, public virtual CBase {
private:

	PEulerConnectionInstance m_pConnectionInstance;

public:


	CEulerConnection(PEulerConnectionInstance pConnectionInstance);

	virtual ~CEulerConnection();

	void Disconnect() override;

	std::string GetIdentifier() override;

	std::string GetApplicationName() override;

	std::string GetApplicationVersion() override;

	std::string GetBaseURL() override;

	std::string GetAPIKey() override;

	std::string GetDeviceID() override;

	void StartHeartBeat() override;

	void StopHeartBeat() override;

	std::string CreateBuild(const std::string & sJobName, const LibMCDriver_Euler_uint32 nLayerCount) override;

	void UploadImage(const std::string& sBuildJobID, const LibMCDriver_Euler_uint32 nLayerIndex, const LibMCDriver_Euler::eEulerImageType eImageType, LibMCEnv::PImageData pImage) override;

	void SetJobStatus(const std::string& sBuildJobID, const LibMCDriver_Euler::eEulerJobStatus eJobStatus) override;

};

} // namespace Impl
} // namespace LibMCDriver_Euler

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_EULER_EULERCONNECTION
