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

#define __AMCIMPL_UI_MENUITEM
#define __AMCIMPL_UI_TOOLBARITEM
#define __AMCIMPL_API_CONSTANTS
#define __AMCIMPL_UI_PAGE
#define __AMCIMPL_UI_DIALOG
#define __AMCIMPL_UI_MODULE

#include "amc_ui_handler.hpp"
#include "amc_ui_menuitem.hpp"
#include "amc_ui_toolbaritem.hpp"
#include "amc_ui_page.hpp"
#include "amc_ui_dialog.hpp"
#include "amc_ui_module_contentitem_form.hpp"
#include "amc_parameterhandler.hpp"

#include "amc_ui_module.hpp"
#include "amc_ui_modulefactory.hpp"
#include "amc_statemachinedata.hpp"
#include "amc_jsonwriter.hpp"
#include "amc_ui_module_item.hpp"
#include "amc_logger.hpp"
#include "amc_statesignalhandler.hpp"

#include "amc_api_constants.hpp"

#include "libmc_interfaceexception.hpp"
#include "libmc_exceptiontypes.hpp"

#include "libmcenv_uienvironment.hpp"

#include "libmcui_dynamic.hpp"

#include "PugiXML/pugixml.hpp"

#include <sstream>

using namespace AMC;

CUIHandleEventResponse::CUIHandleEventResponse(uint32_t nErrorCode, const std::string& sErrorMessage, const std::string& sPageToActivate, bool bCloseModalDialog, const std::string& sDialogToShow)
    : m_nErrorCode (nErrorCode), m_sErrorMessage (sErrorMessage), m_sPageToActivate (sPageToActivate), m_sDialogToShow (sDialogToShow), m_bCloseModalDialog (bCloseModalDialog)
{

}

uint32_t CUIHandleEventResponse::getErrorCode()
{
    return m_nErrorCode;
}

std::string CUIHandleEventResponse::getErrorMessage()
{
    return m_sErrorMessage;
}

std::string CUIHandleEventResponse::getPageToActivate()
{
    return m_sPageToActivate;
}

std::string CUIHandleEventResponse::getDialogToShow()
{
    return m_sDialogToShow;
}

bool CUIHandleEventResponse::hasPageToActivate()
{
    return !m_sPageToActivate.empty();
}

bool CUIHandleEventResponse::hasDialogToShow()
{
    return !m_sDialogToShow.empty();
}

bool CUIHandleEventResponse::closeModalDialog()
{
    return m_bCloseModalDialog;
}


CUIHandler::CUIHandler(PStateMachineData pStateMachineData, PStateSignalHandler pSignalHandler, LibMCEnv::PWrapper pEnvironmentWrapper, PLogger pLogger)
    : m_dLogoAspectRatio (1.0), 
    m_pStateMachineData(pStateMachineData),
    m_pEnvironmentWrapper (pEnvironmentWrapper),
    m_pSignalHandler (pSignalHandler),
    m_pLogger(pLogger)
{
    if (pStateMachineData.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pEnvironmentWrapper.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pSignalHandler.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
    if (pLogger.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

}

CUIHandler::~CUIHandler()
{

}

std::string CUIHandler::getAppName()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);

	return m_sAppName;
}

std::string CUIHandler::getCopyrightString()
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);

    return m_sCopyrightString;
}

void CUIHandler::addMenuItem_Unsafe(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage)
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);

    auto pPage = findPage (sTargetPage);    
    m_MenuItems.push_back(std::make_shared<CUIMenuItem> (sID, sIcon, sCaption, pPage));
}

void CUIHandler::addToolbarItem_Unsafe(const std::string& sID, const std::string& sIcon, const std::string& sCaption, const std::string& sTargetPage)
{
    std::lock_guard<std::mutex> lockGuard(m_Mutex);
    
    auto pPage = findPage(sTargetPage);
    m_ToolbarItems.push_back(std::make_shared<CUIToolbarItem> (sID, sIcon, sCaption, pPage));
}

