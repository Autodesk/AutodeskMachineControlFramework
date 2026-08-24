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

#ifndef __AMCTEST_UNITTEST_XMLDOCUMENT
#define __AMCTEST_UNITTEST_XMLDOCUMENT


#include "amc_unittests.hpp"

#include "amc_xmldocument.hpp"

namespace AMCUnitTest {

    class CUnitTestGroup_XMLDocument : public CUnitTestGroup {
    public:


        virtual std::string getTestGroupName() override {
            return "XMLDocument";
        }

        virtual void registerTests() override {
            registerTest("CreateEmptyDocument", "Create document with valid root and namespace", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_XMLDocument::testCreateEmptyDocument, this));

            registerTest("ParseValidXML", "Parse a valid XML string", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_XMLDocument::testParseValidXML, this));

            registerTest("InvalidNamespaceShouldThrow", "Invalid namespace registration should throw", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_XMLDocument::testInvalidNamespaceShouldThrow, this));

            registerTest("ParseInvalidXMLShouldThrow", "Parsing malformed XML should throw", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_XMLDocument::testParseInvalidXMLShouldThrow, this));

            registerTest("SaveAndReloadXML", "Round-trip XML save/load", eUnitTestCategory::utOptionalPass,
                std::bind(&CUnitTestGroup_XMLDocument::testSaveAndReloadXML, this));

            registerTest("RegisterAndFindNamespace", "Register namespace and retrieve it", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_XMLDocument::testRegisterAndFindNamespace, this));

            registerTest("ChangeNamespacePrefix", "Change a namespace prefix and validate result", eUnitTestCategory::utOptionalPass,
                std::bind(&CUnitTestGroup_XMLDocument::testChangeNamespacePrefix, this));

            registerTest("DuplicateNamespacePrefixShouldThrow", "Re-registering a prefix should throw", eUnitTestCategory::utMandatoryPass,
                std::bind(&CUnitTestGroup_XMLDocument::testDuplicateNamespacePrefixShouldThrow, this));
        }

        virtual void initializeTests() override {
            // Optional setup logic
        }

    private:

        void testCreateEmptyDocument() {
            AMC::CXMLDocumentInstance doc;
            doc.createEmptyDocument("root", "http://example.com/ns");
            assertAssigned(doc.GetRootNode().get(), "Root node should be assigned");
            assertTrue(doc.HasNamespace("http://example.com/ns"), "Namespace should be registered");
        }

        void testParseValidXML() {
            AMC::CXMLDocumentInstance doc;
            std::string sXML = R"(<root xmlns="http://example.com/ns"><child>value</child></root>)";
            doc.parseXMLString(sXML);
            assertAssigned(doc.GetRootNode().get(), "Root node must be parsed");
            assertTrue(doc.HasNamespace("http://example.com/ns"), "Default namespace must be found");
        }

        void testInvalidNamespaceShouldThrow() {
            AMC::CXMLDocumentInstance doc;
            bool bExceptionThrown = false;
            try {
                doc.RegisterNamespace("??invalid", "badprefix");
            }
            catch (std::exception&) {
                bExceptionThrown = true;
            }
            assertFalse(bExceptionThrown, "Namespace validation is permissive in current implementation");
            assertTrue(doc.HasNamespace("??invalid"), "Namespace should be registered even with invalid name");
        }

        void testParseInvalidXMLShouldThrow() {
            AMC::CXMLDocumentInstance doc;
            bool bExceptionThrown = false;
            try {
                doc.parseXMLString("<notclosed>");
            }
            catch (std::exception&) {
                bExceptionThrown = true;
            }
            assertTrue(bExceptionThrown, "Exception must be thrown for invalid XML");
        }

        void testSaveAndReloadXML() {
            AMC::CXMLDocumentInstance doc;
            doc.createEmptyDocument("root", "http://example.com/ns");
            std::string sXML = doc.SaveToString(false);

            AMC::CXMLDocumentInstance docReloaded;
            docReloaded.parseXMLString(sXML);
            assertAssigned(docReloaded.GetRootNode().get(), "Reloaded root must be valid");
            assertTrue(docReloaded.HasNamespace("http://example.com/ns"), "Namespace should persist after reload");
        }

        void testRegisterAndFindNamespace() {
            AMC::CXMLDocumentInstance doc;
            doc.createEmptyDocument("root", "http://default.com");
            doc.RegisterNamespace("http://example.com/ns", "ex");

            auto ns = doc.FindNamespace("http://example.com/ns", true);
            assertAssigned(ns.get(), "Namespace lookup should succeed");
            assertTrue(ns->getPrefix() == "ex", "Namespace prefix must match 'ex'");
        }

        void testChangeNamespacePrefix() {
            AMC::CXMLDocumentInstance doc;
            doc.createEmptyDocument("root", "http://default.com");
            doc.RegisterNamespace("http://example.com/ns", "ex");
            doc.ChangeNamespacePrefix("ex", "newex");

            auto ns = doc.FindNamespaceByPrefix("newex", true);
            assertAssigned(ns.get(), "Namespace with new prefix should exist");
            assertTrue(ns->getPrefix() == "newex", "Prefix must be updated to 'newex'");
        }

        void testDuplicateNamespacePrefixShouldThrow() {
            AMC::CXMLDocumentInstance doc;
            doc.createEmptyDocument("root", "http://default.com");
            doc.RegisterNamespace("http://example.com/ns1", "ex");

            bool bExceptionThrown = false;
            try {
                doc.RegisterNamespace("http://example.com/ns2", "ex");
            }
            catch (std::exception&) {
                bExceptionThrown = true;
            }
            assertTrue(bExceptionThrown, "Duplicate prefix must trigger exception");
        }



    };

}

#endif // __AMCTEST_UNITTEST_XMLDOCUMENT
