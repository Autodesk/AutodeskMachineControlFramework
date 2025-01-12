
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <list>
#include <stdexcept>
#include <algorithm>
#include <string>

#include "include/rlTypes.h"
#include "include/SystemAPI.h"
#include "include/LaserAPI.h"
#include "include/ScannerAPI.h"
#include "include/GpioAPI.h"
#include "include/SfioAPI.h"

#define MAXMEMBERADDRESS (1024 * 1024)

enum class eSchemaVariableType : uint32_t
{
	evtUnknown = 0,
	evtUint8 = 1,
	evtUint16 = 2,
	evtUint32 = 3,
	evtUint64 = 4,
	evtInt8 = 5,
	evtInt16 = 6,
	evtInt32 = 7,
	evtInt64 = 8,
	evtFloat = 9,
	evtDouble = 10,
	evtCharArray = 11,
	evtBool = 12,
	evtEnum = 13
};

class CSchemaDefinition
{
private:
	std::string m_sSchemaName;
	int32_t m_nVersion;
	uint32_t m_nSize;
	void* m_pBasePtr;

	std::list<std::string> m_VariableNames;
	std::map<std::string, std::pair<eSchemaVariableType, uint32_t>> m_Variables;

	std::map<uint32_t, int32_t> m_VersionTags;

public:

	CSchemaDefinition(const std::string& sSchemaName, int32_t nVersion, void* pBasePtr, uint32_t nSize)
		: m_sSchemaName (sSchemaName), m_nVersion (nVersion), m_pBasePtr (pBasePtr), m_nSize (nSize)
	{
		m_VersionTags.insert(std::make_pair (0, m_nVersion));
	}

	virtual ~CSchemaDefinition()
	{

	}

	int32_t getVersion()
	{
		return m_nVersion;
	}

	void registerVariable(const std::string& sVariableName, eSchemaVariableType variableType, void* pMemberPtr)
	{
		uintptr_t nBaseAddress = (uintptr_t)m_pBasePtr;
		uintptr_t nMemberAddress = (uintptr_t)pMemberPtr;

		if (nBaseAddress > nMemberAddress)
			throw std::runtime_error("negative member address");

		size_t nAddress = nMemberAddress - nBaseAddress;
		if (nAddress > MAXMEMBERADDRESS)
			throw std::runtime_error("member address too large");

		m_Variables.insert(std::make_pair (sVariableName, std::make_pair (variableType, (uint32_t) nAddress)));
		m_VariableNames.push_back(sVariableName);
	}

	void registerVersionInfo (int32_t* pVersionMemberPtr)
	{
		uintptr_t nBaseAddress = (uintptr_t)m_pBasePtr;
		uintptr_t nMemberAddress = (uintptr_t)pVersionMemberPtr;

		if (nBaseAddress > nMemberAddress)
			throw std::runtime_error("negative member address");

		size_t nAddress = nMemberAddress - nBaseAddress;
		if (nAddress > MAXMEMBERADDRESS)
			throw std::runtime_error("member address too large");

		m_VersionTags.insert(std::make_pair ((uint32_t)nAddress, *pVersionMemberPtr));
	}


