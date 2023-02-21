/*++

Copyright (C) 2022 Autodesk Inc.

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


Abstract: This is a stub class definition of CDriver_Rasterizer

*/

#include "libmcdriver_rasterizer_driver_rasterizer.hpp"
#include "libmcdriver_rasterizer_interfaceexception.hpp"
#include "libmcdriver_rasterizer_slicestack.hpp"
#include "libmcdriver_rasterizer_layerobject.hpp"

#define __STRINGIZE(x) #x
#define __STRINGIZE_VALUE_OF(x) __STRINGIZE(x)

using namespace LibMCDriver_Rasterizer::Impl;

/*************************************************************************************************************************
 Class definition of CDriver_Rasterizer 
**************************************************************************************************************************/

CDriver_Rasterizer::CDriver_Rasterizer(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment)
	: m_sName(sName), m_pDriverEnvironment(pDriverEnvironment)
{

}

CDriver_Rasterizer::~CDriver_Rasterizer()
{

}

void CDriver_Rasterizer::Configure(const std::string& sConfigurationString)
{
}

std::string CDriver_Rasterizer::GetName()
{
	return m_sName;
}

std::string CDriver_Rasterizer::GetType()
{
	return "rasterizer";
}

void CDriver_Rasterizer::GetVersion(LibMCDriver_Rasterizer_uint32& nMajor, LibMCDriver_Rasterizer_uint32& nMinor, LibMCDriver_Rasterizer_uint32& nMicro, std::string& sBuild)
{
	nMajor = LIBMCDRIVER_RASTERIZER_VERSION_MAJOR;
	nMinor = LIBMCDRIVER_RASTERIZER_VERSION_MINOR;
	nMicro = LIBMCDRIVER_RASTERIZER_VERSION_MICRO;
	sBuild = __STRINGIZE_VALUE_OF(__GITHASH);
}

void CDriver_Rasterizer::QueryParameters()
{
	QueryParametersEx(m_pDriverEnvironment->CreateStatusUpdateSession());
}

void CDriver_Rasterizer::QueryParametersEx(LibMCEnv::PDriverStatusUpdateSession pDriverUpdateInstance)
{
	if (pDriverUpdateInstance.get() == nullptr)
		return;

}


ISliceStack * CDriver_Rasterizer::CreateSliceStack(const LibMCDriver_Rasterizer_uint32 nLayerCount, const LibMCDriver_Rasterizer_double dLayerThickness, const LibMCDriver_Rasterizer_double dBottomZ)
{
	return new CSliceStack(nLayerCount, dLayerThickness, dBottomZ);
}

ILayerObject * CDriver_Rasterizer::CreateEmptyLayer()
{
	return new CLayerObject(std::make_shared<CLayerDataObject>());
}

IRasterizer * CDriver_Rasterizer::RegisterInstance(const std::string & sIdentifier, const LibMCDriver_Rasterizer_uint32 nPixelSizeX, const LibMCDriver_Rasterizer_uint32 nPixelSizeY, const LibMCDriver_Rasterizer_double dDPIX, const LibMCDriver_Rasterizer_double dDPIY)
{
	if (sIdentifier.empty())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDIDENTIFIER);

	if (m_Instances.find (sIdentifier) != m_Instances.end ())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_IDENTIFIERALREADYREGISTERED, "identifier already registered: " + sIdentifier);

	auto pRasterizerInstance = std::make_shared<CRasterizerInstance>(nPixelSizeX, nPixelSizeY, dDPIX, dDPIY);
	m_Instances.insert(std::make_pair(sIdentifier, pRasterizerInstance));

	return new CRasterizer(pRasterizerInstance);
}

void CDriver_Rasterizer::UnregisterInstance(const std::string & sIdentifier)
{
	if (sIdentifier.empty())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDIDENTIFIER);

	if (m_Instances.find(sIdentifier) == m_Instances.end())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_IDENTIFIERNOTREGISTERED, "identifier not registered: " + sIdentifier);

	m_Instances.erase(sIdentifier);
}

IRasterizer * CDriver_Rasterizer::GetInstance(const std::string & sIdentifier)
{
	if (sIdentifier.empty())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDIDENTIFIER);

	auto iIterator = m_Instances.find(sIdentifier);
	if (iIterator == m_Instances.end())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_IDENTIFIERNOTREGISTERED, "identifier not registered: " + sIdentifier);

	return new CRasterizer(iIterator->second);
}

bool CDriver_Rasterizer::HasInstance(const std::string& sIdentifier)
{
	if (sIdentifier.empty())
		throw ELibMCDriver_RasterizerInterfaceException(LIBMCDRIVER_RASTERIZER_ERROR_INVALIDIDENTIFIER);

	auto iIterator = m_Instances.find(sIdentifier);
	return (iIterator != m_Instances.end());
}
