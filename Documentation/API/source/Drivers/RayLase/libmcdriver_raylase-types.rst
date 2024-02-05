
Types used in MC Driver Raylase
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_Raylase_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_Raylase_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_Raylase_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_Raylase_uint64
	
	.. cpp:type:: int8_t LibMCDriver_Raylase_int8
	
	.. cpp:type:: int16_t LibMCDriver_Raylase_int16
	
	.. cpp:type:: int32_t LibMCDriver_Raylase_int32
	
	.. cpp:type:: int64_t LibMCDriver_Raylase_int64
	
	.. cpp:type:: float LibMCDriver_Raylase_single
	
	.. cpp:type:: double LibMCDriver_Raylase_double
	
	.. cpp:type:: LibMCDriver_Raylase_pvoid = void*
	
	.. cpp:type:: LibMCDriver_RaylaseResult = LibMCDriver_Raylase_int32
	
	

Structs
--------------

	All structs are defined as `packed`, i.e. with the
	
	.. code-block:: c
		
		#pragma pack (1)

	.. cpp:struct:: sPoint2D
	
		.. cpp:member:: LibMCDriver_Raylase_single m_X
	
		.. cpp:member:: LibMCDriver_Raylase_single m_Y
	

	.. cpp:struct:: sHatch2D
	
		.. cpp:member:: LibMCDriver_Raylase_single m_X1
	
		.. cpp:member:: LibMCDriver_Raylase_single m_Y1
	
		.. cpp:member:: LibMCDriver_Raylase_single m_X2
	
		.. cpp:member:: LibMCDriver_Raylase_single m_Y2
	

	
ELibMCDriver_RaylaseException: The standard exception class of MC Driver Raylase
============================================================================================================================================================================================================
	
	Errors in MC Driver Raylase are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_Raylase::ELibMCDriver_RaylaseException
	
		.. cpp:function:: void ELibMCDriver_RaylaseException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_RaylaseResult ELibMCDriver_RaylaseException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of MC Driver Raylase expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	MC Driver Raylase provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_Raylase::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