	void registerBool(const std::string& sVariableName, bool* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtBool, (void*)pMemberPtr);
	}

	void registerEnum(const std::string& sVariableName, uint32_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtEnum, (void*)pMemberPtr);
	}

	void registerUint8(const std::string& sVariableName, uint8_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtUint8, (void*)pMemberPtr);
	}

	void registerUint16(const std::string& sVariableName, uint16_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtUint16, (void*)pMemberPtr);
	}

	void registerUint32 (const std::string& sVariableName, uint32_t * pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtUint32, (void*)pMemberPtr);
	}
		
	void registerUint64(const std::string& sVariableName, uint64_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtUint64, (void*)pMemberPtr);
	}

	void registerInt8(const std::string& sVariableName, int8_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtInt8, (void*)pMemberPtr);
	}

	void registerInt16(const std::string& sVariableName, int16_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtInt16, (void*)pMemberPtr);
	}

	void registerInt32(const std::string& sVariableName, int32_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtInt32, (void*)pMemberPtr);
	}

	void registerInt64(const std::string& sVariableName, int64_t* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtInt64, (void*)pMemberPtr);
	}

	void registerDouble(const std::string& sVariableName, double* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtDouble, (void*)pMemberPtr);
	}

	void registerFloat(const std::string& sVariableName, double* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtFloat, (void*)pMemberPtr);
	}

	void registerCharArray(const std::string& sVariableName, char* pMemberPtr)
	{
		registerVariable(sVariableName, eSchemaVariableType::evtCharArray, (void*)pMemberPtr);
	}

	static std::string variableTypeToString(eSchemaVariableType variableType)
	{
		switch (variableType) {
			case eSchemaVariableType::evtBool: return "bool";
			case eSchemaVariableType::evtCharArray: return "chararray";
			case eSchemaVariableType::evtDouble: return "double";
			case eSchemaVariableType::evtFloat: return "float";
			case eSchemaVariableType::evtEnum: return "enum";
			case eSchemaVariableType::evtUint8: return "uint8";
			case eSchemaVariableType::evtUint16: return "uint16";
			case eSchemaVariableType::evtUint32: return "uint32";
			case eSchemaVariableType::evtUint64: return "uint64";
			case eSchemaVariableType::evtInt8: return "int8";
			case eSchemaVariableType::evtInt16: return "int16";
			case eSchemaVariableType::evtInt32: return "int32";
			case eSchemaVariableType::evtInt64: return "int64";

		default:
			throw std::runtime_error("invalid variable type: " + std::to_string ((uint32_t) variableType));
		}
	}

	static std::string variableTypeToCppEnum(eSchemaVariableType variableType)
	{
		switch (variableType) {
		case eSchemaVariableType::evtBool: return "evtBool";
		case eSchemaVariableType::evtCharArray: return "evtCharArray";
		case eSchemaVariableType::evtDouble: return "evtDouble";
		case eSchemaVariableType::evtFloat: return "evtFloat";
		case eSchemaVariableType::evtEnum: return "evtEnum";
		case eSchemaVariableType::evtUint8: return "evtUint8";
		case eSchemaVariableType::evtUint16: return "evtUint16";
		case eSchemaVariableType::evtUint32: return "evtUint32";
		case eSchemaVariableType::evtUint64: return "evtUint64";
		case eSchemaVariableType::evtInt8: return "evtInt8";
		case eSchemaVariableType::evtInt16: return "evtInt16";
		case eSchemaVariableType::evtInt32: return "evtInt32";
		case eSchemaVariableType::evtInt64: return "evtInt64";

		default:
			throw std::runtime_error("invalid variable type: " + std::to_string((uint32_t)variableType));
		}
	}

	void writeToJSON(std::iostream& sStream)
	{
		sStream << "{" << std::endl;
		sStream << "    \"name\": \"" << m_sSchemaName << "\"," << std::endl;
		sStream << "    \"version\": " << m_nVersion << "," << std::endl;
		sStream << "    \"size\": \"" << m_nSize << "\"," << std::endl;
		sStream << "    \"variables\": [" << std::endl;

		for (auto variableName : m_VariableNames) {
			auto variableIter = m_Variables.find(variableName);
			if (variableIter == m_Variables.end())
				throw std::runtime_error("variable not found: " + variableName);

			sStream << "        {" << std::endl;
			sStream << "            \"name\": \"" << variableIter->first << "\"," << std::endl;
			sStream << "            \"offset\": \"" << variableIter->second.second << "\"," << std::endl;
			sStream << "            \"type\": \"" << variableTypeToString (variableIter->second.first) << "\"" << std::endl;
			sStream << "        }, " << std::endl;

		}
		sStream << "    ]" << std::endl;
		sStream << "}" << std::endl;

	}

	void generateCppClass(std::iostream& sStream)
	{

		std::string sClassName = "CRaylaseAPIField_" + m_sSchemaName + "_v" + std::to_string (m_nVersion);

		sStream << "class " << sClassName << " : public CRaylaseAPIField {" << std::endl;
		sStream << "  public:" << std::endl;
		sStream << "    " << sClassName << " ()" << std::endl;
		sStream << "      : CRaylaseAPIField (\"" << m_sSchemaName << "\", "<< m_nVersion << ", " << m_nSize << ")" << std::endl;
		sStream << "    {" << std::endl;
		for (auto variableName : m_VariableNames) {
			auto variableIter = m_Variables.find(variableName);
			if (variableIter == m_Variables.end())
				throw std::runtime_error("variable not found: " + variableName);

			sStream << "      registerVariable (\"" << variableIter->first << "\", " << variableIter->second.second << ", "<< variableTypeToCppEnum (variableIter->second.first) << ");" << std::endl;

		}
		sStream << "    }" << std::endl;
		sStream << "    " << std::endl;
		sStream << "    virtual ~" << sClassName << " ()" << std::endl;
		sStream << "    {" << std::endl;
		sStream << "    }" << std::endl;
		sStream << "};" << std::endl;
		sStream << "" << std::endl;
	}

	void generateHppHeaderFile(std::string & sFileName)
	{
		std::string sHeaderSpaceName = m_sSchemaName + "_" + std::to_string (m_nVersion);
		std::transform(sHeaderSpaceName.begin(), sHeaderSpaceName.end(), sHeaderSpaceName.begin(), ::toupper);

		std::fstream fStream(sFileName, std::ios::out);

		fStream << "/*++" << std::endl;
		fStream << "" << std::endl;
		fStream << "Copyright (C) 2024 Autodesk Inc." << std::endl;
		fStream << "" << std::endl;
		fStream << "All rights reserved." << std::endl;
		fStream << "" << std::endl;
		fStream << "Redistribution and use in source and binary forms, with or without" << std::endl;
		fStream << "modification, are permitted provided that the following conditions are met:" << std::endl;
		fStream << "* Redistributions of source code must retain the above copyright" << std::endl;
		fStream << "  notice, this list of conditions and the following disclaimer." << std::endl;
		fStream << "* Redistributions in binary form must reproduce the above copyright" << std::endl;
		fStream << "  notice, this list of conditions and the following disclaimer in the" << std::endl;
		fStream << "  documentation and /or other materials provided with the distribution." << std::endl;
		fStream << " * Neither the name of the Autodesk Inc. nor the" << std::endl;
		fStream << "  names of its contributors may be used to endorse or promote products" << std::endl;
		fStream << "  derived from this software without specific prior written permission." << std::endl;
		fStream << "" << std::endl;
		fStream << "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND" << std::endl;
		fStream << "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED" << std::endl;
		fStream << "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE" << std::endl;
		fStream << "DISCLAIMED.IN NO EVENT SHALL AUTODESK INC.BE LIABLE FOR ANY" << std::endl;
		fStream << "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES" << std::endl;
		fStream << "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;" << std::endl;
		fStream << "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND" << std::endl;
		fStream << "ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT" << std::endl;
		fStream << "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS" << std::endl;
		fStream << "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE." << std::endl;
		fStream << "" << std::endl;
		fStream << "*/" << std::endl;
		fStream << "" << std::endl;
		fStream << "" << std::endl;
		fStream << "#ifndef __LIBMCDRIVER_RAYLASE_" + sHeaderSpaceName << std::endl;
		fStream << "#define __LIBMCDRIVER_RAYLASE_" + sHeaderSpaceName << std::endl;
		fStream << "" << std::endl;
		fStream << "#include <string>" << std::endl;
		fStream << "#include <memory>" << std::endl;
		fStream << "" << std::endl;

		generateCppClass(fStream);

		fStream << "" << std::endl;
		fStream << "#endif //__LIBMCDRIVER_RAYLASE_" + sHeaderSpaceName << std::endl;
		fStream << "" << std::endl;

	}

};


