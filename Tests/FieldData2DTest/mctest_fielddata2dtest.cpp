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

using namespace LibMCPlugin::Impl;

#include <iostream>
#include <fstream>


/*************************************************************************************************************************
 Import functionality for Driver into current plugin
**************************************************************************************************************************/
__NODRIVERIMPORT


/*************************************************************************************************************************
 Class definition of CPLCData
**************************************************************************************************************************/
class CTestData : public virtual CPluginData {
protected:

public:

};

/*************************************************************************************************************************
 Class definition of CTestState
**************************************************************************************************************************/
typedef CState<CTestData> CTestState;


/*************************************************************************************************************************
 Class definition of CRasterizerState_Init
**************************************************************************************************************************/
class CTestState_Init : public virtual CTestState {
public:

	CTestState_Init(const std::string& sStateName, PPluginData pPluginData)
		: CTestState(getStateName(), sStateName, pPluginData)
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

		auto pFieldData = pStateEnvironment->CreateDiscreteField2D(200, 300, 100, 100, 10.0, 10.0, 2.0);

		pFieldData->SetPixel(5, 8, 10.0);

		double dValue1 = pFieldData->GetPixel(5, 5);
		pStateEnvironment->LogMessage("Testing if value at (5/5) is 2.0: " + std::to_string (dValue1));
		if (dValue1 != 2.0)
			throw std::runtime_error("invalid pixel value");

		double dValue2 = pFieldData->GetPixel(5, 8);
		pStateEnvironment->LogMessage("Testing if value at (5/8) is 10.0: " + std::to_string(dValue2));
		if (dValue2 != 10.0)
			throw std::runtime_error("invalid pixel value");

		LibMCEnv::sColorRGB minColor = { 0.0, 0.0, 1.0 };
		LibMCEnv::sColorRGB midColor = { 0.0, 1.0, 0.0 };
		LibMCEnv::sColorRGB maxColor = { 1.0, 0.0, 0.0 };

		auto pImage = pFieldData->RenderToImageRaw(0.0, minColor, 8.0, midColor, 16.0, maxColor);
		pImage->EncodePNG();

		std::vector<uint8_t> pngData;
		pImage->GetEncodedPNGData(pngData);

		std::ofstream pngStream("output.png", std::ios::binary | std::ios::out);
		if (pngData.size() > 0)		
			pngStream.write((const char*)pngData.data(), pngData.size());
		pngStream.close();


		pStateEnvironment->SetNextState("success");
	}

};



/*************************************************************************************************************************
 Class definition of CTestState_Success
**************************************************************************************************************************/
class CTestState_Success : public virtual CTestState {
public:

	CTestState_Success(const std::string& sStateName, PPluginData pPluginData)
		: CTestState(getStateName(), sStateName, pPluginData)
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
 Class definition of CTestState_FatalError
**************************************************************************************************************************/
class CTestState_FatalError : public virtual CTestState {
public:

	CTestState_FatalError(const std::string& sStateName, PPluginData pPluginData)
		: CTestState(getStateName(), sStateName, pPluginData)
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
	m_pPluginData = std::make_shared<CTestData>();
}

IState* CStateFactory::CreateState(const std::string& sStateName)
{

	IState* pStateInstance = nullptr;

	if (createStateInstanceByName<CTestState_Init>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;


	if (createStateInstanceByName<CTestState_Success>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	if (createStateInstanceByName<CTestState_FatalError>(sStateName, pStateInstance, m_pPluginData))
		return pStateInstance;

	throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

}


