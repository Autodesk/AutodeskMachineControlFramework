
CDriver_S7Net
====================================================================================================


.. cpp:class:: LibMCDriver_S7Net::CDriver_S7Net : public CDriver 

	A Driver for a S7 PLC.




	.. cpp:function:: void Connect(const eS7CPUType eCPUType, const std::string & sIPAddress, const LibMCDriver_S7Net_uint32 nRack, const LibMCDriver_S7Net_uint32 nSlot)

		Creates and initializes a new S7 PLC.

		:param eCPUType: S7 CPU Type 
		:param sIPAddress: PLC IP Address 
		:param nRack: Rack Number 
		:param nSlot: Slot Number 


	.. cpp:function:: void Disconnect()

		Disconnects from the S7 PLC.



	.. cpp:function:: PPLCCommand CreateCommand(const std::string & sCommand)

		Create Command

		:param sCommand: Command to execute 
		:returns: Command instance


	.. cpp:function:: void ExecuteCommand(classParam<CPLCCommand> pPLCCommand)

		Execute Command

		:param pPLCCommand: Command instance 


	.. cpp:function:: bool WaitForCommand(classParam<CPLCCommand> pPLCCommand, const LibMCDriver_S7Net_uint32 nReactionTimeInMS, const LibMCDriver_S7Net_uint32 nWaitForTimeInMS)

		Wait for Command to finish executing

		:param pPLCCommand: Command instance 
		:param nReactionTimeInMS: How much time the PLC may need to react to the command in Milliseconds. Will fail if no reaction in that time. 
		:param nWaitForTimeInMS: How long to wait for the command to be finished in Milliseconds. Will return false if command has not finished. 
		:returns: Returns true if the command was finished successfully.


.. cpp:type:: std::shared_ptr<CDriver_S7Net> LibMCDriver_S7Net::PDriver_S7Net

	Shared pointer to CDriver_S7Net to easily allow reference counting.

