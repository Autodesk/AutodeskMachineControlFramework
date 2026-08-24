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

#include "libmcdriver_scanlabsmc_sdk.hpp"
#include "libmcdriver_scanlabsmc_interfaceexception.hpp"

#ifdef _WIN32
#include <windows.h>
#else // _WIN32
#include <dlfcn.h>
#endif // _WIN32


#include <vector>
#include <sstream>
#include <iostream>
#include <string>

using namespace LibMCDriver_ScanLabSMC::Impl;


#define SCANLAB_MAXDLLNAMELENGTH 1024 * 1024

#ifdef _WIN32
void* _loadScanLabSMCAddress (HMODULE hLibrary, const char * pSymbolName, bool bMandatory = true) {
	void * pFuncPtr = (void*) GetProcAddress(hLibrary, pSymbolName);
	if (bMandatory) {
		if (pFuncPtr == nullptr)
			throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));
	}

	return pFuncPtr;
}
#else
void* _loadScanLabSMCAddress(void * hLibrary, const char* pSymbolName, bool bMandatory = true) {

	void* pFuncPtr = (void*) dlsym(hLibrary, pSymbolName);
	dlerror();
	if (bMandatory) {
		if (pFuncPtr == nullptr)
			throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTFINDLIBRARYEXPORT, "could not find library export: " + std::string(pSymbolName));
	}

	return pFuncPtr;
}
#endif


CScanLabSMCSDK_DLLDirectoryCache::CScanLabSMCSDK_DLLDirectoryCache()
{
#ifdef _WIN32
	std::vector<wchar_t> buffer;
	buffer.resize(MAX_PATH + 1);
	GetDllDirectoryW(MAX_PATH, buffer.data());

	buffer.at(MAX_PATH) = 0;
	m_sCachedDLLDirectoryW = std::wstring(buffer.data());
#endif // _WIN32
}

CScanLabSMCSDK_DLLDirectoryCache::~CScanLabSMCSDK_DLLDirectoryCache()
{
#ifdef _WIN32
	if (!m_sCachedDLLDirectoryW.empty()) {
		SetDllDirectoryW(m_sCachedDLLDirectoryW.c_str());
	}
#endif // _WIN32
}


CScanLabSMCSDKJournal::CScanLabSMCSDKJournal(const std::string& sDebugFileName)
{

	m_CStream.open(sDebugFileName, std::ios::out);
	if (!m_CStream.is_open())
		throw std::runtime_error("could not create sdk debug log: " + sDebugFileName);
}

CScanLabSMCSDKJournal::~CScanLabSMCSDKJournal()
{
}

void CScanLabSMCSDKJournal::writeCLine(const std::string& sLine)
{
	std::lock_guard<std::mutex> lockGuard(m_Mutex);
	m_CStream << sLine << std::endl;

}


void CScanLabSMCSDKJournal::logCall(const std::string& sFunctionName, const std::string& sCParameters)
{
	writeCLine(sFunctionName + "(" + sCParameters + ");");
}

std::string CScanLabSMCSDKJournal::getVariableSuffix(const std::string& sVariableBaseName)
{
	auto iIter = m_DefinedVariables.find(sVariableBaseName);
	if (iIter == m_DefinedVariables.end()) {
		m_DefinedVariables.insert(std::make_pair(sVariableBaseName, 1));
		return "1";
	}
	else {
		iIter->second++;
		return std::to_string(iIter->second);
	}
}


std::string CScanLabSMCSDKJournal::defineDoubleVariable(const std::string& sVariableBaseName)
{
	std::string sVariableName = sVariableBaseName + getVariableSuffix(sVariableBaseName);

	writeCLine("double " + sVariableName + ";");
	return sVariableName;
}

std::string CScanLabSMCSDKJournal::defineUint32Variable(const std::string& sVariableBaseName)
{
	std::string sVariableName = sVariableBaseName + getVariableSuffix(sVariableBaseName);

	writeCLine("uint32_t " + sVariableName + ";");
	return sVariableName;

}