PUIPage CUIHandler::findPage(const std::string& sName)
{
    auto iter = m_Pages.find(sName);

    if (iter == m_Pages.end())
        throw ELibMCCustomException(LIBMC_ERROR_PAGENOTFOUND, sName);

    return iter->second;
}


void CUIHandler::writeConfigurationToJSON(CJSONWriter& writer)
{
    writer.addString(AMC_API_KEY_UI_APPNAME, m_sAppName);
    writer.addString(AMC_API_KEY_UI_COPYRIGHT, m_sCopyrightString);
    writer.addString(AMC_API_KEY_UI_MAINPAGE, m_pMainPage->getName());
    writer.addString(AMC_API_KEY_UI_LOGOUUID, m_sLogoUUID);
    writer.addDouble(AMC_API_KEY_UI_LOGOASPECTRATIO, m_dLogoAspectRatio);

    CJSONWriterObject colorsObject(writer);
    for (auto color : m_Colors) {

        std::stringstream sColorStream;
        uint32_t nRed = color.second & 0xff;
        uint32_t nGreen = (color.second >> 8) & 0xff;
        uint32_t nBlue = (color.second >> 16) & 0xff;

        sColorStream << "#" << std::setfill('0') << std::setw(2) << std::hex << nRed << nGreen << nBlue;

        colorsObject.addString(color.first, sColorStream.str());
    }

    writer.addObject(AMC_API_KEY_UI_COLORS, colorsObject);

}

void CUIHandler::writeStateToJSON(CJSONWriter& writer)
{
	CJSONWriterArray menuItems(writer);

	for (auto iter : m_MenuItems) {
		CJSONWriterObject menuItem(writer);
		menuItem.addString(AMC_API_KEY_UI_ID, iter->getID ());
		menuItem.addString(AMC_API_KEY_UI_ICON, iter->getIcon ());
		menuItem.addString(AMC_API_KEY_UI_CAPTION, iter->getCaption ());
		menuItem.addString(AMC_API_KEY_UI_TARGETPAGE, iter->getPage()->getName ());
		menuItems.addObject(menuItem);
	}

	writer.addArray(AMC_API_KEY_UI_MENUITEMS, menuItems);

	CJSONWriterArray toolbarItems(writer);

	for (auto iter : m_ToolbarItems) {
		CJSONWriterObject toolbarItem(writer);
		toolbarItem.addString(AMC_API_KEY_UI_ID, iter->getID());
		toolbarItem.addString(AMC_API_KEY_UI_ICON, iter->getIcon());
		toolbarItem.addString(AMC_API_KEY_UI_CAPTION, iter->getCaption());
        toolbarItem.addString(AMC_API_KEY_UI_TARGETPAGE, iter->getPage()->getName());
        toolbarItems.addObject(toolbarItem);
	}
	writer.addArray(AMC_API_KEY_UI_TOOLBARITEMS, toolbarItems);

    CJSONWriterArray pages(writer);
    for (auto iter : m_Pages) {
        CJSONWriterObject page(writer);
        page.addString(AMC_API_KEY_UI_PAGENAME, iter.second->getName());

        CJSONWriterArray modules(writer);
        iter.second->writeModulesToJSON (writer, modules);

        page.addArray(AMC_API_KEY_UI_MODULES, modules);

        pages.addObject(page);
    }
    writer.addArray(AMC_API_KEY_UI_PAGES, pages);


    CJSONWriterArray dialogs(writer);
    for (auto iter : m_Dialogs) {
        CJSONWriterObject dialog(writer);
        dialog.addString(AMC_API_KEY_UI_DIALOGNAME, iter.second->getName());
        dialog.addString(AMC_API_KEY_UI_DIALOGTITLE, iter.second->getTitle());

        CJSONWriterArray modules(writer);
        iter.second->writeModulesToJSON(writer, modules);

        dialog.addArray(AMC_API_KEY_UI_MODULES, modules);

        dialogs.addObject(dialog);
    }
    writer.addArray(AMC_API_KEY_UI_DIALOGS, dialogs);

}

