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
#include "libmc_exceptiontypes.hpp"
#include <map>
#include <thread>
#include <iostream>
#include <mutex>

namespace AMC {

	#define STATEJOURNAL_MAXVARIABLECOUNT  (16 * 1024 * 1024)

	#define STATEJOURNAL_VARIABLE_MINUNITS 1.0E-6
	#define STATEJOURNAL_VARIABLE_MAXUNITS 1.0E6

	class CStateJournalImplVariable {
	protected:
		CStateJournalStream* m_pStream;
		uint32_t m_nID;
		std::string m_sName;
	public:
		CStateJournalImplVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: m_pStream(pStream), m_nID (nID), m_sName (sName)
		{
			LibMCAssertNotNull(pStream);
		}

		uint32_t getID()
		{
			return m_nID;
		}

		std::string getName()
		{
			return m_sName;
		}


		virtual void defineVariableInStream() = 0;

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

		void setValue(const bool bValue, const uint64_t nAbsoluteTimeStamp)
		{
			if (bValue != m_bCurrentValue) {

				m_pStream->writeTimeStamp (nAbsoluteTimeStamp);
				m_pStream->writeBool (m_nID, bValue);
				m_bCurrentValue = bValue;
			}
		}

		void defineVariableInStream() override
		{
			m_pStream->writeNameDefinition(m_nID, m_sName);
			m_pStream->writeBool(m_nID, m_bCurrentValue);
		}


	};


	class CStateJournalImplIntegerVariable : public CStateJournalImplVariable {
	private:
		int64_t m_nCurrentValue;
		std::list<std::pair<uint64_t, int64_t>> m_TimeStream;

	public:
		CStateJournalImplIntegerVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, sName), m_nCurrentValue(0)
		{
		}

		virtual eStateJournalVariableType getType()
		{
			return eStateJournalVariableType::vtIntegerParameter;
		}

		void setValue(const int64_t nValue, const uint64_t nAbsoluteTimeStamp)
		{
			if (m_nCurrentValue != nValue) {

				int64_t nDelta = (nValue - m_nCurrentValue);

				m_pStream->writeTimeStamp(nAbsoluteTimeStamp);
				m_pStream->writeInt64Delta(m_nID, nDelta);

				m_TimeStream.push_back(std::make_pair(nAbsoluteTimeStamp, nValue));

				m_nCurrentValue = nValue;
			}
		}

		void defineVariableInStream() override
		{
			m_pStream->writeNameDefinition(m_nID, m_sName);
			m_pStream->writeInt64Delta(m_nID, m_nCurrentValue);
		}