std::string CScanLabSMCSDKJournal::defineInt32Variable(const std::string& sVariableBaseName)
{
	std::string sVariableName = sVariableBaseName + getVariableSuffix(sVariableBaseName);

	writeCLine("int32_t " + sVariableName + ";");
	return sVariableName;

}

std::string CScanLabSMCSDKJournal::escapeString(const std::string& sString)
{
	return "\"" + sString + "\"";
}


CScanLabSMCSDK::CScanLabSMCSDK(const std::string& sDLLNameUTF8, const std::string& sDLLDirectoryUTF8)
	: m_LibraryHandle (nullptr), m_bIsInitialized (false)
{

	resetFunctionPtrs();

#ifdef _WIN32

	if (sDLLNameUTF8.length() > SCANLAB_MAXDLLNAMELENGTH)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_INVALIDPARAM);

	// Convert filename to UTF16-string
	int nLength = (int)sDLLNameUTF8.length ();
	int nBufferSize = nLength * 2 + 2;
	std::vector<wchar_t> wsLibraryFileName(nBufferSize);
	int nResult = MultiByteToWideChar(CP_UTF8, 0, sDLLNameUTF8.c_str(), nLength, &wsLibraryFileName[0], nBufferSize);
	if (nResult == 0)
		throw ELibMCDriver_ScanLabSMCInterfaceException (LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);

	int nPathLength = (int)sDLLDirectoryUTF8.length();
	int nPathBufferSize = nPathLength * 2 + 2;
	std::vector<wchar_t> wsDLLPath(nPathBufferSize);
	int nPathResult = MultiByteToWideChar(CP_UTF8, 0, sDLLDirectoryUTF8.c_str(), nPathLength, &wsDLLPath[0], nPathBufferSize);
	if (nPathResult == 0)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);

	m_sDLLDirectoryW = std::wstring(wsDLLPath.data());

	auto pDirectoryCache = cacheDllDirectory();

	HMODULE hLibrary = LoadLibraryW(wsLibraryFileName.data());
	if (hLibrary == 0)
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);
#else // _WIN32
	void* hLibrary = dlopen(sDLLNameUTF8.c_str(), RTLD_LAZY);
	if (hLibrary == 0) {
		dlerror();
		throw ELibMCDriver_ScanLabSMCInterfaceException(LIBMCDRIVER_SCANLABSMC_ERROR_COULDNOTLOADLIBRARY);
	}
