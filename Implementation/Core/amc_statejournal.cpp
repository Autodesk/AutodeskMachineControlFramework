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
#include "amc_logger.hpp"
#include "amc_constants.hpp"
#include <map>
#include <string>
#include <thread>
#include <future>
#include <iostream>
#include <mutex>

namespace AMC {



	class CStateJournalImplVariable {
	protected:
		CStateJournalStream* m_pStream;
		uint32_t m_nID;
		uint32_t m_nStorageIndex;
		std::string m_sName;
	public:
		CStateJournalImplVariable(CStateJournalStream* pStream, const uint32_t nID, const uint32_t nStorageIndex, const std::string& sName)
			: m_pStream(pStream), m_nID (nID), m_sName (sName), m_nStorageIndex (nStorageIndex)
		{
			LibMCAssertNotNull(pStream);
		}

		uint32_t getID()
		{
			return m_nID;
		}

		uint32_t getStorageIndex()
		{
			return m_nStorageIndex;
		}

		std::string getName()
		{
			return m_sName;
		}

		virtual LibMCData::eParameterDataType getType() = 0;

		virtual double computeNumericSample(const uint64_t nTimeStampInMicroseconds)
		{
			throw ELibMCCustomException(LIBMC_ERROR_JOURNALVARIABLEISNOTNUMERIC, m_sName);
		}

	};


	class CStateJournalImplBoolVariable : public CStateJournalImplVariable {
	private:
		bool m_bCurrentValue;
	public:
		CStateJournalImplBoolVariable(CStateJournalStream* pStream, const uint32_t nID, const uint32_t nStorageIndex, const std::string& sName)
			: CStateJournalImplVariable (pStream, nID, nStorageIndex, sName), m_bCurrentValue (false)
		{

		}

		virtual LibMCData::eParameterDataType getType()
		{
			return LibMCData::eParameterDataType::Bool;
		}

		void setValue_MicroSecond(const bool bValue, const uint64_t nAbsoluteTimeStampInMicroSeconds)
		{
			if (bValue != m_bCurrentValue) {

				m_pStream->writeBool_MicroSecond (nAbsoluteTimeStampInMicroSeconds, m_nStorageIndex, bValue);
				m_bCurrentValue = bValue;
			}
		}

		void setInitialValue(const bool bValue)
		{
			m_bCurrentValue = bValue;
		}

		/*void readTimeStream(const sStateJournalInterval& interval, std::vector<sJournalTimeStreamInt64Entry>& timeStream)
		{
			m_pStream->readRawIntegerData(m_nStorageIndex, interval, timeStream);
		} */

		double computeNumericSample(const uint64_t nTimeStampInMicroseconds) override
		{
			if (m_pStream->sampleBoolData(m_nStorageIndex, nTimeStampInMicroseconds))
				return 1.0;

			return 0.0;
		}

	};


	class CStateJournalImplIntegerVariable : public CStateJournalImplVariable {
	private:
		int64_t m_nCurrentValue;

	public:
		CStateJournalImplIntegerVariable(CStateJournalStream* pStream, const uint32_t nID, const uint32_t nStorageIndex, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, nStorageIndex, sName), m_nCurrentValue(0)
		{
		}

		virtual LibMCData::eParameterDataType getType()
		{
			return LibMCData::eParameterDataType::Integer;
		}

		void setValue_MicroSecond(const int64_t nValue, const uint64_t nAbsoluteTimeStampInMicroSeconds)
		{
			if (m_nCurrentValue != nValue) {

				m_pStream->writeInt64_MicroSecond(nAbsoluteTimeStampInMicroSeconds, m_nStorageIndex, nValue);
				m_nCurrentValue = nValue;
			}
		}

		void setInitialValue(const int64_t nValue)
		{
			m_nCurrentValue = nValue;
		}


		/*void readTimeStream(const sStateJournalInterval& interval, std::vector<sJournalTimeStreamInt64Entry>& timeStream)
		{

			m_pStream->readRawIntegerData (m_nStorageIndex, interval, timeStream);

		} */

