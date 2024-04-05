
Types used in MC Driver S7Net 2.0
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_S7Net_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_S7Net_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_S7Net_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_S7Net_uint64
	
	.. cpp:type:: int8_t LibMCDriver_S7Net_int8
	
	.. cpp:type:: int16_t LibMCDriver_S7Net_int16
	
	.. cpp:type:: int32_t LibMCDriver_S7Net_int32
	
	.. cpp:type:: int64_t LibMCDriver_S7Net_int64
	
	.. cpp:type:: float LibMCDriver_S7Net_single
	
	.. cpp:type:: double LibMCDriver_S7Net_double
	
	.. cpp:type:: LibMCDriver_S7Net_pvoid = void*
	
	.. cpp:type:: LibMCDriver_S7NetResult = LibMCDriver_S7Net_int32
	
	

Enumerations
--------------

	.. cpp:enum-class:: eS7CPUType : LibMCDriver_S7Net_int32
	
		.. cpp:enumerator:: S7200 = 0
		.. cpp:enumerator:: S7300 = 10
		.. cpp:enumerator:: S7400 = 20
		.. cpp:enumerator:: S71200 = 30
		.. cpp:enumerator:: S71500 = 40
	
	
ELibMCDriver_S7NetException: The standard exception class of MC Driver S7Net 2.0
============================================================================================================================================================================================================
	
	Errors in MC Driver S7Net 2.0 are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_S7Net::ELibMCDriver_S7NetException
	
		.. cpp:function:: void ELibMCDriver_S7NetException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_S7NetResult ELibMCDriver_S7NetException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of MC Driver S7Net 2.0 expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	MC Driver S7Net 2.0 provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_S7Net::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