#endif // _WIN32

	this->ptr_slsc_cfg_get_scanmotioncontrol_version = (PScanLabSMCPtr_slsc_cfg_get_scanmotioncontrol_version)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_get_scanmotioncontrol_version");
	this->ptr_slsc_cfg_initialize_from_file = (PScanLabSMCPtr_slsc_cfg_initialize_from_file)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_initialize_from_file");
	this->ptr_slsc_cfg_delete = (PScanLabSMCPtr_slsc_cfg_delete)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_delete");
	this->ptr_slsc_job_begin = (PScanLabSMCPtr_slsc_job_begin)_loadScanLabSMCAddress(hLibrary, "slsc_job_begin");
	this->ptr_slsc_job_end = (PScanLabSMCPtr_slsc_job_end)_loadScanLabSMCAddress(hLibrary, "slsc_job_end");
	this->ptr_slsc_job_jump = (PScanLabSMCPtr_slsc_job_jump)_loadScanLabSMCAddress(hLibrary, "slsc_job_jump");
	this->ptr_slsc_job_begin_polyline = (PScanLabSMCPtr_slsc_job_begin_polyline)_loadScanLabSMCAddress(hLibrary, "slsc_job_begin_polyline");
	this->ptr_slsc_job_end_polyline = (PScanLabSMCPtr_slsc_slsc_job_end_polyline)_loadScanLabSMCAddress(hLibrary, "slsc_job_end_polyline");
	this->ptr_slsc_job_line = (PScanLabSMCPtr_slsc_job_line)_loadScanLabSMCAddress(hLibrary, "slsc_job_line");

	this->ptr_slsc_job_para_enable = (PScanLabSMCPtr_slsc_job_para_enable)_loadScanLabSMCAddress(hLibrary, "slsc_job_para_enable");
	this->ptr_slsc_job_para_disable = (PScanLabSMCPtr_slsc_job_para_disable)_loadScanLabSMCAddress(hLibrary, "slsc_job_para_disable");
	this->ptr_slsc_job_para_line = (PScanLabSMCPtr_slsc_job_para_line)_loadScanLabSMCAddress(hLibrary, "slsc_job_para_line");
	this->ptr_slsc_job_multi_para_line = (PScanLabSMCPtr_slsc_job_multi_para_line)_loadScanLabSMCAddress(hLibrary, "slsc_job_multi_para_line");

	this->ptr_slsc_ctrl_start_execution = (PScanLabSMCPtr_slsc_ctrl_start_execution)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_start_execution");
	this->ptr_slsc_ctrl_stop = (PScanLabSMCPtr_slsc_ctrl_stop)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_stop");
	this->ptr_slsc_ctrl_stop_controlled = (PScanLabSMCPtr_slsc_ctrl_stop_controlled)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_stop_controlled");
	this->ptr_slsc_ctrl_get_exec_state = (PScanLabSMCPtr_slsc_ctrl_get_exec_state)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_exec_state");
	this->ptr_slsc_job_set_jump_speed = (PScanLabSMCPtr_slsc_job_set_jump_speed)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_jump_speed");
	this->ptr_slsc_job_set_mark_speed = (PScanLabSMCPtr_slsc_job_set_mark_speed)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_mark_speed");
	this->ptr_slsc_job_set_min_mark_speed = (PScanLabSMCPtr_slsc_job_set_min_mark_speed)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_min_mark_speed");
	this->ptr_slsc_job_jump_min_time = (PScanLabSMCPtr_slsc_job_jump_min_time)_loadScanLabSMCAddress(hLibrary, "slsc_job_jump_min_time");
	this->ptr_slsc_job_set_corner_tolerance = (PScanLabSMCPtr_slsc_job_set_corner_tolerance)_loadScanLabSMCAddress(hLibrary, "slsc_job_set_corner_tolerance");
	this->ptr_slsc_ctrl_get_error = (PScanLabSMCPtr_slsc_ctrl_get_error)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_error");
	this->ptr_slsc_ctrl_get_error_count = (PScanLabSMCPtr_slsc_ctrl_get_error_count)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_error_count");
	this->ptr_slsc_ctrl_get_simulation_filename = (PScanLabSMCPtr_slsc_ctrl_get_simulation_filename)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_simulation_filename");
	this->ptr_slsc_ctrl_get_job_characteristic = (PScanLabSMCPtr_slsc_ctrl_get_job_characteristic)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_get_job_characteristic");
	this->ptr_slsc_cfg_get_blend_mode = (PScanLabSMCPtr_slsc_cfg_get_blend_mode)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_get_blend_mode");
	this->ptr_slsc_cfg_set_blend_mode = (PScanLabSMCPtr_slsc_cfg_set_blend_mode)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_set_blend_mode");

	this->ptr_slsc_job_start_record = (PScanLabSMCPtr_slsc_job_start_record)_loadScanLabSMCAddress(hLibrary, "slsc_job_start_record");
	this->ptr_slsc_job_stop_record = (PScanLabSMCPtr_slsc_job_stop_record)_loadScanLabSMCAddress(hLibrary, "slsc_job_stop_record");
	this->ptr_slsc_ctrl_log_record = (PScanLabSMCPtr_slsc_ctrl_log_record)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_log_record");

	this->ptr_slsc_ctrl_exec_init_laser_sequence = (PScanLabSMCPtr_slsc_ctrl_exec_init_laser_sequence)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_exec_init_laser_sequence");
	this->ptr_slsc_ctrl_exec_shutdown_laser_sequence = (PScanLabSMCPtr_slsc_ctrl_exec_shutdown_laser_sequence)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_exec_shutdown_laser_sequence");
	this->ptr_slsc_job_write_analog_x = (PScanLabSMCPtr_slsc_job_write_analog_x)_loadScanLabSMCAddress(hLibrary, "slsc_job_write_analog_x");
	
	this->ptr_slsc_ctrl_write_digital_x = (PScanLabSMCPtr_slsc_ctrl_write_digital_x)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_write_digital_x");
	this->ptr_slsc_ctrl_write_digital_mask_x = (PScanLabSMCPtr_slsc_ctrl_write_digital_mask_x)_loadScanLabSMCAddress(hLibrary, "slsc_ctrl_write_digital_mask_x");

	this->ptr_slsc_job_write_digital_x = (PScanLabSMCPtr_slsc_job_write_digital_x)_loadScanLabSMCAddress(hLibrary, "slsc_job_write_digital_x");
	this->ptr_slsc_job_write_digital_mask_x = (PScanLabSMCPtr_slsc_job_write_digital_mask_x)_loadScanLabSMCAddress(hLibrary, "slsc_job_write_digital_mask_x");

	this->ptr_slsc_cfg_register_callback_job_calculated = (PScanLabSMCPtr_slsc_cfg_register_callback_job_calculated)_loadScanLabSMCAddress(hLibrary, "slsc_cfg_register_callback_job_calculated");
	this->ptr_slsc_job_begin_module = (PScanLabSMCPtr_slsc_job_begin_module)_loadScanLabSMCAddress(hLibrary, "slsc_job_begin_module");

	m_LibraryHandle = (void*) hLibrary;
}