class CSchemaDefinition_SystemConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_SystemConfig(rlSystemConfig * pSystemConfig)
		: CSchemaDefinition ("rlSystemConfig", pSystemConfig->ConfigVersion, (void*)pSystemConfig, sizeof (rlSystemConfig))
	{
		registerUint32("TimeZone.IdLen", &pSystemConfig->TimeZone.IdLen);
		registerCharArray("TimeZone.Id", &pSystemConfig->TimeZone.Id[0]);
		registerBool("EnableStandAloneMode", &pSystemConfig->EnableStandAloneMode);
		registerInt32("StandAloneListID", &pSystemConfig->StandAloneListID);
		registerInt32("StandAloneExecuteOnErrorListID", &pSystemConfig->StandAloneExecuteOnErrorListID);
		registerBool("ScannerMonitoring.Enabled", &pSystemConfig->ScannerMonitoring.Enabled);
		registerUint32("ScannerMonitoring.Command", &pSystemConfig->ScannerMonitoring.Command);
		registerUint32("ScannerMonitoring.Mask", &pSystemConfig->ScannerMonitoring.Mask);
		registerUint32("ScannerMonitoring.GoodValue", &pSystemConfig->ScannerMonitoring.GoodValue);
		registerInt32("ScannerMonitoring.Period", &pSystemConfig->ScannerMonitoring.Period);
		registerInt32("ScannerMonitoring.MinConsecutiveErrors", &pSystemConfig->ScannerMonitoring.MinConsecutiveErrors);
		registerBool("ScannerMonitoring.ContinueOnError", &pSystemConfig->ScannerMonitoring.ContinueOnError);
		registerUint32("ScannerMonitoring.GoodValue", &pSystemConfig->ScannerMonitoring.GoodValue);

		uint32_t nAxisCount = (uint32_t)((sizeof(pSystemConfig->ScannerMonitoring.ScanHeadAxes) / sizeof(pSystemConfig->ScannerMonitoring.ScanHeadAxes[0])));
		for (uint32_t nAxisIndex = 0; nAxisIndex < nAxisCount; nAxisIndex++) {
			registerUint32("ScannerMonitoring.ScanHeadAxes" + std::to_string (nAxisIndex), (uint32_t*)&pSystemConfig->ScannerMonitoring.ScanHeadAxes[nAxisIndex]);

		}
	}

	virtual ~CSchemaDefinition_SystemConfig()
	{

	}

};


