
CBuild
====================================================================================================


.. cpp:class:: LibMCEnv::CBuild : public CBase 

	Build instance to access build files




	.. cpp:function:: std::string GetName()

		Returns name of the build.

		:returns: Name of the build.


	.. cpp:function:: std::string GetBuildUUID()

		Returns uuid of the build.

		:returns: UUID of the build.


	.. cpp:function:: std::string GetStorageUUID()

		Returns storage uuid of the build.

		:returns: Storage UUID of the build.


	.. cpp:function:: std::string GetStorageSHA256()

		Returns SHA256 of the build stream.

		:returns: SHA256 of the build stream.


	.. cpp:function:: LibMCEnv_uint32 GetLayerCount()

		Returns cached layer count of the toolpath.

		:returns: Returns layer count.


	.. cpp:function:: void LoadToolpath()

		loads the a toolpath into memory



	.. cpp:function:: void UnloadToolpath()

		unloads the a toolpath from memory, if it has been loaded before.



	.. cpp:function:: bool ToolpathIsLoaded()

		checks, if a toolpath object is loaded to memory.

		:returns: returns if toolpath is loaded.


	.. cpp:function:: PToolpathAccessor CreateToolpathAccessor()

		Creates an accessor object for a toolpath. Toolpath MUST have been loaded with LoadToolpath before.

		:returns: Toolpath instance.


	.. cpp:function:: std::string AddBinaryData(const std::string & sName, const std::string & sMIMEType, const CInputVector<LibMCEnv_uint8> & ContentBuffer)

		Adds binary data to store with the build.

		:param sName: Name of the attache data block. 
		:param sMIMEType: Mime type of the data. 
		:param ContentBuffer: Stream content to store 
		:returns: Data UUID of the attachment.


.. cpp:type:: std::shared_ptr<CBuild> LibMCEnv::PBuild

	Shared pointer to CBuild to easily allow reference counting.

