
Types used in MC Driver Technosoft Motion Library
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCDriver_TML_uint8
	
	.. cpp:type:: uint16_t LibMCDriver_TML_uint16
	
	.. cpp:type:: uint32_t LibMCDriver_TML_uint32
	
	.. cpp:type:: uint64_t LibMCDriver_TML_uint64
	
	.. cpp:type:: int8_t LibMCDriver_TML_int8
	
	.. cpp:type:: int16_t LibMCDriver_TML_int16
	
	.. cpp:type:: int32_t LibMCDriver_TML_int32
	
	.. cpp:type:: int64_t LibMCDriver_TML_int64
	
	.. cpp:type:: float LibMCDriver_TML_single
	
	.. cpp:type:: double LibMCDriver_TML_double
	
	.. cpp:type:: LibMCDriver_TML_pvoid = void*
	
	.. cpp:type:: LibMCDriver_TMLResult = LibMCDriver_TML_int32
	
	

Enumerations
--------------

	.. cpp:enum-class:: eChannelType : LibMCDriver_TML_int32
	
		.. cpp:enumerator:: Unknown = 0
		.. cpp:enumerator:: RS232 = 1000
		.. cpp:enumerator:: RS485 = 1001
		.. cpp:enumerator:: IXXAT_CAN = 1002
		.. cpp:enumerator:: SysTecUSBCAN = 1003
		.. cpp:enumerator:: PEAK_SYS_PCAN_PCI = 1004
		.. cpp:enumerator:: CHANNEL_ESD_CAN = 1005
	
	.. cpp:enum-class:: eProtocolType : LibMCDriver_TML_int32
	
		.. cpp:enumerator:: Unknown = 0
		.. cpp:enumerator:: TMLCAN = 1
		.. cpp:enumerator:: TechnoCAN = 2
	
	
ELibMCDriver_TMLException: The standard exception class of MC Driver Technosoft Motion Library
============================================================================================================================================================================================================
	
	Errors in MC Driver Technosoft Motion Library are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCDriver_TML::ELibMCDriver_TMLException
	
		.. cpp:function:: void ELibMCDriver_TMLException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCDriver_TMLResult ELibMCDriver_TMLException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of MC Driver Technosoft Motion Library expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	MC Driver Technosoft Motion Library provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCDriver_TML::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