PUIPage CUIHandler::addPage_Unsafe(const std::string& sName)
{
    if (sName.empty ())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPAGENAME);

    auto iIterator = m_Pages.find (sName);
    if (iIterator != m_Pages.end())
        throw ELibMCInterfaceException(LIBMC_ERROR_DUPLICATEPAGE);

    auto pPage = std::make_shared<CUIPage> (sName, this);
    m_Pages.insert (std::make_pair (sName, pPage));

    return pPage;
}


PUIDialog CUIHandler::addDialog_Unsafe(const std::string& sName, const std::string& sTitle)
{
    if (sName.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDDIALOGNAME);

    auto iIterator = m_Dialogs.find(sName);
    if (iIterator != m_Dialogs.end())
        throw ELibMCCustomException(LIBMC_ERROR_DUPLICATEDIALOG, sName);

    auto pDialog = std::make_shared<CUIDialog>(sName, sTitle, this);
    m_Dialogs.insert(std::make_pair(sName, pDialog));

    return pDialog;

}

PUIDialog CUIHandler::findDialog(const std::string& sName)
{
    auto iter = m_Dialogs.find(sName);

    if (iter == m_Dialogs.end())
        throw ELibMCCustomException(LIBMC_ERROR_DIALOGNOTFOUND, sName);

    return iter->second;

}



