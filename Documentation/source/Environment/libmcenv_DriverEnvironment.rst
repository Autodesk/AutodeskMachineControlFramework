
CDriverEnvironment
====================================================================================================


.. cpp:class:: LibMCEnv::CDriverEnvironment : public CBase 

	Environment of the driver




	.. cpp:function:: PWorkingDirectory CreateWorkingDirectory()

		creates a temporary working directory.

		:returns: creates a working directory


	.. cpp:function:: void RetrieveDriverData(const std::string & sIdentifier, std::vector<LibMCEnv_uint8> & DataBufferBuffer)

		retrieves attached driver data into a memory buffer.

		:param sIdentifier: identifier of the binary data in the driver package. 
		:param DataBufferBuffer: buffer data. 


	.. cpp:function:: PToolpathAccessor CreateToolpathAccessor(const std::string & sStreamUUID)

		Creates an accessor object for a toolpath. Toolpath MUST have been loaded into memory before.

		:param sStreamUUID: UUID of the stream. 
		:returns: Toolpath instance.


	.. cpp:function:: void RegisterStringParameter(const std::string & sParameterName, const std::string & sDescription, const std::string & sDefaultValue)

		registers a string parameter. Must only be called during driver creation.

		:param sParameterName: Parameter Name 
		:param sDescription: Parameter Description 
		:param sDefaultValue: default value to set 


	.. cpp:function:: void RegisterUUIDParameter(const std::string & sParameterName, const std::string & sDescription, const std::string & sDefaultValue)

		registers a uuid parameter. Must only be called during driver creation.

		:param sParameterName: Parameter Name 
		:param sDescription: Parameter Description 
		:param sDefaultValue: default value to set 


	.. cpp:function:: void RegisterDoubleParameter(const std::string & sParameterName, const std::string & sDescription, const LibMCEnv_double dDefaultValue)

		registers a double parameter. Must only be called during driver creation.

		:param sParameterName: Parameter Name 
		:param sDescription: Parameter Description 
		:param dDefaultValue: default value to set 


	.. cpp:function:: void RegisterIntegerParameter(const std::string & sParameterName, const std::string & sDescription, const LibMCEnv_int64 nDefaultValue)

		registers an int parameter. Must only be called during driver creation.

		:param sParameterName: Parameter Name 
		:param sDescription: Parameter Description 
		:param nDefaultValue: default value to set 


	.. cpp:function:: void RegisterBoolParameter(const std::string & sParameterName, const std::string & sDescription, const bool bDefaultValue)

		registers a bool parameter. Must only be called during driver creation.

		:param sParameterName: Parameter Name 
		:param sDescription: Parameter Description 
		:param bDefaultValue: default value to set 


	.. cpp:function:: void SetStringParameter(const std::string & sParameterName, const std::string & sValue)

		sets a string parameter

		:param sParameterName: Parameter Name 
		:param sValue: Value to set 


	.. cpp:function:: void SetUUIDParameter(const std::string & sParameterName, const std::string & sValue)

		sets a uuid parameter

		:param sParameterName: Parameter Name 
		:param sValue: Value to set 


	.. cpp:function:: void SetDoubleParameter(const std::string & sParameterName, const LibMCEnv_double dValue)

		sets a double parameter

		:param sParameterName: Parameter Name 
		:param dValue: Value to set 


	.. cpp:function:: void SetIntegerParameter(const std::string & sParameterName, const LibMCEnv_int64 nValue)

		sets an int parameter

		:param sParameterName: Parameter Name 
		:param nValue: Value to set 


	.. cpp:function:: void SetBoolParameter(const std::string & sParameterName, const bool bValue)

		sets a bool parameter

		:param sParameterName: Parameter Name 
		:param bValue: Value to set 


	.. cpp:function:: void Sleep(const LibMCEnv_uint32 nDelay)

		Puts the current instance to sleep for a definite amount of time. MUST be used instead of a blocking sleep call.

		:param nDelay: Milliseconds to sleeps 


	.. cpp:function:: LibMCEnv_uint64 GetGlobalTimerInMilliseconds()

		Returns the global timer in milliseconds.

		:returns: Timer value in Milliseconds


	.. cpp:function:: void LogMessage(const std::string & sLogString)

		logs a string as message

		:param sLogString: String to Log 


	.. cpp:function:: void LogWarning(const std::string & sLogString)

		logs a string as warning

		:param sLogString: String to Log 


	.. cpp:function:: void LogInfo(const std::string & sLogString)

		logs a string as info

		:param sLogString: String to Log 


	.. cpp:function:: PImageData CreateEmptyImage(const LibMCEnv_uint32 nPixelSizeX, const LibMCEnv_uint32 nPixelSizeY, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const eImagePixelFormat ePixelFormat)

		creates an empty image object.

		:param nPixelSizeX: Pixel size in X. MUST be positive. 
		:param nPixelSizeY: Pixel size in Y. MUST be positive. 
		:param dDPIValueX: DPI Value in X. MUST be positive. 
		:param dDPIValueY: DPI Value in Y. MUST be positive. 
		:param ePixelFormat: Pixel format to use. 
		:returns: Empty image instance.


	.. cpp:function:: PImageData LoadPNGImage(const CInputVector<LibMCEnv_uint8> & PNGDataBuffer, const LibMCEnv_double dDPIValueX, const LibMCEnv_double dDPIValueY, const eImagePixelFormat ePixelFormat)

		creates an image object from a PNG data stream.

		:param PNGDataBuffer: DPI Value in X. MUST be positive. 
		:param dDPIValueX: DPI Value in X. MUST be positive. 
		:param dDPIValueY: DPI Value in Y. MUST be positive. 
		:param ePixelFormat: Pixel format to use. Might lose color and alpha information. 
		:returns: Image instance containing the PNG image.


.. cpp:type:: std::shared_ptr<CDriverEnvironment> LibMCEnv::PDriverEnvironment

	Shared pointer to CDriverEnvironment to easily allow reference counting.

