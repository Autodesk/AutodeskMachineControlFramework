/*++

Copyright (C) 2023 Autodesk Inc.

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


Abstract: This is the class declaration of CDriver_CifX

*/


#ifndef __LIBMCDRIVER_CIFX_DRIVER_CIFX
#define __LIBMCDRIVER_CIFX_DRIVER_CIFX

#include "libmcdriver_cifx_interfaces.hpp"

// Parent classes
#include "libmcdriver_cifx_driver.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdriver_cifx_sdk.hpp"
#include <map>
#include <pugixml.hpp>
#include <mutex>
namespace LibMCDriver_CifX {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CDriver_CifX 
**************************************************************************************************************************/

	enum class eDriver_CifXParameterType : int32_t {
		CifXParameter_Unknown = 0,
		CifXParameter_BOOL = 1,
		CifXParameter_UINT8 = 2,
		CifXParameter_UINT16 = 3,
		CifXParameter_UINT32 = 4,
		CifXParameter_INT8 = 5,
		CifXParameter_INT16 = 6,
		CifXParameter_INT32 = 7,
		CifXParameter_FLOAT = 8,
		CifXParameter_DOUBLE = 9,
	};

	class CDriver_CifXParameter {
	private:
		std::string m_sName;
		std::string m_sDescription;
		eDriver_CifXParameterType m_eType;
	public:
		CDriver_CifXParameter(const std::string& sName, const std::string& sDescription, const std::string& sADSName);
		virtual ~CDriver_CifXParameter();

		std::string getName();
		std::string getDescription();
		eDriver_CifXParameterType getType();

	};

	typedef std::shared_ptr<CDriver_CifXParameter> PDriver_CifXParameter;


	class CDriver_CifXChannel {
	private:
		std::string m_sBoardName;
		uint32_t m_nChannelIndex;
		uint32_t m_nInputSize;
		uint32_t m_nOutputSize;

		std::vector<PDriver_CifXParameter> m_Inputs;
		std::map<std::string, PDriver_CifXParameter> m_InputMap;
		std::vector<PDriver_CifXParameter> m_Outputs;
		std::map<std::string, PDriver_CifXParameter> m_OutputMap;

	public:

		CDriver_CifXChannel(pugi::xml_node& xmlNode);
		virtual ~CDriver_CifXChannel ();

	};


class CDriver_CifX : public virtual IDriver_CifX, public virtual CDriver {
private:
	std::string m_sName;

	bool m_bIsSimulationMode;

	std::mutex m_driverEnvironmentMutex;
	LibMCEnv::PDriverEnvironment m_pDriverEnvironment;

	std::string m_sCustomSDKResource;

	PCifXSDK m_pCifXSDK;


public:

	CDriver_CifX(const std::string& sName, LibMCEnv::PDriverEnvironment pDriverEnvironment);

	virtual ~CDriver_CifX();

	void Configure(const std::string& sConfigurationString) override;

	std::string GetName() override;

	std::string GetType() override;

	void GetVersion(LibMCDriver_CifX_uint32& nMajor, LibMCDriver_CifX_uint32& nMinor, LibMCDriver_CifX_uint32& nMicro, std::string& sBuild) override;

	void GetHeaderInformation(std::string& sNameSpace, std::string& sBaseName) override;

	void QueryParameters() override;


	void SetToSimulationMode() override;

	bool IsSimulationMode() override;

	void SetCustomSDKResource(const std::string & sResourceName) override;

	LibMCDriver_CifX_uint32 EnumerateBoards() override;

	IBoardInformation * GetBoardInformation(const LibMCDriver_CifX_uint32 nBoardIndex) override;

	void Connect() override;

	void Disconnect() override;

	bool IsConnected() override;

	bool ValueExists(const std::string& sName) override;

	void WriteIntegerValue(const std::string& sName, const LibMCDriver_CifX_int64 nValue, const LibMCDriver_CifX_uint32 nTimeOutInMs) override;

	void WriteBoolValue(const std::string& sName, const bool bValue, const LibMCDriver_CifX_uint32 nTimeOutInMs) override;

	void WriteDoubleValue(const std::string& sName, const LibMCDriver_CifX_double dValue, const LibMCDriver_CifX_uint32 nTimeOutInMs) override;

	LibMCDriver_CifX_int64 ReadIntegerValue(const std::string& sName) override;

	bool ReadBoolValue(const std::string& sName) override;

	LibMCDriver_CifX_double ReadDoubleValue(const std::string& sName) override;

};

} // namespace Impl
} // namespace LibMCDriver_CifX

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCDRIVER_CIFX_DRIVER_CIFX
