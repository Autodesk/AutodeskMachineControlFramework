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


Abstract: This is the class declaration of CUIEnvironment

*/


#ifndef __LIBMCENV_UIENVIRONMENT
#define __LIBMCENV_UIENVIRONMENT

#include "libmcenv_interfaces.hpp"
#include "amc_logger.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_statesignalhandler.hpp"
#include "amc_ui_clientaction.hpp"
#include "amc_systemstate.hpp"

#include <vector>

// Parent classes
#include "libmcenv_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace AMC {
	class CUIHandler;
}

namespace LibMCEnv {
namespace Impl {


/*************************************************************************************************************************
 Class declaration of CUIEnvironment 
**************************************************************************************************************************/

class CUIEnvironment : public virtual IUIEnvironment, public virtual CBase {
private:

	AMC::PLogger m_pLogger;
	AMC::PStateMachineData m_pStateMachineData;
	AMC::PParameterHandler m_pClientVariableHandler;
	AMC::PStateSignalHandler m_pSignalHandler;
	AMC::PToolpathHandler m_pToolpathHandler;
	AMC::CUIHandler * m_pUIHandler;

	LibMCData::PStorage m_pStorage;
	LibMCData::PBuildJobHandler m_pBuildJobHandler;

	std::string m_sLogSubSystem;
	std::string m_sSenderUUID;
	std::string m_sSenderName;
	std::string m_sTestEnvironmentPath;
	std::string m_sSystemUserID;

	AMCCommon::CChrono m_Chrono;

	std::vector<AMC::PUIClientAction> m_ClientActions;

protected:

public:

	CUIEnvironment(AMC::PLogger pLogger, AMC::PToolpathHandler pToolpathHandler, LibMCData::PBuildJobHandler pBuildJobHandler, LibMCData::PStorage pStorage, AMC::PStateMachineData pStateMachineData, AMC::PStateSignalHandler pSignalHandler, AMC::CUIHandler * pUIHandler, const std::string& sSenderUUID, const std::string& sSenderName, AMC::PParameterHandler pClientVariableHandler, const std::string & sTestEnvironmentPath, const std::string & sSystemUserID);

	virtual ~CUIEnvironment();

	void ActivateModalDialog(const std::string& sDialogName) override;

	void CloseModalDialog() override;

	void ActivatePage(const std::string& sPageName) override;

	std::string RetrieveEventSender() override;

	std::string RetrieveEventSenderUUID() override;

	ISignalTrigger * PrepareSignal(const std::string & sMachineInstance, const std::string & sSignalName) override;

	std::string GetMachineState(const std::string & sMachineInstance) override;

	void LogMessage(const std::string & sLogString) override;

	void LogWarning(const std::string & sLogString) override;

	void LogInfo(const std::string & sLogString) override;


	std::string GetMachineParameter(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName) override;

	std::string GetMachineParameterAsUUID(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName) override;

	LibMCEnv_double GetMachineParameterAsDouble(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName) override;

	LibMCEnv_int64 GetMachineParameterAsInteger(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName) override;

	bool GetMachineParameterAsBool(const std::string& sMachineInstance, const std::string& sParameterGroup, const std::string& sParameterName) override;

	std::string GetUIProperty(const std::string& sElementPath, const std::string& sPropertyName) override;

	std::string GetUIPropertyAsUUID(const std::string& sElementPath, const std::string& sPropertyName) override;

	LibMCEnv_double GetUIPropertyAsDouble(const std::string& sElementPath, const std::string& sPropertyName) override;

	LibMCEnv_int64 GetUIPropertyAsInteger(const std::string& sElementPath, const std::string& sPropertyName) override;

	bool GetUIPropertyAsBool(const std::string& sElementPath, const std::string& sPropertyName) override;

	void SetUIProperty(const std::string& sElementPath, const std::string& sPropertyName, const std::string& sValue) override;

	void SetUIPropertyAsUUID(const std::string& sElementPath, const std::string& sPropertyName, const std::string& sValue) override;

	void SetUIPropertyAsDouble(const std::string& sElementPath, const std::string& sPropertyName, const LibMCEnv_double dValue) override;

	void SetUIPropertyAsInteger(const std::string& sElementPath, const std::string& sPropertyName, const LibMCEnv_int64 nValue) override;

	void SetUIPropertyAsBool(const std::string& sElementPath, const std::string& sPropertyName, const bool bValue) override;

	IImageData* CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	IImageData* LoadPNGImage(const LibMCEnv_uint64 nPNGDataBufferSize, const LibMCEnv_uint8* pPNGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv::eImagePixelFormat ePixelFormat) override;

	LibMCEnv_uint64 GetGlobalTimerInMilliseconds() override;

	void LogOut() override;

	void ShowHint(const std::string& sHint, const LibMCEnv_uint32 nTimeoutInMS) override;

	void ShowHintColored(const std::string& sHint, const LibMCEnv_uint32 nTimeoutInMS, const LibMCEnv::sColorRGB Color, const LibMCEnv::sColorRGB FontColor) override;

	void HideHint() override;

	std::string ShowMessageDlg(const std::string& sCaption, const std::string& sTitle, const LibMCEnv::eMessageDialogType eDialogType, const std::string& sYesEvent, const std::string& sNoEvent, const std::string& sCancelEvent) override;

	std::vector<AMC::PUIClientAction>& getClientActions();

	ITestEnvironment* GetTestEnvironment() override;

	IXMLDocument* CreateXMLDocument(const std::string& sRootNodeName, const std::string& sDefaultNamespace) override;

	IXMLDocument* ParseXMLString(const std::string& sXMLString) override;

	IXMLDocument* ParseXMLData(const LibMCEnv_uint64 nXMLDataBufferSize, const LibMCEnv_uint8* pXMLDataBuffer) override;

	IBuild* GetBuildJob(const std::string& sBuildUUID) override;

	IDiscreteFieldData2D* CreateDiscreteField2D(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const LibMCEnv_double dOriginX, const LibMCEnv_double dOriginY, const LibMCEnv_double dDefaultValue) override;

};

} // namespace Impl
} // namespace LibMCEnv

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCENV_UIENVIRONMENT