class CSchemaDefinition_GpioConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_GpioConfig(rlGpioConfig* pGpioConfig)
		: CSchemaDefinition("rlGpioConfig", pGpioConfig->ConfigVersion, (void*)pGpioConfig, sizeof(rlGpioConfig))
	{

		uint32_t nPortHeadCount = (uint32_t)(sizeof(pGpioConfig->Ports) / sizeof(pGpioConfig->Ports[0]));

		for (uint32_t nPortHeadIndex = 0; nPortHeadIndex < nPortHeadCount; nPortHeadIndex++) {
			std::string sPortName = "Port" + std::to_string(nPortHeadIndex) + ".";

			registerVersionInfo(&pGpioConfig->Ports[nPortHeadIndex].Config.ConfigVersion);

			registerEnum(sPortName + "Port", (uint32_t*)&pGpioConfig->Ports[nPortHeadIndex].Port);
			registerEnum(sPortName + "Config.IOLevel", (uint32_t*)&pGpioConfig->Ports[nPortHeadIndex].Config.IOLevel);

			uint32_t nPolaritiesArrayCount = (uint32_t)(sizeof(pGpioConfig->Ports[nPortHeadIndex].Config.Polarities) / sizeof(pGpioConfig->Ports[nPortHeadIndex].Config.Polarities[0]));
			for (uint32_t nPolarityIndex = 0; nPolarityIndex < nPolaritiesArrayCount; nPolarityIndex++) {
				registerEnum(sPortName + "Config.Polarity" + std::to_string(nPolarityIndex), (uint32_t*)&pGpioConfig->Ports[nPortHeadIndex].Config.Polarities[nPolarityIndex]);
			}
			registerUint32(sPortName + "Config.PolaritiesLen", &pGpioConfig->Ports[nPortHeadIndex].Config.PolaritiesLen);

			uint32_t nDirectionsArrayCount = (uint32_t)(sizeof(pGpioConfig->Ports[nPortHeadIndex].Config.Directions) / sizeof(pGpioConfig->Ports[nPortHeadIndex].Config.Directions[0]));
			for (uint32_t nDirectionsIndex = 0; nDirectionsIndex < nDirectionsArrayCount; nDirectionsIndex++) {
				registerEnum(sPortName + "Config.Direction" + std::to_string(nDirectionsIndex), (uint32_t*)&pGpioConfig->Ports[nPortHeadIndex].Config.Directions[nDirectionsIndex]);
			}
			registerUint32(sPortName + "Config.DirectionsLen", &pGpioConfig->Ports[nPortHeadIndex].Config.DirectionsLen);

			uint32_t nFunctionsArrayCount = (uint32_t)(sizeof(pGpioConfig->Ports[nPortHeadIndex].Config.Functions) / sizeof(pGpioConfig->Ports[nPortHeadIndex].Config.Functions[0]));
			for (uint32_t nFunctionsIndex = 0; nFunctionsIndex < nFunctionsArrayCount; nFunctionsIndex++) {
				registerEnum(sPortName + "Config.Function" + std::to_string(nFunctionsIndex), (uint32_t*)&pGpioConfig->Ports[nPortHeadIndex].Config.Functions[nFunctionsIndex]);
			}
			registerUint32(sPortName + "Config.FunctionsLen", &pGpioConfig->Ports[nPortHeadIndex].Config.FunctionsLen);

			registerDouble(sPortName + "Config.WriteDelay", &pGpioConfig->Ports[nPortHeadIndex].Config.WriteDelay);
			registerDouble(sPortName + "Config.WriteWidth", &pGpioConfig->Ports[nPortHeadIndex].Config.WriteWidth);

		}

		registerInt32("PortsLen", &pGpioConfig->PortsLen);
	}

	virtual ~CSchemaDefinition_GpioConfig()
	{

	}

};