CScanLabSMCSDK::~CScanLabSMCSDK()
{

	if (m_LibraryHandle != nullptr) {
#ifdef _WIN32
		HMODULE hModule = (HMODULE)m_LibraryHandle;
		FreeLibrary(hModule);
#else // _WIN32
		dlclose(m_LibraryHandle);
#endif // _WIN32

		m_LibraryHandle = nullptr;
		resetFunctionPtrs();
	}
}

void CScanLabSMCSDK::initDLL()
{
	if (!m_bIsInitialized) {

		if (ptr_slsc_cfg_initialize_from_file == nullptr)
			throw std::runtime_error("SMC DLL not loaded");

		m_bIsInitialized = true;

	}
}

void CScanLabSMCSDK::checkError(size_t hHandle, uint32_t nSMCError)
{

	if (nSMCError != 0) {
		if ((hHandle != 0) && (ptr_slsc_ctrl_get_error != nullptr) && (ptr_slsc_ctrl_get_error_count != nullptr)) {

			std::lock_guard<std::mutex> lockGuard(m_ListErrorMutex);

			size_t nErrorCount = 0;
			uint32_t nErrorCountReturn = this->slsc_ctrl_get_error_count(hHandle, nErrorCount);

			if (nErrorCountReturn == 0) {

				std::stringstream sErrorStream;

				for (size_t nErrorIndex = 0; nErrorIndex < nErrorCount; nErrorIndex++) {

					if (nErrorIndex > 0)
						sErrorStream << " | ";

					size_t nBufferSize = 1024;
					std::vector<char> buffer(nBufferSize + 1);
					size_t nErrorCode = 0;

					uint32_t nErrorGetReturn = this->slsc_ctrl_get_error(hHandle, nErrorIndex, nErrorCode, buffer.data(), nBufferSize);
					if (nErrorGetReturn == 0) {
						buffer.at(nBufferSize) = 0;
						sErrorStream << "error #" << std::to_string(nErrorCode) << ": " << std::string(buffer.data ());
					}
					else {
						sErrorStream << "<could not get error message for #" << std::to_string(nErrorIndex) << ">";
					}

				}

				throw std::runtime_error("SMC Error: " + std::to_string(nSMCError) + ", Error Details: " + sErrorStream.str () );

			} else {
				throw std::runtime_error("Could not get error count for: " + std::to_string(nSMCError));

			}

		}
		else {
			throw std::runtime_error("Generic SMC Error: " + std::to_string(nSMCError));
		}
		
	}
}


