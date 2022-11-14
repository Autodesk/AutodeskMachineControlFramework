
Types used in MC Environment Interface
==========================================================================================================


Simple types
--------------

	.. cpp:type:: uint8_t LibMCEnv_uint8
	
	.. cpp:type:: uint16_t LibMCEnv_uint16
	
	.. cpp:type:: uint32_t LibMCEnv_uint32
	
	.. cpp:type:: uint64_t LibMCEnv_uint64
	
	.. cpp:type:: int8_t LibMCEnv_int8
	
	.. cpp:type:: int16_t LibMCEnv_int16
	
	.. cpp:type:: int32_t LibMCEnv_int32
	
	.. cpp:type:: int64_t LibMCEnv_int64
	
	.. cpp:type:: float LibMCEnv_single
	
	.. cpp:type:: double LibMCEnv_double
	
	.. cpp:type:: LibMCEnv_pvoid = void*
	
	.. cpp:type:: LibMCEnvResult = LibMCEnv_int32
	
	

Enumerations
--------------

	.. cpp:enum-class:: eImagePixelFormat : LibMCEnv_int32
	
		.. cpp:enumerator:: Unknown = 0
		.. cpp:enumerator:: GreyScale8bit = 1
		.. cpp:enumerator:: RGB24bit = 2
		.. cpp:enumerator:: RGBA32bit = 3
	
	.. cpp:enum-class:: eToolpathSegmentType : LibMCEnv_int32
	
		.. cpp:enumerator:: Unknown = 0
		.. cpp:enumerator:: Hatch = 1
		.. cpp:enumerator:: Loop = 2
		.. cpp:enumerator:: Polyline = 3
	
	.. cpp:enum-class:: eToolpathProfileValueType : LibMCEnv_int32
	
		.. cpp:enumerator:: Custom = 0
		.. cpp:enumerator:: Speed = 1
		.. cpp:enumerator:: JumpSpeed = 2
		.. cpp:enumerator:: ExtrusionFactor = 3
		.. cpp:enumerator:: LaserPower = 4
		.. cpp:enumerator:: StartDelay = 5
		.. cpp:enumerator:: EndDelay = 6
		.. cpp:enumerator:: PolyDelay = 7
		.. cpp:enumerator:: PolyDelayAngleDependent = 8
		.. cpp:enumerator:: LaserOnDelay = 9
		.. cpp:enumerator:: LaserOffDelay = 10
		.. cpp:enumerator:: JumpDelay = 11
		.. cpp:enumerator:: MarkDelay = 12
		.. cpp:enumerator:: LaserFocus = 13
	
	.. cpp:enum-class:: eMessageDialogType : LibMCEnv_int32
	
		.. cpp:enumerator:: Unknown = 0
		.. cpp:enumerator:: DialogOK = 1
		.. cpp:enumerator:: DialogOKCancel = 2
		.. cpp:enumerator:: DialogYesNo = 3
		.. cpp:enumerator:: DialogYesNoCancel = 4
	

Structs
--------------

	All structs are defined as `packed`, i.e. with the
	
	.. code-block:: c
		
		#pragma pack (1)

	.. cpp:struct:: sPosition2D
	
		.. cpp:member:: LibMCEnv_int32 m_Coordinates[2]
	

	.. cpp:struct:: sToolpathPartTransform
	
		.. cpp:member:: LibMCEnv_double m_Matrix[3][3]
	
		.. cpp:member:: LibMCEnv_double m_Translation[3]
	

	.. cpp:struct:: sColorRGB
	
		.. cpp:member:: LibMCEnv_double m_Red
	
		.. cpp:member:: LibMCEnv_double m_Green
	
		.. cpp:member:: LibMCEnv_double m_Blue
	

	
ELibMCEnvException: The standard exception class of MC Environment Interface
============================================================================================================================================================================================================
	
	Errors in MC Environment Interface are reported as Exceptions. It is recommended to not throw these exceptions in your client code.
	
	
	.. cpp:class:: LibMCEnv::ELibMCEnvException
	
		.. cpp:function:: void ELibMCEnvException::what() const noexcept
		
			 Returns error message
		
			 :return: the error message of this exception
		
	
		.. cpp:function:: LibMCEnvResult ELibMCEnvException::getErrorCode() const noexcept
		
			 Returns error code
		
			 :return: the error code of this exception
		
	
CInputVector: Adapter for passing arrays as input for functions
===============================================================================================================================================================
	
	Several functions of MC Environment Interface expect arrays of integral types or structs as input parameters.
	To not restrict the interface to, say, std::vector<type>,
	and to have a more abstract interface than a location in memory and the number of elements to input to a function
	MC Environment Interface provides a templated adapter class to pass arrays as input for functions.
	
	Usually, instances of CInputVector are generated anonymously (or even implicitly) in the call to a function that expects an input array.
	
	
	.. cpp:class:: template<typename T> LibMCEnv::CInputVector
	
		.. cpp:function:: CInputVector(const std::vector<T>& vec)
	
			Constructs of a CInputVector from a std::vector<T>
	
		.. cpp:function:: CInputVector(const T* in_data, size_t in_size)
	
			Constructs of a CInputVector from a memory address and a given number of elements
	
		.. cpp:function:: const T* CInputVector::data() const
	
			returns the start address of the data captured by this CInputVector
	
		.. cpp:function:: size_t CInputVector::size() const
	
			returns the number of elements captured by this CInputVector
	
 