		double computeNumericSample(const uint64_t nTimeStampInMicroseconds) override
		{			

			return (double)m_pStream->sampleIntegerData(m_nStorageIndex, nTimeStampInMicroseconds);
		}



	};

	class CStateJournalImplDoubleVariable : public CStateJournalImplVariable {
	private:
		int64_t m_nCurrentValueInUnits;
		double m_dUnits;
		bool m_bHasUnits;

	public:
		CStateJournalImplDoubleVariable(CStateJournalStream* pStream, const uint32_t nID, const uint32_t nStorageIndex, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, nStorageIndex, sName), m_nCurrentValueInUnits(0), m_dUnits (0.0001), m_bHasUnits (false)
		{
		}

		virtual LibMCData::eParameterDataType getType()
		{
			return LibMCData::eParameterDataType::Double;
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

		void setValue_MicroSecond(const double dValue, const uint64_t nAbsoluteTimeStampInMicroSeconds)
		{
			if (!m_bHasUnits)
				throw ELibMCCustomException(LIBMC_ERROR_UNITSHAVENOTBEENSET, m_sName);

			int64_t nValueInUnits = (int64_t) (dValue / m_dUnits);
			if (m_nCurrentValueInUnits != nValueInUnits) {

				m_pStream->writeDouble_MicroSecond (nAbsoluteTimeStampInMicroSeconds, m_nStorageIndex, nValueInUnits);

				m_nCurrentValueInUnits = nValueInUnits;

			}

		}

		void setInitialValue(const double dValue)
		{
			if (!m_bHasUnits)
				throw ELibMCCustomException(LIBMC_ERROR_UNITSHAVENOTBEENSET, m_sName);

			int64_t nValueInUnits = (int64_t)(dValue / m_dUnits);
			m_nCurrentValueInUnits = nValueInUnits;

		}

		/*void readTimeStream(const sStateJournalInterval& interval, std::vector<sJournalTimeStreamDoubleEntry>& timeStream)
		{
			if (!m_bHasUnits)
				throw ELibMCCustomException(LIBMC_ERROR_UNITSHAVENOTBEENSET, m_sName);

			m_pStream->readRawDoubleData (m_nStorageIndex, interval, m_dUnits, timeStream);

		} */

		double computeNumericSample(const uint64_t nTimeStampInMicroseconds) 
		{
			return m_pStream->sampleDoubleData (m_nStorageIndex, nTimeStampInMicroseconds, m_dUnits);
		}

	};

	class CStateJournalImplStringVariable : public CStateJournalImplVariable {
	private:
		std::string m_sCurrentValue;
	public:
		CStateJournalImplStringVariable(CStateJournalStream* pStream, const uint32_t nID, const uint32_t nStorageIndex, const std::string& sName)
			: CStateJournalImplVariable(pStream, nID, nStorageIndex, sName), m_sCurrentValue("")
		{
		}

		virtual LibMCData::eParameterDataType getType()
		{
			return LibMCData::eParameterDataType::String;
		}

		void setValue_MicroSecond(const std::string & sValue, const uint64_t nAbsoluteTimeStamp_MicroSecond)
		{
			if (m_sCurrentValue != sValue) {

				// TODO
				m_sCurrentValue = sValue;
			}
		}

