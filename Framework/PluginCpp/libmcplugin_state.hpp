/*++

Copyright (C) 2020 Autodesk Inc.

All rights reserved.

Abstract: This is the class declaration of CState

*/


#ifndef __LIBMCPLUGIN_STATE
#define __LIBMCPLUGIN_STATE

#include "libmcplugin_statefactory.hpp"
#include "libmcplugin_interfaceexception.hpp"
#include "libmcplugin_interfaces.hpp"
#include "libmcplugin_data.hpp"
#include "libmcenv_drivercast.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4250)
#endif

#include <functional>
#include <map>

// Parent classes
#include "libmcplugin_base.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

// Include custom headers here.


namespace LibMCPlugin {
namespace Impl {


typedef  std::function <void(LibMCEnv::PStateEnvironment, PPluginData)> PluginCallbackFunction;


/*************************************************************************************************************************
 Class declaration of CState 
**************************************************************************************************************************/

template<class DataClass> class CState : public virtual IState, public virtual CBase {
private:

protected:

	PluginCallbackFunction m_Callback;
	std::shared_ptr<DataClass> m_pPluginData;

public:

	CState(const std::string& sClassStateName, const std::string& sPassedStateName, PPluginData pPluginData, PluginCallbackFunction callback)
		: m_Callback (callback)
	{
		if (sClassStateName != sPassedStateName)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

		if (pPluginData.get () == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		m_pPluginData = std::dynamic_pointer_cast<DataClass> (pPluginData);
		if (m_pPluginData == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDCAST);

	}

	CState(const std::string& sClassStateName, const std::string& sPassedStateName, PPluginData pPluginData)
		: m_Callback(nullptr)
	{
		if (sClassStateName != sPassedStateName)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);

		if (pPluginData.get() == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDPARAM);

		m_pPluginData = std::dynamic_pointer_cast<DataClass> (pPluginData);
		if (m_pPluginData == nullptr)
			throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDCAST);

	}

	virtual ~CState()
	{

	}

	virtual void Execute(LibMCEnv::PStateEnvironment pStateEnvironment) override
	{
		if (m_Callback != nullptr)
			m_Callback (pStateEnvironment, m_pPluginData);
	}
};


class CInternalStateFactoryInterface {
public:
	virtual IState* createStateInstance(PPluginData pPluginData) = 0;
	virtual std::string getStateName() = 0;
};

template<class DataClass> class CInternalStateFactory : public CInternalStateFactoryInterface {
private:
	std::string m_sStateName;
	PluginCallbackFunction m_callback;
public:
	CInternalStateFactory(const std::string& sStateName, PluginCallbackFunction callback)
		: m_callback(callback), m_sStateName(sStateName)
	{

	}

	IState* createStateInstance(PPluginData pPluginData) override
	{
		return new CState<DataClass>(m_sStateName, m_sStateName, pPluginData, m_callback);
	}

	std::string getStateName() override
	{
		return m_sStateName;
	}


};

typedef std::shared_ptr<CInternalStateFactoryInterface> PInternalStateFactoryInterface;

class CInternalStateFactoryList {
private:
	std::map<std::string, PInternalStateFactoryInterface> m_Factories;

public:

	static CInternalStateFactoryList& getSingleton() {
		static CInternalStateFactoryList the_singleton_instance;
		return the_singleton_instance;
	}

	void registerFactory(PInternalStateFactoryInterface pInterface)
	{
		m_Factories.insert(std::make_pair(pInterface->getStateName(), pInterface));
	}

	PInternalStateFactoryInterface findFactory(const std::string& sStateName)
	{
		auto iIterator = m_Factories.find(sStateName);
		if (iIterator != m_Factories.end())
			return iIterator->second;

		return nullptr;
	}

	IState* createState(const std::string& sStateName, PPluginData pPluginData)
	{
		IState* pStateInstance = nullptr;
		auto pFactory = findFactory(sStateName);
		if (pFactory != nullptr)
			return pFactory->createStateInstance(pPluginData);

		throw ELibMCPluginInterfaceException(LIBMCPLUGIN_ERROR_INVALIDSTATENAME);
	}

};


class RegisterInternalStateFactory {
public:

	RegisterInternalStateFactory(PInternalStateFactoryInterface pInterface)
	{
		CInternalStateFactoryList::getSingleton().registerFactory(pInterface);
	}

};

#ifndef PLUGINDRIVERCLASS
#define PLUGINDRIVERCLASS CDriverContainerClass
#endif

#define STATEFACTORYNAME(STATENAME) _statedefinition_ ## STATENAME

#define __STRINGIZESTATENAME(x) #x
#define __STRINGIZESTATENAME_VALUE_OF(x) __STRINGIZESTATENAME(x)

#define __DECLARESTATE(STATENAME) void STATENAME(LibMCEnv::PStateEnvironment, PPluginData); RegisterInternalStateFactory STATEFACTORYNAME(STATENAME) (std::make_shared<CInternalStateFactory<PLUGINDRIVERCLASS>> (__STRINGIZESTATENAME_VALUE_OF (STATENAME), STATENAME)); void STATENAME (LibMCEnv::PStateEnvironment pStateEnvironment, PPluginData pPluginData)

#define __BEGINDRIVERIMPORT class PLUGINDRIVERCLASS : public virtual CPluginData {
#define __IMPORTDRIVER(CLASSNAME, DRIVERNAMESTRING) protected: PDriverCast_ ## CLASSNAME m_DriverCast_ ## CLASSNAME; public: PDriver_ ## CLASSNAME acquire ## CLASSNAME ## Driver(LibMCEnv::PStateEnvironment pStateEnvironment) { return m_DriverCast_ ## CLASSNAME.acquireDriver(pStateEnvironment, DRIVERNAMESTRING); }
#define __ENDDRIVERIMPORT };

#define __NODRIVERIMPORT __BEGINDRIVERIMPORT __ENDDRIVERIMPORT

#define __BEGINSTATEDEFINITIONS
#define __ENDSTATEDEFINITIONS CStateFactory::CStateFactory(const std::string& sInstanceName) { m_pPluginData = std::make_shared<PLUGINDRIVERCLASS>(); } IState* CStateFactory::CreateState(const std::string& sStateName) { return CInternalStateFactoryList::getSingleton().createState(sStateName, m_pPluginData); }

#define __acquireDriver(CLASSNAME) (std::dynamic_pointer_cast<PLUGINDRIVERCLASS> (pPluginData)->acquire ## CLASSNAME ## Driver (pStateEnvironment))

} // namespace Impl
} // namespace LibMCPlugin

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif // __LIBMCPLUGIN_STATE