void CUIHandler::loadFromXML(pugi::xml_node& xmlNode, PResourcePackage pResourcePackage, const std::string& sUILibraryPath, LibMCData::PBuildJobHandler pBuildJobHandler)
{
    if (pResourcePackage.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    m_sAppName = "";
    m_sCopyrightString = "";
    m_pCoreResourcePackage = pResourcePackage;

    try {
        auto pUIPluginWrapper = LibMCUI::CWrapper::loadLibrary(sUILibraryPath);
        auto pUIEventHandler = pUIPluginWrapper->CreateEventHandler();

        pUIPluginWrapper->InjectComponent ("LibMCEnv", m_pEnvironmentWrapper->GetSymbolLookupMethod ());

        m_pUIPluginWrapper = pUIPluginWrapper;
        m_pUIEventHandler = pUIEventHandler;
    }
    catch (std::exception& E) {
        throw ELibMCInterfaceException(LIBMC_ERROR_COULDNOTLOADUILIBRARY, E.what ());
    }

    m_ToolbarItems.clear();
    m_MenuItems.clear();

    auto appnameAttrib = xmlNode.attribute("appname");
    if (appnameAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGAPPNAME);
    m_sAppName = appnameAttrib.as_string();

    auto copyrightAttrib = xmlNode.attribute("copyright");
    if (copyrightAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCOPYRIGHT);
    m_sCopyrightString = copyrightAttrib.as_string();

    auto mainpageAttrib = xmlNode.attribute("mainpage");
    if (mainpageAttrib.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMAINPAGE);
    std::string sMainPage(mainpageAttrib.as_string());

    auto colorsNode = xmlNode.child("colors");
    if (!colorsNode.empty()) {

        auto colorNodes = colorsNode.children("color");
        for (pugi::xml_node colorNode : colorNodes) {
            auto nameColorAttrib = colorNode.attribute("name");
            auto redColorAttrib = colorNode.attribute("red");
            auto greenColorAttrib = colorNode.attribute("green");
            auto blueColorAttrib = colorNode.attribute("blue");

            std::string sColorName = nameColorAttrib.as_string();
            if (sColorName.empty ())
                throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCOLORNAME);
            if (redColorAttrib.empty())
                throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCOLORREDCHANNEL);
            if (greenColorAttrib.empty())
                throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCOLORGREENCHANNEL);
            if (blueColorAttrib.empty())
                throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGCOLORBLUECHANNEL);


            double dRed = redColorAttrib.as_double(-1.0);
            double dGreen = greenColorAttrib.as_double(-1.0);
            double dBlue = blueColorAttrib.as_double(-1.0);
            if ((dRed < 0.0) || (dRed > 1.0))
                throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDCOLORREDCHANNEL);
            if ((dGreen < 0.0) || (dGreen > 1.0))
                throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDCOLORGREENCHANNEL);
            if ((dBlue < 0.0) || (dBlue > 1.0))
                throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDCOLORBLUECHANNEL);

            int32_t nRed = (int32_t)(dRed * 255.0f);
            int32_t nGreen = (int32_t)(dGreen * 255.0f);
            int32_t nBlue = (int32_t)(dBlue * 255.0f);
            if (nRed < 0) nRed = 0;
            if (nGreen < 0) nGreen = 0;
            if (nBlue < 0) nBlue = 0;
            if (nRed > 255) nRed = 255;
            if (nGreen > 255) nGreen = 255;
            if (nBlue > 255) nBlue = 255;

            uint32_t nColor = (nRed + nGreen * 256 + nBlue * 65536);
            m_Colors.insert(std::make_pair (sColorName, nColor));

        }

    }


    auto logoNode = xmlNode.child("logo");
    if (!logoNode.empty()) {

        auto resourceAttrib = logoNode.attribute("resource");
        if (resourceAttrib.empty ())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGLOGORESOURCE);

        auto pResourceEntry = pResourcePackage->findEntryByName(resourceAttrib.as_string(), true);
        m_sLogoUUID = pResourceEntry->getUUID ();

        auto aspectratioAttrib = logoNode.attribute("aspectratio");
        if (!aspectratioAttrib.empty()) {
            m_dLogoAspectRatio = aspectratioAttrib.as_float();
        }
        else {
            m_dLogoAspectRatio = 1.0;
        }

        if ((m_dLogoAspectRatio < AMC_UI_IMAGE_MINASPECTRATIO) || (m_dLogoAspectRatio > AMC_UI_IMAGE_MAXASPECTRATIO))
            throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDASPECTRATIO, std::to_string(m_dLogoAspectRatio));
    }

    auto pageNodes = xmlNode.children("page");
    for (pugi::xml_node pageNode : pageNodes) {

        auto pageNameAttrib = pageNode.attribute("name");
        if (pageNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPAGENAME);
        std::string sPageName(pageNameAttrib.as_string());

        auto pPage = addPage_Unsafe(sPageName);

        auto pageChildren = pageNode.children();
        for (pugi::xml_node pageChild : pageChildren) {
            
            auto pModuleEnvironment = std::make_shared<CUIModuleEnvironment>(m_pStateMachineData, m_pCoreResourcePackage, pBuildJobHandler, pPage.get());
            auto pModule = CUIModuleFactory::createModule(pageChild, sPageName, pModuleEnvironment);
            pPage->addModule(pModule);

        }

    }


    auto dialogNodes = xmlNode.children("dialog");
    for (pugi::xml_node dialogNode : dialogNodes) {

        auto dialogNameAttrib = dialogNode.attribute("name");
        if (dialogNameAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGPAGENAME);
        std::string sDialogName(dialogNameAttrib.as_string());

        auto dialogTitleAttrib = dialogNode.attribute("title");
        std::string sDialogTitle(dialogTitleAttrib.as_string());

        auto pDialog = addDialog_Unsafe(sDialogName, sDialogTitle);

        auto dialogChildren = dialogNode.children();
        for (pugi::xml_node dialogChild : dialogChildren) {

            auto pModuleEnvironment = std::make_shared<CUIModuleEnvironment>(m_pStateMachineData, m_pCoreResourcePackage, pBuildJobHandler, pDialog.get());
            auto pModule = CUIModuleFactory::createModule(dialogChild, sDialogName, pModuleEnvironment);
            pDialog->addModule(pModule);

        }

    }


    auto menuNode = xmlNode.child("menu");
    if (menuNode.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUNODE);

    auto menuItems = menuNode.children("item");
    for (pugi::xml_node menuItem : menuItems) {
        auto idAttrib = menuItem.attribute("id");
        if (idAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUITEMID);

        auto iconAttrib = menuItem.attribute("icon");
        if (iconAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUITEMICON);

        auto captionAttrib = menuItem.attribute("caption");
        if (captionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGMENUITEMCAPTION);

        auto targetPageAttrib = menuItem.attribute("targetpage");
        if (targetPageAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTARGETPAGE);

        addMenuItem_Unsafe(idAttrib.as_string(), iconAttrib.as_string(), captionAttrib.as_string(), targetPageAttrib.as_string());
    }

    auto toolbarNode = xmlNode.child("toolbar");
    if (toolbarNode.empty())
        throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARNODE);

    auto toolbarItems = toolbarNode.children("item");
    for (pugi::xml_node toolbarItem : toolbarItems) {
        auto idAttrib = toolbarItem.attribute("id");
        if (idAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARITEMID);

        auto iconAttrib = toolbarItem.attribute("icon");
        if (iconAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARITEMICON);

        auto captionAttrib = toolbarItem.attribute("caption");
        if (captionAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTOOLBARITEMCAPTION);

        auto targetPageAttrib = toolbarItem.attribute("targetpage");
        if (targetPageAttrib.empty())
            throw ELibMCInterfaceException(LIBMC_ERROR_MISSINGTARGETPAGE);

        addToolbarItem_Unsafe(idAttrib.as_string(), iconAttrib.as_string(), captionAttrib.as_string(), targetPageAttrib.as_string());
    }

    m_pMainPage = findPage (sMainPage);

    // Update all cross-references!
    for (auto pPage : m_Pages)
        pPage.second->configurePostLoading();
    for (auto pDialog : m_Dialogs)
        pDialog.second->configurePostLoading();

}

