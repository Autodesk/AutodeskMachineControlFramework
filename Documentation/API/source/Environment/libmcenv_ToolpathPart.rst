
CToolpathPart
====================================================================================================


.. cpp:class:: LibMCEnv::CToolpathPart : public CBase 

	




	.. cpp:function:: std::string GetName()

		Returns Part Name.

		:returns: Returns toolpath part name.


	.. cpp:function:: std::string GetUUID()

		Returns Part UUID.

		:returns: Returns toolpath part uuid.


	.. cpp:function:: std::string GetMeshUUID()

		Returns Mesh UUID of the part.

		:returns: Returns toolpath part mesh uuid.


	.. cpp:function:: sToolpathPartTransform GetTransform()

		Returns Mesh Transform of the part.

		:returns: Returns the mesh transform of the toolpath.


.. cpp:type:: std::shared_ptr<CToolpathPart> LibMCEnv::PToolpathPart

	Shared pointer to CToolpathPart to easily allow reference counting.

