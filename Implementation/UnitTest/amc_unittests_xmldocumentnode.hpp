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


#ifndef __AMCTEST_UNITTEST_XMLDOCUMENTNODE
#define __AMCTEST_UNITTEST_XMLDOCUMENTNODE

#include "amc_unittests.hpp"
#include "amc_xmldocument.hpp"
#include "amc_xmldocumentnode.hpp"

#include "Libraries/PugiXML/pugixml.hpp"
#include <sstream>

namespace AMCUnitTest
{

    class CUnitTestGroup_XMLDocumentNode : public CUnitTestGroup {
    public:


        std::string getTestGroupName() override {
            return "XMLDocumentNode";
        }

        void registerTests() override {
            registerTest("AddAndFindAttribute", "Add attribute and retrieve it", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testAddAndFindAttribute, this));
            registerTest("AddAndRemoveAttribute", "Add and remove attribute", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testAddAndRemoveAttribute, this));
            registerTest("SetTextContentSuccess", "Set text content successfully", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testSetTextContentSuccess, this));
            registerTest("SetTextContentFailWithChildren", "Text setting fails if node has children", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testSetTextContentFailWithChildren, this));
            registerTest("AddAndFindChildNode", "Add and retrieve a child node", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testAddAndFindChildNode, this));
            registerTest("CopyNodeContent", "Copy all content from one node to another", eUnitTestCategory::utOptionalPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testCopyNodeContent, this));
            registerTest("RemoveChildrenByName", "Add and remove children by name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testRemoveChildrenByName, this));
            registerTest("CountChildrenByName", "Count children with a specific name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testCountChildrenByName, this));
            registerTest("PrefixedNodeAndAttributeNames", "Test prefixed names are resolved correctly", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testPrefixedNodeAndAttributeNames, this));
            registerTest("DuplicateAttributeShouldFail", "Adding the same attribute twice should fail", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testDuplicateAttributeShouldFail, this));
            registerTest("InvalidParamShouldThrow", "Various functions should throw on nullptr", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testInvalidParamShouldThrow, this));
            registerTest("SplitNamespaceName", "Test namespace splitting from prefixed name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testSplitNamespaceName, this));
            registerTest("StoreToPugiXML", "Export node to pugi::xml_document and check format", eUnitTestCategory::utOptionalPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testStoreToPugiXML, this));
            registerTest("AddChildFailsWithTextContent", "Adding a child to a node that has text should fail", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testAddChildFailsWithTextContent, this));
            registerTest("RemoveNonexistentAttribute", "Removing a non-existent attribute should be safe", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testRemoveNonexistentAttribute, this));
            registerTest("RemoveAttributeByIndex", "Removing attributes by index and checking order", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testRemoveAttributeByIndex, this));
            registerTest("RemoveChildPointer", "Remove a child by pointer reference", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testRemoveChildPointer, this));
            registerTest("ChildCountTracking", "Track and verify multiple children with the same name", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testChildCountTracking, this));
            registerTest("InvalidAttributeIndex", "Accessing or removing invalid attribute index should throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testInvalidAttributeIndex, this));
            registerTest("GetPrefixedNameNoPrefix", "Check prefixed name without a namespace prefix", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testGetPrefixedNameNoPrefix, this));
            registerTest("CopyDeepNode", "Recursively copy a node with children and text", eUnitTestCategory::utOptionalPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testCopyDeepNode, this));
            registerTest("NamespacePrefixConflictShouldThrow", "Registering duplicate namespace prefix should throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testNamespacePrefixConflictShouldThrow, this));
            registerTest("AddDuplicateChildShouldNotThrow", "Multiple children with same name should not throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testAddDuplicateChildShouldNotThrow, this));
            registerTest("CrossDocumentCopyShouldThrow", "Copying across documents should throw", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testCrossDocumentCopyShouldThrow, this));
            registerTest("TextContentClearsChildrenShouldFail", "Cannot set text content if children exist", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testTextContentClearsChildrenShouldFail, this));
            registerTest("RemoveAttributeAfterCopy", "Removing copied attribute should clean up maps", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testRemoveAttributeAfterCopy, this));
            registerTest("CompareNameLogic", "compareName should match namespace and name properly", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testCompareNameLogic, this));
            registerTest("ChildMapIntegrityAfterRemovals", "Child counters should update correctly after removals", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_XMLDocumentNode::testChildMapIntegrityAfterRemovals, this));

        }

        void initializeTests() override {
            // No special setup
        }

    private:

        AMC::PXMLDocumentInstance createBaseDocument(std::string rootName = "root", std::string ns = "http://example.com") {
            auto doc = std::make_shared<AMC::CXMLDocumentInstance>();
            doc->createEmptyDocument(rootName, ns);
            return doc;
        }

        void testAddAndFindAttribute() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "id", "123");
            auto attr = root->FindAttribute(ns.get(), "id", true);
            assertTrue(attr->getValue() == "123", "Attribute value mismatch");
        }

        void testAddAndRemoveAttribute() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "version", "1.0");
            assertTrue(root->HasAttribute(ns.get(), "version"));
            root->RemoveAttribute(ns.get(), "version");
            assertFalse(root->HasAttribute(ns.get(), "version"));
        }

        void testSetTextContentSuccess() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            root->SetTextContent("Hello, world!");
            assertTrue(root->GetTextContent() == "Hello, world!");
        }

        void testSetTextContentFailWithChildren() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddChild(ns, "child");
            bool exceptionThrown = false;
            try {
                root->SetTextContent("This should fail");
            }
            catch (std::exception&) {
                exceptionThrown = true;
            }
            assertTrue(exceptionThrown, "Setting text on node with children should fail");
        }

        void testAddAndFindChildNode() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto child = root->AddChild(ns, "child");
            assertTrue(root->HasChild(ns.get(), "child"));
            assertTrue(root->HasUniqueChild(ns.get(), "child"));
            auto found = root->FindChild(ns.get(), "child", true);
            assertAssigned(found.get());
        }

        void testCopyNodeContent() {
            auto doc = createBaseDocument();
            auto ns = doc->GetDefaultNamespace();
            auto node1 = doc->GetRootNode();
            node1->AddAttribute(ns, "type", "primary");
            node1->SetTextContent("text");

            auto node2 = std::make_shared<AMC::CXMLDocumentNodeInstance>(doc.get(), nullptr, ns, "copy");
            node2->CopyFrom(node1.get());

            assertTrue(node2->GetTextContent() == "text", "Copied text mismatch");
            assertTrue(node2->HasAttribute(ns.get(), "type"), "Copied attribute missing");
        }

        void testRemoveChildrenByName() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddChild(ns, "a");
            root->AddChild(ns, "a");
            root->AddChild(ns, "b");

            root->RemoveChildrenWithName(ns.get(), "a");
            assertFalse(root->HasChild(ns.get(), "a"));
            assertTrue(root->HasChild(ns.get(), "b"));
        }

        void testCountChildrenByName() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddChild(ns, "item");
            root->AddChild(ns, "item");
            root->AddChild(ns, "item");
            root->AddChild(ns, "other");

            uint64_t count = root->CountChildrenByName(ns.get(), "item");
            assertTrue(count == 3, "Expected 3 children named 'item'");
        }

        void testPrefixedNodeAndAttributeNames() {
            auto doc = createBaseDocument();
            doc->RegisterNamespace("http://ns.example.com", "ns");

            auto ns1 = doc->FindNamespaceByPrefix("ns", true);
            auto root = doc->GetRootNode();

            auto node = root->AddChild(ns1, "child");
            node->AddAttribute(ns1, "attr", "value");

            assertTrue(node->getPrefixedName() == "ns:child");
            auto attr = node->FindAttribute(ns1.get(), "attr", true);
            assertTrue(attr->getPrefixedName() == "attr");
        }

        void testDuplicateAttributeShouldFail() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "id", "1");
            bool thrown = false;
            try {
                root->AddAttribute(ns, "id", "2");
            }
            catch (std::exception&) {
                thrown = true;
            }
            assertTrue(thrown, "Adding duplicate attribute should throw");
        }

        void testInvalidParamShouldThrow() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            bool thrown = false;

            try {
                root->HasAttribute(nullptr, "id");
            }
            catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Expected exception on nullptr namespace");

            thrown = false;
            try {
                root->AddChild(nullptr, "invalid");
            }
            catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Expected exception on null namespace for AddChild");

            thrown = false;
            try {
                root->FindChild(nullptr, "child", true);
            }
            catch (...) {
                thrown = true;
            }
            assertTrue(thrown, "Expected exception on null namespace for FindChild");
        }

        void testSplitNamespaceName() {
            std::string fullName = "prefix:name";
            std::string name;
            std::string prefix;

            AMC::CXMLDocumentNodeInstance::splitNameSpaceName(fullName, name, prefix);
            assertTrue(name == "name");
            assertTrue(prefix == "prefix");

            fullName = "noprefix";
            AMC::CXMLDocumentNodeInstance::splitNameSpaceName(fullName, name, prefix);
            assertTrue(name == "noprefix");
            assertTrue(prefix == "");
        }

        void testStoreToPugiXML() {
            auto doc = createBaseDocument("myroot", "http://example.com");
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "version", "1.0");
            auto child = root->AddChild(ns, "child");
            child->SetTextContent("hello");

            auto xmlDoc = std::make_shared<pugi::xml_document>();
            auto rootNode = xmlDoc->append_child(root->getPrefixedName().c_str());

            root->storeToPugiNode(xmlDoc.get(), &rootNode);
            struct pugi_xml_string_writer : pugi::xml_writer {
                std::stringstream stream;
                void write(const void* data, size_t size) override {
                    stream.write(static_cast<const char*>(data), size);
                }
            };

            pugi_xml_string_writer writer;
            xmlDoc->save(writer, "  ", pugi::format_indent);

            std::string xmlOutput = writer.stream.str();

            assertTrue(xmlOutput.find("<child>hello</child>") != std::string::npos, "Serialized XML missing child content");
            assertTrue(xmlOutput.find("version=\"1.0\"") != std::string::npos, "Serialized XML missing attribute");
        }

        void testAddChildFailsWithTextContent() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->SetTextContent("Some text");
            bool thrown = false;
            try {
                root->AddChild(ns, "child");
            }
            catch (std::exception&) {
                thrown = true;
            }
            assertTrue(thrown, "Adding child after text should throw");
        }



        void testRemoveAttributeByIndex() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "a", "1");
            root->AddAttribute(ns, "b", "2");

            root->RemoveAttributeByIndex(0);
            assertTrue(root->HasAttribute(ns.get(), "b"));
            assertFalse(root->HasAttribute(ns.get(), "a"));
        }

        void testRemoveChildPointer() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto child = root->AddChild(ns, "x");
            root->RemoveChild(child.get());

            assertFalse(root->HasChild(ns.get(), "x"));
        }

        void testChildCountTracking() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddChild(ns, "x");
            root->AddChild(ns, "x");

            assertTrue(root->CountChildrenByName(ns.get(), "x") == 2);
            assertFalse(root->HasUniqueChild(ns.get(), "x"));
        }

        void testInvalidAttributeIndex() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();

            bool thrown = false;
            try {
                root->GetAttribute(0);
            }
            catch (std::exception&) {
                thrown = true;
            }
            assertTrue(thrown, "Accessing invalid attribute index should throw");
        }

        void testGetPrefixedNameNoPrefix() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();

            assertTrue(root->getPrefixedName() == "root");
        }

        void testCopyDeepNode() {
            auto doc = createBaseDocument();
            auto ns = doc->GetDefaultNamespace();
            auto root = doc->GetRootNode();

            auto child = root->AddChild(ns, "child");
            auto grandchild = child->AddChild(ns, "grand");

            grandchild->SetTextContent("deep");
            auto copy = std::make_shared<AMC::CXMLDocumentNodeInstance>(doc.get(), nullptr, ns, "copy");
            copy->CopyFrom(root.get());

            auto copiedChild = copy->FindChild(ns.get(), "child", true);
            auto copiedGrand = copiedChild->FindChild(ns.get(), "grand", true);
            assertAssigned(copiedGrand.get(), "Deep grandchild should exist");
        }

        void testRemoveNonexistentAttribute() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            // Removing before it exists should not throw
            root->RemoveAttribute(ns.get(), "nonexistent");

            // Add and remove to confirm it's clean
            root->AddAttribute(ns, "attr", "value");
            root->RemoveAttribute(ns.get(), "attr");

            assertFalse(root->HasAttribute(ns.get(), "attr"));
        }


        void testNamespacePrefixConflictShouldThrow() {
            auto doc = createBaseDocument();
            doc->RegisterNamespace("http://example.com/ns1", "pfx");

            bool thrown = false;
            try {
                doc->RegisterNamespace("http://example.com/ns2", "pfx"); // same prefix, different URI
            }
            catch (...) {
                thrown = true;
            }

            assertTrue(thrown, "Registering duplicate namespace prefix should throw.");
        }


        void testAddDuplicateChildShouldNotThrow() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddChild(ns, "duplicate");
            root->AddChild(ns, "duplicate");

            assertTrue(root->CountChildrenByName(ns.get(), "duplicate") == 2);
        }


        void testCrossDocumentCopyShouldThrow() {
            auto doc1 = createBaseDocument();
            auto doc2 = createBaseDocument("other", "http://other.com");

            auto root1 = doc1->GetRootNode();
            auto root2 = doc2->GetRootNode();
            auto ns2 = doc2->GetDefaultNamespace();
            root2->AddAttribute(ns2, "attr", "value");

            bool thrown = false;
            try {
                root1->CopyFrom(root2.get()); // mismatch of document instances
            }
            catch (...) {
                thrown = true;
            }

            assertTrue(thrown, "Copying nodes between documents should throw.");
        }


        void testTextContentClearsChildrenShouldFail() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddChild(ns, "a");

            bool thrown = false;
            try {
                root->SetTextContent("conflict");
            }
            catch (...) {
                thrown = true;
            }

            assertTrue(thrown, "Text content cannot overwrite existing children.");
        }

        void testRemoveAttributeAfterCopy() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            root->AddAttribute(ns, "x", "1");

            auto copy = std::make_shared<AMC::CXMLDocumentNodeInstance>(doc.get(), nullptr, ns, "copy");
            copy->CopyFrom(root.get());

            assertTrue(copy->HasAttribute(ns.get(), "x"), "Attribute must exist after copy.");
            copy->RemoveAttribute(ns.get(), "x");
            assertFalse(copy->HasAttribute(ns.get(), "x"), "Attribute must be gone after removal.");
        }


        void testCompareNameLogic() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            assertTrue(root->compareName("http://example.com", "root"), "compareName should match correctly.");
            bool thrown = false;
            try {
                root->compareName("http://wrong.com", "root");
            }
            catch (std::exception&) {
                thrown = true;
            }
            assertTrue(thrown, "Namespace mismatch must throw.");
            assertFalse(root->compareName("http://example.com", "wrong"), "Name mismatch must fail.");
        }


        void testChildMapIntegrityAfterRemovals() {
            auto doc = createBaseDocument();
            auto root = doc->GetRootNode();
            auto ns = doc->GetDefaultNamespace();

            auto child1 = root->AddChild(ns, "node");
            auto child2 = root->AddChild(ns, "node");

            root->RemoveChild(child1.get());
            assertTrue(root->CountChildrenByName(ns.get(), "node") == 1);
            root->RemoveChild(child2.get());
            assertTrue(root->CountChildrenByName(ns.get(), "node") == 0);
        }



    };

}

#endif // __AMCTEST_UNITTEST_XMLDOCUMENTNODE
