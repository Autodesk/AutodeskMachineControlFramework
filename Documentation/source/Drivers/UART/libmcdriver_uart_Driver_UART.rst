
CDriver_UART
====================================================================================================


.. cpp:class:: LibMCDriver_UART::CDriver_UART : public CDriver 

	A generic UART Driver.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: void SetParity(const eUARTParity eParity)

		Sets a parity mode for the next connection.

		:param eParity: Parity mode. 


	.. cpp:function:: eUARTParity GetParity()

		Returns parity mode of the next connection.

		:returns: Parity mode.


	.. cpp:function:: void SetStopBits(const eUARTStopBits eStopBits)

		Sets the stop bits for the next connection.

		:param eStopBits: Stop bits 


	.. cpp:function:: eUARTStopBits GetStopBits()

		Returns the stop bits of the next connection.

		:returns: Stop bits


	.. cpp:function:: void SetByteSize(const eUARTByteSize eByteSize)

		Sets the bytesize for the next connection.

		:param eByteSize: Stop bits 


	.. cpp:function:: eUARTByteSize GetByteSize()

		Returns the bytesize of the next connection.

		:returns: Stop bits


	.. cpp:function:: void SetFlowControl(const eUARTFlowControl eFlowControl)

		Sets the flow control for the next connection.

		:param eFlowControl: Flow control 


	.. cpp:function:: eUARTFlowControl GetFlowControl()

		Returns the flow control of the next connection.

		:returns: Flow control


	.. cpp:function:: void Connect(const std::string & sDeviceAddress, const LibMCDriver_UART_uint32 nBaudRate, const LibMCDriver_UART_uint32 nTimeout)

		Connects to a UART device.

		:param sDeviceAddress: Device Address of COM Port. 
		:param nBaudRate: BaudRate in baud. 
		:param nTimeout: Timeout in milliseconds. 


	.. cpp:function:: void Disconnect()

		Disconnects from device



	.. cpp:function:: bool IsConnected()

		Returns if the driver is connected.

		:returns: .


	.. cpp:function:: void SendString(const std::string & sStringToSend, const LibMCDriver_UART_uint32 nTimeout)

		Sends a string over UART.

		:param sStringToSend: String to send 
		:param nTimeout: Timeout in milliseconds. 


	.. cpp:function:: std::string ReceiveString(const LibMCDriver_UART_uint32 nTimeout, const std::string & sReceiveStringTermination)

		Waits for a received string.

		:param nTimeout: Timeout in milliseconds. 
		:param sReceiveStringTermination: Termination String for retrieval ending before timeout. 
		:returns: Received string. Maximum string length is 64kB.


	.. cpp:function:: std::string SendAndReceiveString(const std::string & sStringToSend, const std::string & sReceiveStringTermination, const LibMCDriver_UART_uint32 nTimeout)

		Sends a string over UART and waits for a returning string.

		:param sStringToSend: String to send 
		:param sReceiveStringTermination: Termination String for retrieval ending before timeout. 
		:param nTimeout: Timeout in milliseconds. 
		:returns: Received string. Maximum string length is 64kB.


.. cpp:type:: std::shared_ptr<CDriver_UART> LibMCDriver_UART::PDriver_UART

	Shared pointer to CDriver_UART to easily allow reference counting.

