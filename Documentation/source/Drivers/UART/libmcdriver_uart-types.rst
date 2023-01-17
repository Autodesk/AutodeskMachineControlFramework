
Types used in Generic UART Driver
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_UART_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_UART_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_UART_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_UART_uint64
	
	.. cpp:type:: int8_t LibMCDriver_UART_int8
	
	.. cpp:type:: int16_t LibMCDriver_UART_int16
	
	.. cpp:type:: int32_t LibMCDriver_UART_int32
	
	.. cpp:type:: int64_t LibMCDriver_UART_int64
	
	.. cpp:type:: float LibMCDriver_UART_single
	
	.. cpp:type:: double LibMCDriver_UART_double
	
	.. cpp:type:: LibMCDriver_UART_pvoid = void*
	
	.. cpp:type:: LibMCDriver_UARTResult = LibMCDriver_UART_int32
	
	

Enumerations
--------------

	.. cpp:enum-class:: eUARTParity : LibMCDriver_UART_int32
	
		.. cpp:enumerator:: None = 0
		.. cpp:enumerator:: Odd = 1
		.. cpp:enumerator:: Even = 2
		.. cpp:enumerator:: Mark = 3
		.. cpp:enumerator:: Space = 4
	
	.. cpp:enum-class:: eUARTStopBits : LibMCDriver_UART_int32
	
		.. cpp:enumerator:: OneStopBit = 1
		.. cpp:enumerator:: TwoStopBits = 2
		.. cpp:enumerator:: OnePointFiveStopBits = 3
	
	.. cpp:enum-class:: eUARTByteSize : LibMCDriver_UART_int32
	
		.. cpp:enumerator:: FiveBits = 5
		.. cpp:enumerator:: SixBits = 6
		.. cpp:enumerator:: SevenBits = 7
		.. cpp:enumerator:: EightBits = 8
	
	.. cpp:enum-class:: eUARTFlowControl : LibMCDriver_UART_int32
	
		.. cpp:enumerator:: NoFlowControl = 0
		.. cpp:enumerator:: Software = 1
		.. cpp:enumerator:: Hardware = 2
	
	
ELibMCDriver_UARTException: The standard exception class of Generic UART Driver
============================================================================================================================================================================================================
	
	Errors in Generic UART Driver are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_UART::ELibMCDriver_UARTException
	
		.. cpp:function:: void ELibMCDriver_UARTException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_UARTResult ELibMCDriver_UARTException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of Generic UART Driver expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	Generic UART Driver provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_UART::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
