
CToolpathAccessor
====================================================================================================


.. cpp:class:: LibMCEnv::CToolpathAccessor : public CBase 

	




	.. cpp:function:: std::string GetStorageUUID()

		Returns Toolpath storage UUID.

		:returns: Returns toolpath storage uuid.


	.. cpp:function:: LibMCEnv_uint32 GetLayerCount()

		Returns layer count.

		:returns: Returns layer count.


	.. cpp:function:: PToolpathLayer LoadLayer(const LibMCEnv_uint32 nLayerIndex)

		Reads and returns a layer object.

		:param nLayerIndex: Layer Index. 
		:returns: Toolpath layer object.


	.. cpp:function:: LibMCEnv_double GetUnits()

		Retrieves the toolpath units in mm.

		:returns: Toolpath units.


	.. cpp:function:: bool HasMetaData(const std::string & sNameSpace, const std::string & sName)

		Checks if a metadata value exists for this toolpath model.

		:param sNameSpace: Namespace of metadata. 
		:param sName: Name of metadata. 
		:returns: Returns if metadata exists.


	.. cpp:function:: std::string GetMetaDataValue(const std::string & sNameSpace, const std::string & sName)

		Returns the value of a metadata for this toolpath model.

		:param sNameSpace: Namespace of metadata. 
		:param sName: Name of metadata. 
		:returns: Returns the value


	.. cpp:function:: std::string GetMetaDataType(const std::string & sNameSpace, const std::string & sName)

		Returns the type of a metadata for this toolpath model.

		:param sNameSpace: Namespace of metadata. 
		:param sName: Name of metadata. 
		:returns: Returns the type


	.. cpp:function:: LibMCEnv_uint32 GetPartCount()

		Retrieves the number of parts in the toolpath.

		:returns: Number of parts.


	.. cpp:function:: PToolpathPart GetPart(const LibMCEnv_uint32 nPartIndex)

		Retrieves the part information of a toolpath.

		:param nPartIndex: Index of part. MUST be between 0 and PartCount-1 
		:returns: Part Instance


	.. cpp:function:: PToolpathPart FindPartByUUID(const std::string & sPartUUID)

		Finds the part information of a toolpath.

		:param sPartUUID: UUID of part. 
		:returns: Part Instance. Returns null if part does not exist.


.. cpp:type:: std::shared_ptr<CToolpathAccessor> LibMCEnv::PToolpathAccessor

	Shared pointer to CToolpathAccessor to easily allow reference counting.

