
CDriver_Raylase
====================================================================================================


.. cpp:class:: LibMCDriver_Raylase::CDriver_Raylase : public CDriver 

	A LaserScanner Driver for a Raylase card.




	.. cpp:function:: void SetToSimulationMode()

		Sets the driver in Simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in Simulation mode.

		:returns: Simulation mode is active.


	.. cpp:function:: void LoadSDK()

		Initializes the Raylase SDK.



	.. cpp:function:: PRaylaseCard ConnectByIP(const std::string & sCardName, const std::string & sCardIP, const LibMCDriver_Raylase_uint32 nPort, const LibMCDriver_Raylase_double dMaxLaserPowerInWatts)

		Connects to the Raylase Scanner Card. Fails if connection is not successful.

		:param sCardName: Name of scanner to be connected to. MUST not be in use. 
		:param sCardIP: IP Address to connect to. 
		:param nPort: Port to connect to. 
		:param dMaxLaserPowerInWatts: Maximum laser power in Watts. 
		:returns: Instance of connected card.


	.. cpp:function:: PRaylaseCard GetConnectedCard(const std::string & sCardName)

		Retrieves a connected card instance.

		:param sCardName: Name of scanner card to retrieve. MUST have been connected before. 
		:returns: Instance of connected card.


.. cpp:type:: std::shared_ptr<CDriver_Raylase> LibMCDriver_Raylase::PDriver_Raylase

	Shared pointer to CDriver_Raylase to easily allow reference counting.