PUIModuleItem CUIHandler::findModuleItem(const std::string& sUUID)
{
    for (auto pPage : m_Pages) {
        auto pModuleItem = pPage.second->findModuleItemByUUID(sUUID);
        if (pModuleItem.get() != nullptr)
            return pModuleItem;
    }

    for (auto pDialog : m_Dialogs) {
        auto pModuleItem = pDialog.second->findModuleItemByUUID(sUUID);
        if (pModuleItem.get() != nullptr)
            return pModuleItem;
    }

    return nullptr;
}

PUIPage CUIHandler::findPageOfModuleItem(const std::string& sUUID)
{
    for (auto pPage : m_Pages) {
        auto pModuleItem = pPage.second->findModuleItemByUUID(sUUID);
        if (pModuleItem.get() != nullptr)
            return pPage.second;
    }

    for (auto pDialog : m_Dialogs) {
        auto pModuleItem = pDialog.second->findModuleItemByUUID(sUUID);
        if (pModuleItem.get() != nullptr)
            return pDialog.second;
    }

    return nullptr;
}

PResourcePackage CUIHandler::getCoreResourcePackage()
{
    if (m_pCoreResourcePackage.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_NOCORERESOURCEPACKAGE);

    return m_pCoreResourcePackage;

}

template <class C> std::shared_ptr<C> mapInternalUIEnvInstance(std::shared_ptr<LibMCEnv::Impl::IBase> pImplInstance, LibMCEnv::PWrapper pWrapper)
{
    if (pWrapper.get() == nullptr)
        throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

    auto pExternalInstance = std::make_shared <C>(pWrapper.get(), (LibMCEnv::Impl::IBase*) (pImplInstance.get()));
    pImplInstance->IncRefCount();
    return pExternalInstance;
}


void CUIHandler::ensureUIEventExists(const std::string& sEventName)
{
    std::string sSenderUUID = AMCCommon::CUtils::createUUID();
    std::string sContextUUID = AMCCommon::CUtils::createUUID();

    LibMCEnv::Impl::PUIEnvironment pInternalUIEnvironment = std::make_shared<LibMCEnv::Impl::CUIEnvironment>(m_pLogger, m_pStateMachineData, m_pSignalHandler, sSenderUUID, nullptr);
    auto pExternalEnvironment = mapInternalUIEnvInstance<LibMCEnv::CUIEnvironment>(pInternalUIEnvironment, m_pEnvironmentWrapper);

    // Create event to see if it exists.
    try {
        auto pEvent = m_pUIEventHandler->CreateEvent(sEventName, pExternalEnvironment);
        pEvent = nullptr;
    }
    catch (LibMCUI::ELibMCUIException& E) {
        throw ELibMCCustomException(LIBMC_ERROR_EVENTNOTFOUND, sEventName + "/" + E.what ());
    }
    catch (...) {
        throw;
    }

}


