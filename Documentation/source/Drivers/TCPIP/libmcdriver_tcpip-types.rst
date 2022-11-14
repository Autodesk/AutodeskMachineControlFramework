
Types used in Generic TCP IP Driver
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_TCPIP_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_TCPIP_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_TCPIP_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_TCPIP_uint64
	
	.. cpp:type:: int8_t LibMCDriver_TCPIP_int8
	
	.. cpp:type:: int16_t LibMCDriver_TCPIP_int16
	
	.. cpp:type:: int32_t LibMCDriver_TCPIP_int32
	
	.. cpp:type:: int64_t LibMCDriver_TCPIP_int64
	
	.. cpp:type:: float LibMCDriver_TCPIP_single
	
	.. cpp:type:: double LibMCDriver_TCPIP_double
	
	.. cpp:type:: LibMCDriver_TCPIP_pvoid = void*
	
	.. cpp:type:: LibMCDriver_TCPIPResult = LibMCDriver_TCPIP_int32
	
	
	
ELibMCDriver_TCPIPException: The standard exception class of Generic TCP IP Driver
============================================================================================================================================================================================================
	
	Errors in Generic TCP IP Driver are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_TCPIP::ELibMCDriver_TCPIPException
	
		.. cpp:function:: void ELibMCDriver_TCPIPException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_TCPIPResult ELibMCDriver_TCPIPException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of Generic TCP IP Driver expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	Generic TCP IP Driver provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_TCPIP::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