		void setInitialValue(const std::string& sValue)
		{
			m_sCurrentValue = sValue;
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
		std::map<std::string, PStateJournalImplVariable> m_VariableAliasMap;
		std::map<uint32_t, PStateJournalImplVariable> m_VariableIDMap;
		std::vector<PStateJournalImplVariable> m_VariableList;

		uint32_t m_nChunkWriteIntervalInSeconds;

		eStateJournalMode m_JournalMode;
		AMCCommon::PChrono m_pGlobalChrono;
		uint64_t m_nAbsoluteStartTimeInMicroseconds;
		uint64_t m_nLifetimeInMicroseconds;

		PStateJournalStream m_pStream;
		PLogger m_pLogger;

		std::mutex m_Mutex;

		std::atomic<bool> m_ThreadStopFlag;
		std::future<void> m_ThreadFuture;

		PStateJournalImplVariable findVariable(const std::string& sName);

	public:

		CStateJournalImpl(PStateJournalStream pStream, AMCCommon::PChrono pGlobalChrono);
		virtual ~CStateJournalImpl();

		PStateJournalImplVariable generateVariable(const LibMCData::eParameterDataType eVariableType, const std::string& sName);
		PStateJournalImplVariable createAlias (const std::string& sName, const std::string& sSourceName);

		void startRecording();
		void finishRecording();

		void updateBoolValue(const uint32_t nVariableID, const bool bValue);
		void updateIntegerValue(const uint32_t nVariableID, const int64_t nValue);
		void updateStringValue(const uint32_t nVariableID, const std::string& sValue);
		void updateDoubleValue(const uint32_t nVariableID, const double dValue);

		uint64_t retrieveTimeStamp_MicroSecond();

		//void readDoubleTimeStream(const std::string& sName, const sStateJournalInterval& interval, std::vector<sJournalTimeStreamDoubleEntry>& timeStream);

		double computeSample(const std::string& sName, const uint64_t nTimeStampInMicroseconds);

		void recordingThread();
		
		std::string getStartTimeAsUTC();

		uint64_t getLifeTimeInMicroseconds();

	};


	CStateJournalImpl::CStateJournalImpl(PStateJournalStream pStream, AMCCommon::PChrono pGlobalChrono)
		: m_JournalMode(eStateJournalMode::sjmInitialising),	     
		m_pStream (pStream),
		m_nChunkWriteIntervalInSeconds (1),
		m_nLifetimeInMicroseconds (0),
		m_ThreadStopFlag (false),
		m_pGlobalChrono (pGlobalChrono)

	{
		if (pStream.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);
		if (pGlobalChrono.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDPARAM);

		m_nAbsoluteStartTimeInMicroseconds = pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();

		m_nChunkWriteIntervalInSeconds = 10;
	}

	CStateJournalImpl::~CStateJournalImpl()
	{

		// Close thread
		m_ThreadStopFlag = true;
		if (m_ThreadFuture.valid())
			m_ThreadFuture.wait();
		m_ThreadStopFlag = false;
	}

	PStateJournalImplVariable CStateJournalImpl::createAlias(const std::string& sName, const std::string& sSourceName)
	{
		auto iIter = m_VariableStringMap.find (sSourceName);
		if (iIter == m_VariableStringMap.end()) 
			throw ELibMCCustomException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND, sSourceName);

		auto pVariable = iIter->second;
		m_VariableAliasMap.insert(std::make_pair(sName, pVariable));
		return pVariable;
	}


	PStateJournalImplVariable CStateJournalImpl::generateVariable(const LibMCData::eParameterDataType eVariableType, const std::string& sName)
	{

		if (m_JournalMode != eStateJournalMode::sjmInitialising)
			throw ELibMCCustomException(LIBMC_ERROR_JOURNALISNOTINITIALISING, sName);

		if (m_VariableList.size () >= STATEJOURNAL_MAXVARIABLECOUNT)
			throw ELibMCCustomException(LIBMC_ERROR_TOOMANYJOURNALVARIABLES, sName);

		PStateJournalImplVariable pVariable;
		uint32_t nVariableIndex = (uint32_t)m_VariableList.size();
		uint32_t nVariableID = (uint32_t)(m_VariableList.size() + 1);

		auto pCache = m_pStream->getCache();
		pCache->createVariableInJournalDB (sName, nVariableID, nVariableIndex, eVariableType);

		switch (eVariableType) {
		case LibMCData::eParameterDataType::Bool:
			pVariable = std::make_shared<CStateJournalImplBoolVariable>(m_pStream.get(), nVariableID, nVariableIndex, sName);
			break;

		case LibMCData::eParameterDataType::Integer:
			pVariable = std::make_shared<CStateJournalImplIntegerVariable>(m_pStream.get(), nVariableID, nVariableIndex, sName);
			break;

		case LibMCData::eParameterDataType::Double:
			pVariable = std::make_shared<CStateJournalImplDoubleVariable>(m_pStream.get(), nVariableID, nVariableIndex, sName);
			break;

		case LibMCData::eParameterDataType::String:
			pVariable = std::make_shared<CStateJournalImplStringVariable>(m_pStream.get(), nVariableID, nVariableIndex, sName);
			break;

		default:
			throw ELibMCCustomException(LIBMC_ERROR_INVALIDVARIABLETYPE, sName);
		}
		
		m_VariableIDMap.insert (std::make_pair (pVariable->getID (), pVariable));
		m_VariableStringMap.insert(std::make_pair(sName, pVariable));
		m_VariableList.push_back(pVariable);

		return pVariable;

	}



