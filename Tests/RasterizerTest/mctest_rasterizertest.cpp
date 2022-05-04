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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
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

#include "libmcplugin_impl.hpp"
#include "libmcdriver_rasterizer_dynamic.hpp"

using namespace LibMCPlugin::Impl;

#include <iostream>
#include <fstream>


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
LIBMC_IMPORTDRIVERCLASSES(Rasterizer, Rasterizer)


/*************************************************************************************************************************
 Class definition of CPLCData
**************************************************************************************************************************/
class CRasterizerData : public virtual CPluginData {
protected:
	// We need to globally store driver wrappers in the plugin
	PDriverCast_Rasterizer m_DriverCast_Rasterizer;

public:

	PDriver_Rasterizer acquireRasterizer (LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		return m_DriverCast_Rasterizer.acquireDriver(pStateEnvironment, "rasterizer");
	}

};

/*************************************************************************************************************************
 Class definition of CRasterizerState
**************************************************************************************************************************/
typedef CState<CRasterizerData> CRasterizerState;


/*************************************************************************************************************************
 Class definition of CRasterizerState_Init
**************************************************************************************************************************/
class CRasterizerState_Init : public virtual CRasterizerState {
public:

	CRasterizerState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CRasterizerState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "init";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		auto pDriver = m_pPluginData->acquireRasterizer(pStateEnvironment);
		auto pLayer = pDriver->CreateEmptyLayer();

		std::vector<LibMCDriver_Rasterizer::sPosition2D> PointsBuffer;
		PointsBuffer.push_back({ 324.0, 248.0});
		PointsBuffer.push_back({ 860.0, 290.0 });
		PointsBuffer.push_back({ 1038.0, 703.0 });
		//PointsBuffer.push_back({ 235.0, 684.0 });


		pLayer->AddEntity(PointsBuffer, LibMCDriver_Rasterizer::eGeometryType::SolidGeometry);

		auto pRasterizer = pDriver->RegisterInstance("rasterizer", 1920, 1080, 20, 20);
		pRasterizer->SetSamplingParameters(128, 4);
		pRasterizer->SetSubsampling(1, 1);
		pRasterizer->AddLayer(pLayer);

		auto pImage = pStateEnvironment->CreateEmptyImage(1920, 1080, 20, 20, LibMCEnv::eImagePixelFormat::GreyScale8bit);

		pRasterizer->CalculateImage(pImage, true);

		pImage->EncodePNG();
		std::vector<uint8_t> pngData;
		pImage->GetEncodedPNGData (pngData);

		std::ofstream pngStream("output.png", std::ios::binary | std::ios::out);
		if (pngData.size() > 0)
			pngStream.write((const char*)pngData.data(), pngData.size());
		pngStream.close();

		pStateEnvironment->SetNextState("success");
	}

};



/*************************************************************************************************************************
 Class definition of CPLCState_Success
**************************************************************************************************************************/
class CRasterizerState_Success : public virtual CRasterizerState {
public:

	CRasterizerState_Success(const std::string& sStateName, PPluginData pPluginData)
		: CRasterizerState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "success";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{

		pStateEnvironment->SetNextState("success");

	}

};


/*************************************************************************************************************************
 Class definition of CMainState_FatalError
**************************************************************************************************************************/
class CRasterizerState_FatalError : public virtual CRasterizerState {
public:

	CRasterizerState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CRasterizerState(getStateName(), sStateName, pPluginData)
	{
	}

	static const std::string getStateName()
	{
		return "fatalerror";
	}


	void Execute(LibMCEnv::PStateEnvironment pStateEnvironment)
	{
		if (pStateEnvironment.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		pStateEnvironment->SetNextState("fatalerror");
	}

};



/*************************************************************************************************************************
 Class definition of CStateFactory
**************************************************************************************************************************/

CStateFactory::CStateFactory(const std::string& sInstanceName)
{
	m_pPluginData = std::make_shared<CRasterizerData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CRasterizerState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;


	if (createStateInstanceByName<CRasterizerState_Success>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CRasterizerState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