class CSchemaDefinition_SerialPortConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_SerialPortConfig(rlSerialPortConfig* pGpioConfig)
		: CSchemaDefinition("rlSerialPortConfig", pGpioConfig->ConfigVersion, (void*)pGpioConfig, sizeof(rlSerialPortConfig))
	{

	}

	virtual ~CSchemaDefinition_SerialPortConfig()
	{

	}

};


class CSchemaDefinition_LaserConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_LaserConfig(rlLaserConfig* pLaserConfig)
		: CSchemaDefinition("rlLaserConfig", pLaserConfig->ConfigVersion, (void*)pLaserConfig, sizeof(rlLaserConfig))
	{
		registerEnum("FpsPolarity", (uint32_t*)&pLaserConfig->FpsPolarity);
		registerEnum("GatePolarity", (uint32_t*)&pLaserConfig->GatePolarity);
		registerEnum("LMPolarity", (uint32_t*)&pLaserConfig->LMPolarity);
		registerDouble("FpsStart", &pLaserConfig->FpsStart);
		registerDouble("FpsWidth", &pLaserConfig->FpsWidth);
		registerDouble("GateSetup", &pLaserConfig->GateSetup);
		registerDouble("GateHold", &pLaserConfig->GateHold);
		registerDouble("LaserTriggerDelay", &pLaserConfig->LaserTriggerDelay);
		registerDouble("PowerScale", &pLaserConfig->PowerScale);
		registerDouble("PowerScale1", &pLaserConfig->PowerScale1);
		registerDouble("PowerChangeSetup", &pLaserConfig->PowerChangeSetup);
		registerDouble("PowerWriteDelay", &pLaserConfig->PowerWriteDelay);
		registerDouble("PowerWriteWidth", &pLaserConfig->PowerWriteWidth);
		registerEnum("HotPowerTarget", (uint32_t*)&pLaserConfig->HotPowerTarget);
		registerEnum("HotPowerTarget1", (uint32_t*)&pLaserConfig->HotPowerTarget1);
		registerUint16("SimmerPower", &pLaserConfig->SimmerPower);
		registerBool("EnableTickle", &pLaserConfig->EnableTickle);
		registerDouble("TickleFrequency", &pLaserConfig->TickleFrequency);
		registerDouble("TickleWidth", &pLaserConfig->TickleWidth);
		registerBool("EnableLaserSync", &pLaserConfig->EnableLaserSync);
		registerBool("EnableGateModulatesLm", &pLaserConfig->EnableGateModulatesLm);
		registerBool("EnablePowerCorrection", &pLaserConfig->EnablePowerCorrection);
		registerBool("EnablePowerCorrection1", &pLaserConfig->EnablePowerCorrection1);
		registerBool("EnableVelocityCorrection", &pLaserConfig->EnableVelocityCorrection);
		registerBool("EnableVelocityCorrection1", &pLaserConfig->EnableVelocityCorrection1);
		registerBool("EnablePowerCalibration", &pLaserConfig->EnablePowerCalibration);
		registerBool("EnablePowerCalibration1", &pLaserConfig->EnablePowerCalibration1);
		registerBool("EnableEquallySpacedPulses", &pLaserConfig->EnableEquallySpacedPulses);
	}

	virtual ~CSchemaDefinition_LaserConfig()
	{

	}

};