		void readTimeStream(uint64_t nStartTimeStamp, uint64_t nEndTimeStamp, int64_t& dStartValue, std::vector<sJournalTimeStreamInt64Entry>& timeStream)
		{
			dStartValue = 0;

			for (auto& timeStreamEntry : m_TimeStream) {
				if (timeStreamEntry.first <= nStartTimeStamp) {
					dStartValue = timeStreamEntry.second;
				}
				else {
					if (timeStreamEntry.first <= nEndTimeStamp) {
						sJournalTimeStreamDoubleEntry entry;
						entry.m_nTimeStamp = timeStreamEntry.first;
						entry.m_dValue = timeStreamEntry.second;
						timeStream.push_back(entry);
					}
				}
			}
		}


	};

	class CStateJournalImplDoubleVariable : public CStateJournalImplVariable {
	private:
		int64_t m_nCurrentValueInUnits;
		uint64_t m_nLastEntryTimeStamp;
		double m_dUnits;
		bool m_bHasUnits;

		std::list<std::pair<uint64_t, int64_t>> m_TimeStream;

	public:
		CStateJournalImplDoubleVariable(CStateJournalStream* pStream, const uint32_t nID, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, sName), m_nCurrentValueInUnits(0), m_dUnits (1.0), m_bHasUnits (false), m_nLastEntryTimeStamp (0)
		{
		}

		virtual eStateJournalVariableType getType()
		{
			return eStateJournalVariableType::vtDoubleParameter;
		}

		void setUnits(const double dUnits)
		{
			if ((dUnits < STATEJOURNAL_VARIABLE_MINUNITS) || (dUnits > STATEJOURNAL_VARIABLE_MAXUNITS))
				throw ELibMCCustomException(LIBMC_ERROR_INVALIDVARIABLEUNITS, m_sName);
			if (m_bHasUnits)
				throw ELibMCCustomException(LIBMC_ERROR_UNITSHAVEALREADYBEENSET, m_sName);

			m_dUnits = dUnits;
			m_bHasUnits = true;

		}

		void setValue(const double dValue, const uint64_t nAbsoluteTimeStamp)
		{
			if (!m_bHasUnits)
				throw ELibMCCustomException(LIBMC_ERROR_UNITSHAVENOTBEENSET, m_sName);

			if (nAbsoluteTimeStamp < m_nLastEntryTimeStamp)
				throw ELibMCCustomException(LIBMC_ERROR_TIMESTAMPISNOTINCREMENTING, m_sName);

			int64_t nValueInUnits = (int64_t) (dValue / m_dUnits);
			if (m_nCurrentValueInUnits != nValueInUnits) {

				m_pStream->writeTimeStamp (nAbsoluteTimeStamp);
				m_pStream->writeDoubleDelta (m_nID, (nValueInUnits - m_nCurrentValueInUnits));			

				m_nLastEntryTimeStamp = nAbsoluteTimeStamp;
				m_nCurrentValueInUnits = nValueInUnits;

				m_TimeStream.push_back (std::make_pair (nAbsoluteTimeStamp, nValueInUnits));
			}

		}

		void defineVariableInStream() override
		{
			if (!m_bHasUnits)
				throw ELibMCCustomException(LIBMC_ERROR_UNITSHAVENOTBEENSET, m_sName);

			m_pStream->writeNameDefinition(m_nID, m_sName);
			m_pStream->writeUnits(m_nID, m_dUnits);
			m_pStream->writeInt64Delta(m_nID, m_nCurrentValueInUnits);
		}

		void readTimeStream(uint64_t nStartTimeStamp, uint64_t nEndTimeStamp, double & dStartValue, std::vector<sJournalTimeStreamDoubleEntry>& timeStream)
		{
			dStartValue = 0.0;

			for (auto& timeStreamEntry : m_TimeStream) {
				if (timeStreamEntry.first <= nStartTimeStamp) {
					dStartValue = timeStreamEntry.second * m_dUnits;
				}
				else {
					if (timeStreamEntry.first <= nEndTimeStamp) {
						sJournalTimeStreamDoubleEntry entry;
						entry.m_nTimeStamp = timeStreamEntry.first;
						entry.m_dValue = timeStreamEntry.second * m_dUnits;
						timeStream.push_back(entry);
					}
				}
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

		void setValue(const std::string & sValue, const uint64_t nAbsoluteTimeStamp)
		{
			if (m_sCurrentValue != sValue) {

				m_pStream->writeTimeStamp(nAbsoluteTimeStamp);
				m_pStream->writeString(m_nID, sValue.c_str ());
				m_sCurrentValue = sValue;
			}
		}

		void defineVariableInStream() override
		{
			m_pStream->writeNameDefinition(m_nID, m_sName);
			m_pStream->writeString(m_nID, m_sCurrentValue);
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

		uint64_t retrieveTimeStamp();

		void readDoubleTimeStream(const std::string& sName, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp, double & dStartValue, std::vector<sJournalTimeStreamDoubleEntry>& timeStream);

	};


	CStateJournalImpl::CStateJournalImpl(PStateJournalStream pStream)
		: m_JournalMode(eStateJournalMode::sjmInitialising),
	     m_Chrono (false),
		m_nVariableCount (1),
		m_pStream (pStream)

	{
		LibMCAssertNotNull(pStream.get());

		// Create a new stream chunk
		m_pStream->startNewChunk();
		for (auto pVariable : m_VariableIDMap) 
			pVariable.second->defineVariableInStream();

	}

	PStateJournalImplVariable CStateJournalImpl::generateVariable(const eStateJournalVariableType eVariableType, const std::string& sName)
	{

		if (m_JournalMode != eStateJournalMode::sjmInitialising)
			throw ELibMCCustomException(LIBMC_ERROR_JOURNALISNOTINITIALISING, sName);

		if (m_nVariableCount >= STATEJOURNAL_MAXVARIABLECOUNT)
			throw ELibMCCustomException(LIBMC_ERROR_TOOMANYJOURNALVARIABLES, sName);

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
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDVARIABLETYPE, sName);
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

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (iIter->second);
		if (pBoolVariable.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pBoolVariable->setValue (bValue, retrieveTimeStamp());
	}

	void CStateJournalImpl::updateIntegerValue(const uint32_t nVariableID, const int64_t nValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (iIter->second);
		if (pIntegerVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pIntegerVariable->setValue(nValue, retrieveTimeStamp());

	}

	void CStateJournalImpl::updateStringValue(const uint32_t nVariableID, const std::string& sValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pStringVariable = std::dynamic_pointer_cast<CStateJournalImplStringVariable> (iIter->second);
		if (pStringVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pStringVariable->setValue(sValue, retrieveTimeStamp());
	}

	void CStateJournalImpl::updateDoubleValue(const uint32_t nVariableID, const double dValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (iIter->second);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pDoubleVariable->setValue(dValue, retrieveTimeStamp());

	}

	void CStateJournalImpl::readDoubleTimeStream(const std::string& sName, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp, double& dStartValue, std::vector<sJournalTimeStreamDoubleEntry>& timeStream)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableStringMap.find(sName);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (iIter->second);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDVARIABLETYPE, sName);

		pDoubleVariable->readTimeStream(nStartTimeStamp, nEndTimeStamp, dStartValue, timeStream);

	}


	uint64_t CStateJournalImpl::retrieveTimeStamp()
	{
		return m_Chrono.getExistenceTimeInMilliseconds();
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

	uint32_t CStateJournal::registerBooleanValue(const std::string& sName, const bool bInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtBoolParameter, sName);
		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (pVariable);
		if (pBoolVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pBoolVariable->setValue(bInitialValue, 0);

		return pVariable->getID ();
	}

	uint32_t CStateJournal::registerIntegerValue(const std::string& sName, const int64_t nInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtIntegerParameter, sName);		
		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (pVariable);
		if (pIntegerVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pIntegerVariable->setValue(nInitialValue, 0);

		return pVariable->getID();
	}

	uint32_t CStateJournal::registerStringValue(const std::string& sName, const std::string & sInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtStringParameter, sName);	
		auto pStringVariable = std::dynamic_pointer_cast<CStateJournalImplStringVariable> (pVariable);
		if (pStringVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE);

		pStringVariable->setValue(sInitialValue, 0);

		return pVariable->getID();
	}

	uint32_t CStateJournal::registerDoubleValue(const std::string& sName, const double dInitialValue, double dUnits)
	{
		dUnits = 1.0;
		if ((dUnits < STATEJOURNAL_VARIABLE_MINUNITS) || (dUnits > STATEJOURNAL_VARIABLE_MAXUNITS))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLEUNITS);

		auto pVariable = m_pImpl->generateVariable(eStateJournalVariableType::vtDoubleParameter, sName);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (pVariable);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INTERNALERROR);

		pDoubleVariable->setUnits(dUnits);
		pDoubleVariable->setValue(dInitialValue, 0);
		return pVariable->getID();
	}



	void CStateJournal::updateBoolValue(const uint32_t nVariableID, const bool bValue)
	{
		m_pImpl->updateBoolValue(nVariableID, bValue);
	}

	void CStateJournal::updateIntegerValue(const uint32_t nVariableID, const int64_t nValue)
	{
		m_pImpl->updateIntegerValue(nVariableID, nValue);
	}

	void CStateJournal::updateStringValue(const uint32_t nVariableID, const std::string& sValue)
	{
		m_pImpl->updateStringValue(nVariableID, sValue);
	}

	void CStateJournal::updateDoubleValue(const uint32_t nVariableID, const double dValue)
	{
		m_pImpl->updateDoubleValue(nVariableID, dValue);
	}

	void CStateJournal::readDoubleTimeStream(const std::string& sName, uint64_t nStartTimeStamp, uint64_t nEndTimeStamp, double& dStartValue, std::vector<sJournalTimeStreamDoubleEntry>& timeStream)
	{
		m_pImpl->readDoubleTimeStream(sName, nStartTimeStamp, nEndTimeStamp, dStartValue, timeStream);
	}

	void CStateJournal::retrieveRecentInterval(uint64_t nLastMilliseconds, uint64_t& nStartTimeStamp, uint64_t& nEndTimeStamp)
	{
		nEndTimeStamp = m_pImpl->retrieveTimeStamp();
		if (nLastMilliseconds < nEndTimeStamp)
			nStartTimeStamp = nEndTimeStamp - nLastMilliseconds;
		else
			nStartTimeStamp = 0;
		
		if (nStartTimeStamp >= nEndTimeStamp)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDTIMESTAMPINTERVAL);
	}

}