void CScanLabSMCSDK::resetFunctionPtrs()
{
	ptr_slsc_cfg_get_scanmotioncontrol_version = nullptr;
	ptr_slsc_cfg_initialize_from_file = nullptr;
	ptr_slsc_cfg_delete = nullptr;
	ptr_slsc_job_begin = nullptr;
	ptr_slsc_job_end = nullptr;
	ptr_slsc_job_jump = nullptr;
	ptr_slsc_job_begin_polyline = nullptr;
	ptr_slsc_job_end_polyline = nullptr;
	ptr_slsc_job_line = nullptr;
	ptr_slsc_job_para_enable = nullptr;
	ptr_slsc_job_para_disable = nullptr;
	ptr_slsc_job_para_line = nullptr;
	ptr_slsc_job_multi_para_line = nullptr;

	ptr_slsc_ctrl_start_execution = nullptr;
	ptr_slsc_ctrl_stop = nullptr;
	ptr_slsc_ctrl_stop_controlled = nullptr;
	ptr_slsc_ctrl_get_exec_state = nullptr;
	ptr_slsc_job_set_jump_speed = nullptr;
	ptr_slsc_job_set_mark_speed = nullptr;
	ptr_slsc_job_set_min_mark_speed = nullptr;
	ptr_slsc_job_jump_min_time = nullptr;
	ptr_slsc_job_set_corner_tolerance = nullptr;
	ptr_slsc_ctrl_get_error = nullptr;
	ptr_slsc_ctrl_get_error_count = nullptr;
	ptr_slsc_ctrl_get_simulation_filename = nullptr;
	ptr_slsc_ctrl_get_job_characteristic = nullptr;
	ptr_slsc_cfg_get_blend_mode = nullptr;
	ptr_slsc_cfg_set_blend_mode = nullptr;
	ptr_slsc_job_start_record = nullptr;
	ptr_slsc_job_stop_record = nullptr;
	ptr_slsc_ctrl_log_record = nullptr;
}


PScanLabSMCSDK_DLLDirectoryCache CScanLabSMCSDK::cacheDllDirectory()
{
	auto pCache = std::make_shared<CScanLabSMCSDK_DLLDirectoryCache>();

#ifdef _WIN32
	SetDllDirectoryW(m_sDLLDirectoryW.c_str());
	SetCurrentDirectoryW(m_sDLLDirectoryW.c_str());
#endif // _WIN32

	return pCache;

}


slsc_VersionInfo CScanLabSMCSDK::slsc_cfg_get_scanmotioncontrol_version(void)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_cfg_get_scanmotioncontrol_version", "");

	return this->ptr_slsc_cfg_get_scanmotioncontrol_version();
}

slscReturnValue CScanLabSMCSDK::slsc_cfg_initialize_from_file(slscHandle* pHandle, const char* XmlConfigFileName)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_cfg_initialize_from_file", "&" + std::to_string(intptr_t(pHandle)) + ", " + std::string(XmlConfigFileName));

	return this->ptr_slsc_cfg_initialize_from_file(pHandle, XmlConfigFileName);
}

slscReturnValue CScanLabSMCSDK::slsc_cfg_delete(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_cfg_delete", std::to_string(Handle));

	return this->ptr_slsc_cfg_delete(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_job_begin(size_t Handle, size_t* JobID)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_begin", std::to_string(Handle) + ", &" + std::to_string(intptr_t(JobID)));

	return this->ptr_slsc_job_begin(Handle, JobID);
}

slscReturnValue CScanLabSMCSDK::slsc_job_end(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_end", std::to_string(Handle));


	return this->ptr_slsc_job_end(Handle);
}
slscReturnValue CScanLabSMCSDK::slsc_job_jump(size_t Handle, const double* Target)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_jump", std::to_string(Handle) + ", [" + std::to_string(Target[0]) + ", " + std::to_string(Target[1]) + "]");

	return this->ptr_slsc_job_jump(Handle, Target);
}