	void CStateJournalImpl::startRecording()
	{
		{ 
			std::lock_guard<std::mutex> lockGuard(m_Mutex);

			if (m_JournalMode != eStateJournalMode::sjmInitialising)
				throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTINITIALISING);

			m_JournalMode = eStateJournalMode::sjmRecording;
		}

		m_pStream->setVariableCount(m_VariableList.size());

		m_ThreadStopFlag = false;
		m_ThreadFuture = std::async(std::launch::async, [this] { this->recordingThread(); });


	}

	void CStateJournalImpl::finishRecording()
	{
		
		// Close thread
		m_ThreadStopFlag = true;
		if (m_ThreadFuture.valid())
			m_ThreadFuture.wait();
		m_ThreadStopFlag = false;

		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		m_JournalMode = eStateJournalMode::sjmFinished;

	}

	void CStateJournalImpl::recordingThread()
	{
		while (!m_ThreadStopFlag) {
			try {
				m_pStream->serializeChunksThreaded();
			}
			catch (std::exception & E) {
				m_pLogger->logMessage ("could not serialize journal chunk: " + std::string (E.what ()), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::FatalError);
				throw;
			}


			try {
				m_pStream->writeChunksToDiskThreaded();
			}
			catch (std::exception& E) {
				m_pLogger->logMessage("could not write journal chunk to disk: " + std::string(E.what()), LOG_SUBSYSTEM_SYSTEM, AMC::eLogLevel::FatalError);
				throw;
			}

			AMCCommon::CChrono chrono;
			uint64_t nTimeOutTimeStamp = chrono.getUTCTimeStampInMicrosecondsSince1970 () +  (uint64_t) m_nChunkWriteIntervalInSeconds * 1000000ULL;
			uint32_t nThreadSleepTimeInMilliseconds = 1;

			while ((!m_ThreadStopFlag) && (chrono.getUTCTimeStampInMicrosecondsSince1970() < nTimeOutTimeStamp)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(nThreadSleepTimeInMilliseconds));
			}
		}
	}


	void CStateJournalImpl::updateBoolValue(const uint32_t nVariableID, const bool bValue)
	{

		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (iIter->second);
		if (pBoolVariable.get () == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + iIter->second->getName () + " is not a boolean variable");

		uint64_t nNewTimeStamp = retrieveTimeStamp_MicroSecond();
		m_nLifetimeInMicroseconds = nNewTimeStamp;

		pBoolVariable->setValue_MicroSecond (bValue, nNewTimeStamp);
	}

	void CStateJournalImpl::updateIntegerValue(const uint32_t nVariableID, const int64_t nValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (iIter->second);
		if (pIntegerVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + iIter->second->getName() + " is not a integer variable");

		uint64_t nNewTimeStamp = retrieveTimeStamp_MicroSecond();
		m_nLifetimeInMicroseconds = nNewTimeStamp;

		pIntegerVariable->setValue_MicroSecond(nValue, nNewTimeStamp);

	}

	void CStateJournalImpl::updateStringValue(const uint32_t nVariableID, const std::string& sValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pStringVariable = std::dynamic_pointer_cast<CStateJournalImplStringVariable> (iIter->second);
		if (pStringVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + iIter->second->getName() + " is not a string variable");

		uint64_t nNewTimeStamp = retrieveTimeStamp_MicroSecond();
		m_nLifetimeInMicroseconds = nNewTimeStamp;

		pStringVariable->setValue_MicroSecond(sValue, nNewTimeStamp);
	}

	void CStateJournalImpl::updateDoubleValue(const uint32_t nVariableID, const double dValue)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);

		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto iIter = m_VariableIDMap.find(nVariableID);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (iIter->second);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + iIter->second->getName() + " is not a double variable");

		uint64_t nNewTimeStamp = retrieveTimeStamp_MicroSecond();
		m_nLifetimeInMicroseconds = nNewTimeStamp;

		pDoubleVariable->setValue_MicroSecond(dValue, nNewTimeStamp);

	}

	PStateJournalImplVariable CStateJournalImpl::findVariable(const std::string& sName)
	{
		auto iVariableIter = m_VariableStringMap.find(sName);
		if (iVariableIter != m_VariableStringMap.end())
			return iVariableIter->second;

		auto iAliasIter = m_VariableAliasMap.find(sName);
		if (iAliasIter != m_VariableAliasMap.end())
			return iAliasIter->second;
		
		throw ELibMCCustomException(LIBMC_ERROR_JOURNALVARIABLENOTFOUND, sName);

	}

	double CStateJournalImpl::computeSample(const std::string& sName, const uint64_t nTimeStampInMicroseconds)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto pVariable = findVariable(sName);
		return pVariable->computeNumericSample(nTimeStampInMicroseconds);

	}


	/*void CStateJournalImpl::readDoubleTimeStream(const std::string& sName, const sStateJournalInterval& interval, std::vector<sJournalTimeStreamDoubleEntry>& timeStream)
	{
		std::lock_guard<std::mutex> lockGuard(m_Mutex);
		if (m_JournalMode != eStateJournalMode::sjmRecording)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALISNOTRECORDING);

		auto pVariable = findVariable(sName);

		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (pVariable);
		if (pDoubleVariable.get() != nullptr) {
			pDoubleVariable->readTimeStream(interval, timeStream);
			return;
		}

		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (pVariable);
		if (pIntegerVariable.get() != nullptr) {
			std::vector<sJournalTimeStreamInt64Entry> intTimeStream;
			pIntegerVariable->readTimeStream(interval, intTimeStream);

			timeStream.resize(intTimeStream.size ());
			for (size_t nIndex = 0; nIndex < intTimeStream.size(); nIndex++) {
				auto& srcEntry = intTimeStream.at(nIndex);
				auto& targetEntry = timeStream.at(nIndex);
				targetEntry.m_nTimeStampInMicroSeconds = srcEntry.m_nTimeStampInMicroSeconds;
				targetEntry.m_dValue = (double) srcEntry.m_nValue;
			}

			return;
		}

		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (pVariable);
		if (pBoolVariable.get() != nullptr) {
			std::vector<sJournalTimeStreamInt64Entry> intTimeStream;
			pBoolVariable->readTimeStream(interval, intTimeStream);

			timeStream.resize(intTimeStream.size());
			for (size_t nIndex = 0; nIndex < intTimeStream.size(); nIndex++) {
				auto& srcEntry = intTimeStream.at(nIndex);
				auto& targetEntry = timeStream.at(nIndex);
				targetEntry.m_nTimeStampInMicroSeconds = srcEntry.m_nTimeStampInMicroSeconds;
				targetEntry.m_dValue = (double)srcEntry.m_nValue;
			}

			return;
		}


		throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + pVariable->getName() + " is not a numeric variable");


	} */

	std::string CStateJournalImpl::getStartTimeAsUTC()
	{
		return m_pGlobalChrono->convertToISO8601TimeUTC (m_nAbsoluteStartTimeInMicroseconds);
	}

	uint64_t CStateJournalImpl::getLifeTimeInMicroseconds()
	{
		return m_nLifetimeInMicroseconds;
	}


	uint64_t CStateJournalImpl::retrieveTimeStamp_MicroSecond()
	{
		uint64_t nAbsoluteTimeStamp = m_pGlobalChrono->getUTCTimeStampInMicrosecondsSince1970();
		if (nAbsoluteTimeStamp < m_nAbsoluteStartTimeInMicroseconds)
			throw ELibMCInterfaceException(LIBMC_ERROR_JOURNALTIMESTAMPINVALID);

		return nAbsoluteTimeStamp - m_nAbsoluteStartTimeInMicroseconds;
	}



	
	CStateJournal::CStateJournal(PStateJournalStream pStream, AMCCommon::PChrono pGlobalChrono)
		: m_pImpl(std::make_shared<CStateJournalImpl> (pStream, pGlobalChrono))
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
		auto pVariable = m_pImpl->generateVariable(LibMCData::eParameterDataType::Bool, sName);
		auto pBoolVariable = std::dynamic_pointer_cast<CStateJournalImplBoolVariable> (pVariable);
		if (pBoolVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + sName + " is not a bool variable");

		pBoolVariable->setInitialValue(bInitialValue);

		return pVariable->getID ();
	}

	uint32_t CStateJournal::registerIntegerValue(const std::string& sName, const int64_t nInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(LibMCData::eParameterDataType::Integer, sName);
		auto pIntegerVariable = std::dynamic_pointer_cast<CStateJournalImplIntegerVariable> (pVariable);
		if (pIntegerVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + sName + " is not a integer variable");

		pIntegerVariable->setInitialValue(nInitialValue);

		return pVariable->getID();
	}

	uint32_t CStateJournal::registerStringValue(const std::string& sName, const std::string & sInitialValue)
	{
		auto pVariable = m_pImpl->generateVariable(LibMCData::eParameterDataType::String, sName);
		auto pStringVariable = std::dynamic_pointer_cast<CStateJournalImplStringVariable> (pVariable);
		if (pStringVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLETYPE, "variable " + sName + " is not a string variable");

		pStringVariable->setInitialValue(sInitialValue);

		return pVariable->getID();
	}

	uint32_t CStateJournal::registerDoubleValue(const std::string& sName, const double dInitialValue, double dUnits)
	{
		
		if ((dUnits < STATEJOURNAL_VARIABLE_MINUNITS) || (dUnits > STATEJOURNAL_VARIABLE_MAXUNITS))
			throw ELibMCInterfaceException(LIBMC_ERROR_INVALIDVARIABLEUNITS);

		auto pVariable = m_pImpl->generateVariable(LibMCData::eParameterDataType::Double, sName);
		auto pDoubleVariable = std::dynamic_pointer_cast<CStateJournalImplDoubleVariable> (pVariable);
		if (pDoubleVariable.get() == nullptr)
			throw ELibMCInterfaceException(LIBMC_ERROR_INTERNALERROR);

		pDoubleVariable->setUnits(dUnits);
		pDoubleVariable->setInitialValue(dInitialValue);
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


	double CStateJournal::computeSample(const std::string& sName, const uint64_t nTimeStamp)
	{
		return m_pImpl->computeSample(sName, nTimeStamp);
	}


	void CStateJournal::registerAlias(const std::string& sName, const std::string& sSourceName)
	{
		m_pImpl->createAlias(sName, sSourceName);
	}

	std::string CStateJournal::getStartTimeAsUTC()
	{
		return m_pImpl->getStartTimeAsUTC();
	}

	uint64_t CStateJournal::getLifeTimeInMicroseconds()
	{
		return m_pImpl->getLifeTimeInMicroseconds();
	}


}


