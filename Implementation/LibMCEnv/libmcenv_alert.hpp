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


Abstract: This is the class declaration of CAlert

*/


#ifndef __LIBMCENV_ALERT
#define __LIBMCENV_ALERT

#include "libmcenv_interfaces.hpp"
#include "amc_languagestring.hpp"
#include "amc_logger.hpp"

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.
#include "libmcdata_dynamic.hpp"
#include <mutex>

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CAlert 
**************************************************************************************************************************/

class CAlert : public virtual IAlert, public virtual CBase {
private:

	// ATTENTION: Alert Session is not thread safe.
	// So the Alert session needs to be private and surrounded by a mutex
	std::mutex m_AlertMutex;

	LibMCData::PDataModel m_pDataModel;
	LibMCData::PAlert m_pAlertData;
	AMC::PLogger m_pLogger;
	const std::string& m_sLogInstance;

	std::string m_sCurrentUserUUID;
	bool m_bUserContextExists;


public:

	static CAlert* makeFrom(CAlert * pAlert);

	static std::shared_ptr<CAlert> makeSharedFrom(CAlert* pAlert);

	CAlert(LibMCData::PDataModel pDataModel, const std::string & sUUID, const std::string & sCurrentUserUUID, AMC::PLogger pLogger, const std::string & sLogInstance);

	virtual ~CAlert();

	std::string GetUUID() override;

	bool IsActive() override;

	LibMCEnv::eAlertLevel GetAlertLevel() override;

	std::string GetIdentifier() override;

	std::string GetReadableContextInformation() override;

	bool NeedsAcknowledgement() override;

	bool HasBeenAcknowledged() override;

	void GetAcknowledgementInformation(std::string & sUserUUID, std::string & sUserComment, std::string & sAckTime) override;
	
	void AcknowledgeForUser(const std::string& sUserUUID, const std::string& sUserComment) override;

	void AcknowledgeAlertForCurrentUser(const std::string& sUserComment) override;

	void DeactivateAlert() override;

	LibMCData::PDataModel getDataModel ();

	std::string getCurrentUserUUID ();

	AMC::PLogger getLogger();

	std::string getLogInstance();

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_ALERT
