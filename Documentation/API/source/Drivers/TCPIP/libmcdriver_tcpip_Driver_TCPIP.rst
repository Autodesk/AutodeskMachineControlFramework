
CDriver_TCPIP
====================================================================================================


.. cpp:class:: LibMCDriver_TCPIP::CDriver_TCPIP : public CDriver 

	A generic TCP/IP Driver.




	.. cpp:function:: void SetToSimulationMode()

		Turns the driver into a simulation mode.



	.. cpp:function:: bool IsSimulationMode()

		Returns if the driver is in simulation mode.

		:returns: Flag if driver is in simulation mode.


	.. cpp:function:: void Connect(const std::string & sIPAddress, const LibMCDriver_TCPIP_uint32 nPort, const LibMCDriver_TCPIP_uint32 nTimeout)

		Connects to TCP Server.

		:param sIPAddress: IP Address. 
		:param nPort: Port. 
		:param nTimeout: Timeout in milliseconds. 


	.. cpp:function:: bool IsConnected()

		Returns if the driver is connected.

		:returns: .


	.. cpp:function:: void Disconnect()

		Disconnects from the Server.



	.. cpp:function:: void SendBuffer(const CInputVector<LibMCDriver_TCPIP_uint8> & BufferBuffer)

		Sends a buffer of bytes to the Server.

		:param BufferBuffer: packet payload. 


	.. cpp:function:: bool WaitForData(const LibMCDriver_TCPIP_uint32 nTimeOutInMS)

		Waits for a server packet to arrive.

		:param nTimeOutInMS: timeout in Milliseconds. 
		:returns: Flag if a new packet has arrived.


	.. cpp:function:: PDriver_TCPIPPacket ReceivePacket(const LibMCDriver_TCPIP_uint32 nPacketSize, const LibMCDriver_TCPIP_uint32 nTimeOutInMS)

		Receives a fixed length packet. Fails if there is a connection error.

		:param nPacketSize: Size of packet to receive. 
		:param nTimeOutInMS: timeout in Milliseconds. 
		:returns: Port.


.. cpp:type:: std::shared_ptr<CDriver_TCPIP> LibMCDriver_TCPIP::PDriver_TCPIP

	Shared pointer to CDriver_TCPIP to easily allow reference counting.

