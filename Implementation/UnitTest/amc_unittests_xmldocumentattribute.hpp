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

#ifndef __AMCTEST_UNITTEST_XMLDOCUMENTATTRIBUTE
#define __AMCTEST_UNITTEST_XMLDOCUMENTATTRIBUTE

#include "amc_xmldocument.hpp"
#include "amc_xmldocumentattribute.hpp"

namespace AMCUnitTest {

    class CUnitTestGroup_XMLDocumentAttribute : public CUnitTestGroup {
    public:

        std::string getTestGroupName() override {
            return "XMLDocumentAttribute";
        }

        void registerTests() override {
            registerTest("CreateAndRetrieveAttribute", "Create attribute and retrieve name, value, namespace", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testCreateAndRetrieveAttribute, this));
            registerTest("SetAndGetValue", "Set and retrieve attribute value", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testSetAndGetValue, this));
            registerTest("PrefixedNameSameNamespace", "Prefix omitted if node and attribute namespace match", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testPrefixedNameSameNamespace, this));
            registerTest("PrefixedNameDifferentNamespace", "Prefix included if namespaces differ", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testPrefixedNameDifferentNamespace, this));
            registerTest("InvalidNamespaceShouldThrow", "Constructor throws if namespace is nullptr", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testInvalidNamespaceShouldThrow, this));
            registerTest("EmptyValueIsAccepted", "Setting an empty value should be allowed", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testEmptyValueIsAccepted, this));
            registerTest("LongAttributeValue", "Handle very long attribute values", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testLongAttributeValue, this));
            registerTest("SpecialCharacterValue", "Handle special XML characters in attribute values", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testSpecialCharacterValue, this));
            registerTest("AttributeBoundToCorrectNode", "Ensure attributes are bound to their nodes correctly", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testAttributeBoundToCorrectNode, this));
            registerTest("AttributeNameCaseSensitive", "Attribute names should be case-sensitive", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testAttributeNameCaseSensitive, this));
            registerTest("AttributeSerializationRoundtrip", "Ensure attributes survive roundtrip through XML", eUnitTestCategory::utOptionalPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testAttributeSerializationRoundtrip, this));
            registerTest("FuzzedAttributeValues", "Feed fuzzed data into attribute values", eUnitTestCategory::utOptionalPass, std::bind(&CUnitTestGroup_XMLDocumentAttribute::testFuzzedAttributeValues, this));

        }

        void initializeTests() override {
            // No special setup needed
        }

    private:

        AMC::PXMLDocumentInstance createBaseDocWithRoot(const std::string& nsURI = "http://example.com", const std::string& rootName = "root") {
            auto doc = std::make_shared<AMC::CXMLDocumentInstance>();
            doc->createEmptyDocument(rootName, nsURI);
            return doc;
        }

        void testCreateAndRetrieveAttribute() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto attr = root->AddAttribute(ns, "version", "1.0");

            assertTrue(attr->getAttributeName() == "version");
            assertTrue(attr->getNameSpace() == ns);
            assertTrue(attr->getValue() == "1.0");
        }

        void testSetAndGetValue() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto attr = root->AddAttribute(ns, "status", "ok");
            attr->setValue("updated");
            assertTrue(attr->getValue() == "updated");
        }

        void testPrefixedNameSameNamespace() {
            auto doc = createBaseDocWithRoot();
            auto ns = doc->GetDefaultNamespace();
            auto root = doc->GetRootNode();
            auto attr = root->AddAttribute(ns, "id", "42");

            // root and attr have same namespace prefix -> no prefix
            assertTrue(attr->getPrefixedName() == "id");
        }

        void testPrefixedNameDifferentNamespace() {
            auto doc = createBaseDocWithRoot();
            doc->RegisterNamespace("http://ns1.com", "ns1");
            doc->RegisterNamespace("http://ns2.com", "ns2");

            auto ns1 = doc->FindNamespaceByPrefix("ns1", true);
            auto ns2 = doc->FindNamespaceByPrefix("ns2", true);
            auto root = doc->GetRootNode();

            auto attr = std::make_shared<AMC::CXMLDocumentAttributeInstance>(doc.get(), root.get(), ns2, "config");
            attr->setValue("value");

            assertTrue(attr->getPrefixedName() == "ns2:config");
        }

        void testInvalidNamespaceShouldThrow() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();

            bool thrown = false;
            try {
                auto invalidAttr = std::make_shared<AMC::CXMLDocumentAttributeInstance>(doc.get(), root.get(), nullptr, "bad");
            } catch (...) {
                thrown = true;
            }

            assertTrue(thrown, "Expected exception for null namespace");
        }

        void testEmptyValueIsAccepted() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto attr = root->AddAttribute(ns, "empty", "");
            assertTrue(attr->getValue() == "", "Empty attribute value should be allowed");
        }


        void testLongAttributeValue() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            std::string longValue(2048, 'A');
            auto attr = root->AddAttribute(ns, "long", longValue);
            assertTrue(attr->getValue() == longValue, "Long value mismatch");
        }

        void testSpecialCharacterValue() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            std::string special = "<>&\"'";
            auto attr = root->AddAttribute(ns, "specialchars", special);
            assertTrue(attr->getValue() == special, "Special characters should be retained");
        }


        void testAttributeBoundToCorrectNode() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto attr = root->AddAttribute(ns, "nodeCheck", "yes");
            auto nodePtr = attr->getNode();
            assertTrue(nodePtr == root.get(), "Attribute node pointer should match parent node");
        }


        void testAttributeNameCaseSensitive() {
            auto doc = createBaseDocWithRoot();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "case", "lower");
            root->AddAttribute(ns, "Case", "upper");

            assertTrue(root->HasAttribute(ns.get(), "case"));
            assertTrue(root->HasAttribute(ns.get(), "Case"));
            assertTrue(root->GetAttributeCount() == 2, "Attributes should be case-sensitive");
        }

     

        void testAttributeSerializationRoundtrip() {
            auto doc = createBaseDocWithRoot("http://example.com", "root");
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            std::string value = "A&B<C>\"'\xE2\x9C\x93\xC3\xBC"; // includes ✓ (U+2713) and ü (U+00FC)
            root->AddAttribute(ns, "data", value);

            std::string xmlString = doc->SaveToString(true);

            AMC::CXMLDocumentInstance doc2;
            doc2.parseXMLString(xmlString);
            auto root2 = doc2.GetRootNode();
            auto ns2 = doc2.GetDefaultNamespace();

            auto attr = root2->FindAttribute(ns2.get(), "data", true);
            assertTrue(attr->getValue() == value, "Attribute value was not preserved after roundtrip");
        }

        void testFuzzedAttributeValues() {
            std::vector<std::string> fuzzSamples = {
                "normalText",
                "1234567890",
                "!@#$%^&*()_+",
                "<>&\"'",
                "unicode_check",
                std::string(512, 'x'),
                "",
                "      ",
                "\t\n\r",
                "\xE2\x9C\x94\xF0\x9F\xA6\x80\xF0\x9F\x9A\x80" // ✓🦀🚀 in UTF-8 hex
            };

            for (size_t i = 0; i < fuzzSamples.size(); ++i) {
                auto doc = createBaseDocWithRoot();
                auto root = doc->GetRootNode();
                auto ns = doc->GetDefaultNamespace();

                std::string key = "attr" + std::to_string(i);
                root->AddAttribute(ns, key, fuzzSamples[i]);
                auto attr = root->FindAttribute(ns.get(), key, true);
                assertTrue(attr->getValue() == fuzzSamples[i], "Fuzz test failed at index " + std::to_string(i));
            }
        }


    };

}


#endif // __AMCTEST_UNITTEST_XMLDOCUMENTATTRIBUTE
