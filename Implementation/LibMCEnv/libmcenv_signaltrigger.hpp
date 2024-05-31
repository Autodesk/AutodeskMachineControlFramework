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

#ifndef __LIBMCENV_SIGNALTRIGGER
#define __LIBMCENV_SIGNALTRIGGER

#include "libmcenv_interfaces.hpp"
#include "amc_statesignalhandler.hpp"
#include "amc_parametergroup.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "common_chrono.hpp"

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CSignalTrigger 
**************************************************************************************************************************/

class CSignalTrigger : public virtual ISignalTrigger, public virtual CBase {
private:

	AMC::PStateSignalHandler m_pSignalHandler;
	std::string m_sInstanceName;
	std::string m_sSignalName;
	std::string m_sTriggeredUUID;

	AMC::PParameterGroup m_pParameterGroup;
	AMC::PParameterGroup m_pResultGroup;

	AMCCommon::PChrono m_pGlobalChrono;


protected:


public:

	CSignalTrigger(AMC::PStateSignalHandler pSignalHandler, std::string sInstanceName, std::string sSignalName, AMCCommon::PChrono pGlobalChrono);

	virtual ~CSignalTrigger();


	bool CanTrigger() override;

	void Trigger() override;

	bool WaitForHandling(const LibMCEnv_uint32 nTimeOut);

	std::string GetName() override;

	std::string GetStateMachine() override;

	void SetString(const std::string & sName, const std::string & sValue) override;

	void SetUUID(const std::string& sName, const std::string& sValue) override;

	void SetDouble(const std::string & sName, const LibMCEnv_double dValue) override;

	void SetInteger(const std::string & sName, const LibMCEnv_int64 nValue) override;

	void SetBool(const std::string & sName, const bool bValue) override;

	std::string GetStringResult(const std::string & sName) override;

	std::string GetUUIDResult(const std::string& sName) override;

	LibMCEnv_double GetDoubleResult(const std::string & sName) override;

	LibMCEnv_int64 GetIntegerResult(const std::string & sName) override;

	bool GetBoolResult(const std::string & sName) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_SIGNALTRIGGER