slscReturnValue CScanLabSMCSDK::slsc_job_begin_polyline(size_t Handle, const slsc_PolylineOptions Options)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_begin_polyline", std::to_string(Handle) + ", &" + std::to_string(intptr_t(&Options)));

	return this->ptr_slsc_job_begin_polyline(Handle, Options);
}

slscReturnValue CScanLabSMCSDK::slsc_job_end_polyline(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_end_polyline", std::to_string(Handle));

	return this->ptr_slsc_job_end_polyline(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_job_line(size_t Handle, const double* Target)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_line", std::to_string(Handle) + ", [" + std::to_string(Target[0]) + ", " + std::to_string(Target[1]) + "]");

	return this->ptr_slsc_job_line(Handle, Target);
}

slscReturnValue CScanLabSMCSDK::slsc_job_para_enable(size_t Handle, double* dParaTargetDefault)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_para_enable", std::to_string(Handle) + ", &" + std::to_string(intptr_t(dParaTargetDefault)));

	return this->ptr_slsc_job_para_enable(Handle, dParaTargetDefault);
}

slscReturnValue CScanLabSMCSDK::slsc_job_para_disable(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_para_disable", std::to_string(Handle));

	return this->ptr_slsc_job_para_disable(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_job_para_line(size_t Handle, const double* Target, const double* ParaTarget)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_para_line", std::to_string(Handle) + ", [" + std::to_string(Target[0]) + ", " + std::to_string(Target[1]) + "], &" + std::to_string(intptr_t(ParaTarget)));

	return this->ptr_slsc_job_para_line(Handle, Target, ParaTarget);
}

slscReturnValue CScanLabSMCSDK::slsc_job_multi_para_line(size_t Handle, const double* Target, const slsc_MultiParaTarget* pParaTarget)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_multi_para_line", std::to_string(Handle) + ", [" + std::to_string(Target[0]) + ", " + std::to_string(Target[1]) + "], &" + std::to_string(intptr_t(pParaTarget)));

	return this->ptr_slsc_job_multi_para_line(Handle, Target, pParaTarget);
}


slscReturnValue CScanLabSMCSDK::slsc_ctrl_start_execution(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_start_execution", std::to_string(Handle));

	return this->ptr_slsc_ctrl_start_execution(Handle);

}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_stop(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_stop", std::to_string(Handle));

	return this->ptr_slsc_ctrl_stop(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_stop_controlled(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_stop_controlled", std::to_string(Handle));

	return this->ptr_slsc_ctrl_stop_controlled(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_get_exec_state(size_t Handle, slsc_ExecState* execState)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_get_exec_state", std::to_string(Handle) + ", &" + std::to_string(intptr_t(execState)));

	return this->ptr_slsc_ctrl_get_exec_state(Handle, execState);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_get_error(size_t Handle, size_t ErrorNr, uint64_t& nErrorCode, char* pErrorMsg, size_t nBufSize)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_get_error", std::to_string(Handle) + ", " + std::to_string(ErrorNr) + ", &" + std::to_string(intptr_t(&nErrorCode)) + ", &" + std::to_string(intptr_t(pErrorMsg)) + ", " + std::to_string(nBufSize));
	return this->ptr_slsc_ctrl_get_error(Handle, ErrorNr, nErrorCode, pErrorMsg, nBufSize);

}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_get_error_count(size_t Handle, size_t& nErrorCount)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_get_error_count", std::to_string(Handle) + ", &" + std::to_string(intptr_t(&nErrorCount)));

	return this->ptr_slsc_ctrl_get_error_count(Handle, nErrorCount);
}

slscReturnValue CScanLabSMCSDK::slsc_job_set_jump_speed(size_t Handle, double dJumpSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_set_jump_speed", std::to_string(Handle) + ", " + std::to_string(dJumpSpeed));
	return this->ptr_slsc_job_set_jump_speed(Handle, dJumpSpeed);

}

slscReturnValue CScanLabSMCSDK::slsc_job_set_mark_speed(size_t Handle, double dMarkSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_set_mark_speed", std::to_string(Handle) + ", " + std::to_string(dMarkSpeed));

	return this->ptr_slsc_job_set_mark_speed(Handle, dMarkSpeed);
}

slscReturnValue CScanLabSMCSDK::slsc_job_set_min_mark_speed(size_t Handle, double dMinimalMarkSpeed)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_set_min_mark_speed", std::to_string(Handle) + ", " + std::to_string(dMinimalMarkSpeed));

	return this->ptr_slsc_job_set_min_mark_speed(Handle, dMinimalMarkSpeed);
}

slscReturnValue CScanLabSMCSDK::slsc_job_jump_min_time(size_t Handle, const double* Target, double dMinimalJumpTime)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_jump_min_time", std::to_string(Handle) + ", [" + std::to_string(Target[0]) + ", " + std::to_string(Target[1]) + "], " + std::to_string(dMinimalJumpTime));
	return this->ptr_slsc_job_jump_min_time(Handle, Target, dMinimalJumpTime);

}

