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


#include "amc_statejournal.hpp"

#include "common_utils.hpp"
#include "common_chrono.hpp"
#include "libmc_interfaceexception.hpp"
#include <map>
#include <thread>
#include <iostream>
#include <mutex>

namespace AMC {

	#define STATEJOURNAL_MAXVARIABLECOUNT  (16 * 1024 * 1024)
    #define STATEJOURNAL_MAXTIMESTAMPDELTA  (256 * 1024 * 1024)

	#define STATEJOURNAL_VARIABLE_MINUNITS 1.0E-6
	#define STATEJOURNAL_VARIABLE_MAXUNITS 1.0E6

	#define STATEJOURNAL_VARIABLEFLAG_TIMESTAMP 0
	#define STATEJOURNAL_VARIABLEFLAG_INTEGER 1
	#define STATEJOURNAL_VARIABLEFLAG_STRING 2
	#define STATEJOURNAL_VARIABLEFLAG_DOUBLE 3
	#define STATEJOURNAL_VARIABLEFLAG_BOOL 4
	#define STATEJOURNAL_VARIABLEFLAG_EVENT 5

	#define STATEJOURNAL_VARIABLEFLAG_BOOL_FALSE 0
	#define STATEJOURNAL_VARIABLEFLAG_BOOL_TRUE 8
	#define STATEJOURNAL_VARIABLEFLAG_INTEGER_POSITIVE 0
	#define STATEJOURNAL_VARIABLEFLAG_INTEGER_NEGATIVE 8
	#define STATEJOURNAL_VARIABLEFLAG_DOUBLE_POSITIVE 0
	#define STATEJOURNAL_VARIABLEFLAG_DOUBLE_NEGATIVE 8

	#define STATEJOURNAL_VARIABLEIDFACTOR 16
	#define STATEJOURNAL_TIMESTAMPFACTOR 8

	class CStateJournalImplVariable {
	protected:
		CStateJournalStream* m_pStream;
		uint32_t m_nID;
		std::string m_sName;
	public:
		CStateJournalImplVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: m_pStream(pStream), m_nID (nID), m_sName (sName)
		{
			if (pStream == nullptr)
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

			std::cout << "registered " << sName << ": " << nID << std::endl;
		}

		uint32_t getID()
		{
			return m_nID;
		}

		std::string getName()
		{
			return m_sName;
		}


		virtual eStateJournalVariableType getType() = 0;
	};


	class CStateJournalImplBoolVariable : public CStateJournalImplVariable {
	private:
		bool m_bCurrentValue;
	public:
		CStateJournalImplBoolVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: CStateJournalImplVariable (pStream, nID, sName), m_bCurrentValue (false)
		{

		}

		virtual eStateJournalVariableType getType() 
		{
			return eStateJournalVariableType::vtBoolParameter;
		}

