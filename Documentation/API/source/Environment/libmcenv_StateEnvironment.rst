
CStateEnvironment
====================================================================================================


.. cpp:class:: LibMCEnv::CStateEnvironment : public CBase 

	Environment of the state




	.. cpp:function:: std::string GetMachineState(const std::string & sMachineInstance)

		Retrieves the machine state

		:param sMachineInstance: State machine instance name 
		:returns: Name of current state


	.. cpp:function:: PSignalTrigger PrepareSignal(const std::string & sMachineInstance, const std::string & sSignalName)

		prepares a signal object to trigger later.

		:param sMachineInstance: State machine instance name 
		:param sSignalName: Name Of signal channel. 
		:returns: Signal trigger object.


	.. cpp:function:: bool WaitForSignal(const std::string & sSignalName, const LibMCEnv_uint32 nTimeOut, PSignalHandler & pHandlerInstance)

		waits for a signal.

		:param sSignalName: Name Of Signal 
		:param nTimeOut: Timeout in Milliseconds. 0 for Immediate return. 
		:param pHandlerInstance: Signal object. If Success is false, the Signal Handler Object will be null. 
		:returns: Signal has been triggered


	.. cpp:function:: void GetDriverLibrary(const std::string & sDriverName, std::string & sDriverType, LibMCEnv_pvoid & pDriverLookup)

		Returns the driver type and library lookup for a specific registered driver.

		:param sDriverName: Name identifier of the driver. 
		:param sDriverType: returns type identifier of the driver. 
		:param pDriverLookup: GetProcAddress Handle of the driver. 


	.. cpp:function:: void CreateDriverAccess(const std::string & sDriverName, LibMCEnv_pvoid & pDriverHandle)

		Accesses and locks a driver for the execution of the current state.

		:param sDriverName: Name identifier of the driver. 
		:param pDriverHandle: Handle of the driver. 


	.. cpp:function:: PBuild GetBuildJob(const std::string & sBuildUUID)

		Returns a instance of a build object.

		:param sBuildUUID: UUID of the build entity. 
		:returns: Build instance


	.. cpp:function:: void UnloadAllToolpathes()

		unloads all toolpath in memory to clean up



	.. cpp:function:: void SetNextState(const std::string & sStateName)

		sets the next state

		:param sStateName: Name of next state 


	.. cpp:function:: void LogMessage(const std::string & sLogString)

		logs a string as message

		:param sLogString: String to Log 


	.. cpp:function:: void LogWarning(const std::string & sLogString)

		logs a string as warning

		:param sLogString: String to Log 


	.. cpp:function:: void LogInfo(const std::string & sLogString)

		logs a string as info

		:param sLogString: String to Log 


	.. cpp:function:: void Sleep(const LibMCEnv_uint32 nDelay)

		Puts the current instance to sleep for a definite amount of time. MUST be used instead of a blocking sleep call.

		:param nDelay: Milliseconds to sleeps 


	.. cpp:function:: bool CheckForTermination()

		checks environment for termination signal. MUST be called frequently in longer-running operations.

		:returns: Returns if termination shall appear


	.. cpp:function:: void StoreSignal(const std::string & sName, classParam<CSignalHandler> pHandler)

		stores a signal handler in the current state machine

		:param sName: Name 
		:param pHandler: Signal handler to store. 


	.. cpp:function:: PSignalHandler RetrieveSignal(const std::string & sName)

		retrieves a signal handler from the current state machine. Fails if value has not been stored before or signal has been already handled.

		:param sName: Name 
		:returns: Signal handler instance.


	.. cpp:function:: void ClearStoredValue(const std::string & sName)

		deletes a value from the data store.

		:param sName: Name 


	.. cpp:function:: void SetStringParameter(const std::string & sParameterGroup, const std::string & sParameterName, const std::string & sValue)

		sets a string parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:param sValue: Value to set 


	.. cpp:function:: void SetUUIDParameter(const std::string & sParameterGroup, const std::string & sParameterName, const std::string & sValue)

		sets a uuid parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:param sValue: Value to set 


	.. cpp:function:: void SetDoubleParameter(const std::string & sParameterGroup, const std::string & sParameterName, const LibMCEnv_double dValue)

		sets a double parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:param dValue: Value to set 


	.. cpp:function:: void SetIntegerParameter(const std::string & sParameterGroup, const std::string & sParameterName, const LibMCEnv_int64 nValue)

		sets an int parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:param nValue: Value to set 


	.. cpp:function:: void SetBoolParameter(const std::string & sParameterGroup, const std::string & sParameterName, const bool bValue)

		sets a bool parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:param bValue: Value to set 


	.. cpp:function:: std::string GetStringParameter(const std::string & sParameterGroup, const std::string & sParameterName)

		returns a string parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Value to set


	.. cpp:function:: std::string GetUUIDParameter(const std::string & sParameterGroup, const std::string & sParameterName)

		returns a uuid parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Value to set


	.. cpp:function:: LibMCEnv_double GetDoubleParameter(const std::string & sParameterGroup, const std::string & sParameterName)

		returns a double parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Value to set


	.. cpp:function:: LibMCEnv_int64 GetIntegerParameter(const std::string & sParameterGroup, const std::string & sParameterName)

		returns an int parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Value to set


	.. cpp:function:: bool GetBoolParameter(const std::string & sParameterGroup, const std::string & sParameterName)

		returns a bool parameter

		:param sParameterGroup: Parameter Group 
		:param sParameterName: Parameter Name 
		:returns: Value to set


	.. cpp:function:: void LoadResourceData(const std::string & sResourceName, std::vector<LibMCEnv_uint8> & ResourceDataBuffer)

		loads a plugin resource file into memory.

		:param sResourceName: Name of the resource. 
		:param ResourceDataBuffer: Resource Data Buffer. 


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


	.. cpp:function:: LibMCEnv_uint64 GetGlobalTimerInMilliseconds()

		Returns the global timer in milliseconds.

		:returns: Timer value in Milliseconds


	.. cpp:function:: PTestEnvironment GetTestEnvironment()

		Returns a test environment instance.

		:returns: Test Environment Instance


.. cpp:type:: std::shared_ptr<CStateEnvironment> LibMCEnv::PStateEnvironment

	Shared pointer to CStateEnvironment to easily allow reference counting.