slscReturnValue CScanLabSMCSDK::slsc_job_set_corner_tolerance(size_t Handle, const double* Target, double dCornerTolerance)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_set_corner_tolerance", std::to_string(Handle) + ", &" + std::to_string(intptr_t(Target)) + ", " + std::to_string(dCornerTolerance));

	return this->ptr_slsc_job_set_corner_tolerance(Handle, Target, dCornerTolerance);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_get_simulation_filename(size_t Handle, size_t nJobID, char* pszBuffer, size_t nBufferSize)
{

	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_get_simulation_filename", std::to_string(Handle) + ", " + std::to_string(nJobID) + ", &" + std::to_string(intptr_t(pszBuffer)) + ", " + std::to_string(nBufferSize));
	return this->ptr_slsc_ctrl_get_simulation_filename(Handle, nJobID, pszBuffer, nBufferSize);

}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_get_job_characteristic(size_t Handle, size_t nJobID, slsc_JobCharacteristic eKey, double* pdValue)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_get_job_characteristic", std::to_string(Handle) + ", " + std::to_string(nJobID) + ", " + std::to_string((uint32_t)eKey) + ", &" + std::to_string(intptr_t(pdValue)));

	return this->ptr_slsc_ctrl_get_job_characteristic(Handle, nJobID, eKey, pdValue);
}


slscReturnValue CScanLabSMCSDK::slsc_cfg_get_blend_mode(size_t Handle, slsc_BlendModes* BlendMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_cfg_get_blend_mode", std::to_string(Handle) + ", &" + std::to_string(intptr_t(BlendMode)));

	return this->ptr_slsc_cfg_get_blend_mode(Handle, BlendMode);
}

slscReturnValue CScanLabSMCSDK::slsc_cfg_set_blend_mode(size_t Handle, slsc_BlendModes BlendMode)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_cfg_set_blend_mode", std::to_string(Handle) + ", " + std::to_string((uint32_t)BlendMode));

	return this->ptr_slsc_cfg_set_blend_mode(Handle, BlendMode);
}

slscReturnValue CScanLabSMCSDK::slsc_job_start_record(size_t Handle, slsc_RecordSet RecordSetA, slsc_RecordSet RecordSetB)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_start_record", std::to_string(Handle) + ", " + std::to_string((uint32_t)RecordSetA) + ", " + std::to_string((uint32_t)RecordSetB));

	return this->ptr_slsc_job_start_record(Handle, RecordSetA, RecordSetB);
}