		void setValue(const bool bValue, const uint32_t nDeltaTimeStamp)
		{
			if (bValue != m_bCurrentValue) {

				if (nDeltaTimeStamp >= STATEJOURNAL_MAXTIMESTAMPDELTA)
					throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

				m_pStream->writeCommand (nDeltaTimeStamp * STATEJOURNAL_TIMESTAMPFACTOR | STATEJOURNAL_VARIABLEFLAG_TIMESTAMP);
				if (bValue) {
					m_pStream->writeCommand ((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_BOOL | STATEJOURNAL_VARIABLEFLAG_BOOL_TRUE);
				}
				else {
					m_pStream->writeCommand((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_BOOL | STATEJOURNAL_VARIABLEFLAG_BOOL_FALSE);
				}

				m_bCurrentValue = bValue;
			}
		}

	};


	class CStateJournalImplIntegerVariable : public CStateJournalImplVariable {
	private:
		int64_t m_nCurrentValue;
	public:
		CStateJournalImplIntegerVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, sName), m_nCurrentValue(0)
		{
		}

		virtual eStateJournalVariableType getType()
		{
			return eStateJournalVariableType::vtIntegerParameter;
		}

		void setValue(const int64_t nValue, const uint32_t nDeltaTimeStamp)
		{
			if (m_nCurrentValue != nValue) {

				if (nDeltaTimeStamp >= STATEJOURNAL_MAXTIMESTAMPDELTA)
					throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

				m_pStream->writeCommand(nDeltaTimeStamp * STATEJOURNAL_TIMESTAMPFACTOR | STATEJOURNAL_VARIABLEFLAG_TIMESTAMP);

				if (m_nCurrentValue < nValue) {
					int64_t nDelta = (nValue - m_nCurrentValue);

					m_pStream->writeCommand((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_INTEGER | STATEJOURNAL_VARIABLEFLAG_INTEGER_POSITIVE);
				}
				else {
					int64_t nDelta = (m_nCurrentValue - nValue);

					m_pStream->writeCommand((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_INTEGER | STATEJOURNAL_VARIABLEFLAG_INTEGER_NEGATIVE);
				}

				m_nCurrentValue = nValue;
			}
		}

	};

	class CStateJournalImplDoubleVariable : public CStateJournalImplVariable {
	private:
		int64_t m_nCurrentValueInUnits;
		double m_dUnits;
		bool m_bHasUnits;

	public:
		CStateJournalImplDoubleVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, sName), m_nCurrentValueInUnits(0), m_dUnits (1.0), m_bHasUnits (false)
		{
		}

		virtual eStateJournalVariableType getType()
		{
			return eStateJournalVariableType::vtDoubleParameter;
		}

		void setUnits(const double dUnits)
		{
			if ((dUnits < STATEJOURNAL_VARIABLE_MINUNITS) || (dUnits > STATEJOURNAL_VARIABLE_MAXUNITS))
				throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLEUNITS);
			if (m_bHasUnits)
				throw ELibMCInterfaceException(LIBMC_ERROR_UNITSHAVEALREADYBEENSET);

			m_dUnits = dUnits;
			m_bHasUnits = true;

		}

		void setValue(const double dValue, const uint32_t nDeltaTimeStamp)
		{
			if (!m_bHasUnits)
				throw ELibMCInterfaceException(LIBMC_ERROR_UNITSHAVENOTBEENSET);

			int64_t nValueInUnits = (int64_t) (dValue / m_dUnits);
			if (m_nCurrentValueInUnits != nValueInUnits) {

				if (nDeltaTimeStamp >= STATEJOURNAL_MAXTIMESTAMPDELTA)
					throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

				m_pStream->writeCommand(nDeltaTimeStamp * STATEJOURNAL_TIMESTAMPFACTOR | STATEJOURNAL_VARIABLEFLAG_TIMESTAMP);

				if (m_nCurrentValueInUnits < nValueInUnits) {
					int64_t nDelta = (nValueInUnits - m_nCurrentValueInUnits);

					m_pStream->writeCommand((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_DOUBLE | STATEJOURNAL_VARIABLEFLAG_DOUBLE_POSITIVE);
				}
				else {
					int64_t nDelta = (m_nCurrentValueInUnits - nValueInUnits);

					m_pStream->writeCommand((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_DOUBLE | STATEJOURNAL_VARIABLEFLAG_DOUBLE_NEGATIVE);
				}

				m_nCurrentValueInUnits = nValueInUnits;
			}

		}

	};

	class CStateJournalImplStringVariable : public CStateJournalImplVariable {
	private:
		std::string m_sCurrentValue;
	public:
		CStateJournalImplStringVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, sName), m_sCurrentValue("")
		{
		}

		virtual eStateJournalVariableType getType()
		{
			return eStateJournalVariableType::vtStringParameter;
		}

		void setValue(const std::string & sValue, const uint32_t nDeltaTimeStamp)
		{
			if (m_sCurrentValue != sValue) {

				if (nDeltaTimeStamp >= STATEJOURNAL_MAXTIMESTAMPDELTA)
					throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

				m_pStream->writeCommand(nDeltaTimeStamp * STATEJOURNAL_TIMESTAMPFACTOR | STATEJOURNAL_VARIABLEFLAG_TIMESTAMP);
				m_pStream->writeCommand((m_nID * STATEJOURNAL_VARIABLEIDFACTOR) | STATEJOURNAL_VARIABLEFLAG_STRING);
				m_pStream->writeString(sValue.c_str ());
				m_sCurrentValue = sValue;
			}
		}

	};

	typedef std::shared_ptr<CStateJournalImplVariable> PStateJournalImplVariable;
	typedef std::shared_ptr<CStateJournalImplBoolVariable> PStateJournalImplBoolVariable;
	typedef std::shared_ptr<CStateJournalImplIntegerVariable> PStateJournalImplIntegerVariable;
	typedef std::shared_ptr<CStateJournalImplDoubleVariable> PStateJournalImplDoubleVariable;
	typedef std::shared_ptr<CStateJournalImplStringVariable> PStateJournalImplStringVariable;

	class CStateJournalImpl {
	private:		
		std::map<std::string, PStateJournalImplVariable> m_VariableStringMap;
		std::map<uint32_t, PStateJournalImplVariable> m_VariableIDMap;

		uint32_t m_nVariableCount;

		eStateJournalMode m_JournalMode;
		AMCCommon::CChrono m_Chrono;
		uint64_t m_nRecordingStartTimeinMS;
		uint64_t m_nLastTimeStampInMS;

		PStateJournalStream m_pStream;

		std::mutex m_Mutex;

	public:

		CStateJournalImpl(PStateJournalStream pStream);

		PStateJournalImplVariable generateVariable(const eStateJournalVariableType eVariableType, const std::string& sName);

		void startRecording();
		void finishRecording();

		void updateBoolValue(const uint32_t nVariableID, const bool bValue);
		void updateIntegerValue(const uint32_t nVariableID, const int64_t nValue);
		void updateStringValue(const uint32_t nVariableID, const std::string& sValue);
		void updateDoubleValue(const uint32_t nVariableID, const double dValue);

		uint32_t retrieveDeltaTimeStamp();

	};


	CStateJournalImpl::CStateJournalImpl(PStateJournalStream pStream)
		: m_JournalMode(eStateJournalMode::sjmInitialising),
	     m_Chrono (false),
		m_nVariableCount (1),
		m_nRecordingStartTimeinMS (0),
		m_nLastTimeStampInMS (0),
		m_pStream (pStream)

	{
		if (pStream.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
	}

	PStateJournalImplVariable CStateJournalImpl::generateVariable(const eStateJournalVariableType eVariableType, const std::string& sName)
	{

		if (m_JournalMode != eStateJournalMode::sjmInitialising)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTINITIALISING);

		if (m_nVariableCount >= STATEJOURNAL_MAXVARIABLECOUNT)
			throw ELibMCInterfaceException(LIBMC_ERROR_TOOMANYJOURNALVARIABLES);

		PStateJournalImplVariable pVariable;

		switch (eVariableType) {
		case eStateJournalVariableType::vtBoolParameter:			
			pVariable = std::make_shared<CStateJournalImplBoolVariable>(m_pStream.get(), m_nVariableCount, sName);
			break;

		case eStateJournalVariableType::vtIntegerParameter:
			pVariable = std::make_shared<CStateJournalImplIntegerVariable>(m_pStream.get(), m_nVariableCount, sName);
			break;

		case eStateJournalVariableType::vtDoubleParameter:
			pVariable = std::make_shared<CStateJournalImplDoubleVariable>(m_pStream.get(), m_nVariableCount, sName);
			break;

		case eStateJournalVariableType::vtStringParameter:
			pVariable = std::make_shared<CStateJournalImplStringVariable>(m_pStream.get(), m_nVariableCount, sName);
			break;

		default:
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);
		}
		
		m_VariableIDMap.insert (std::make_pair (pVariable->getID (), pVariable));
		m_VariableStringMap.insert(std::make_pair(sName, pVariable));

		m_nVariableCount++;

		return pVariable;

	}



