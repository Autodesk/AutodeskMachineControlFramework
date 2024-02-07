
CChannel
====================================================================================================


.. cpp:class:: LibMCDriver_TML::CChannel : public CBase 

	A Channel of a TML Connection.




	.. cpp:function:: std::string GetIdentifier()

		Returns the channel identifier.

		:returns: Channel identifier.


	.. cpp:function:: PAxis SetupAxis(const std::string & sIdentifier, const LibMCDriver_TML_uint32 nAxisID, const CInputVector<LibMCDriver_TML_uint8> & ConfigurationBuffer, const LibMCDriver_TML_uint32 nCountsPerMM)

		Setups an axis for this channel. The identifier MUST be globally unique.

		:param sIdentifier: Identifier for the axis. Fails if axis already exist. 
		:param nAxisID: Hardware ID of the axis. MUST be unique in the channel. 
		:param ConfigurationBuffer: Configuration ZIP file for the axis. 
		:param nCountsPerMM: Sets the mm per count used for all moves and accelerations. 
		:returns: Returns the axis instance.


	.. cpp:function:: PAxis FindAxis(const std::string & sIdentifier)

		Finds an existing axis of this channel.

		:param sIdentifier: Identifier for the axis. Fails if axis already exist. 
		:returns: Returns the axis instance.


	.. cpp:function:: bool AxisExists(const std::string & sIdentifier)

		Returns if the axis exists on this channel.

		:param sIdentifier: Identifier of the axis. 
		:returns: Flag if the axis exists.


	.. cpp:function:: void Close()

		Closes the channel. Any other call will fail after closing.



.. cpp:type:: std::shared_ptr<CChannel> LibMCDriver_TML::PChannel

	Shared pointer to CChannel to easily allow reference counting.

