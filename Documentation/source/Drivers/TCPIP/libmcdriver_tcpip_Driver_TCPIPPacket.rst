
CDriver_TCPIPPacket
====================================================================================================


.. cpp:class:: LibMCDriver_TCPIP::CDriver_TCPIPPacket : public CBase 

	Contains a received packet data.




	.. cpp:function:: LibMCDriver_TCPIP_uint32 GetSize()

		Returns the size of the packet.

		:returns: returns size of packet.


	.. cpp:function:: void GetData(std::vector<LibMCDriver_TCPIP_uint8> & BufferBuffer)

		Returns the data of the packet.

		:param BufferBuffer: packet data. 


.. cpp:type:: std::shared_ptr<CDriver_TCPIPPacket> LibMCDriver_TCPIP::PDriver_TCPIPPacket

	Shared pointer to CDriver_TCPIPPacket to easily allow reference counting.