CUIHandleEventResponse CUIHandler::handleEvent(const std::string& sEventName, const std::string& sSenderUUID, const std::string& sContextUUID, const std::string& sFormValueJSON, PParameterHandler pClientVariableHandler)
{

    LibMCEnv::Impl::PUIEnvironment pInternalUIEnvironment = std::make_shared<LibMCEnv::Impl::CUIEnvironment>(m_pLogger, m_pStateMachineData, m_pSignalHandler, sSenderUUID, pClientVariableHandler);
    auto pExternalEnvironment = mapInternalUIEnvInstance<LibMCEnv::CUIEnvironment>(pInternalUIEnvironment, m_pEnvironmentWrapper);

    uint32_t nErrorCode = 0;
    std::string sErrorMessage;

    try {

        auto pPage = findPageOfModuleItem(sSenderUUID);
        if (pPage.get() == nullptr)
            throw ELibMCCustomException(LIBMC_ERROR_COULDNOTFINDEVENTSENDERPAGE, sEventName + "/" + sSenderUUID);

        auto pModuleItem = pPage->findModuleItemByUUID(sSenderUUID);
        if (pModuleItem.get() == nullptr)
            throw ELibMCCustomException(LIBMC_ERROR_COULDNOTFINDEVENTSENDER, sEventName + "/" + sSenderUUID);

        auto pEvent = m_pUIEventHandler->CreateEvent(sEventName, pExternalEnvironment);

        if ((pClientVariableHandler.get() != nullptr) && (!sFormValueJSON.empty())) {

            rapidjson::Document document;
            document.Parse(sFormValueJSON.c_str());
            if (!document.IsObject())
                throw ELibMCCustomException(LIBMC_ERROR_COULDNOTPARSEEVENTPARAMETERS, sEventName);


            for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
                itr != document.MemberEnd(); ++itr)
            {
                if (!itr->name.IsString())
                    throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTPARAMETERS, sEventName);
                std::string sEntityUUID = itr->name.GetString();
                std::string sFormValue;

                if (itr->value.IsString()) {
                    sFormValue = itr->value.GetString();
                }
                else if (itr->value.IsInt64()) {
                    sFormValue = std::to_string(itr->value.GetInt64());
                }
                else if (itr->value.IsBool()) {
                    sFormValue = std::to_string(itr->value.GetBool());
                }
                else if (itr->value.IsDouble()) {
                    sFormValue = std::to_string(itr->value.GetDouble());
                }
                else
                    throw ELibMCCustomException(LIBMC_ERROR_INVALIDEVENTPARAMETERS, sEventName);



                auto pFormItem = pPage->findModuleItemByUUID(sEntityUUID);
                auto pForm = std::dynamic_pointer_cast<CUIModule_ContentForm> (pFormItem);
                if (pForm.get() != nullptr) {

                    auto pFormEntity = pForm->findEntityByUUID(sEntityUUID);
                    if (pFormEntity.get() == nullptr)
                        throw ELibMCCustomException(LIBMC_ERROR_FORMENTITYNOTFOUND, sEventName + "/" + sEntityUUID);

                    auto pGroup = pClientVariableHandler->findGroup(pFormEntity->getElementPath(), true);
                    pGroup->setParameterValueByName("value", sFormValue);                     

                }


            }

        }

        pEvent->Handle(pExternalEnvironment);

    } 
    catch (LibMCUI::ELibMCUIException & UIException) {
        nErrorCode = UIException.getErrorCode();
        sErrorMessage = UIException.what();
    }
    catch (ELibMCInterfaceException & Exception) {
        nErrorCode = Exception.getErrorCode();
        sErrorMessage = Exception.what();
    }
    catch (std::exception & StdException) {
        nErrorCode = LIBMC_ERROR_COULDNOTHANDLEEVENT;
        sErrorMessage = StdException.what();
    }
 


    return CUIHandleEventResponse (nErrorCode, sErrorMessage, pInternalUIEnvironment->getPageToActivate(), pInternalUIEnvironment->getCloseModalDialog (),  pInternalUIEnvironment->getModalDialogToShow ());
       

}