class CSchemaDefinition_ScannerConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_ScannerConfig(rlScannerConfig* pScannerConfig)
		: CSchemaDefinition("rlScannerConfig", pScannerConfig->ConfigVersion, (void*)pScannerConfig, sizeof(rlScannerConfig))
	{
		registerDouble("FieldSize.X", &pScannerConfig->FieldSize.X);
		registerDouble("FieldSize.Y", &pScannerConfig->FieldSize.Y);
		registerDouble("FieldSize.Z", &pScannerConfig->FieldSize.Z);
		registerInt32("HeadCount", &pScannerConfig->HeadCount);

		uint32_t nHeadArrayCount = (uint32_t)(sizeof(pScannerConfig->Heads) / sizeof(pScannerConfig->Heads[0]));

		for (uint32_t nHeadIndex = 0; nHeadIndex < nHeadArrayCount; nHeadIndex++) {
			registerVersionInfo(&pScannerConfig->Heads[nHeadIndex].ConfigVersion);

			std::string sHead = "Head" + std::to_string(nHeadIndex) + ".";
			for (uint32_t i = 0; i < 3; i++)
				for (uint32_t j = 0; j < 3; j++)
					registerDouble(sHead + "FieldTransform.A" + std::to_string(i) + std::to_string(j), &pScannerConfig->Heads[nHeadIndex].FieldTransform.A[i][j]);
			for (uint32_t i = 0; i < 3; i++)
				registerDouble(sHead + "FieldTransform.b" + std::to_string(i), &pScannerConfig->Heads[nHeadIndex].FieldTransform.b[i]);

			for (uint32_t nAxis = 0; nAxis < 5; nAxis++) {
				std::string sAxis = sHead + "Axes" + std::to_string(nAxis) + ".";

				registerDouble(sAxis + "AccelerationTime", &pScannerConfig->Heads[nHeadIndex].Axes[nAxis].AccelerationTime);
				registerDouble(sAxis + "TrackingError", &pScannerConfig->Heads[nHeadIndex].Axes[nAxis].TrackingError);
				registerDouble(sAxis + "HeadBias", &pScannerConfig->Heads[nHeadIndex].Axes[nAxis].HeadBias);
			}
			registerBool(sHead + "EnableFieldCorrection", &pScannerConfig->Heads[nHeadIndex].EnableFieldCorrection);

		}
		registerDouble("ScanHeadDelay", &pScannerConfig->ScanHeadDelay);
		registerEnum("HeadFormat", (uint32_t*)&pScannerConfig->HeadFormat);
		registerDouble("MaxMagnification", &pScannerConfig->MaxMagnification);

	}

	virtual ~CSchemaDefinition_ScannerConfig()
	{

	}

};

class CSchemaDefinition_ProcessVariables : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_ProcessVariables(rlProcessVariables* pProcessVariables)
		: CSchemaDefinition("rlProcessVariables", pProcessVariables->ConfigVersion, (void*)pProcessVariables, sizeof(rlProcessVariables))
	{

	}

	virtual ~CSchemaDefinition_ProcessVariables()
	{

	}

};

class CSchemaDefinition_OpticalParameters : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_OpticalParameters(rlOpticalParameters* pOpticalParameters)
		: CSchemaDefinition("rlOpticalParameters", pOpticalParameters->ConfigVersion, (void*)pOpticalParameters, sizeof(rlOpticalParameters))
	{

	}

	virtual ~CSchemaDefinition_OpticalParameters()
	{

	}

};



class CSchemaDefinition_QuadratureDecoderConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_QuadratureDecoderConfig(rlQuadratureDecoderConfig* pQuadratureDecoderConfig)
		: CSchemaDefinition("rlQuadratureDecoderConfig", pQuadratureDecoderConfig->ConfigVersion, (void*)pQuadratureDecoderConfig, sizeof(rlQuadratureDecoderConfig))
	{

	}

	virtual ~CSchemaDefinition_QuadratureDecoderConfig()
	{

	}

};


class CSchemaDefinition_MotfConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_MotfConfig(rlMotfConfig* pMotfConfig)
		: CSchemaDefinition("rlMotfConfig", pMotfConfig->ConfigVersion, (void*)pMotfConfig, sizeof(rlMotfConfig))
	{

	}

	virtual ~CSchemaDefinition_MotfConfig()
	{

	}

};

class CSchemaDefinition_SpiConfig : public CSchemaDefinition {
private:
public:

