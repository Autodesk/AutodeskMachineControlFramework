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


Abstract: This is the class declaration of CDriver_FrameBuffer

*/


#ifndef __LIBMCDRIVER_FRAMEBUFFER_DRIVER_FRAMEBUFFER
#define __LIBMCDRIVER_FRAMEBUFFER_DRIVER_FRAMEBUFFER

#include "libmcdriver_framebuffer_interfaces.hpp"

// Parent classes
#include "libmcdriver_framebuffer_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_framebuffer_framebufferinstance.hpp"
#include <map>

namespace LibMCDriver_FrameBuffer {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_FrameBuffer 
**************************************************************************************************************************/

class CDriver_FrameBuffer : public virtual IDriver_FrameBuffer, public virtual CDriver {
private:

	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;
	std::string m_sName;

	std::map<std::string, PFrameBufferInstance> m_Instances;

public:

	CDriver_FrameBuffer(const std::string & sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	~CDriver_FrameBuffer();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_FrameBuffer_uint32& nMajor, LibMCDriver_FrameBuffer_uint32& nMinor, LibMCDriver_FrameBuffer_uint32& nMicro, std::string& sBuild) override;

	void QueryParameters() override;

	void QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance) override;

	bool SupportsSimulation() override;

	bool SupportsDevice() override;

	IFrameBufferAccess* CreateFrameBufferSimulation(const std::string & sIdentifier, const LibMCDriver_FrameBuffer_uint32 nScreenWidth, const LibMCDriver_FrameBuffer_uint32 nScreenHeight, const LibMCDriver_FrameBuffer::eFrameBufferBitDepth eBitDepth) override;

	IFrameBufferAccess* OpenFrameBufferDevice(const std::string & sIdentifier, const std::string & sDeviceName) override;

	void ReleaseFramebuffer(const std::string & sIdentifier) override;

	IFrameBufferAccess* FindFrameBuffer(const std::string & sIdentifier) override;

	bool FrameBufferExists(const std::string & sIdentifier) override;

};

} // namespace Impl
} // namespace LibMCDriver_Framebuffer

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_FRAMEBUFFER_DRIVER_FRAMEBUFFER
