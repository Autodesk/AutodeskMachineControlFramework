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


#ifndef __LIBMCENV_STATEENVIRONMENT
#define __LIBMCENV_STATEENVIRONMENT

#include "libmcenv_interfaces.hpp"
#include "amc_systemstate.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CStateEnvironment 
**************************************************************************************************************************/

class CStateEnvironment : public virtual IStateEnvironment, public virtual CBase {
private:

	std::string m_sNextState;
	std::string m_sInstanceName;

	AMC::PSystemState m_pSystemState;
	AMC::PParameterHandler m_pParameterHandler;

	
protected:


public:

	CStateEnvironment(AMC::PSystemState pSystemState, AMC::PParameterHandler pParameterHandler, std::string sInstanceName);

	ISignalTrigger* CreateSignal(const std::string& sMachineInstance, const std::string& sSignalName) override;

	bool WaitForSignal(const std::string& sSignalName, const LibMCEnv_uint32 nTimeOut, ISignalHandler*& pHandlerInstance) override;

	void LoadToolpath(const std::string& sToolpathUUID) override;

	void UnloadToolpath(const std::string& sToolpathUUID) override;

	void UnloadAllToolpathes() override;

	bool ToolpathIsLoaded(const std::string& sToolpathUUID) override;

	IToolpathAccessor* CreateToolpathAccessor(const std::string& sToolpathUUID) override;

	void GetDriverLibrary(const std::string& sDriverName, std::string& sDriverType, LibMCEnv_pvoid& pDriverLookup) override;

	void CreateDriverAccess(const std::string& sDriverName, LibMCEnv_pvoid& pDriverHandle) override;

	void SetNextState(const std::string & sStateName) override;

	std::string getNextState ();

	void LogMessage(const std::string& sLogString) override;

	void LogWarning(const std::string& sLogString) override;

	void LogInfo(const std::string& sLogString) override;

	void Sleep(const LibMCEnv_uint32 nDelay) override;

	bool CheckForTermination() override;

	void StoreString(const std::string& sName, const std::string& sValue) override;

	void StoreUUID(const std::string& sName, const std::string& sValue) override;

	void StoreInteger(const std::string& sName, const LibMCEnv_int64 nValue) override;

	void StoreDouble(const std::string& sName, const LibMCEnv_double dValue) override;

	void StoreBool(const std::string& sName, const bool bValue) override;

	void StoreSignal(const std::string& sName, ISignalHandler* pHandler) override;

	std::string RetrieveString(const std::string& sName) override;

	std::string RetrieveUUID(const std::string& sName) override;

	LibMCEnv_int64 RetrieveInteger(const std::string& sName) override;

	LibMCEnv_double RetrieveDouble(const std::string& sName) override;

	bool RetrieveBool(const std::string& sName) override;

	ISignalHandler* RetrieveSignal(const std::string& sName) override;

	void ClearStoredValue(const std::string& sName) override;

	void SetStringParameter(const std::string& sParameterGroup, const std::string& sParameterName, const std::string& sValue) override;

	void SetUUIDParameter(const std::string& sParameterGroup, const std::string& sParameterName, const std::string& sValue) override;

	void SetDoubleParameter(const std::string& sParameterGroup, const std::string& sParameterName, const LibMCEnv_double dValue) override;

	void SetIntegerParameter(const std::string& sParameterGroup, const std::string& sParameterName, const LibMCEnv_int64 nValue) override;

	void SetBoolParameter(const std::string& sParameterGroup, const std::string& sParameterName, const bool bValue) override;

	std::string GetStringParameter(const std::string& sParameterGroup, const std::string& sParameterName) override;

	std::string GetUUIDParameter(const std::string& sParameterGroup, const std::string& sParameterName) override;

	LibMCEnv_double GetDoubleParameter(const std::string& sParameterGroup, const std::string& sParameterName) override;

	LibMCEnv_int64 GetIntegerParameter(const std::string& sParameterGroup, const std::string& sParameterName) override;

	bool GetBoolParameter(const std::string& sParameterGroup, const std::string& sParameterName) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_STATEENVIRONMENT