	CSchemaDefinition_SpiConfig(rlSpiConfig* pSpiConfig)
		: CSchemaDefinition("rlSpiConfig", pSpiConfig->ConfigVersion, (void*)pSpiConfig, sizeof(rlSpiConfig))
	{

		uint32_t nModuleCount = rlSPIModuleCount;
		for (uint32_t nModuleIndex = 0; nModuleIndex < nModuleCount; nModuleIndex++) {
			auto pModule = &pSpiConfig->Modules[nModuleIndex];
			std::string sPrefix = "Module" + std::to_string(nModuleIndex) + ".";

			registerVersionInfo(&pModule->ConfigVersion);
			registerBool(sPrefix + "Enabled", &pModule->Enabled);
			registerEnum(sPrefix + "SpiSyncMode", (uint32_t*)&pModule->SpiSyncMode);
			registerUint16(sPrefix + "BitsPerWord", &pModule->BitsPerWord);
			registerDouble(sPrefix + "PreDelay", &pModule->PreDelay);
			registerDouble(sPrefix + "PostDelay", &pModule->PostDelay);
			registerDouble(sPrefix + "FrameDelay", &pModule->FrameDelay);
			registerEnum(sPrefix + "OutputSource", (uint32_t*)&pModule->OutputSource);
			registerEnum(sPrefix + "BitOrder", (uint32_t*)&pModule->BitOrder);
			registerDouble(sPrefix + "ClockPeriod", &pModule->ClockPeriod);
		}

	}

	virtual ~CSchemaDefinition_SpiConfig()
	{

	}

};


int main()
{

	try {
		std::cout << "-----------------------------------------------------------" << std::endl;
		std::cout << "Raylase Schema Extractor" << std::endl;
		std::cout << "-----------------------------------------------------------" << std::endl;

		rlSystemConfig systemConfig;
		rlSystemInitConfig(&systemConfig);
		CSchemaDefinition_SystemConfig schema_SystemConfig(&systemConfig);
		

		rlLaserConfig laserConfig;
		rlLaserInitConfig(&laserConfig);
		CSchemaDefinition_LaserConfig schema_LaserConfig (&laserConfig);
		

		rlScannerConfig scannerConfig;
		rlScannerInitConfig(&scannerConfig);
		CSchemaDefinition_ScannerConfig schema_ScannerConfig(&scannerConfig);

		
		rlGpioConfig gpioConfig;
		rlGpioInitConfig(&gpioConfig);
		CSchemaDefinition_GpioConfig schema_GpioConfig(&gpioConfig);
	
		rlSpiConfig spiConfig;
		rlSfioSpiInitConfig(&spiConfig);
		CSchemaDefinition_SpiConfig schema_SpiConfig(&spiConfig);

		std::string sSystemConfigFileName = "libmcdriver_raylase_systemconfig_" + std::to_string(schema_SystemConfig.getVersion()) + ".hpp";
		std::cout << "writing " << sSystemConfigFileName << std::endl;
		schema_SystemConfig.generateHppHeaderFile(sSystemConfigFileName);

		std::string sLaserConfigFileName = "libmcdriver_raylase_laserconfig_" + std::to_string(schema_LaserConfig.getVersion()) + ".hpp";
		std::cout << "writing " << sLaserConfigFileName << std::endl;
		schema_LaserConfig.generateHppHeaderFile(sLaserConfigFileName);

		std::string sScannerConfigFileName = "libmcdriver_raylase_scannerconfig_" + std::to_string(schema_ScannerConfig.getVersion()) + ".hpp";
		std::cout << "writing " << sScannerConfigFileName << std::endl;
		schema_ScannerConfig.generateHppHeaderFile(sScannerConfigFileName);

		std::string sGpioConfigFileName = "libmcdriver_raylase_gpioconfig_" + std::to_string(schema_GpioConfig.getVersion()) + ".hpp";
		std::cout << "writing " << sGpioConfigFileName << std::endl;
		schema_GpioConfig.generateHppHeaderFile(sGpioConfigFileName);

		std::string sSpiConfigFileName = "libmcdriver_raylase_spiconfig_" + std::to_string(schema_SpiConfig.getVersion()) + ".hpp";
		std::cout << "writing " << sSpiConfigFileName << std::endl;
		schema_SpiConfig.generateHppHeaderFile(sSpiConfigFileName);

	}
	catch (std::exception& E) {
		std::cout << E.what() << std::endl;
	}

}