
Types used in MC Driver ScanLab RTC6
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_ScanLab_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_ScanLab_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_ScanLab_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_ScanLab_uint64
	
	.. cpp:type:: int8_t LibMCDriver_ScanLab_int8
	
	.. cpp:type:: int16_t LibMCDriver_ScanLab_int16
	
	.. cpp:type:: int32_t LibMCDriver_ScanLab_int32
	
	.. cpp:type:: int64_t LibMCDriver_ScanLab_int64
	
	.. cpp:type:: float LibMCDriver_ScanLab_single
	
	.. cpp:type:: double LibMCDriver_ScanLab_double
	
	.. cpp:type:: LibMCDriver_ScanLab_pvoid = void*
	
	.. cpp:type:: LibMCDriver_ScanLabResult = LibMCDriver_ScanLab_int32
	
	

Enumerations
--------------

	.. cpp:enum-class:: eLaserMode : LibMCDriver_ScanLab_int32
	
		.. cpp:enumerator:: CO2 = 0
		.. cpp:enumerator:: YAG1 = 1
		.. cpp:enumerator:: YAG2 = 2
		.. cpp:enumerator:: YAG3 = 3
		.. cpp:enumerator:: LaserMode4 = 4
		.. cpp:enumerator:: YAG5 = 5
		.. cpp:enumerator:: LaserMode6 = 6
	
	.. cpp:enum-class:: eLaserPort : LibMCDriver_ScanLab_int32
	
		.. cpp:enumerator:: Port8bitDigital = 1
		.. cpp:enumerator:: Port16bitDigital = 2
		.. cpp:enumerator:: Port12BitAnalog1 = 3
		.. cpp:enumerator:: Port12BitAnalog2 = 4
	

Structs
--------------

	All structs are defined as `packed`, i.e. with the
	
	.. code-block:: c
		
		#pragma pack (1)

	.. cpp:struct:: sPoint2D
	
		.. cpp:member:: LibMCDriver_ScanLab_single m_X
	
		.. cpp:member:: LibMCDriver_ScanLab_single m_Y
	

	.. cpp:struct:: sHatch2D
	
		.. cpp:member:: LibMCDriver_ScanLab_single m_X1
	
		.. cpp:member:: LibMCDriver_ScanLab_single m_Y1
	
		.. cpp:member:: LibMCDriver_ScanLab_single m_X2
	
		.. cpp:member:: LibMCDriver_ScanLab_single m_Y2
	

	
ELibMCDriver_ScanLabException: The standard exception class of MC Driver ScanLab RTC6
============================================================================================================================================================================================================
	
	Errors in MC Driver ScanLab RTC6 are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_ScanLab::ELibMCDriver_ScanLabException
	
		.. cpp:function:: void ELibMCDriver_ScanLabException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_ScanLabResult ELibMCDriver_ScanLabException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of MC Driver ScanLab RTC6 expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	MC Driver ScanLab RTC6 provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_ScanLab::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
