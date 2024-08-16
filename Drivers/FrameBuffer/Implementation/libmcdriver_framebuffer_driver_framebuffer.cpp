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


Abstract: This is a stub class definition of CDriver_FrameBuffer

*/

#include "libmcdriver_framebuffer_driver_framebuffer.hpp"
#include "libmcdriver_framebuffer_interfaceexception.hpp"
#include "libmcdriver_framebuffer_framebufferaccess.hpp"
#include "libmcdriver_framebuffer_framebufferdevice.hpp"

// Include custom headers here.
#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)


using namespace LibMCDriver_FrameBuffer::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_FrameBuffer 
**************************************************************************************************************************/
CDriver_FrameBuffer::CDriver_FrameBuffer(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
{

}

CDriver_FrameBuffer::~CDriver_FrameBuffer()
{

}

void CDriver_FrameBuffer::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_FrameBuffer::GetName()
{
	return m_sName;
}

std::string CDriver_FrameBuffer::GetType()
{
	return "framebuffer";
}

void CDriver_FrameBuffer::GetVersion(LibMCDriver_FrameBuffer_uint32& nMajor, LibMCDriver_FrameBuffer_uint32& nMinor, LibMCDriver_FrameBuffer_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_FRAMEBUFFER_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_FRAMEBUFFER_VERSION_MINOR;
	nMicro = LIBMCDRIVER_FRAMEBUFFER_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_FrameBuffer::QueryParameters()
{
}

void CDriver_FrameBuffer::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
}



bool CDriver_FrameBuffer::SupportsSimulation()
{
	return false;
}

bool CDriver_FrameBuffer::SupportsDevice()
{
#ifdef __linux__
	return true;
#else
	return false;
#endif
	
}

IFrameBufferAccess* CDriver_FrameBuffer::CreateFrameBufferSimulation(const std::string & sIdentifier, const LibMCDriver_FrameBuffer_uint32 nScreenWidth, const LibMCDriver_FrameBuffer_uint32 nScreenHeight, const LibMCDriver_FrameBuffer::eFrameBufferBitDepth eBitDepth)
{
	throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_NOTIMPLEMENTED);
}

IFrameBufferAccess* CDriver_FrameBuffer::OpenFrameBufferDevice(const std::string& sIdentifier, const std::string& sDeviceName, const bool bAllowSimulationFallback)
{
	checkIdentifier(sIdentifier);
	
	PFrameBufferDeviceInstance pDevice;
	
	if (SupportsDevice()) {
		pDevice = std::make_shared<CFrameBufferDeviceInstance>(sIdentifier, sDeviceName);
	}
	else {
		if (bAllowSimulationFallback)
			return CreateFrameBufferSimulation(sIdentifier, 1920, 1080, eFrameBufferBitDepth::RGB888);

		throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_DEVICENOTSUPPORTEDONPLATFORM);
	}

	m_Instances.insert(std::make_pair (sIdentifier, pDevice));

	return new CFrameBufferAccess(pDevice);
}

void CDriver_FrameBuffer::ReleaseFramebuffer(const std::string & sIdentifier)
{
	m_Instances.erase(sIdentifier);
}

IFrameBufferAccess* CDriver_FrameBuffer::FindFrameBuffer(const std::string & sIdentifier)
{
	checkIdentifier(sIdentifier);

	auto iIter = m_Instances.find(sIdentifier);
	if (iIter != m_Instances.end()) {
		return new CFrameBufferAccess(iIter->second);
	}
	else {
		throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_FRAMEBUFFERDEVICENOTFOUND, "framebuffer device not found: " + sIdentifier);
	}
	
}

bool CDriver_FrameBuffer::FrameBufferExists(const std::string & sIdentifier)
{
	auto iIter = m_Instances.find(sIdentifier);
	return (iIter != m_Instances.end());
}

void CDriver_FrameBuffer::checkIdentifier(const std::string& sIdentifier)
{
	if (sIdentifier.empty ())
		throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_EMPTYFRAMEBUFFERIDENTIFIER);

	if (sIdentifier.length() > 256)
		throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_FRAMEBUFFERIDENTIFIERISTOOLONG);

	for (auto ch : sIdentifier) {
		if (!(isalnum (ch) || (ch == '_') || (ch == '-')))
			throw ELibMCDriver_FrameBufferInterfaceException(LIBMCDRIVER_FRAMEBUFFER_ERROR_INVALIDFRAMEBUFFERIDENTIFIER, "invalid framebuffer identifier: " + sIdentifier);
	}

}
