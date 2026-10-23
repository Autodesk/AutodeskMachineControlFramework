/*++

Copyright (C) 2025 Autodesk Inc.

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

#ifndef __AMCTEST_UNITTEST_DATASERIES
#define __AMCTEST_UNITTEST_DATASERIES


#include "amc_unittests.hpp"
#include "amc_dataseries.hpp"
#include "amc_dataserieshandler.hpp"
#include "common_utils.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_DataSeries : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "DataSeries";
		}

		void registerTests() override {
			registerTest("DataSeriesBasics", "Data series properties and entry operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DataSeries::testDataSeriesBasics, this));
			registerTest("DataSeriesHandlerBasics", "Data series handler create/find/memory operations", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_DataSeries::testDataSeriesHandlerBasics, this));
		}

		void initializeTests() override {
		}

	private:

		void testDataSeriesBasics()
		{
			std::string rawUUID = " {A1B2C3D4-1111-2222-3333-444455556666} ";
			AMC::CDataSeries series(rawUUID, "SeriesOne");
			assertTrue(series.getUUID() == "a1b2c3d4-1111-2222-3333-444455556666");
			assertTrue(series.getName() == "SeriesOne");
			assertTrue(series.getVersion() == 1);
			assertTrue(series.isEmpty());
			assertTrue(series.getEntryCount() == 0);

			bool thrown = false;
			try {
				series.getMinimum();
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getMinimum to throw on empty data series");

			thrown = false;
			try {
				series.getMaximum();
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getMaximum to throw on empty data series");

			auto& entries = series.getEntries();
			entries.push_back({ 10, 1.0 });
			entries.push_back({ 20, 2.0 });
			entries.push_back({ 30, 3.0 });

			assertFalse(series.isEmpty());
			assertTrue(series.getEntryCount() == 3);
			assertTrue(series.getMinimum() == 10);
			assertTrue(series.getMaximum() == 30);
			assertTrue(series.getMemoryUsageInBytes() >= series.getEntryCount() * sizeof(AMC::sDataSeriesEntry));

			series.increaseVersion();
			assertTrue(series.getVersion() == 2);

			series.clearData();
			assertTrue(series.isEmpty());
			assertTrue(series.getEntryCount() == 0);
		}

		void testDataSeriesHandlerBasics()
		{
			AMC::CDataSeriesHandler handler;
			std::string missingUUID = AMCCommon::CUtils::createUUID();

			assertFalse(handler.hasDataSeries(missingUUID));
			assertTrue(handler.findDataSeries(missingUUID, false).get() == nullptr);

			bool thrown = false;
			try {
				handler.findDataSeries(missingUUID, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected findDataSeries to throw on missing entry");

			auto pSeries = handler.createDataSeries("SeriesTwo");
			assertTrue(pSeries.get() != nullptr);

			std::string seriesUUID = pSeries->getUUID();
			assertTrue(handler.hasDataSeries(seriesUUID));
			assertTrue(handler.findDataSeries(seriesUUID, true).get() == pSeries.get());

			pSeries->getEntries().push_back({ 100, 1.0 });
			uint64_t usage = handler.getMemoryUsageInBytes();
			assertTrue(usage >= pSeries->getMemoryUsageInBytes());

			handler.unloadDataSeries(seriesUUID);
			handler.unloadAllEntities();
		}
	};

}

#endif // __AMCTEST_UNITTEST_DATASERIES
