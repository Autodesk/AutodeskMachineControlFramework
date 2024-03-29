/*++

Copyright (C) 2023 Autodesk Inc.

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


#include "amc_dataseries.hpp"
#include "libmc_exceptiontypes.hpp"
#include "common_utils.hpp"

namespace AMC {


	CDataSeries::CDataSeries(const std::string& sUUID, const std::string& sName)
		: m_sUUID (AMCCommon::CUtils::normalizeUUIDString (sUUID)), m_sName (sName), m_nVersion (1)
	{

	}

	CDataSeries::~CDataSeries()
	{

	}

	std::string CDataSeries::getUUID()
	{
		return m_sUUID;
	}

	std::string CDataSeries::getName()
	{
		return m_sName;
	}

	void CDataSeries::clearData()
	{
		m_Entries.clear();
	}

	uint64_t CDataSeries::getEntryCount()
	{
		return m_Entries.size();
	}

	bool CDataSeries::isEmpty()
	{
		return (m_Entries.size() == 0);
	}

	uint64_t CDataSeries::getMinimum()
	{
		if (m_Entries.size() == 0)
			throw ELibMCCustomException(LIBMC_ERROR_DATASERIESISEMPTY, m_sName);

		return m_Entries.begin ()->m_nTimeStampInMicroSeconds;
	}

	uint64_t CDataSeries::getMaximum()
	{
		if (m_Entries.size() == 0)
			throw ELibMCCustomException(LIBMC_ERROR_DATASERIESISEMPTY, m_sName);

		return m_Entries.rbegin()->m_nTimeStampInMicroSeconds;
	}

	std::vector<sDataSeriesEntry>& CDataSeries::getEntries()
	{
		return m_Entries;
	}

	void CDataSeries::increaseVersion()
	{
		m_nVersion++;
	}

	uint32_t CDataSeries::getVersion()
	{
		return m_nVersion;
	}

}


