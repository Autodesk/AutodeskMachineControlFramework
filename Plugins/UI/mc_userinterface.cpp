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

#include "libmcui_interfaceexception.hpp"
#include "libmcui_eventhandler.hpp"
#include "libmcui_event.hpp"

using namespace LibMCUI::Impl;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif



/*************************************************************************************************************************
 Class declaration of CEventHandler
**************************************************************************************************************************/

class CEvent_StartBuild : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "startbuild";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);

		auto sJobUUID = pUIEnvironment->GetEventContext();
		auto pSignal = pUIEnvironment->PrepareSignal("main", "signal_startjob");
		pSignal->SetString("jobuuid", sJobUUID);
		pSignal->SetString("jobname", "testjob");
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(200))
			throw std::runtime_error("could not start job");
		
	}

};


IEvent* CEventHandler::CreateEvent(const std::string& sEventName, LibMCEnv::PUIEnvironment pUIEnvironment)
{
	IEvent* pEventInstance = nullptr;
	if (createEventInstanceByName<CEvent_StartBuild>(sEventName, pEventInstance))
		return pEventInstance;

	throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDEVENTNAME);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
