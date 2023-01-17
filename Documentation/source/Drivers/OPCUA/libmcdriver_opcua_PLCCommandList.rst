
CPLCCommandList
====================================================================================================


.. cpp:class:: LibMCDriver_OPCUA::CPLCCommandList : public CBase 

	




	.. cpp:function:: void AddCommand(classParam<CPLCCommand> pCommandInstance)

		Adds a command to the list. List must not be executed before.

		:param pCommandInstance: Add a command instance. 


	.. cpp:function:: void FinishList()

		Finish command list.



	.. cpp:function:: void ExecuteList()

		Execute command list.



	.. cpp:function:: bool WaitForList(const LibMCDriver_OPCUA_uint32 nReactionTimeInMS, const LibMCDriver_OPCUA_uint32 nWaitForTimeInMS)

		Wait for command list to finish executing

		:param nReactionTimeInMS: How much time the PLC may need to react to the command in Milliseconds. Will fail if no reaction in that time. 
		:param nWaitForTimeInMS: How long to wait for the command to be finished in Milliseconds. Will return false if command has not finished. 
		:returns: Returns true if the command was finished successfully.


	.. cpp:function:: void PauseList()

		Pause command list. Must be executed or resumed before.



	.. cpp:function:: void ResumeList()

		Resume command list. Must be paused before.



.. cpp:type:: std::shared_ptr<CPLCCommandList> LibMCDriver_OPCUA::PPLCCommandList

	Shared pointer to CPLCCommandList to easily allow reference counting.

