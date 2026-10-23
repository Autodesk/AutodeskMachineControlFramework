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

#ifndef __AMCTEST_UNITTEST_MESH
#define __AMCTEST_UNITTEST_MESH


#include "amc_unittests.hpp"
#include "amc_meshhandler.hpp"
#include "amc_meshscene.hpp"
#include "amc_meshsceneitem.hpp"
#include "amc_meshutils.hpp"
#include "amc_meshentity.hpp"
#include "common_utils.hpp"


namespace AMCUnitTest {

	class CUnitTestGroup_Mesh : public CUnitTestGroup {
	public:

		std::string getTestGroupName() override {
			return "Mesh";
		}

		void registerTests() override {
			registerTest("MeshUtilsTransforms", "Mesh transform helpers", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Mesh::testMeshUtilsTransforms, this));
			registerTest("MeshSceneItemBasics", "Mesh scene item fields and transforms", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Mesh::testMeshSceneItemBasics, this));
			registerTest("MeshSceneBasics", "Mesh scene add/find/remove items", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Mesh::testMeshSceneBasics, this));
			registerTest("MeshHandlerBasics", "Mesh handler entity and scene management", eUnitTestCategory::utMandatoryPass, std::bind(&CUnitTestGroup_Mesh::testMeshHandlerBasics, this));
		}

		void initializeTests() override {
		}

	private:

		bool nearlyEqual(const double a, const double b, const double epsilon = 1.0e-9)
		{
			return std::abs(a - b) < epsilon;
		}

		void testMeshUtilsTransforms()
		{
			auto identity = AMC::CMeshUtils::createIdentityTransform();
			for (uint32_t i = 0; i < 3; i++) {
				for (uint32_t j = 0; j < 3; j++) {
					if (i == j)
						assertTrue(nearlyEqual(identity.m_Matrix[i][j], 1.0));
					else
						assertTrue(nearlyEqual(identity.m_Matrix[i][j], 0.0));
				}
				assertTrue(nearlyEqual(identity.m_Translation[i], 0.0));
			}

			LibMCEnv::sModelDataTransform transformA = identity;
			transformA.m_Matrix[0][0] = 2.0;
			transformA.m_Matrix[1][1] = 3.0;
			transformA.m_Translation[0] = 10.0;
			transformA.m_Translation[1] = 20.0;
			transformA.m_Translation[2] = -5.0;

			auto multiplied = AMC::CMeshUtils::multiplyTransforms(identity, transformA);
			assertTrue(nearlyEqual(multiplied.m_Matrix[0][0], 2.0));
			assertTrue(nearlyEqual(multiplied.m_Matrix[1][1], 3.0));
			assertTrue(nearlyEqual(multiplied.m_Translation[0], 10.0));

			Lib3MF::sTransform transform3MF;
			for (uint32_t i = 0; i < 4; i++)
				for (uint32_t j = 0; j < 3; j++)
					transform3MF.m_Fields[i][j] = 0.0f;
			transform3MF.m_Fields[0][0] = 1.0f;
			transform3MF.m_Fields[1][1] = 2.0f;
			transform3MF.m_Fields[2][2] = 3.0f;
			transform3MF.m_Fields[3][0] = 4.0f;
			transform3MF.m_Fields[3][1] = 5.0f;
			transform3MF.m_Fields[3][2] = 6.0f;

			auto mapped = AMC::CMeshUtils::map3MFTransform(transform3MF);
			assertTrue(nearlyEqual(mapped.m_Matrix[0][0], 1.0));
			assertTrue(nearlyEqual(mapped.m_Matrix[1][1], 2.0));
			assertTrue(nearlyEqual(mapped.m_Matrix[2][2], 3.0));
			assertTrue(nearlyEqual(mapped.m_Translation[0], 4.0));
			assertTrue(nearlyEqual(mapped.m_Translation[1], 5.0));
			assertTrue(nearlyEqual(mapped.m_Translation[2], 6.0));

			auto roundTrip = AMC::CMeshUtils::mapTo3MFTransform(mapped);
			assertTrue(nearlyEqual(roundTrip.m_Fields[0][0], 1.0));
			assertTrue(nearlyEqual(roundTrip.m_Fields[1][1], 2.0));
			assertTrue(nearlyEqual(roundTrip.m_Fields[2][2], 3.0));
			assertTrue(nearlyEqual(roundTrip.m_Fields[3][0], 4.0));
			assertTrue(nearlyEqual(roundTrip.m_Fields[3][1], 5.0));
			assertTrue(nearlyEqual(roundTrip.m_Fields[3][2], 6.0));
		}

		void testMeshSceneItemBasics()
		{
			std::string itemUUID = AMCCommon::CUtils::createUUID();
			std::string entityUUID = AMCCommon::CUtils::createUUID();

			AMC::CMeshSceneItem item(itemUUID, entityUUID);
			assertTrue(item.getUUID() == AMCCommon::CUtils::normalizeUUIDString(itemUUID));
			assertTrue(item.getMeshEntityUUID() == AMCCommon::CUtils::normalizeUUIDString(entityUUID));

			auto transform = item.getTransform();
			for (uint32_t i = 0; i < 3; i++) {
				assertTrue(nearlyEqual(transform.m_Matrix[i][i], 1.0));
			}

			LibMCEnv::sModelDataTransform updated = AMC::CMeshUtils::createIdentityTransform();
			updated.m_Translation[0] = 12.0;
			item.updateTransform(updated);
			assertTrue(nearlyEqual(item.getTransform().m_Translation[0], 12.0));
		}

		void testMeshSceneBasics()
		{
			std::string sceneUUID = AMCCommon::CUtils::createUUID();
			AMC::CMeshScene scene(sceneUUID);
			assertTrue(scene.getSceneUUID() == AMCCommon::CUtils::normalizeUUIDString(sceneUUID));
			assertFalse(scene.isBoundToLoginSession());
			assertTrue(scene.getItemCount() == 0);

			LibMCEnv::sModelDataTransform transform = AMC::CMeshUtils::createIdentityTransform();
			auto item = scene.addItem(AMCCommon::CUtils::createUUID(), transform);
			assertTrue(scene.getItemCount() == 1);
			assertTrue(scene.getItem(0).get() == item.get());
			assertTrue(scene.findItem(item->getUUID(), true).get() == item.get());

			bool thrown = false;
			try {
				scene.getItem(2);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected getItem to throw on invalid index");

			std::string missingUUID = AMCCommon::CUtils::createUUID();
			assertTrue(scene.findItem(missingUUID, false).get() == nullptr);
			thrown = false;
			try {
				scene.findItem(missingUUID, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected findItem to throw on missing item");

			scene.removeItem(item->getUUID());
			assertTrue(scene.getItemCount() == 0);
		}

		void testMeshHandlerBasics()
		{
			AMC::CMeshHandler handler;

			std::string entityUUID = AMCCommon::CUtils::createUUID();
			auto entity = std::make_shared<AMC::CMeshEntity>(entityUUID, "Entity");

			assertFalse(handler.hasMeshEntity(entityUUID));
			handler.registerEntity(entity);
			assertTrue(handler.hasMeshEntity(entityUUID));
			assertTrue(handler.findMeshEntity(entityUUID, true).get() == entity.get());

			bool thrown = false;
			try {
				handler.registerEntity(entity);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected registerEntity to throw on duplicate");

			std::string missingUUID = AMCCommon::CUtils::createUUID();
			assertTrue(handler.findMeshEntity(missingUUID, false).get() == nullptr);
			thrown = false;
			try {
				handler.findMeshEntity(missingUUID, true);
			}
			catch (...) {
				thrown = true;
			}
			assertTrue(thrown, "Expected findMeshEntity to throw on missing entity");

			handler.unloadMeshEntity(entityUUID);
			assertFalse(handler.hasMeshEntity(entityUUID));

			auto scene = handler.createEmptyScene(false);
			assertTrue(scene.get() != nullptr);
			assertTrue(handler.findScene(scene->getSceneUUID(), true).get() == scene.get());

			handler.releaseScene(scene->getSceneUUID());
			assertTrue(handler.findScene(scene->getSceneUUID(), false).get() == nullptr);

			assertTrue(handler.getMeshEntityMemoryUsageInBytes() == 0);
			assertTrue(handler.getSceneMemoryUsageInBytes() == 0);
		}
	};

}

#endif // __AMCTEST_UNITTEST_MESH
