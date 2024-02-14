
CDriver_ASL
====================================================================================================


.. cpp:class:: LibMCDriver_ASL::CDriver_ASL : public CDriver 

	A Driver for the Added Scientific Ltd Inkjet Driver boards.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: PDriverContext Connect(const std::string & sIdentifier, const std::string & sCOMPort)

		Connects to driver board.

		:param sIdentifier: Identifier of the device. 
		:param sCOMPort: Name of the device. 
		:returns: Returns the channel instance.


	.. cpp:function:: bool ContextExists(const std::string & sIdentifier)

		Returns if the channel exists..

		:param sIdentifier: Identifier of the device. 
		:returns: Flag if the channel exists.


	.. cpp:function:: PDriverContext FindContext(const std::string & sIdentifier)

		Find a communication channel by integer. Fails if the channel does not exist.

		:param sIdentifier: Identifier of the device. 
		:returns: Returns the channel instance.


.. cpp:type:: std::shared_ptr<CDriver_ASL> LibMCDriver_ASL::PDriver_ASL

	Shared pointer to CDriver_ASL to easily allow reference counting.

