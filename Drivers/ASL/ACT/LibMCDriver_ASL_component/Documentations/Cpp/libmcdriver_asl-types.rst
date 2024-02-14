
Types used in ASL Inkjet Driver Interface
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_ASL_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_ASL_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_ASL_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_ASL_uint64
	
	.. cpp:type:: int8_t LibMCDriver_ASL_int8
	
	.. cpp:type:: int16_t LibMCDriver_ASL_int16
	
	.. cpp:type:: int32_t LibMCDriver_ASL_int32
	
	.. cpp:type:: int64_t LibMCDriver_ASL_int64
	
	.. cpp:type:: float LibMCDriver_ASL_single
	
	.. cpp:type:: double LibMCDriver_ASL_double
	
	.. cpp:type:: LibMCDriver_ASL_pvoid = void*
	
	.. cpp:type:: LibMCDriver_ASLResult = LibMCDriver_ASL_int32
	
	

Enumerations
--------------

	.. cpp:enum-class:: eBoardMode : LibMCDriver_ASL_int32
	
		.. cpp:enumerator:: DW_INT = 1000
		.. cpp:enumerator:: DW_EXT = 1001
		.. cpp:enumerator:: IMG_ENC = 1002
		.. cpp:enumerator:: IMG_PD = 1003
	
	
ELibMCDriver_ASLException: The standard exception class of ASL Inkjet Driver Interface
============================================================================================================================================================================================================
	
	Errors in ASL Inkjet Driver Interface are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_ASL::ELibMCDriver_ASLException
	
		.. cpp:function:: void ELibMCDriver_ASLException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_ASLResult ELibMCDriver_ASLException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of ASL Inkjet Driver Interface expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	ASL Inkjet Driver Interface provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_ASL::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
