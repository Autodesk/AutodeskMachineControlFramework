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

#ifndef __AMCTEST_UNITTEST_JSONWRITER
#define __AMCTEST_UNITTEST_JSONWRITER


#include "amc_unittests.hpp"
#include "amc_jsonwriter.hpp"

#include "RapidJSON/document.h"


namespace AMCUnitTest {

	class CUnitTestGroup_JSONWriter : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "JSONWriter";
		}

		void registerTests() override {
			registerTest("RootAndNestedValues", "JSON writer root values and nested objects/arrays", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_JSONWriter::testRootAndNestedValues, this));
			registerTest("CopyFromHelpers", "JSON writer copy helpers for objects/arrays/documents", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_JSONWriter::testCopyFromHelpers, this));
		}

		void initializeTests() override {
		}

	private:

		void testRootAndNestedValues()
		{
			AMC::CJSONWriter writer;
			writer.addString("name", "value");
			writer.addInteger("count", 42);
			writer.addBoolean("flag", true);
			writer.addDouble("ratio", 1.25);

			AMC::CJSONWriterObject object(writer);
			assertTrue(object.isEmpty());
			object.addString("child", "data");
			assertFalse(object.isEmpty());

			AMC::CJSONWriterArray array(writer);
			assertTrue(array.isEmpty());
			array.addString("a");
			array.addInteger(7);
			array.addDouble("unused", 3.5);
			assertFalse(array.isEmpty());

			writer.addObject("obj", object);
			writer.addArray("arr", array);

			rapidjson::Document doc;
			doc.Parse(writer.saveToString().c_str());
			assertTrue(doc.IsObject());
			assertTrue(doc["name"].GetString() == std::string("value"));
			assertTrue(doc["count"].GetInt64() == 42);
			assertTrue(doc["flag"].GetBool());
			assertTrue(doc["ratio"].IsNumber());

			const auto& obj = doc["obj"];
			assertTrue(obj.IsObject());
			assertTrue(obj["child"].GetString() == std::string("data"));

			const auto& arr = doc["arr"];
			assertTrue(arr.IsArray());
			assertTrue(arr.Size() == 3);
			assertTrue(arr[0].GetString() == std::string("a"));
			assertTrue(arr[1].GetInt64() == 7);
			assertTrue(arr[2].IsNumber());
		}

		void testCopyFromHelpers()
		{
			AMC::CJSONWriter writer;

			rapidjson::Document objectDoc;
			objectDoc.SetObject();
			objectDoc.AddMember("x", 1, objectDoc.GetAllocator());

			rapidjson::Document arrayDoc;
			arrayDoc.SetArray();
			rapidjson::Value entry;
			entry.SetString("entry", arrayDoc.GetAllocator());
			arrayDoc.PushBack(entry, arrayDoc.GetAllocator());

			AMC::CJSONWriterObject obj(writer);
			obj.copyFromObject(objectDoc);
			writer.addObject("fromObject", obj);

			AMC::CJSONWriterArray arr(writer);
			arr.copyFromArray(arrayDoc);
			writer.addArray("fromArray", arr);

			rapidjson::Document copyDoc;
			copyDoc.SetObject();
			copyDoc.AddMember("copy", true, copyDoc.GetAllocator());

			AMC::CJSONWriter copyWriter;
			copyWriter.copyFromDocument(copyDoc);

			rapidjson::Document doc;
			doc.Parse(writer.saveToString().c_str());
			assertTrue(doc["fromObject"]["x"].GetInt() == 1);
			assertTrue(doc["fromArray"].IsArray());
			assertTrue(doc["fromArray"][0].GetString() == std::string("entry"));

			rapidjson::Document copied;
			copied.Parse(copyWriter.saveToString().c_str());
			assertTrue(copied["copy"].GetBool());
		}
	};

}

#endif // __AMCTEST_UNITTEST_JSONWRITER