	void CStateJournalImpl::startRecording()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmInitialising)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTINITIALISING);

		m_JournalMode = eStateJournalMode::sjmRecording;
		m_nRecordingStartTimeinMS = m_Chrono.getExistenceTimeInMilliseconds();
		m_nLastTimeStampInMS = m_nRecordingStartTimeinMS;
	}

	void CStateJournalImpl::finishRecording()
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		m_JournalMode = eStateJournalMode::sjmFinished;

	}

	void CStateJournalImpl::updateBoolValue(const uint32_t nVariableID, const bool bValue)
	{

		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return;

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (iIter->second);
		if (pBoolVariable.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pBoolVariable->setValue (bValue, retrieveDeltaTimeStamp ());
	}

	void CStateJournalImpl::updateIntegerValue(const uint32_t nVariableID, const int64_t nValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return;

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (iIter->second);
		if (pIntegerVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pIntegerVariable->setValue(nValue, retrieveDeltaTimeStamp());

	}

	void CStateJournalImpl::updateStringValue(const uint32_t nVariableID, const std::string& sValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return;

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pStringVariable = std::dynamic_pointer_cast<CStateJournalImplStringVariable> (iIter->second);
		if (pStringVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pStringVariable->setValue(sValue, retrieveDeltaTimeStamp());
	}

	void CStateJournalImpl::updateDoubleValue(const uint32_t nVariableID, const double dValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		return;

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (iIter->second);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pDoubleVariable->setValue(dValue, retrieveDeltaTimeStamp());

	}


	uint32_t CStateJournalImpl::retrieveDeltaTimeStamp()
	{
		uint64_t nTimeStamp = m_Chrono.getExistenceTimeInMilliseconds();
		if (nTimeStamp < m_nLastTimeStampInMS) 
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTIMESTAMP);

		int64_t nDeltaTimeStamp = nTimeStamp - m_nLastTimeStampInMS;
		if (nDeltaTimeStamp > STATEJOURNAL_MAXTIMESTAMPDELTA)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTIMESTAMP);

		m_nLastTimeStampInMS = nTimeStamp;
		return (uint32_t) nDeltaTimeStamp;

	}


	CStateJournalVariable::CStateJournalVariable()
		: m_pStateJournalImpl (nullptr), m_nVariableID (0)
	{

	}

	CStateJournalVariable::CStateJournalVariable(PStateJournalImpl pImpl, uint32_t nVariableID)
		: m_pStateJournalImpl (pImpl), m_nVariableID (nVariableID)
	{

	}

	void CStateJournalVariable::updateValue(const bool bValue)
	{
		if (m_pStateJournalImpl.get () != nullptr)
			m_pStateJournalImpl->updateBoolValue(m_nVariableID, bValue);
	}

	void CStateJournalVariable::updateValue(const int64_t nValue)
	{
		if (m_pStateJournalImpl.get() != nullptr)
			m_pStateJournalImpl->updateIntegerValue(m_nVariableID, nValue);
	}

	void CStateJournalVariable::updateValue(const std::string& sValue)
	{
		if (m_pStateJournalImpl.get() != nullptr)
			m_pStateJournalImpl->updateStringValue(m_nVariableID, sValue);
	}

	void CStateJournalVariable::updateValue(const double dValue)
	{
		if (m_pStateJournalImpl.get() != nullptr)
			m_pStateJournalImpl->updateDoubleValue(m_nVariableID, dValue);
	}

	
	CStateJournal::CStateJournal(PStateJournalStream pStream)
		: m_pImpl(std::make_shared<CStateJournalImpl> (pStream))
	{
	
	}

	CStateJournal::~CStateJournal()
	{

	}

	void CStateJournal::startRecording()
	{
		m_pImpl->startRecording();
	}

	void CStateJournal::finishRecording()
	{
		m_pImpl->finishRecording();
	}

	CStateJournalVariable CStateJournal::registerBooleanValue(const std::string& sName, const bool bInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtBoolParameter, sName);
		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (pVariable);
		if (pBoolVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pBoolVariable->setValue(bInitialValue, m_pImpl->retrieveDeltaTimeStamp());

		return CStateJournalVariable(m_pImpl, pVariable->getID ());
	}

	CStateJournalVariable CStateJournal::registerIntegerValue(const std::string& sName, const int64_t nInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtIntegerParameter, sName);		
		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (pVariable);
		if (pIntegerVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pIntegerVariable->setValue(nInitialValue, m_pImpl->retrieveDeltaTimeStamp());

		return CStateJournalVariable(m_pImpl, pVariable->getID());
	}

	CStateJournalVariable CStateJournal::registerStringValue(const std::string& sName, const std::string & sInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtStringParameter, sName);	
		auto pStringVariable = std::dynamic_pointer_cast<CStateJournalImplStringVariable> (pVariable);
		if (pStringVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pStringVariable->setValue(sInitialValue, m_pImpl->retrieveDeltaTimeStamp());

		return CStateJournalVariable(m_pImpl, pVariable->getID());
	}

	CStateJournalVariable CStateJournal::registerDoubleValue(const std::string& sName, const double dInitialValue, double dUnits)
	{
		dUnits = 1.0;
		if ((dUnits < STATEJOURNAL_VARIABLE_MINUNITS) || (dUnits > STATEJOURNAL_VARIABLE_MAXUNITS))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLEUNITS);

		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtDoubleParameter, sName);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (pVariable);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INTERNALERROR);

		pDoubleVariable->setUnits(dUnits);
		pDoubleVariable->setValue(dInitialValue, m_pImpl->retrieveDeltaTimeStamp());
		return CStateJournalVariable(m_pImpl, pVariable->getID());
	}


}


