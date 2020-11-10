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

		if (!pSignal->WaitForHandling(2000))
			pUIEnvironment->LogWarning("Could not start job");

	}

};


class CEvent_Connect : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "connect";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);

		auto pSignal = pUIEnvironment->PrepareSignal("printerconnection", "signal_connect");
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(10000))
			pUIEnvironment->LogWarning("Could not connect printer");

	}

};


class CEvent_Disconnect : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "disconnect";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);

		auto pSignal = pUIEnvironment->PrepareSignal("printerconnection", "signal_disconnect");
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(10000))
			pUIEnvironment->LogWarning("Could not disconnect printer");

	}

};


class CEvent_Home : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "home";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);

		auto pSignal = pUIEnvironment->PrepareSignal("printerconnection", "signal_dohoming");
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(10000))
			pUIEnvironment->LogWarning("Could not home printer");

	}

};


class CEvent_EmergencyStop : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "emergencystop";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);

		auto pSignal = pUIEnvironment->PrepareSignal("printerconnection", "signal_emergencystop");
		pSignal->Trigger();

		if (!pSignal->WaitForHandling(1000))
			pUIEnvironment->LogWarning("Could not perform emergency stop");

	}

};

class CEvent_ResetFatalError : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "resetfatalerror";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);

		auto pSignalMain = pUIEnvironment->PrepareSignal("main", "signal_resetfatalerror");
		pSignalMain->Trigger();

		if (!pSignalMain->WaitForHandling(10000))
			pUIEnvironment->LogWarning("Could not reset state machine main after fatal error");

		auto pSignalPrinterconnection = pUIEnvironment->PrepareSignal("printerconnection", "signal_resetfatalerror");
		pSignalPrinterconnection->Trigger();

		if (!pSignalPrinterconnection->WaitForHandling(10000))
			pUIEnvironment->LogWarning("Culd not reset state machine printerconnection after fatal error");

	}

};

class CEvent_ClearTemperatureAndFan : public virtual CEvent {

public:

	static std::string getEventName()
	{
		return "cleartemperatureandfan";
	}

	void Handle(LibMCEnv::PUIEnvironment pUIEnvironment) override
	{
		if (pUIEnvironment.get() == nullptr)
			throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDPARAM);
		
		auto pSignalTemp = pUIEnvironment->PrepareSignal("main", "signal_cleartemperatureandfan");
		pSignalTemp->Trigger();

		if (pSignalTemp->WaitForHandling(10000)) {
			if (pSignalTemp->GetBoolResult("success")) {
				pUIEnvironment->LogMessage("Clear temperature and fan speed successful. ");
			}
			else {
				pUIEnvironment->LogWarning("Clear temperature and fan speed failure. ");
			}
		}
		else {
			pUIEnvironment->LogWarning("Clear temperature and fan speed timeout!");
		}
	}

};



IEvent* CEventHandler::CreateEvent(const std::string& sEventName, LibMCEnv::PUIEnvironment pUIEnvironment)
{
	IEvent* pEventInstance = nullptr;
	if (createEventInstanceByName<CEvent_StartBuild>(sEventName, pEventInstance))
		return pEventInstance;

	if (createEventInstanceByName<CEvent_Connect>(sEventName, pEventInstance))
		return pEventInstance;

	if (createEventInstanceByName<CEvent_Disconnect>(sEventName, pEventInstance))
		return pEventInstance;

	if (createEventInstanceByName<CEvent_Home>(sEventName, pEventInstance))
		return pEventInstance;

	if (createEventInstanceByName<CEvent_EmergencyStop>(sEventName, pEventInstance))
		return pEventInstance;

	if (createEventInstanceByName<CEvent_ResetFatalError>(sEventName, pEventInstance))
		return pEventInstance;

	if (createEventInstanceByName<CEvent_ClearTemperatureAndFan>(sEventName, pEventInstance))
		return pEventInstance;

	throw ELibMCUIInterfaceException(LIBMCUI_ERROR_INVALIDEVENTNAME);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
