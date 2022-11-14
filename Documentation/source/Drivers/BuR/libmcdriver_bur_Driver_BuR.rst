
CDriver_BuR
====================================================================================================


.. cpp:class:: LibMCDriver_BuR::CDriver_BuR : public CDriver 

	A Driver for a BR Automation PLC.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: void Connect(const std::string & sIPAddress, const LibMCDriver_BuR_uint32 nPort, const LibMCDriver_BuR_uint32 nTimeout)

		Connects to a BuR PLC Controller.

		:param sIPAddress: IP Address of PLC Service. 
		:param nPort: Port of PLC Service. 
		:param nTimeout: Timeout in milliseconds. 


	.. cpp:function:: void Disconnect()

		Disconnects from the BuR PLC Controller.



	.. cpp:function:: PPLCCommandList CreateCommandList()

		Create Command

		:returns: Command list instance


	.. cpp:function:: PPLCCommand CreateCommand(const std::string & sCommandName)

		Creates a command instance.

		:param sCommandName: Command Name. 
		:returns: Returns a command instance.


	.. cpp:function:: void StartJournaling()

		Start Journaling.



	.. cpp:function:: void StopJournaling()

		Stop Journaling.



	.. cpp:function:: void RefreshJournal()

		Refresh Journal.



.. cpp:type:: std::shared_ptr<CDriver_BuR> LibMCDriver_BuR::PDriver_BuR

	Shared pointer to CDriver_BuR to easily allow reference counting.