slscReturnValue CScanLabSMCSDK::slsc_job_stop_record(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_stop_record", std::to_string(Handle));

	return this->ptr_slsc_job_stop_record(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_log_record(size_t Handle, const char* DatasetPath, slsc_TransformationStep Step)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_log_record", std::to_string(Handle) + ", " + std::string(DatasetPath) + ", " + std::to_string((uint32_t)Step));

	return this->ptr_slsc_ctrl_log_record(Handle, DatasetPath, Step);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_exec_init_laser_sequence(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_exec_init_laser_sequence", std::to_string(Handle));

	return this->ptr_slsc_ctrl_exec_init_laser_sequence(Handle);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_exec_shutdown_laser_sequence(size_t Handle)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_exec_shutdown_laser_sequence", std::to_string(Handle));

	return this->ptr_slsc_ctrl_exec_shutdown_laser_sequence(Handle);
}


slscReturnValue CScanLabSMCSDK::slsc_job_write_analog_x(size_t Handle, slsc_AnalogOutput Channel, double Value, double TimeDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_write_analog_x", std::to_string(Handle) + ", " + std::to_string((uint32_t)Channel) + ", " + std::to_string(Value) + ", " + std::to_string(TimeDelay));

	return this->ptr_slsc_job_write_analog_x(Handle, Channel, Value, TimeDelay);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_write_digital_x(size_t Handle, slsc_DigitalOutput Channel, uint16_t Value)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_write_digital_x", std::to_string(Handle) + ", " + std::to_string((uint32_t)Channel) + ", " + std::to_string(Value));

	return this->ptr_slsc_ctrl_write_digital_x(Handle, Channel, Value);
}

slscReturnValue CScanLabSMCSDK::slsc_ctrl_write_digital_mask_x(size_t Handle, slsc_DigitalOutput Channel, uint16_t Mask, uint16_t Value)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_ctrl_write_digital_mask_x", std::to_string(Handle) + ", " + std::to_string((uint32_t)Channel) + ", " + std::to_string(Mask) + ", " + std::to_string(Value));

	return this->ptr_slsc_ctrl_write_digital_mask_x(Handle, Channel, Mask, Value);
}

slscReturnValue CScanLabSMCSDK::slsc_job_write_digital_x(size_t Handle, slsc_DigitalOutput Channel, uint16_t Value, double TimeDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_write_digital_x", std::to_string(Handle) + ", " + std::to_string((uint32_t)Channel) + ", " + std::to_string(Value) + ", " + std::to_string(TimeDelay));

	return this->ptr_slsc_job_write_digital_x(Handle, Channel, Value, TimeDelay);
}

slscReturnValue CScanLabSMCSDK::slsc_job_write_digital_mask_x(size_t Handle, slsc_DigitalOutput Channel, uint16_t Mask, uint16_t Value, double TimeDelay)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_write_digital_mask_x", std::to_string(Handle) + ", " + std::to_string((uint32_t)Channel) + ", " + std::to_string(Mask) + ", " + std::to_string(Value) + ", " + std::to_string(TimeDelay));

	return this->ptr_slsc_job_write_digital_mask_x(Handle, Channel, Mask, Value, TimeDelay);
}

slscReturnValue CScanLabSMCSDK::slsc_cfg_register_callback_job_calculated(size_t Handle, slsc_JobCallback Callback, void* Context)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_cfg_register_callback_job_calculated", std::to_string(Handle) + ", " + std::to_string(intptr_t(Callback)) + ", " + std::to_string(intptr_t(Context)));
	
	return this->ptr_slsc_cfg_register_callback_job_calculated(Handle, Callback, Context);
}

slscReturnValue CScanLabSMCSDK::slsc_job_begin_module(size_t Handle, size_t* JobID, const double* StartPosition, const char* ModuleFileName)
{
	if (m_pLogJournal.get() != nullptr)
		m_pLogJournal->logCall("slsc_job_begin_module", std::to_string(Handle) + ", &" + std::to_string(intptr_t(JobID)) + ", [" + std::to_string(StartPosition[0]) + ", " + std::to_string(StartPosition[1]) + "], " + std::string(ModuleFileName));

	return this->ptr_slsc_job_begin_module(Handle, JobID, StartPosition, ModuleFileName);
}

void CScanLabSMCSDK::setJournal(PScanLabSMCSDKJournal pLogJournal)
{
	m_pLogJournal = pLogJournal;
}

