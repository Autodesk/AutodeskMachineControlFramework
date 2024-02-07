
CDriver_TML
====================================================================================================


.. cpp:class:: LibMCDriver_TML::CDriver_TML : public CDriver 

	A Driver for the Technosoft Motion Library SDK.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: void SetCustomSDKResource(const std::string & sLibResourceName, const std::string & sCommsResourceName)

		Sets the machine resource name of the TML SDK to load. MUST be called before Connect or it has no effect.

		:param sLibResourceName: Resource name of core machine package that contains the proprietary tml_lib.dll. Empty means standard naming applies. 
		:param sCommsResourceName: Resource name of core machine package that contains the proprietary tmlcomms.dll. Empty means standard naming applies. 


	.. cpp:function:: PChannel OpenChannel(const std::string & sIdentifier, const std::string & sDeviceName, const eChannelType eChannelTypeToUse, const eProtocolType eProtocolTypeToUse, const LibMCDriver_TML_uint32 nHostID, const LibMCDriver_TML_uint32 nBaudrate)

		Opens a communication channel.

		:param sIdentifier: Identifier of the device. 
		:param sDeviceName: Name of the device. 
		:param eChannelTypeToUse: Type of the interface device. 
		:param eProtocolTypeToUse: Type of the protocol. 
		:param nHostID: Host ID to use. 
		:param nBaudrate: Baudrate to use. 
		:returns: Returns the channel instance.


	.. cpp:function:: bool ChannelExists(const std::string & sIdentifier)

		Returns if the channel exists..

		:param sIdentifier: Identifier of the device. 
		:returns: Flag if the channel exists.


	.. cpp:function:: PChannel FindChannel(const std::string & sIdentifier)

		Find a communication channel by integer. Fails if the channel does not exist.

		:param sIdentifier: Identifier of the device. 
		:returns: Returns the channel instance.


.. cpp:type:: std::shared_ptr<CDriver_TML> LibMCDriver_TML::PDriver_TML

	Shared pointer to CDriver_TML to easily allow reference counting.

